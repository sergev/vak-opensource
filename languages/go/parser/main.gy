%{
package main

import (
    "fmt"
    "log"
)
%}

%union{
    tok int
    val interface{}
    pair struct{key, val interface{}}
    pairs map[interface{}]interface{}
}

%token KEY
%token VAL

%type <val> KEY VAL
%type <pair> pair
%type <pairs> pairs

%%

goal:
    '{' pairs '}'
    {
        yylex.(*lex).m = $2
    }

pairs:
    pair
    {
        $$ = map[interface{}]interface{}{$1.key: $1.val}
    }
|   pairs '|' pair
    {
        $$[$3.key] = $3.val
    }

pair:
    KEY '=' VAL
    {
        $$.key, $$.val = $1, $3
    }
|   KEY '=' '{' pairs '}'
    {
        $$.key, $$.val = $1, $4
    }


%%

type token struct {
    tok int
    val interface{}
}

type lex struct {
    tokens []token
    m map[interface{}]interface{}
}

func (l *lex) Lex(lval *yySymType) int {
    if len(l.tokens) == 0 {
        return 0
    }

    v := l.tokens[0]
    l.tokens = l.tokens[1:]
    lval.val = v.val
    return v.tok
}

func (l *lex) Error(e string) {
    log.Fatal(e)
}

func main() {
    l := &lex{
        // {key1 = value1 | key2 = {key3 = value3} | key4 = {key5 = { key6 = value6 }}}
        []token{
            {'{', ""},
            {KEY, "key1"},
            {'=', ""},
            {VAL, "value1"},
            {'|', ""},
            {KEY, "key2"},
            {'=', ""},
            {'{', ""},
            {KEY, "key3"},
            {'=', ""},
            {VAL, "value3"},
            {'}', ""},
            {'|', ""},
            {KEY, "key4"},
            {'=', ""},
            {'{', ""},
            {KEY, "key5"},
            {'=', ""},
            {'{', ""},
            {KEY, "key6"},
            {'=', ""},
            {VAL, "value6"},
            {'}', ""},
            {'}', ""},
            {'}', ""},
        },
        map[interface{}]interface{}{},
    }
    yyParse(l)
    fmt.Println(l.m)
}
