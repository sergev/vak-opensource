program CParser;

const
  MAX_TOKENS = 1000;

type
  TokenType = (
    tkIdentifier, tkNumber, tkString,
    tkInt, tkFloat, tkChar, tkVoid,
    tkPlus, tkMinus, tkStar, tkSlash,
    tkEqual, tkEqualEqual, tkBangEqual,
    tkLess, tkGreater, tkLessEqual, tkGreaterEqual,
    tkBang,
    tkLParen, tkRParen, tkLBrace, tkRBrace,
    tkSemicolon, tkComma,
    tkIf, tkElse, tkWhile, tkReturn
  );

  Token = record
    TokenType: TokenType;
    StrValue: string;
    NumValue: integer;
  end;

  TypeKind = (tpInt, tpFloat, tpChar, tpVoid);
  PType = ^TypeRec;
  TypeRec = record
    Kind: TypeKind;
  end;

  PExpr = ^Expr;
  ExprKind = (ekNumber, ekIdentifier, ekBinary, ekUnary, ekAssign);
  BinOp = (boAdd, boSub, boMul, boDiv, boEqual, boNotEqual,
           boLess, boGreater, boLessEqual, boGreaterEqual, boAssign);
  UnOp = (uoNeg, uoNot);
  Expr = record
    case Kind: ExprKind of
      ekNumber: (NumValue: integer);
      ekIdentifier: (IdValue: string);
      ekBinary: (Left, Right: PExpr; Op: BinOp);
      ekUnary: (Operand: PExpr; UnOp: UnOp);
      ekAssign: (Target, Value: PExpr);
  end;

  PStmt = ^Stmt;
  StmtKind = (skCompound, skExpr, skIf, skWhile, skReturn);
  Stmt = record
    Next: PStmt;
    case Kind: StmtKind of
      skCompound: (CompStmt: pointer);
      skExpr: (ExprStmt: PExpr);
      skIf: (Cond: PExpr; ThenStmt, ElseStmt: PStmt);
      skWhile: (WhileCond: PExpr; Body: PStmt);
      skReturn: (RetExpr: PExpr);
  end;

  PParameter = ^Parameter;
  Parameter = record
    ParamType: PType;
    Name: string;
    Next: PParameter;
  end;

  PDeclaration = ^Declaration;
  DeclarationKind = (dkVariable, dkFunction);
  Declaration = record
    Next: PDeclaration;
    case Kind: DeclarationKind of
      dkVariable: (VarType: PType; VarName: string; Init: PExpr);
      dkFunction: (FuncType: PType; FuncName: string; Params: PParameter; Body: pointer);
  end;

  PCompoundStmt = ^CompoundStmt;
  CompoundStmt = record
    Declarations: PDeclaration;
    Statements: PStmt;
  end;

  PTranslationUnit = ^TranslationUnit;
  TranslationUnit = record
    Declarations: PDeclaration;
  end;

  Parser = record
    Tokens: array[0..MAX_TOKENS-1] of Token;
    Position: integer;
    TokenCount: integer;
  end;

var
  GlobalParser: Parser;

// Utility functions
function NewType(Kind: TypeKind): PType;
begin
  New(NewType);
  NewType^.Kind := Kind;
end;

function NewExpr(Kind: ExprKind): PExpr;
begin
  New(NewExpr);
  NewExpr^.Kind := Kind;
end;

function NewStmt(Kind: StmtKind): PStmt;
begin
  New(NewStmt);
  NewStmt^.Kind := Kind;
  NewStmt^.Next := nil;
end;

function NewDeclaration(Kind: DeclarationKind): PDeclaration;
begin
  New(NewDeclaration);
  NewDeclaration^.Kind := Kind;
  NewDeclaration^.Next := nil;
end;

function NewCompoundStmt: PCompoundStmt;
begin
  New(NewCompoundStmt);
  NewCompoundStmt^.Declarations := nil;
  NewCompoundStmt^.Statements := nil;
end;

function NewParameter: PParameter;
begin
  New(NewParameter);
  NewParameter^.Next := nil;
end;

// Tokenizer
function Tokenize(Input: string): boolean;
var
  Pos: integer;
  Ch: char;
  TokenIdx: integer;

  function IsDigit(C: char): boolean;
  begin
    IsDigit := (C >= '0') and (C <= '9');
  end;

  function IsAlpha(C: char): boolean;
  begin
    IsAlpha := ((C >= 'a') and (C <= 'z')) or ((C >= 'A') and (C <= 'Z')) or (C = '_');
  end;

  function IsAlphaNumeric(C: char): boolean;
  begin
    IsAlphaNumeric := IsDigit(C) or IsAlpha(C);
  end;

begin
  TokenIdx := 0;
  Pos := 1;
  GlobalParser.TokenCount := 0;

  while (Pos <= Length(Input)) and (TokenIdx < MAX_TOKENS) do
  begin
    Ch := Input[Pos];
    case Ch of
      ' ', #10, #13, #9: Inc(Pos);
      '(':
      begin
        GlobalParser.Tokens[TokenIdx].TokenType := tkLParen;
        Inc(TokenIdx);
        Inc(Pos);
      end;
      ')':
      begin
        GlobalParser.Tokens[TokenIdx].TokenType := tkRParen;
        Inc(TokenIdx);
        Inc(Pos);
      end;
      '{':
      begin
        GlobalParser.Tokens[TokenIdx].TokenType := tkLBrace;
        Inc(TokenIdx);
        Inc(Pos);
      end;
      '}':
      begin
        GlobalParser.Tokens[TokenIdx].TokenType := tkRBrace;
        Inc(TokenIdx);
        Inc(Pos);
      end;
      ';':
      begin
        GlobalParser.Tokens[TokenIdx].TokenType := tkSemicolon;
        Inc(TokenIdx);
        Inc(Pos);
      end;
      ',':
      begin
        GlobalParser.Tokens[TokenIdx].TokenType := tkComma;
        Inc(TokenIdx);
        Inc(Pos);
      end;
      '+':
      begin
        GlobalParser.Tokens[TokenIdx].TokenType := tkPlus;
        Inc(TokenIdx);
        Inc(Pos);
      end;
      '-':
      begin
        GlobalParser.Tokens[TokenIdx].TokenType := tkMinus;
        Inc(TokenIdx);
        Inc(Pos);
      end;
      '*':
      begin
        GlobalParser.Tokens[TokenIdx].TokenType := tkStar;
        Inc(TokenIdx);
        Inc(Pos);
      end;
      '/':
      begin
        GlobalParser.Tokens[TokenIdx].TokenType := tkSlash;
        Inc(TokenIdx);
        Inc(Pos);
      end;
      '=':
      begin
        Inc(Pos);
        if (Pos <= Length(Input)) and (Input[Pos] = '=') then
        begin
          GlobalParser.Tokens[TokenIdx].TokenType := tkEqualEqual;
          Inc(Pos);
        end
        else
          GlobalParser.Tokens[TokenIdx].TokenType := tkEqual;
        Inc(TokenIdx);
      end;
      '!':
      begin
        Inc(Pos);
        if (Pos <= Length(Input)) and (Input[Pos] = '=') then
        begin
          GlobalParser.Tokens[TokenIdx].TokenType := tkBangEqual;
          Inc(Pos);
        end
        else
          GlobalParser.Tokens[TokenIdx].TokenType := tkBang;
        Inc(TokenIdx);
      end;
      '<':
      begin
        Inc(Pos);
        if (Pos <= Length(Input)) and (Input[Pos] = '=') then
        begin
          GlobalParser.Tokens[TokenIdx].TokenType := tkLessEqual;
          Inc(Pos);
        end
        else
          GlobalParser.Tokens[TokenIdx].TokenType := tkLess;
        Inc(TokenIdx);
      end;
      '>':
      begin
        Inc(Pos);
        if (Pos <= Length(Input)) and (Input[Pos] = '=') then
        begin
          GlobalParser.Tokens[TokenIdx].TokenType := tkGreaterEqual;
          Inc(Pos);
        end
        else
          GlobalParser.Tokens[TokenIdx].TokenType := tkGreater;
        Inc(TokenIdx);
      end;
      '0'..'9':
      begin
        GlobalParser.Tokens[TokenIdx].TokenType := tkNumber;
        GlobalParser.Tokens[TokenIdx].NumValue := Ord(Ch) - Ord('0');
        Inc(Pos);
        while (Pos <= Length(Input)) and IsDigit(Input[Pos]) do
        begin
          GlobalParser.Tokens[TokenIdx].NumValue := GlobalParser.Tokens[TokenIdx].NumValue * 10 + (Ord(Input[Pos]) - Ord('0'));
          Inc(Pos);
        end;
        Inc(TokenIdx);
      end;
      'a'..'z', 'A'..'Z', '_':
      begin
        GlobalParser.Tokens[TokenIdx].StrValue := Ch;
        Inc(Pos);
        while (Pos <= Length(Input)) and IsAlphaNumeric(Input[Pos]) do
        begin
          GlobalParser.Tokens[TokenIdx].StrValue := GlobalParser.Tokens[TokenIdx].StrValue + Input[Pos];
          Inc(Pos);
        end;
        if GlobalParser.Tokens[TokenIdx].StrValue = 'int' then
          GlobalParser.Tokens[TokenIdx].TokenType := tkInt
        else if GlobalParser.Tokens[TokenIdx].StrValue = 'float' then
          GlobalParser.Tokens[TokenIdx].TokenType := tkFloat
        else if GlobalParser.Tokens[TokenIdx].StrValue = 'char' then
          GlobalParser.Tokens[TokenIdx].TokenType := tkChar
        else if GlobalParser.Tokens[TokenIdx].StrValue = 'void' then
          GlobalParser.Tokens[TokenIdx].TokenType := tkVoid
        else if GlobalParser.Tokens[TokenIdx].StrValue = 'if' then
          GlobalParser.Tokens[TokenIdx].TokenType := tkIf
        else if GlobalParser.Tokens[TokenIdx].StrValue = 'else' then
          GlobalParser.Tokens[TokenIdx].TokenType := tkElse
        else if GlobalParser.Tokens[TokenIdx].StrValue = 'while' then
          GlobalParser.Tokens[TokenIdx].TokenType := tkWhile
        else if GlobalParser.Tokens[TokenIdx].StrValue = 'return' then
          GlobalParser.Tokens[TokenIdx].TokenType := tkReturn
        else
          GlobalParser.Tokens[TokenIdx].TokenType := tkIdentifier;
        Inc(TokenIdx);
      end;
      else
        Inc(Pos);
    end;
  end;

  GlobalParser.TokenCount := TokenIdx;
  Tokenize := TokenIdx < MAX_TOKENS;
end;

// Parser procedures
procedure InitParser;
begin
  GlobalParser.Position := 0;
end;

function Peek: Token;
begin
  if GlobalParser.Position < GlobalParser.TokenCount then
    Peek := GlobalParser.Tokens[GlobalParser.Position]
  else
    Peek.TokenType := tkSemicolon;
end;

procedure Advance;
begin
  if GlobalParser.Position < GlobalParser.TokenCount then
    Inc(GlobalParser.Position);
end;

function Expect(Tok: TokenType): boolean;
begin
  if Peek.TokenType = Tok then
  begin
    Advance;
    Expect := True;
  end
  else
    Expect := False;
end;

function ParseTranslationUnit: PTranslationUnit;
var
  TU: PTranslationUnit;
  Decl, LastDecl: PDeclaration;
begin
  New(TU);
  TU^.Declarations := nil;
  LastDecl := nil;

  while GlobalParser.Position < GlobalParser.TokenCount do
  begin
    Decl := ParseDeclaration;
    if Decl = nil then
    begin
      Writeln('Error parsing declaration');
      Exit(nil);
    end;
    if TU^.Declarations = nil then
      TU^.Declarations := Decl
    else
      LastDecl^.Next := Decl;
    LastDecl := Decl;
  end;
  ParseTranslationUnit := TU;
end;

function ParseDeclaration: PDeclaration;
var
  Typ: PType;
  Ident: string;
  Decl: PDeclaration;
begin
  Typ := ParseType;
  if Typ = nil then Exit(nil);

  if Peek.TokenType = tkIdentifier then
  begin
    Ident := Peek.StrValue;
    Advance;
  end
  else
  begin
    Writeln('Error: Expected identifier');
    Exit(nil);
  end;

  Decl := NewDeclaration(dkVariable);
  Decl^.VarType := Typ;
  Decl^.VarName := Ident;
  Decl^.Init := nil;

  case Peek.TokenType of
    tkLParen:
    begin
      Advance;
      Decl^.Kind := dkFunction;
      Decl^.FuncType := Typ;
      Decl^.FuncName := Ident;
      Decl^.Params := ParseParameterList;
      if not Expect(tkRParen) then
      begin
        Writeln('Error: Expected )');
        Exit(nil);
      end;
      Decl^.Body := ParseCompoundStmt;
    end;
    tkEqual:
    begin
      Advance;
      Decl^.Init := ParseExpr;
      if not Expect(tkSemicolon) then
      begin
        Writeln('Error: Expected ; after initializer');
        Exit(nil);
      end;
    end;
    tkSemicolon:
      Advance;
    else
    begin
      Writeln('Error: Expected (, =, or ;');
      Exit(nil);
    end;
  end;
  ParseDeclaration := Decl;
end;

function ParseType: PType;
begin
  case Peek.TokenType of
    tkInt: begin Advance; ParseType := NewType(tpInt); end;
    tkFloat: begin Advance; ParseType := NewType(tpFloat); end;
    tkChar: begin Advance; ParseType := NewType(tpChar); end;
    tkVoid: begin Advance; ParseType := NewType(tpVoid); end;
    else
    begin
      Writeln('Error: Expected type');
      ParseType := nil;
    end;
  end;
end;

function ParseParameterList: PParameter;
var
  Param, LastParam: PParameter;
begin
  ParseParameterList := nil;
  if Peek.TokenType <> tkRParen then
  begin
    Param := NewParameter;
    Param^.ParamType := ParseType;
    if Peek.TokenType = tkIdentifier then
    begin
      Param^.Name := Peek.StrValue;
      Advance;
    end
    else
    begin
      Writeln('Error: Expected parameter name');
      Exit(nil);
    end;
    ParseParameterList := Param;
    LastParam := Param;

    while Peek.TokenType = tkComma do
    begin
     Bund
      Param := NewParameter;
      Param^.ParamType := ParseType;
      if Peek.TokenType = tkIdentifier then
      begin
        Param^.Name := Peek.StrValue;
        Advance;
      end
      else
      begin
        Writeln('Error: Expected parameter name');
        Exit(nil);
      end;
      LastParam^.Next := Param;
      LastParam := Param;
    end;
  end;
end;

function ParseStmt: PStmt;
begin
  case Peek.TokenType of
    tkLBrace: ParseStmt := NewStmt(skCompound);
    tkIf: ParseStmt := ParseIfStmt;
    tkWhile: ParseStmt := ParseWhileStmt;
    tkReturn: ParseStmt := ParseReturnStmt;
    else ParseStmt := NewStmt(skExpr);
  end;
  if ParseStmt^.Kind = skCompound then
    ParseStmt^.CompStmt := ParseCompoundStmt
  else if ParseStmt^.Kind = skExpr then
    ParseStmt^.ExprStmt := ParseExprStmt;
end;

function ParseCompoundStmt: PCompoundStmt;
var
  Comp: PCompoundStmt;
  Decl, LastDecl: PDeclaration;
  Stmt, LastStmt: PStmt;
begin
  if not Expect(tkLBrace) then Exit(nil);
  Comp := NewCompoundStmt;

  LastDecl := nil;
  LastStmt := nil;
  while Peek.TokenType <> tkRBrace do
  begin
    if (Peek.TokenType in [tkInt, tkFloat, tkChar, tkVoid]) then
    begin
      Decl := ParseDeclaration;
      if Decl = nil then Exit(nil);
      if Comp^.Declarations = nil then
        Comp^.Declarations := Decl
      else
        LastDecl^.Next := Decl;
      LastDecl := Decl;
    end
    else
    begin
      Stmt := ParseStmt;
      if Stmt = nil then Exit(nil);
      if Comp^.Statements = nil then
        Comp^.Statements := Stmt
      else
        LastStmt^.Next := Stmt;
      LastStmt := Stmt;
    end;
  end;
  if not Expect(tkRBrace) then Exit(nil);
  ParseCompoundStmt := Comp;
end;

function ParseIfStmt: PStmt;
var
  Stmt: PStmt;
begin
  Advance;
  if not Expect(tkLParen) then Exit(nil);
  Stmt := NewStmt(skIf);
  Stmt^.Cond := ParseExpr;
  if not Expect(tkRParen) then Exit(nil);
  Stmt^.ThenStmt := ParseStmt;
  if Peek.TokenType = tkElse then
  begin
    Advance;
    Stmt^.ElseStmt := ParseStmt;
  end;
  ParseIfStmt := Stmt;
end;

function ParseWhileStmt: PStmt;
var
  Stmt: PStmt;
begin
  Advance;
  if not Expect(tkLParen) then Exit(nil);
  Stmt := NewStmt(skWhile);
  Stmt^.WhileCond := ParseExpr;
  if not Expect(tkRParen) then Exit(nil);
  Stmt^.Body := ParseStmt;
  ParseWhileStmt := Stmt;
end;

function ParseReturnStmt: PStmt;
var
  Stmt: PStmt;
begin
  Advance;
  Stmt := NewStmt(skReturn);
  if Peek.TokenType <> tkSemicolon then
    Stmt^.RetExpr := ParseExpr;
  if not Expect(tkSemicolon) then Exit(nil);
  ParseReturnStmt := Stmt;
end;

function ParseExprStmt: PExpr;
var
  Expr: PExpr;
begin
  Expr := ParseExpr;
  if not Expect(tkSemicolon) then Exit(nil);
  ParseExprStmt := Expr;
end;

function ParseExpr: PExpr;
begin
  ParseExpr := ParseAssignmentExpr;
end;

function ParseAssignmentExpr: PExpr;
var
  Left, Right: PExpr;
begin
  Left := ParseEqualityExpr;
  if Peek.TokenType = tkEqual then
  begin
    Advance;
    Right := ParseAssignmentExpr;
    ParseAssignmentExpr := NewExpr(ekAssign);
    ParseAssignmentExpr^.Target := Left;
    ParseAssignmentExpr^.Value := Right;
  end
  else
    ParseAssignmentExpr := Left;
end;

function ParseEqualityExpr: PExpr;
var
  Left, Right: PExpr;
begin
  Left := ParseRelationalExpr;
  while Peek.TokenType in [tkEqualEqual, tkBangEqual] do
  begin
    ParseEqualityExpr := NewExpr(ekBinary);
    ParseEqualityExpr^.Left := Left;
    if Peek.TokenType = tkEqualEqual then
      ParseEqualityExpr^.Op := boEqual
    else
      ParseEqualityExpr^.Op := boNotEqual;
    Advance;
    Right := ParseRelationalExpr;
    ParseEqualityExpr^.Right := Right;
    Left := ParseEqualityExpr;
  end;
  ParseEqualityExpr := Left;
end;

function ParseRelationalExpr: PExpr;
var
  Left, Right: PExpr;
begin
  Left := ParseAddExpr;
  while Peek.TokenType in [tkLess, tkGreater, tkLessEqual, tkGreaterEqual] do
  begin
    ParseRelationalExpr := NewExpr(ekBinary);
    ParseRelationalExpr^.Left := Left;
    case Peek.TokenType of
      tkLess: ParseRelationalExpr^.Op := boLess;
      tkGreater: ParseRelationalExpr^.Op := boGreater;
      tkLessEqual: ParseRelationalExpr^.Op := boLessEqual;
      tkGreaterEqual: ParseRelationalExpr^.Op := boGreaterEqual;
    end;
    Advance;
    Right := ParseAddExpr;
    ParseRelationalExpr^.Right := Right;
    Left := ParseRelationalExpr;
  end;
  ParseRelationalExpr := Left;
end;

function ParseAddExpr: PExpr;
var
  Left, Right: PExpr;
begin
  Left := ParseMultExpr;
  while Peek.TokenType in [tkPlus, tkMinus] do
  begin
    ParseAddExpr := NewExpr(ekBinary);
    ParseAddExpr^.Left := Left;
    if Peek.TokenType = tkPlus then
      ParseAddExpr^.Op := boAdd
    else
      ParseAddExpr^.Op := boSub;
    Advance;
    Right := ParseMultExpr;
    ParseAddExpr^.Right := Right;
    Left := ParseAddExpr;
  end;
  ParseAddExpr := Left;
end;

function ParseMultExpr: PExpr;
var
  Left, Right: PExpr;
begin
  Left := ParseUnaryExpr;
  while Peek.TokenType in [tkStar, tkSlash] do
  begin
    ParseMultExpr := NewExpr(ekBinary);
    ParseMultExpr^.Left := Left;
    if Peek.TokenType = tkStar then
      ParseMultExpr^.Op := boMul
    else
      ParseMultExpr^.Op := boDiv;
    Advance;
    Right := ParseUnaryExpr;
    ParseMultExpr^.Right := Right;
    Left := ParseMultExpr;
  end;
  ParseMultExpr := Left;
end;

function ParseUnaryExpr: PExpr;
begin
  if Peek.TokenType in [tkMinus, tkBang] then
  begin
    ParseUnaryExpr := NewExpr(ekUnary);
    if Peek.TokenType = tkMinus then
      ParseUnaryExpr^.UnOp := uoNeg
    else
      ParseUnaryExpr^.UnOp := uoNot;
    Advance;
    ParseUnaryExpr^.Operand := ParseUnaryExpr;
  end
  else
    ParseUnaryExpr := ParsePrimaryExpr;
end;

function ParsePrimaryExpr: PExpr;
begin
  case Peek.TokenType of
    tkIdentifier:
    begin
      ParsePrimaryExpr := NewExpr(ekIdentifier);
      ParsePrimaryExpr^.IdValue := Peek.StrValue;
      Advance;
    end;
    tkNumber:
    begin
      ParsePrimaryExpr := NewExpr(ekNumber);
      ParsePrimaryExpr^.NumValue := Peek.NumValue;
      Advance;
    end;
    tkLParen:
    begin
      Advance;
      ParsePrimaryExpr := ParseExpr;
      if not Expect(tkRParen) then Exit(nil);
    end;
    else
    begin
      Writeln('Error: Expected identifier, number, or (');
      ParsePrimaryExpr := nil;
    end;
  end;
end;

// AST Printing
procedure PrintAST(TU: PTranslationUnit);
  procedure PrintIndent(Level: integer);
  var
    i: integer;
  begin
    for i := 1 to Level * 2 do Write(' ');
  end;

  function TypeToString(Typ: PType): string;
  begin
    if Typ = nil then Exit('nil');
    case Typ^.Kind of
      tpInt: TypeToString := 'int';
      tpFloat: TypeToString := 'float';
      tpChar: TypeToString := 'char';
      tpVoid: TypeToString := 'void';
    end;
      end;

  procedure PrintExpr(Expr: PExpr; Level: integer);
  begin
    if Expr = nil then
    begin
      PrintIndent(Level);
      Writeln('nil');
      Exit;
    end;
    PrintIndent(Level);
    case Expr^.Kind of
      ekNumber: Writeln('Number: ', Expr^.NumValue);
      ekIdentifier: Writeln('Identifier: ', Expr^.IdValue);
      ekBinary:
      begin
        case Expr^.Op of
          boAdd: Writeln('Binary: +');
          boSub: Writeln('Binary: -');
          boMul: Writeln('Binary: *');
          boDiv: Writeln('Binary: /');
          boEqual: Writeln('Binary: ==');
          boNotEqual: Writeln('Binary: !=');
          boLess: Writeln('Binary: <');
          boGreater: Writeln('Binary: >');
          boLessEqual: Writeln('Binary: <=');
          boGreaterEqual: Writeln('Binary: >=');
          boAssign: Writeln('Binary: =');
        end;
        PrintExpr(Expr^.Left, Level + 1);
        PrintExpr(Expr^.Right, Level + 1);
      end;
      ekUnary:
      begin
        if Expr^.UnOp = uoNeg then
          Writeln('Unary: -')
        else
          Writeln('Unary: !');
        PrintExpr(Expr^.Operand, Level + 1);
      end;
      ekAssign:
      begin
        Writeln('Assign');
        PrintExpr(Expr^.Target, Level + 1);
        PrintExpr(Expr^.Value, Level + 1);
      end;
    end;
  end;

  procedure PrintStmt(Stmt: PStmt; Level: integer);
  var
    CurrStmt: PStmt;
  begin
    CurrStmt := Stmt;
    while CurrStmt <> nil do
    begin
      PrintIndent(Level);
      case CurrStmt^.Kind of
        skCompound:
        begin
          Writeln('Compound');
          PrintCompoundStmt(PCompoundStmt(CurrStmt^.CompStmt), Level + 1);
        end;
        skExpr:
        begin
          Writeln('ExprStmt');
          PrintExpr(CurrStmt^.ExprStmt, Level + 1);
        end;
        skIf:
        begin
          Writeln('If');
          PrintIndent(Level + 1); Writeln('Condition:');
          PrintExpr(CurrStmt^.Cond, Level + 2);
          PrintIndent(Level + 1); Writeln('Then:');
          PrintStmt(CurrStmt^.ThenStmt, Level + 2);
          if CurrStmt^.ElseStmt <> nil then
          begin
            PrintIndent(Level + 1); Writeln('Else:');
            PrintStmt(CurrStmt^.ElseStmt, Level + 2);
          end;
        end;
        skWhile:
        begin
          Writeln('While');
          PrintIndent(Level + 1); Writeln('Condition:');
          PrintExpr(CurrStmt^.WhileCond, Level + 2);
          PrintIndent(Level + 1); Writeln('Body:');
          PrintStmt(CurrStmt^.Body, Level + 2);
        end;
        skReturn:
        begin
          Writeln('Return');
          if CurrStmt^.RetExpr <> nil then
            PrintExpr(CurrStmt^.RetExpr, Level + 1);
        end;
      end;
      CurrStmt := CurrStmt^.Next;
    end;
  end;

  procedure PrintCompoundStmt(Comp: PCompoundStmt; Level: integer);
  begin
    if Comp = nil then
    begin
      PrintIndent(Level);
      Writeln('nil');
      Exit;
    end;
    PrintIndent(Level); Writeln('Declarations:');
    PrintDeclarations(Comp^.Declarations, Level + 1);
    PrintIndent(Level); Writeln('Statements:');
    PrintStmt(Comp^.Statements, Level + 1);
  end;

  procedure PrintParameters(Params: PParameter; Level: integer);
  var
    CurrParam: PParameter;
  begin
    CurrParam := Params;
    while CurrParam <> nil do
    begin
      PrintIndent(Level);
      Writeln('Parameter: ', CurrParam^.Name, ' : ', TypeToString(CurrParam^.ParamType));
      CurrParam := CurrParam^.Next;
    end;
  end;

  procedure PrintDeclarations(Decl: PDeclaration; Level: integer);
  var
    CurrDecl: PDeclaration;
  begin
    CurrDecl := Decl;
    while CurrDecl <> nil do
    begin
      PrintIndent(Level);
      case CurrDecl^.Kind of
        dkVariable:
        begin
          Writeln('Variable: ', CurrDecl^.VarName, ' : ', TypeToString(CurrDecl^.VarType));
          if CurrDecl^.Init <> nil then
          begin
            PrintIndent(Level + 1); Writeln('Initializer:');
            PrintExpr(CurrDecl^.Init, Level + 2);
          end;
        end;
        dkFunction:
        begin
          Writeln('Function: ', CurrDecl^.FuncName, ' : ', TypeToString(CurrDecl^.FuncType));
          PrintIndent(Level + 1); Writeln('Parameters:');
          PrintParameters(CurrDecl^.Params, Level + 2);
          PrintIndent(Level + 1); Writeln('Body:');
          PrintCompoundStmt(PCompoundStmt(CurrDecl^.Body), Level + 2);
        end;
      end;
      CurrDecl := CurrDecl^.Next;
    end;
  end;

begin
  if TU = nil then
  begin
    Writeln('nil');
    Exit;
  end;
  Writeln('TranslationUnit');
  PrintDeclarations(TU^.Declarations, 1);
end;

// Main program
var
  Input: string;
  TU: PTranslationUnit;
begin
  Input := 'int x = 42;' + #10 +
           'int main(int argc) {' + #10 +
           '    int y = 3 + 5 * 2;' + #10 +
           '    if (y > x) {' + #10 +
           '        return 1;' + #10 +
           '    } else {' + #10 +
           '        return 0;' + #10 +
           '    }' + #10 +
           '}';

  if not Tokenize(Input) then
  begin
    Writeln('Error: Too many tokens, increase MAX_TOKENS');
    Halt(1);
  end;

  InitParser;
  TU := ParseTranslationUnit;
  if TU = nil then
    Writeln('Parsing failed')
  else
  begin
    Writeln('Parsing succeeded');
    PrintAST(TU);
  end;

  // Memory cleanup would be needed here in a real program
end.
