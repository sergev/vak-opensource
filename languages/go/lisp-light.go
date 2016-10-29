/*
  Nukata Lisp Light 1.42 in Go 1.7 by SUZUKI Hisao (H27.5/11 - H28.9/8)

  This is a Lisp interpreter written in Go.
  It differs from the previous version(*1) in that all numbers are
  64-bit floats and the whole interpreter consists of only one file.
  It is a "light" version.
  Intentionally it implements the same language as Nukata Lisp Light
  1.26 in TypeScript 1.8(*2) except that it has also two concurrent
  constructs, future and force.  See *3.

    *1: http://www.oki-osk.jp/esc/golang/lisp3.html (in Japanese)
    *2: http://www.oki-osk.jp/esc/typescript/lisp-en.html
    *3: http://www.oki-osk.jp/esc/golang/lisp4-en.html
*/
package main

import (
	"bufio"
	"errors"
	"fmt"
	"io"
	"math"
	"os"
	"regexp"
	"runtime"
	"strconv"
	"strings"
	"sync"
	"unicode/utf8"
)

// Cell represents a cons cell.
// &Cell{car, cdr} works as the "cons" operation.
type Cell struct {
	Car interface{}
	Cdr interface{}
}

// Nil is a nil of type *Cell and it represents the empty list.
var Nil *Cell = nil

// CdrCell returns cdr of the cell as a *Cell or Nil.
func (j *Cell) CdrCell() *Cell {
	if c, ok := j.Cdr.(*Cell); ok {
		return c
	}
	panic(NewEvalError("proper list expected", j))
}

// (a b c).FoldL(x, fn) returns fn(fn(fn(x, a), b), c)
func (j *Cell) FoldL(x interface{},
	fn func(interface{}, interface{}) interface{}) interface{} {
	for j != Nil {
		x = fn(x, j.Car)
		j = j.CdrCell()
	}
	return x
}

// Len returns the length of list j
func (j *Cell) Len() int {
	return j.FoldL(0, func(i, e interface{}) interface{} {
		return i.(int) + 1
	}).(int)
}

// (a b c).MapCar(fn) returns (fn(a) fn(b) fn(c))
func (j *Cell) MapCar(fn func(interface{}) interface{}) interface{} {
	if j == Nil {
		return Nil
	}
	a := fn(j.Car)
	d := j.Cdr
	if cdr, ok := d.(*Cell); ok {
		d = cdr.MapCar(fn)
	}
	if j.Car == a && j.Cdr == d {
		return j
	}
	return &Cell{a, d}
}

// String returns a raw textual representation of j for debugging.
func (j *Cell) String() string {
	return fmt.Sprintf("(%v . %v)", j.Car, j.Cdr)
}

//----------------------------------------------------------------------

// Sym represents a symbol (or an expression keyword) in Lisp.
// &Sym{name, false} constructs a symbol which is not interned yet.
type Sym struct {
	Name      string
	IsKeyword bool
}

// NewSym constructs an interned symbol for name.
func NewSym(name string) *Sym {
	return NewSym2(name, false)
}

// symbols is a table of interned symbols.
var symbols = make(map[string]*Sym)

// symLock is an exclusive lock for the table.
var symLock sync.RWMutex

// NewSym2 constructs an interned symbol (or an expression keyword
// if isKeyword is true on its first construction) for name.
func NewSym2(name string, isKeyword bool) *Sym {
	symLock.Lock()
	sym, ok := symbols[name]
	if !ok {
		sym = &Sym{name, isKeyword}
		symbols[name] = sym
	}
	symLock.Unlock()
	return sym
}

// IsInterned returns true if sym is interned.
func (sym *Sym) IsInterned() bool {
	symLock.RLock()
	s, ok := symbols[sym.Name]
	symLock.RUnlock()
	return ok && s == sym
}

// String returns a textual representation of sym.
func (sym *Sym) String() string {
	return sym.Name
}

// Defined symbols

var BackQuoteSym = NewSym("`")
var CommaAtSym = NewSym(",@")
var CommaSym = NewSym(",")
var DotSym = NewSym(".")
var LeftParenSym = NewSym("(")
var RightParenSym = NewSym(")")
var SingleQuoteSym = NewSym("'")

var AppendSym = NewSym("append")
var ConsSym = NewSym("cons")
var ListSym = NewSym("list")
var RestSym = NewSym("&rest")
var UnquoteSym = NewSym("unquote")
var UnquoteSplicingSym = NewSym("unquote-splicing")

// Expression keywords

var CondSym = NewSym2("cond", true)
var FutureSym = NewSym2("future", true)
var LambdaSym = NewSym2("lambda", true)
var MacroSym = NewSym2("macro", true)
var ProgNSym = NewSym2("progn", true)
var QuasiquoteSym = NewSym2("quasiquote", true)
var QuoteSym = NewSym2("quote", true)
var SetqSym = NewSym2("setq", true)

//----------------------------------------------------------------------

// Func is a common base type of Lisp functions.
type Func struct {
	// Carity is a number of arguments, made negative if the func has &rest.
	Carity int
}

// hasRest returns true if fn has &rest.
func (fn *Func) hasRest() bool {
	return fn.Carity < 0
}

// fixedArgs returns the number of fixed arguments.
func (fn *Func) fixedArgs() int {
	if c := fn.Carity; c < 0 {
		return -c - 1
	} else {
		return c
	}
}

// MakeFrame makes a call-frame from a list of actual arguments.
// Argument x will be used instead of fn only in error messages.
func (fn *Func) MakeFrame(arg *Cell, x interface{}) []interface{} {
	arity := fn.Carity // number of arguments, counting the whole rests as one
	if arity < 0 {
		arity = -arity
	}
	frame := make([]interface{}, arity)
	n := fn.fixedArgs()
	i := 0
	for i < n && arg != Nil { // Set the list of fixed arguments.
		frame[i] = arg.Car
		arg = arg.CdrCell()
		i++
	}
	if i != n || (arg != Nil && !fn.hasRest()) {
		panic(NewEvalError("arity not matched", x))
	}
	if fn.hasRest() {
		frame[n] = arg
	}
	return frame
}

// EvalFrame evaluates each expression of frame with interp in env.
func (fn *Func) EvalFrame(frame []interface{}, interp *Interp, env *Cell) {
	n := fn.fixedArgs()
	for i := 0; i < n; i++ {
		frame[i] = interp.Eval(frame[i], env)
	}
	if fn.hasRest() {
		if j, ok := frame[n].(*Cell); ok {
			z := Nil
			y := Nil
			for j != Nil {
				e := interp.Eval(j.Car, env)
				x := &Cell{e, Nil}
				if z == Nil {
					z = x
				} else {
					y.Cdr = x
				}
				y = x
				j = j.CdrCell()
			}
			frame[n] = z
		}
	}
}

//----------------------------------------------------------------------

// Macro represents a compiled macro expression.
type Macro struct {
	Func
	// body is a list which will be used as the function body.
	body *Cell
}

// NewMacro constructs a Macro.
func NewMacro(carity int, body *Cell, env *Cell) interface{} {
	return &Macro{Func{carity}, body}
}

// ExpandWith expands the macro with a list of actual arguments.
func (x *Macro) ExpandWith(interp *Interp, arg *Cell) interface{} {
	frame := x.MakeFrame(arg, x)
	env := &Cell{frame, Nil}
	var y interface{} = Nil
	for j := x.body; j != Nil; j = j.CdrCell() {
		y = interp.Eval(j.Car, env)
	}
	return y
}

// String returns a textual representation of the macro.
func (x *Macro) String() string {
	return fmt.Sprintf("#<macro:%d:%s>", x.Carity, Str(x.body))
}

// Lambda represents a compiled lambda expression (within another function).
type Lambda struct {
	Func
	// Body is a list which will be used as the function body.
	Body *Cell
}

// NewLambda constructs a Lambda.
func NewLambda(carity int, body *Cell, env *Cell) interface{} {
	return &Lambda{Func{carity}, body}
}

// String returns a textual representation of the lambda.
func (x *Lambda) String() string {
	return fmt.Sprintf("#<lambda:%d:%s>", x.Carity, Str(x.Body))
}

// Closure represents a compiled lambda expression with its own environment.
type Closure struct {
	Lambda
	// Env is the closure's own environment.
	Env *Cell
}

// NewClosure constructs a Closure.
func NewClosure(carity int, body *Cell, env *Cell) interface{} {
	return &Closure{Lambda{Func{carity}, body}, env}
}

// MakeEnv makes a new environment from a list of acutual arguments,
// which will be used in evaluation of the body of the closure.
func (x *Closure) MakeEnv(interp *Interp, arg *Cell, interpEnv *Cell) *Cell {
	frame := x.MakeFrame(arg, x)
	x.EvalFrame(frame, interp, interpEnv)
	return &Cell{frame, x.Env} // Prepend the frame to Env of the closure.
}

// String returns a textual representation of the closure.
func (x *Closure) String() string {
	return fmt.Sprintf("#<closure:%d:%s:%s>",
		x.Carity, Str(x.Env), Str(x.Body))
}

//----------------------------------------------------------------------

// BuiltInFunc represents a built-in function.
type BuiltInFunc struct {
	Func
	name string
	body func([]interface{}) interface{}
}

// NewBuiltInFunc constructs a BuiltInFunc.
func NewBuiltInFunc(name string, carity int,
	body func([]interface{}) interface{}) *BuiltInFunc {
	return &BuiltInFunc{Func{carity}, name, body}
}

// EvalWith invokes the built-in function with a list of actual arguments.
func (x *BuiltInFunc) EvalWith(interp *Interp, arg *Cell,
	interpEnv *Cell) interface{} {
	frame := x.MakeFrame(arg, x)
	x.EvalFrame(frame, interp, interpEnv)
	defer func() {
		if err := recover(); err != nil {
			if _, ok := err.(*EvalError); ok {
				panic(err)
			} else {
				msg := fmt.Sprintf("%v -- %s", err, x.name)
				panic(NewEvalError(msg, frame))
			}
		}
	}()
	return x.body(frame)
}

// String returns a textual representation of the BuiltInFunc.
func (x *BuiltInFunc) String() string {
	return fmt.Sprintf("#<%s:%d>", x.name, x.Carity)
}

//----------------------------------------------------------------------

// Arg represents a bound variable in a compiled lambda/macro expression.
// It is constructed with &Arg{level, offset, symbol}.
type Arg struct {
	// Level is a nesting level of the lexical scope.
	// 0 for the innermost scope.
	Level int

	// Offset is an offset of the variable within the frame of the Level.
	// 0 for the first variable within the frame.
	Offset int

	// Sym is a symbol which represented the variable before compilation.
	Symbol *Sym
}

// GetValue gets a value from the location corresponding to the variable x
// within an environment env.
func (x *Arg) GetValue(env *Cell) interface{} {
	for i := 0; i < x.Level; i++ {
		env = env.Cdr.(*Cell)
	}
	return (env.Car.([]interface{}))[x.Offset]
}

// SetValue sets a value y to the location corresponding to the variable x
// within an environment env.
func (x *Arg) SetValue(y interface{}, env *Cell) {
	for i := 0; i < x.Level; i++ {
		env = env.Cdr.(*Cell)
	}
	(env.Car.([]interface{}))[x.Offset] = y
}

// String returns a textual representation of the Arg.
func (x *Arg) String() string {
	return fmt.Sprintf("#%d:%d:%v", x.Level, x.Offset, x.Symbol)
}

//----------------------------------------------------------------------

// EvalError represents an error in evaluation.
type EvalError struct {
	Message string
	Trace   []string
}

// NewEvalError constructs an EvalError.
func NewEvalError(msg string, x interface{}) *EvalError {
	return &EvalError{msg + ": " + Str(x), nil}
}

// NewNotVariableError constructs an EvalError which indicates an absence
// of variable.
func NewNotVariableError(x interface{}) *EvalError {
	return NewEvalError("variable expected", x)
}

// Error returns a textual representation of the error.
// It is defined in compliance with the error type.
func (err *EvalError) Error() string {
	s := "EvalError: " + err.Message
	for _, line := range err.Trace {
		s += "\n\t" + line
	}
	return s
}

// EofToken is a token which represents the end of file.
var EofToken error = errors.New("end of file")

//----------------------------------------------------------------------

// Interp represents a core of the interpreter.
type Interp struct {
	globals map[*Sym]interface{}
	lock    sync.RWMutex
}

// Future represents a "promise" for future/force.
type Future struct {
	// Chan is a channel which transmits a pair of result and error.
	// The pair is represented by Cell.
	Chan <-chan Cell

	// Result is a pair of the result (in a narrow meaning) and the error.
	Result Cell

	// Lock is an exclusive lock to receive the result at "force".
	Lock sync.Mutex
}

// String returns a textual representation of the Future.
func (fu *Future) String() string {
	return fmt.Sprintf("#<future:%v:%s:%v>",
		fu.Chan, Str(&fu.Result), &fu.Lock)
}

// GetGlobalVar gets a global value of symbol sym within the interpreter.
func (interp *Interp) GetGlobalVar(sym *Sym) (interface{}, bool) {
	interp.lock.RLock()
	val, ok := interp.globals[sym]
	interp.lock.RUnlock()
	return val, ok
}

// SetGlobalVar sets a global value of symbol sym within the interpreter.
func (interp *Interp) SetGlobalVar(sym *Sym, val interface{}) {
	interp.lock.Lock()
	interp.globals[sym] = val
	interp.lock.Unlock()
}

// NewInterp constructs an interpreter and sets built-in functions etc. as
// the global values of symbols within the interpreter.
func NewInterp() *Interp {
	interp := &Interp{globals: make(map[*Sym]interface{})}

	interp.Def("car", 1, func(a []interface{}) interface{} {
		if a[0] == Nil {
			return Nil
		}
		return a[0].(*Cell).Car
	})

	interp.Def("cdr", 1, func(a []interface{}) interface{} {
		if a[0] == Nil {
			return Nil
		}
		return a[0].(*Cell).Cdr
	})

	interp.Def("cons", 2, func(a []interface{}) interface{} {
		return &Cell{a[0], a[1]}
	})

	interp.Def("atom", 1, func(a []interface{}) interface{} {
		if j, ok := a[0].(*Cell); ok && j != Nil {
			return Nil
		}
		return true
	})

	interp.Def("eq", 2, func(a []interface{}) interface{} {
		if a[0] == a[1] { // Cells are compared by address.
			return true
		}
		return Nil
	})

	interp.Def("list", -1, func(a []interface{}) interface{} {
		return a[0]
	})

	interp.Def("rplaca", 2, func(a []interface{}) interface{} {
		a[0].(*Cell).Car = a[1]
		return a[1]
	})

	interp.Def("rplacd", 2, func(a []interface{}) interface{} {
		a[0].(*Cell).Cdr = a[1]
		return a[1]
	})

	interp.Def("length", 1, func(a []interface{}) interface{} {
		switch x := a[0].(type) {
		case *Cell:
			return float64(x.Len())
		case string: // Each multi-bytes character counts 1.
			return float64(utf8.RuneCountInString(x))
		default:
			panic(NewEvalError("list or string expected", x))
		}
	})

	interp.Def("stringp", 1, func(a []interface{}) interface{} {
		if _, ok := a[0].(string); ok {
			return true
		}
		return Nil
	})

	interp.Def("numberp", 1, func(a []interface{}) interface{} {
		if _, ok := a[0].(float64); ok {
			return true
		}
		return Nil
	})

	interp.Def("eql", 2, func(a []interface{}) interface{} {
		if a[0] == a[1] { // Numbers are compared by value.  See "eq".
			return true
		}
		return Nil
	})

	interp.Def("<", 2, func(a []interface{}) interface{} {
		if a[0].(float64) < a[1].(float64) {
			return true
		}
		return Nil
	})

	interp.Def("%", 2, func(a []interface{}) interface{} {
		return math.Mod(a[0].(float64), a[1].(float64))
	})

	interp.Def("mod", 2, func(a []interface{}) interface{} {
		x, y := a[0].(float64), a[1].(float64)
		if (x < 0 && y > 0) || (x > 0 && y < 0) {
			return math.Mod(x, y) + y
		}
		return math.Mod(x, y)
	})

	interp.Def("+", -1, func(a []interface{}) interface{} {
		return a[0].(*Cell).FoldL(0.0,
			func(x, y interface{}) interface{} {
				return x.(float64) + y.(float64)
			})
	})

	interp.Def("*", -1, func(a []interface{}) interface{} {
		return a[0].(*Cell).FoldL(1.0,
			func(x, y interface{}) interface{} {
				return x.(float64) * y.(float64)
			})
	})

	interp.Def("-", -2, func(a []interface{}) interface{} {
		if a[1] == Nil {
			return -a[0].(float64)
		} else {
			return a[1].(*Cell).FoldL(a[0].(float64),
				func(x, y interface{}) interface{} {
					return x.(float64) - y.(float64)
				})
		}
	})

	interp.Def("/", -3, func(a []interface{}) interface{} {
		return a[2].(*Cell).FoldL(a[0].(float64)/a[1].(float64),
			func(x, y interface{}) interface{} {
				return x.(float64) / y.(float64)
			})
	})

	interp.Def("truncate", -2, func(a []interface{}) interface{} {
		x, y := a[0].(float64), a[1].(*Cell)
		if y == Nil {
			return math.Trunc(x)
		} else if y.Cdr == Nil {
			return math.Trunc(x / y.Car.(float64))
		} else {
			panic("one or two arguments expected")
		}
	})

	interp.Def("prin1", 1, func(a []interface{}) interface{} {
		fmt.Print(Str2(a[0], true))
		return a[0]
	})

	interp.Def("princ", 1, func(a []interface{}) interface{} {
		fmt.Print(Str2(a[0], false))
		return a[0]
	})

	interp.Def("terpri", 0, func(a []interface{}) interface{} {
		fmt.Println()
		return true
	})

	gensymCounterSym := NewSym("*gensym-counter*")
	interp.SetGlobalVar(gensymCounterSym, 1.0)
	interp.Def("gensym", 0, func(a []interface{}) interface{} {
		interp.lock.Lock()
		defer interp.lock.Unlock()
		x := interp.globals[gensymCounterSym].(float64)
		interp.globals[gensymCounterSym] = x + 1.0
		return &Sym{fmt.Sprintf("G%d", int(x)), false}
	})

	interp.Def("make-symbol", 1, func(a []interface{}) interface{} {
		return &Sym{a[0].(string), false}
	})

	interp.Def("intern", 1, func(a []interface{}) interface{} {
		return NewSym(a[0].(string))
	})

	interp.Def("symbol-name", 1, func(a []interface{}) interface{} {
		return a[0].(*Sym).Name
	})

	interp.Def("apply", 2, func(a []interface{}) interface{} {
		args := a[1].(*Cell).MapCar(QqQuote)
		return interp.Eval(&Cell{a[0], args}, Nil)
	})

	interp.Def("exit", 1, func(a []interface{}) interface{} {
		n := int(a[0].(float64))
		os.Exit(n)
		return Nil // *not reached*
	})

	interp.Def("dump", 0, func(a []interface{}) interface{} {
		interp.lock.RLock()
		defer interp.lock.RUnlock()
		r := Nil
		for key := range interp.globals {
			r = &Cell{key, r}
		}
		return r
	})

	// Wait until the "promise" of Future is delivered.
	interp.Def("force", 1, func(a []interface{}) interface{} {
		if fu, ok := a[0].(*Future); ok {
			fu.Lock.Lock()
			defer fu.Lock.Unlock()
			if fu.Chan != nil {
				fu.Result = <-fu.Chan
				fu.Chan = nil
			}
			if err := fu.Result.Cdr; err != nil {
				fu.Result.Cdr = nil
				panic(err) // Transmit the error.
			}
			return fu.Result.Car
		} else {
			return a[0]
		}
	})

	interp.SetGlobalVar(NewSym("*version*"),
		&Cell{
			1.42,
			&Cell{
				fmt.Sprintf("%s %s/%s",
					runtime.Version(), runtime.GOOS, runtime.GOARCH),
				&Cell{
					"Nukata Lisp Light",
					Nil}}})
	// named after Nukata-gun (額田郡) in Tōkai-dō Mikawa-koku (東海道 三河国)

	return interp
}

// Def defines a built-in function by giving a name, arity, and body.
func (interp *Interp) Def(name string, carity int,
	body func([]interface{}) interface{}) {
	sym := NewSym(name)
	fnc := NewBuiltInFunc(name, carity, body)
	interp.SetGlobalVar(sym, fnc)
}

// Eval evaluates a Lisp expression in a given environment env.
func (interp *Interp) Eval(expression interface{}, env *Cell) interface{} {
	defer func() {
		if err := recover(); err != nil {
			if ex, ok := err.(*EvalError); ok {
				if ex.Trace == nil {
					ex.Trace = make([]string, 0, 10)
				}
				if len(ex.Trace) < 10 {
					ex.Trace = append(ex.Trace, Str(expression))
				}
			}
			panic(err)
		}
	}()
	for {
		switch x := expression.(type) {
		case *Arg:
			return x.GetValue(env)
		case *Sym:
			r, ok := interp.GetGlobalVar(x)
			if ok {
				return r
			}
			panic(NewEvalError("void variable", x))
		case *Cell:
			if x == Nil {
				return x // an empty list
			}
			fn := x.Car
			arg := x.CdrCell()
			sym, ok := fn.(*Sym)
			if ok && sym.IsKeyword {
				switch sym {
				case QuoteSym:
					if arg != Nil && arg.Cdr == Nil {
						return arg.Car
					}
					panic(NewEvalError("bad quote", x))
				case ProgNSym:
					expression = interp.evalProgN(arg, env)
				case CondSym:
					expression = interp.evalCond(arg, env)
				case SetqSym:
					return interp.evalSetQ(arg, env)
				case LambdaSym:
					return interp.compile(arg, env, NewClosure)
				case MacroSym:
					if env != Nil {
						panic(NewEvalError("nested macro", x))
					}
					return interp.compile(arg, Nil, NewMacro)
				case QuasiquoteSym:
					if arg != Nil && arg.Cdr == Nil {
						expression = QqExpand(arg.Car)
					} else {
						panic(NewEvalError("bad quasiquote", x))
					}
				case FutureSym:
					ch := make(chan Cell)
					go interp.futureTask(arg, env, ch)
					return &Future{Chan: ch}
				default:
					panic(NewEvalError("bad keyword", fn))
				}
			} else { // Apply fn to arg.
				// Expand fn = interp.Eval(fn, env) here on Sym for speed.
				if ok {
					fn, ok = interp.GetGlobalVar(sym)
					if !ok {
						panic(NewEvalError("undefined", x.Car))
					}
				} else {
					fn = interp.Eval(fn, env)
				}
				switch f := fn.(type) {
				case *Closure:
					env = f.MakeEnv(interp, arg, env)
					expression = interp.evalProgN(f.Body, env)
				case *Macro:
					expression = f.ExpandWith(interp, arg)
				case *BuiltInFunc:
					return f.EvalWith(interp, arg, env)
				default:
					panic(NewEvalError("not applicable", fn))
				}
			}
		case *Lambda:
			return &Closure{*x, env}
		default:
			return x // numbers, strings etc.
		}
	}
}

// SafeEval evaluates a Lisp expression in a given environment env and
// returns the result and nil.
// If an error happens, it returns Nil and the error
func (interp *Interp) SafeEval(expression interface{}, env *Cell) (
	result interface{}, err interface{}) {
	defer func() {
		if e := recover(); e != nil {
			result, err = Nil, e
		}
	}()
	return interp.Eval(expression, env), nil
}

// evalProgN evaluates E1, E2, .., E(n-1) and returns the tail expression En.
func (interp *Interp) evalProgN(j *Cell, env *Cell) interface{} {
	if j == Nil {
		return Nil
	}
	for {
		x := j.Car
		j = j.CdrCell()
		if j == Nil {
			return x // The tail expression will be evaluated at the caller.
		}
		interp.Eval(x, env)
	}
}

// futureTask is a task for goroutine to deliver the "promise" of Future.
// It returns the En value of (future E1 E2 .. En) via the channel and
// closes the channel.
func (interp *Interp) futureTask(j *Cell, env *Cell, ch chan<- Cell) {
	defer close(ch)
	result, err := interp.safeProgN(j, env)
	ch <- Cell{result, err}
}

// safeProgN evaluates E1, E2, .. En and returns the value of En and nil.
// If an error happens, it returns Nil and the error.
func (interp *Interp) safeProgN(j *Cell, env *Cell) (result interface{},
	err interface{}) {
	defer func() {
		if e := recover(); e != nil {
			result, err = Nil, e
		}
	}()
	x := interp.evalProgN(j, env)
	return interp.Eval(x, env), nil
}

// evalCond evaluates a conditional expression and returns the selection
// unevaluated.
func (interp *Interp) evalCond(j *Cell, env *Cell) interface{} {
	for j != Nil {
		clause, ok := j.Car.(*Cell)
		if ok {
			if clause != Nil {
				result := interp.Eval(clause.Car, env)
				if result != Nil { // If the condition holds...
					body := clause.CdrCell()
					if body == Nil {
						return QqQuote(result)
					} else {
						return interp.evalProgN(body, env)
					}
				}
			}
		} else {
			panic(NewEvalError("cond test expected", j.Car))
		}
		j = j.CdrCell()
	}
	return Nil // No clause holds.
}

// evalSeqQ evaluates each Ei of (setq .. Vi Ei ..) and assigns it to Vi
// repectively.  It returns the value of the last expression En.
func (interp *Interp) evalSetQ(j *Cell, env *Cell) interface{} {
	var result interface{} = Nil
	for j != Nil {
		lval := j.Car
		j = j.CdrCell()
		if j == Nil {
			panic(NewEvalError("right value expected", lval))
		}
		result = interp.Eval(j.Car, env)
		switch v := lval.(type) {
		case *Arg:
			v.SetValue(result, env)
		case *Sym:
			if v.IsKeyword {
				panic(NewNotVariableError(lval))
			}
			interp.SetGlobalVar(v, result)
		default:
			panic(NewNotVariableError(lval))
		}
		j = j.CdrCell()
	}
	return result
}

// compile compiles a Lisp list (macro ...) or (lambda ...).
func (interp *Interp) compile(arg *Cell, env *Cell,
	factory func(int, *Cell, *Cell) interface{}) interface{} {
	if arg == Nil {
		panic(NewEvalError("arglist and body expected", arg))
	}
	table := make(map[*Sym]*Arg)
	hasRest := makeArgTable(arg.Car, table)
	arity := len(table)
	body := arg.CdrCell()
	body = scanForArgs(body, table).(*Cell)
	body = interp.expandMacros(body, 20).(*Cell) // Expand up to 20 nestings.
	body = interp.compileInners(body).(*Cell)
	if hasRest {
		arity = -arity
	}
	return factory(arity, body, env)
}

// expandMacros expands macros and quasi-quotes in x up to count nestings.
func (interp *Interp) expandMacros(x interface{}, count int) interface{} {
	if count > 0 {
		if j, ok := x.(*Cell); ok {
			if j == Nil {
				return Nil
			}
			switch k := j.Car; k {
			case QuoteSym, LambdaSym, MacroSym:
				return j
			case QuasiquoteSym:
				d := j.CdrCell()
				if d != Nil && d.Cdr == Nil {
					z := QqExpand(d.Car)
					return interp.expandMacros(z, count)
				}
				panic(NewEvalError("bad quasiquote", j))
			default:
				if sym, ok := k.(*Sym); ok {
					if v, ok := interp.GetGlobalVar(sym); ok {
						k = v
					}
				}
				if f, ok := k.(*Macro); ok {
					d := j.CdrCell()
					z := f.ExpandWith(interp, d)
					return interp.expandMacros(z, count-1)
				} else {
					return j.MapCar(func(y interface{}) interface{} {
						return interp.expandMacros(y, count)
					})
				}
			}
		}
	}
	return x
}

// compileInners replaces inner lambda-expressions with Lambda instances.
func (interp *Interp) compileInners(x interface{}) interface{} {
	if j, ok := x.(*Cell); ok {
		if j == Nil {
			return Nil
		}
		switch k := j.Car; k {
		case QuoteSym:
			return j
		case LambdaSym:
			d := j.CdrCell()
			return interp.compile(d, Nil, NewLambda)
		case MacroSym:
			panic(NewEvalError("nested macro", j))
		default:
			return j.MapCar(func(y interface{}) interface{} {
				return interp.compileInners(y)
			})
		}
	}
	return x
}

//----------------------------------------------------------------------

// makeArgTable makes an argument-table.  It returns true if x has &rest.
func makeArgTable(x interface{}, table map[*Sym]*Arg) bool {
	arg, ok := x.(*Cell)
	if !ok {
		panic(NewEvalError("arglist expected", x))
	}
	if arg == Nil {
		return false
	} else {
		offset := 0 // offset value within the call-frame
		hasRest := false
		for arg != Nil {
			j := arg.Car
			if hasRest {
				panic(NewEvalError("2nd rest", j))
			}
			if j == RestSym { // &rest var
				arg = arg.CdrCell()
				if arg == Nil {
					panic(NewNotVariableError(arg))
				}
				j = arg.Car
				if j == RestSym {
					panic(NewNotVariableError(j))
				}
				hasRest = true
			}
			var sym *Sym
			switch v := j.(type) {
			case *Sym:
				sym = v
			case *Arg:
				sym = v.Symbol
			default:
				panic(NewNotVariableError(j))
			}
			if _, ok := table[sym]; ok {
				panic(NewEvalError("duplicated argument name", sym))
			}
			table[sym] = &Arg{0, offset, sym}
			offset++
			arg = arg.CdrCell()
		}
		return hasRest
	}
}

// scanForArgs scans x for formal arguments in table and replaces them
// with Args.
// Also it scans x for free Args not in table and promotes their levels.
func scanForArgs(x interface{}, table map[*Sym]*Arg) interface{} {
	switch j := x.(type) {
	case *Sym:
		if a, ok := table[j]; ok {
			return a
		}
		return j
	case *Arg:
		if a, ok := table[j.Symbol]; ok {
			return a
		}
		return &Arg{j.Level + 1, j.Offset, j.Symbol}
	case *Cell:
		if j == Nil {
			return Nil
		}
		switch j.Car {
		case QuoteSym:
			return j
		case QuasiquoteSym:
			return &Cell{QuasiquoteSym, scanForQQ(j.Cdr, table, 0)}
		default:
			return j.MapCar(func(y interface{}) interface{} {
				return scanForArgs(y, table)
			})
		}
	default:
		return j
	}
}

// scanForQQ scans x for quasi-quotes and executes scanForArgs on the
// nesting level of quotes.
func scanForQQ(x interface{}, table map[*Sym]*Arg,
	level int) interface{} {
	j, ok := x.(*Cell)
	if ok {
		if j == Nil {
			return Nil
		}
		switch k := j.Car; k {
		case QuasiquoteSym:
			return &Cell{k, scanForQQ(j.Cdr, table, level+1)}
		case UnquoteSym, UnquoteSplicingSym:
			var d interface{}
			if level == 0 {
				d = scanForArgs(j.Cdr, table)
			} else {
				d = scanForQQ(j.Cdr, table, level-1)
			}
			if d == j.Cdr {
				return j
			}
			return &Cell{k, d}
		default:
			return j.MapCar(func(y interface{}) interface{} {
				return scanForQQ(y, table, level)
			})
		}
	} else {
		return x
	}
}

//----------------------------------------------------------------------
// Quasi-Quotation

// QqExpand expands x of any quasi-quote `x into the equivalent S-expression.
func QqExpand(x interface{}) interface{} {
	return qqExpand0(x, 0) // Begin with the nesting level 0.
}

// QqQuote quotes x so that the result evaluates to x.
func QqQuote(x interface{}) interface{} {
	if x == Nil {
		return Nil
	}
	switch x.(type) {
	case *Sym, *Cell:
		return &Cell{QuoteSym, &Cell{x, Nil}}
	default:
		return x
	}
}

func qqExpand0(x interface{}, level int) interface{} {
	if j, ok := x.(*Cell); ok {
		if j == Nil {
			return Nil
		}
		if j.Car == UnquoteSym { // ,a
			if level == 0 {
				return j.CdrCell().Car // ,a => a
			}
		}
		t := qqExpand1(j, level)
		if t.Cdr == Nil {
			if k, ok := t.Car.(*Cell); ok {
				if k.Car == ListSym || k.Car == ConsSym {
					return k
				}
			}
		}
		return &Cell{AppendSym, t}
	} else {
		return QqQuote(x)
	}
}

// qqExpand1 expands x of `x so that the result can be used as an argument of
// append.  Example 1: (,a b) => ((list a 'b))
//          Example 2: (,a ,@(cons 2 3)) => ((cons a (cons 2 3)))
func qqExpand1(x interface{}, level int) *Cell {
	if j, ok := x.(*Cell); ok {
		if j == Nil {
			return &Cell{Nil, Nil}
		}
		switch j.Car {
		case UnquoteSym: // ,a
			if level == 0 {
				return j.CdrCell() // ,a => (a)
			}
			level--
		case QuasiquoteSym: // `a
			level++
		}
		h := qqExpand2(j.Car, level)
		t := qqExpand1(j.Cdr, level) // != Nil
		if t.Car == Nil && t.Cdr == Nil {
			return &Cell{h, Nil}
		} else if hc, ok := h.(*Cell); ok {
			if hc.Car == ListSym {
				if tcar, ok := t.Car.(*Cell); ok {
					if tcar.Car == ListSym {
						hh := qqConcat(hc, tcar.Cdr)
						return &Cell{hh, t.Cdr}
					}
				}
				if hcdr, ok := hc.Cdr.(*Cell); ok {
					hh := qqConsCons(hcdr, t.Car)
					return &Cell{hh, t.Cdr}
				}
			}
		}
		return &Cell{h, t}
	} else {
		return &Cell{QqQuote(x), Nil}
	}
}

// (1 2), (3 4) => (1 2 3 4)
func qqConcat(x *Cell, y interface{}) interface{} {
	if x == Nil {
		return y
	} else {
		return &Cell{x.Car, qqConcat(x.CdrCell(), y)}
	}
}

// (1 2 3), "a" => (cons 1 (cons 2 (cons 3 "a")))
func qqConsCons(x *Cell, y interface{}) interface{} {
	if x == Nil {
		return y
	} else {
		return &Cell{ConsSym, &Cell{x.Car,
			&Cell{qqConsCons(x.CdrCell(), y), Nil}}}
	}
}

// qqExpand2 expands x.car (= y) of `x so that the result can be used as an
// argument of append.
// Examples: ,a => (list a); ,@(foo 1 2) => (foo 1 2); b => (list 'b)
func qqExpand2(y interface{}, level int) interface{} {
	if j, ok := y.(*Cell); ok {
		if j == Nil {
			return &Cell{ListSym, &Cell{Nil, Nil}} // (list nil)
		}
		switch j.Car {
		case UnquoteSym: // ,a
			if level == 0 {
				return &Cell{ListSym, j.Cdr} // ,a => (list a)
			}
			level--
		case UnquoteSplicingSym: // ,@a
			if level == 0 {
				return j.CdrCell().Car // ,@a => a
			}
			level--
		case QuasiquoteSym: // `a
			level++
		}
	}
	return &Cell{ListSym, &Cell{qqExpand0(y, level), Nil}}
}

//----------------------------------------------------------------------

// Reader represents a reader of Lisp expressions.
type Reader struct {
	scanner *bufio.Scanner
	token   interface{} // the current token
	tokens  []string    // tokens read from the current line
	index   int         // the next index of tokens
	line    string      // the current line
	lineNo  int         // the current line number
	erred   bool        // a flag if an error has happened
}

// NewReader constructs a reader which will read Lisp expressions from r.
func NewReader(r io.Reader) *Reader {
	scanner := bufio.NewScanner(r)
	return &Reader{scanner, nil, nil, 0, "", 0, false}
}

// Read reads a Lisp expression and returns the expression and nil.
// If the input runs out, it returns EofToken and nil.
// If an error happens, it returns Nil and the error.
func (rr *Reader) Read() (result interface{}, err interface{}) {
	defer func() {
		if e := recover(); e != nil {
			result, err = Nil, e
		}
	}()
	rr.readToken()
	return rr.parseExpression(), nil
}

func (rr *Reader) newSynatxError(msg string, arg interface{}) *EvalError {
	rr.erred = true
	s := fmt.Sprintf("syntax error: %s -- %d: %s",
		fmt.Sprintf(msg, arg), rr.lineNo, rr.line)
	return &EvalError{s, nil}
}

func (rr *Reader) parseExpression() interface{} {
	switch rr.token {
	case LeftParenSym: // (a b c)
		rr.readToken()
		return rr.parseListBody()
	case SingleQuoteSym: // 'a => (quote a)
		rr.readToken()
		return &Cell{QuoteSym, &Cell{rr.parseExpression(), Nil}}
	case BackQuoteSym: // `a => (quasiquote a)
		rr.readToken()
		return &Cell{QuasiquoteSym, &Cell{rr.parseExpression(), Nil}}
	case CommaSym: // ,a => (unquote a)
		rr.readToken()
		return &Cell{UnquoteSym, &Cell{rr.parseExpression(), Nil}}
	case CommaAtSym: // ,@a => (unquote-splicing a)
		rr.readToken()
		return &Cell{UnquoteSplicingSym, &Cell{rr.parseExpression(), Nil}}
	case DotSym, RightParenSym:
		panic(rr.newSynatxError("unexpected \"%v\"", rr.token))
	default:
		return rr.token
	}
}

func (rr *Reader) parseListBody() *Cell {
	if rr.token == EofToken {
		panic(rr.newSynatxError("unexpected EOF%s", ""))
	} else if rr.token == RightParenSym {
		return Nil
	} else {
		e1 := rr.parseExpression()
		rr.readToken()
		var e2 interface{}
		if rr.token == DotSym { // (a . b)
			rr.readToken()
			e2 = rr.parseExpression()
			rr.readToken()
			if rr.token != RightParenSym {
				panic(rr.newSynatxError("\")\" expected: %v", rr.token))
			}
		} else {
			e2 = rr.parseListBody()
		}
		return &Cell{e1, e2}
	}
}

// readToken reads the next token and set it to rr.token.
func (rr *Reader) readToken() {
	// Read the next line if the line ends or an error happened last time.
	for len(rr.tokens) <= rr.index || rr.erred {
		rr.erred = false
		if rr.scanner.Scan() {
			rr.line = rr.scanner.Text()
			rr.lineNo++
		} else {
			if err := rr.scanner.Err(); err != nil {
				panic(err)
			}
			rr.token = EofToken
			return
		}
		mm := tokenPat.FindAllStringSubmatch(rr.line, -1)
		tt := make([]string, 0, len(mm)*3/5) // Estimate 40% will be spaces.
		for _, m := range mm {
			if m[1] != "" {
				tt = append(tt, m[1])
			}
		}
		rr.tokens = tt
		rr.index = 0
	}
	// Read the next token.
	s := rr.tokens[rr.index]
	rr.index++
	if s[0] == '"' {
		n := len(s) - 1
		if n < 1 || s[n] != '"' {
			panic(rr.newSynatxError("bad string: '%s'", s))
		}
		s = s[1:n]
		s = escapePat.ReplaceAllStringFunc(s, func(t string) string {
			r, ok := escapes[t] // r, err := strconv.Unquote("'" + t + "'")
			if !ok {
				r = t // Leave any invalid escape sequence as it is.
			}
			return r
		})
		rr.token = s
		return
	}
	f, err := strconv.ParseFloat(s, 64) // Try to read s as a float64.
	if err == nil {
		rr.token = f
		return
	}
	if s == "nil" {
		rr.token = Nil
		return
	} else if s == "t" {
		rr.token = true
		return
	}
	rr.token = NewSym(s)
	return
}

// tokenPat is a regular expression to split a line to Lisp tokens.
var tokenPat = regexp.MustCompile(`\s+|;.*$|("(\\.?|.)*?"|,@?|[^()'` +
	"`" + `~"; \t]+|.)`)

// escapePat is a reg. expression to take an escape sequence out of a string.
var escapePat = regexp.MustCompile(`\\(.)`)

// escapes is a mapping from an escape sequence to its string value.
var escapes = map[string]string{
	`\\`: `\`,
	`\"`: `"`,
	`\n`: "\n", `\r`: "\r", `\f`: "\f", `\b`: "\b", `\t`: "\t", `\v`: "\v",
}

//----------------------------------------------------------------------

// Str returns a textual representation of any Lisp expression x.
func Str(x interface{}) string {
	return Str2(x, true)
}

// Str2 returns a textual representation of any Lisp expression x.
// If quoteString is true, any strings in the expression are represented
// with enclosing quotes respectively.
func Str2(x interface{}, quoteString bool) string {
	return str4(x, quoteString, -1, nil)
}

// quotes is a mapping from a quote symbol to its string representation.
var quotes = map[*Sym]string{
	QuoteSym:           "'",
	QuasiquoteSym:      "`",
	UnquoteSym:         ",",
	UnquoteSplicingSym: ",@",
}

func str4(a interface{}, quoteString bool, count int,
	printed map[*Cell]bool) string {
	if a == true {
		return "t"
	}
	switch x := a.(type) {
	case *Cell:
		if x == Nil {
			return "nil"
		}
		if s, ok := x.Car.(*Sym); ok {
			if q, ok := quotes[s]; ok {
				if d, ok := x.Cdr.(*Cell); ok {
					if d.Cdr == Nil {
						return q + str4(d.Car, true, count, printed)
					}
				}
			}
		}
		return "(" + strListBody(x, count, printed) + ")"
	case string:
		if quoteString {
			return strconv.Quote(x)
		}
		return x
	case []interface{}:
		s := make([]string, len(x))
		for i, e := range x {
			s[i] = str4(e, true, count, printed)
		}
		return "[" + strings.Join(s, ", ") + "]"
	case *Sym:
		if x.IsInterned() {
			return x.Name
		}
		return "#:" + x.Name
	}
	return fmt.Sprintf("%v", a)
}

// strListBody makes a string representation of a list, omitting its parens.
func strListBody(x *Cell, count int, printed map[*Cell]bool) string {
	if printed == nil {
		printed = make(map[*Cell]bool)
	}
	if count < 0 {
		count = 4 // threshold of ellipsis for circular lists
	}
	s := make([]string, 0, 10)
	y := x
	for y != Nil {
		if _, ok := printed[y]; ok {
			count--
			if count < 0 {
				s = append(s, "...") // ellipsis for a circular list
				return strings.Join(s, " ")
			}
		} else {
			printed[y] = true
			count = 4
		}
		s = append(s, str4(y.Car, true, count, printed))
		if cdr, ok := y.Cdr.(*Cell); ok {
			y = cdr
		} else {
			s = append(s, ".")
			s = append(s, str4(y.Cdr, true, count, printed))
			break
		}
	}
	y = x
	for y != Nil {
		delete(printed, y)
		if cdr, ok := y.Cdr.(*Cell); ok {
			y = cdr
		} else {
			break
		}
	}
	return strings.Join(s, " ")
}

//----------------------------------------------------------------------

// Run executes REPL (Read-Eval-Print Loop).
// It returns false if REPL was ceased by an error.
// It returns true if REPL was finished normally.
func Run(interp *Interp, input io.Reader) bool {
	interactive := (input == nil)
	if interactive {
		input = os.Stdin
	}
	reader := NewReader(input)
	for {
		if interactive {
			os.Stdout.WriteString("> ")
		}
		x, err := reader.Read()
		if err == nil {
			if x == EofToken {
				return true // Finished normally.
			}
			x, err = interp.SafeEval(x, Nil)
			if err == nil {
				if interactive {
					fmt.Println(Str(x))
				}
			}
		}
		if err != nil {
			fmt.Println(err)
			if !interactive {
				return false // Ceased by an error.
			}
		}
	}
}

// Main runs each element of args as a name of Lisp script file.
// It ignores args[0].
// If it does not have args[1] or some element is "-", it begins REPL.
func Main(args []string) int {
	interp := NewInterp()
	ss := strings.NewReader(Prelude)
	if !Run(interp, ss) {
		return 1
	}
	if len(args) < 2 {
		args = []string{args[0], "-"}
	}
	for i, fileName := range args {
		if i == 0 {
			continue
		}
		if fileName == "-" {
			Run(interp, nil)
			fmt.Println("Goodbye")
		} else {
			file, err := os.Open(fileName)
			if err != nil {
				fmt.Println(err)
				return 1
			}
			if !Run(interp, file) {
				return 1
			}
		}
	}
	return 0
}

func main() {
	os.Exit(Main(os.Args))
}

// Prelude is an initialization script of Lisp.
// Each "~" is replaced by "`" at runtime.
var Prelude = strings.Replace(`
(setq defmacro
      (macro (name args &rest body)
             ~(progn (setq ,name (macro ,args ,@body))
                     ',name)))

(defmacro defun (name args &rest body)
  ~(progn (setq ,name (lambda ,args ,@body))
          ',name))

(defun caar (x) (car (car x)))
(defun cadr (x) (car (cdr x)))
(defun cdar (x) (cdr (car x)))
(defun cddr (x) (cdr (cdr x)))
(defun caaar (x) (car (car (car x))))
(defun caadr (x) (car (car (cdr x))))
(defun cadar (x) (car (cdr (car x))))
(defun caddr (x) (car (cdr (cdr x))))
(defun cdaar (x) (cdr (car (car x))))
(defun cdadr (x) (cdr (car (cdr x))))
(defun cddar (x) (cdr (cdr (car x))))
(defun cdddr (x) (cdr (cdr (cdr x))))
(defun not (x) (eq x nil))
(defun consp (x) (not (atom x)))
(defun print (x) (prin1 x) (terpri) x)
(defun identity (x) x)

(setq
 = eql
 null not
 setcar rplaca
 setcdr rplacd)

(defun > (x y) (< y x))
(defun >= (x y) (not (< x y)))
(defun <= (x y) (not (< y x)))
(defun /= (x y) (not (= x y)))

(defun equal (x y)
  (cond ((atom x) (eql x y))
        ((atom y) nil)
        ((equal (car x) (car y)) (equal (cdr x) (cdr y)))))

(defmacro if (test then &rest else)
  ~(cond (,test ,then)
         ,@(cond (else ~((t ,@else))))))

(defmacro when (test &rest body)
  ~(cond (,test ,@body)))

(defmacro let (args &rest body)
  ((lambda (vars vals)
     (defun vars (x)
       (cond (x (cons (if (atom (car x))
                          (car x)
                        (caar x))
                      (vars (cdr x))))))
     (defun vals (x)
       (cond (x (cons (if (atom (car x))
                          nil
                        (cadar x))
                      (vals (cdr x))))))
     ~((lambda ,(vars args) ,@body) ,@(vals args)))
   nil nil))

(defmacro letrec (args &rest body)      ; (letrec ((v e) ...) body...)
  (let (vars setqs)
    (defun vars (x)
      (cond (x (cons (caar x)
                     (vars (cdr x))))))
    (defun sets (x)
      (cond (x (cons ~(setq ,(caar x) ,(cadar x))
                     (sets (cdr x))))))
    ~(let ,(vars args) ,@(sets args) ,@body)))

(defun _append (x y)
  (if (null x)
      y
    (cons (car x) (_append (cdr x) y))))
(defmacro append (x &rest y)
  (if (null y)
      x
    ~(_append ,x (append ,@y))))

(defmacro and (x &rest y)
  (if (null y)
      x
    ~(cond (,x (and ,@y)))))

(defun mapcar (f x)
  (and x (cons (f (car x)) (mapcar f (cdr x)))))

(defmacro or (x &rest y)
  (if (null y)
      x
    ~(cond (,x)
           ((or ,@y)))))

(defun listp (x)
  (or (null x) (consp x)))    ; NB (listp (lambda (x) (+ x 1))) => nil

(defun memq (key x)
  (cond ((null x) nil)
        ((eq key (car x)) x)
        (t (memq key (cdr x)))))

(defun member (key x)
  (cond ((null x) nil)
        ((equal key (car x)) x)
        (t (member key (cdr x)))))

(defun assq (key alist)
  (cond (alist (let ((e (car alist)))
                 (if (and (consp e) (eq key (car e)))
                     e
                   (assq key (cdr alist)))))))

(defun assoc (key alist)
  (cond (alist (let ((e (car alist)))
                 (if (and (consp e) (equal key (car e)))
                     e
                   (assoc key (cdr alist)))))))

(defun _nreverse (x prev)
  (let ((next (cdr x)))
    (setcdr x prev)
    (if (null next)
        x
      (_nreverse next x))))
(defun nreverse (list)            ; (nreverse '(a b c d)) => (d c b a)
  (cond (list (_nreverse list nil))))

(defun last (list)
  (if (atom (cdr list))
      list
    (last (cdr list))))

(defun nconc (&rest lists)
  (if (null (cdr lists))
      (car lists)
    (if (null (car lists))
        (apply nconc (cdr lists))
      (setcdr (last (car lists))
              (apply nconc (cdr lists)))
      (car lists))))

(defmacro while (test &rest body)
  (let ((loop (gensym)))
    ~(letrec ((,loop (lambda () (cond (,test ,@body (,loop))))))
       (,loop))))

(defmacro dolist (spec &rest body) ; (dolist (name list [result]) body...)
  (let ((name (car spec))
        (list (gensym)))
    ~(let (,name
           (,list ,(cadr spec)))
       (while ,list
         (setq ,name (car ,list))
         ,@body
         (setq ,list (cdr ,list)))
       ,@(if (cddr spec)
             ~((setq ,name nil)
               ,(caddr spec))))))

(defmacro dotimes (spec &rest body) ; (dotimes (name count [result]) body...)
  (let ((name (car spec))
        (count (gensym)))
    ~(let ((,name 0)
           (,count ,(cadr spec)))
       (while (< ,name ,count)
         ,@body
         (setq ,name (+ ,name 1)))
       ,@(if (cddr spec)
             ~(,(caddr spec))))))
`, "~", "`", -1)

/*
  Copyright (c) 2015, 2016 OKI Software Co., Ltd.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/
