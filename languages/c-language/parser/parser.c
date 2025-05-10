#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "c-scanner.h"

/* Global lexer state */
static int current_token;
static char *current_lexeme;

/* Error handling */
void parse_error(const char *message)
{
    fprintf(stderr, "Parse error: %s (token: %d, lexeme: %s)\n", message, current_token,
            current_lexeme ? current_lexeme : "");
    exit(1);
}

/* Token handling */
void next_token()
{
    current_token  = yylex();
    current_lexeme = get_yytext();
}

void expect_token(int expected)
{
    if (current_token != expected) {
        char msg[100];
        snprintf(msg, sizeof(msg), "Expected token %d, got %d", expected, current_token);
        parse_error(msg);
    }
    next_token();
}

/* Helper functions for AST construction */
Type *new_type(TypeKind kind)
{
    Type *t                = malloc(sizeof(Type));
    t->kind                = kind;
    t->qualifiers          = NULL;
    t->u.char_t.signedness = SIGNED_SIGNED; /* Default */
    return t;
}

TypeQualifier *new_type_qualifier(TypeQualifierKind kind)
{
    TypeQualifier *q = malloc(sizeof(TypeQualifier));
    q->kind          = kind;
    q->next          = NULL;
    return q;
}

Field *new_field(Ident name, Type *type, Expr *bitfield, bool is_anonymous)
{
    Field *f        = malloc(sizeof(Field));
    f->is_anonymous = is_anonymous;
    if (is_anonymous) {
        f->u.anonymous.type = type;
    } else {
        f->u.named.name     = name;
        f->u.named.type     = type;
        f->u.named.bitfield = bitfield;
    }
    f->next = NULL;
    return f;
}

Enumerator *new_enumerator(Ident name, Expr *value)
{
    Enumerator *e = malloc(sizeof(Enumerator));
    e->name       = name;
    e->value      = value;
    e->next       = NULL;
    return e;
}

Param *new_param(Ident name, Type *type)
{
    Param *p = malloc(sizeof(Param));
    p->name  = name;
    p->type  = type;
    p->next  = NULL;
    return p;
}

ParamList *new_param_list(bool is_empty, bool is_ident_list)
{
    ParamList *pl     = malloc(sizeof(ParamList));
    pl->is_empty      = is_empty;
    pl->is_ident_list = is_ident_list;
    if (is_empty) {
        pl->u.params = NULL;
    } else if (is_ident_list) {
        pl->u.idents = NULL;
    } else {
        pl->u.params = NULL;
    }
    return pl;
}

Declaration *new_declaration(DeclarationKind kind)
{
    Declaration *d = malloc(sizeof(Declaration));
    d->kind        = kind;
    d->next        = NULL;
    return d;
}

DeclSpec *new_decl_spec()
{
    DeclSpec *ds   = malloc(sizeof(DeclSpec));
    ds->storage    = NULL;
    ds->qualifiers = NULL;
    ds->type_specs = NULL;
    ds->func_specs = NULL;
    ds->align_spec = NULL;
    return ds;
}

StorageClass *new_storage_class(StorageClassKind kind)
{
    StorageClass *sc = malloc(sizeof(StorageClass));
    sc->kind         = kind;
    return sc;
}

TypeSpec *new_type_spec(TypeSpecKind kind)
{
    TypeSpec *ts = malloc(sizeof(TypeSpec));
    ts->kind     = kind;
    ts->next     = NULL;
    return ts;
}

FunctionSpec *new_function_spec(FunctionSpecKind kind)
{
    FunctionSpec *fs = malloc(sizeof(FunctionSpec));
    fs->kind         = kind;
    fs->next         = NULL;
    return fs;
}

AlignmentSpec *new_alignment_spec(AlignmentSpecKind kind)
{
    AlignmentSpec *as = malloc(sizeof(AlignmentSpec));
    as->kind          = kind;
    return as;
}

InitDeclarator *new_init_declarator(Declarator *declarator, Initializer *init)
{
    InitDeclarator *id = malloc(sizeof(InitDeclarator));
    id->declarator     = declarator;
    id->init           = init;
    id->next           = NULL;
    return id;
}

Declarator *new_declarator(DeclaratorKind kind)
{
    Declarator *d = malloc(sizeof(Declarator));
    d->kind       = kind;
    if (kind == DECLARATOR_NAMED) {
        d->u.named.name     = NULL;
        d->u.named.pointers = NULL;
        d->u.named.suffixes = NULL;
    } else {
        d->u.abstract.pointers = NULL;
        d->u.abstract.suffixes = NULL;
    }
    return d;
}

Pointer *new_pointer(TypeQualifier *qualifiers)
{
    Pointer *p    = malloc(sizeof(Pointer));
    p->qualifiers = qualifiers;
    p->next       = NULL;
    return p;
}

DeclaratorSuffix *new_declarator_suffix(DeclaratorSuffixKind kind)
{
    DeclaratorSuffix *ds = malloc(sizeof(DeclaratorSuffix));
    ds->kind             = kind;
    ds->next             = NULL;
    return ds;
}

Initializer *new_initializer(InitializerKind kind)
{
    Initializer *i = malloc(sizeof(Initializer));
    i->kind        = kind;
    return i;
}

InitItem *new_init_item(Designator *designators, Initializer *init)
{
    InitItem *ii    = malloc(sizeof(InitItem));
    ii->designators = designators;
    ii->init        = init;
    ii->next        = NULL;
    return ii;
}

Designator *new_designator(DesignatorKind kind)
{
    Designator *d = malloc(sizeof(Designator));
    d->kind       = kind;
    d->next       = NULL;
    return d;
}

Expr *new_expr(ExprKind kind)
{
    Expr *e = malloc(sizeof(Expr));
    e->kind = kind;
    e->type = NULL;
    e->next = NULL;
    return e;
}

Literal *new_literal(LiteralKind kind)
{
    Literal *l = malloc(sizeof(Literal));
    l->kind    = kind;
    return l;
}

UnaryOp *new_unary_op(UnaryOpKind kind)
{
    UnaryOp *op = malloc(sizeof(UnaryOp));
    op->kind    = kind;
    return op;
}

BinaryOp *new_binary_op(BinaryOpKind kind)
{
    BinaryOp *op = malloc(sizeof(BinaryOp));
    op->kind     = kind;
    return op;
}

AssignOp *new_assign_op(AssignOpKind kind)
{
    AssignOp *op = malloc(sizeof(AssignOp));
    op->kind     = kind;
    return op;
}

GenericAssoc *new_generic_assoc(GenericAssocKind kind)
{
    GenericAssoc *ga = malloc(sizeof(GenericAssoc));
    ga->kind         = kind;
    ga->next         = NULL;
    return ga;
}

Stmt *new_stmt(StmtKind kind)
{
    Stmt *s = malloc(sizeof(Stmt));
    s->kind = kind;
    return s;
}

DeclOrStmt *new_decl_or_stmt(DeclOrStmtKind kind)
{
    DeclOrStmt *ds = malloc(sizeof(DeclOrStmt));
    ds->kind       = kind;
    ds->next       = NULL;
    return ds;
}

ForInit *new_for_init(ForInitKind kind)
{
    ForInit *fi = malloc(sizeof(ForInit));
    fi->kind    = kind;
    return fi;
}

ExternalDecl *new_external_decl(ExternalDeclKind kind)
{
    ExternalDecl *ed = malloc(sizeof(ExternalDecl));
    ed->kind         = kind;
    ed->next         = NULL;
    return ed;
}

Program *new_program()
{
    Program *p = malloc(sizeof(Program));
    p->decls   = NULL;
    return p;
}

/* Append to linked list */
void append_list(void *head_ptr, void *node)
{
    if (!node)
        return;
    void **head = (void **)head_ptr;
    if (*head == NULL) {
        *head = node;
    } else {
        void *current = *head;
        while (*(void **)((char *)current + sizeof(void *) - sizeof(void *))) {
            current = *(void **)((char *)current + sizeof(void *) - sizeof(void *));
        }
        *(void **)((char *)current + sizeof(void *) - sizeof(void *)) = node;
    }
}

/* Parser functions */
Expr *parse_primary_expression();
Expr *parse_constant();
Expr *parse_string();
Expr *parse_generic_selection();
GenericAssoc *parse_generic_assoc_list();
GenericAssoc *parse_generic_association();
Expr *parse_postfix_expression();
Expr *parse_argument_expression_list();
Expr *parse_unary_expression();
UnaryOp *parse_unary_operator();
Expr *parse_cast_expression();
Expr *parse_multiplicative_expression();
Expr *parse_additive_expression();
Expr *parse_shift_expression();
Expr *parse_relational_expression();
Expr *parse_equality_expression();
Expr *parse_and_expression();
Expr *parse_exclusive_or_expression();
Expr *parse_inclusive_or_expression();
Expr *parse_logical_and_expression();
Expr *parse_logical_or_expression();
Expr *parse_conditional_expression();
Expr *parse_assignment_expression();
AssignOp *parse_assignment_operator();
Expr *parse_expression();
Expr *parse_constant_expression();
Declaration *parse_declaration();
DeclSpec *parse_declaration_specifiers();
InitDeclarator *parse_init_declarator_list();
InitDeclarator *parse_init_declarator();
StorageClass *parse_storage_class_specifier();
TypeSpec *parse_type_specifier();
Type *parse_struct_or_union_specifier();
int parse_struct_or_union();
Field *parse_struct_declaration_list();
Field *parse_struct_declaration();
TypeSpec *parse_specifier_qualifier_list();
Declarator *parse_struct_declarator_list();
Declarator *parse_struct_declarator();
Type *parse_enum_specifier();
Enumerator *parse_enumerator_list();
Enumerator *parse_enumerator();
Type *parse_atomic_type_specifier();
TypeQualifier *parse_type_qualifier();
FunctionSpec *parse_function_specifier();
AlignmentSpec *parse_alignment_specifier();
Declarator *parse_declarator();
Declarator *parse_direct_declarator();
Pointer *parse_pointer();
TypeQualifier *parse_type_qualifier_list();
ParamList *parse_parameter_type_list();
Param *parse_parameter_list();
Param *parse_parameter_declaration();
Ident parse_identifier_list();
Type *parse_type_name();
Declarator *parse_abstract_declarator();
Declarator *parse_direct_abstract_declarator();
Initializer *parse_initializer();
InitItem *parse_initializer_list();
Designator *parse_designation();
Designator *parse_designator_list();
Designator *parse_designator();
Declaration *parse_static_assert_declaration();
Stmt *parse_statement();
Stmt *parse_labeled_statement();
Stmt *parse_compound_statement();
DeclOrStmt *parse_block_item_list();
DeclOrStmt *parse_block_item();
Stmt *parse_expression_statement();
Stmt *parse_selection_statement();
Stmt *parse_iteration_statement();
Stmt *parse_jump_statement();
Program *parse_translation_unit();
ExternalDecl *parse_external_declaration();
Declaration *parse_function_definition();
Declaration *parse_declaration_list();

Expr *parse_primary_expression()
{
    Expr *expr = NULL;
    switch (current_token) {
    case TOKEN_IDENTIFIER:
        expr        = new_expr(EXPR_VAR);
        expr->u.var = strdup(current_lexeme);
        next_token();
        break;
    case TOKEN_I_CONSTANT:
    case TOKEN_F_CONSTANT:
    case TOKEN_ENUMERATION_CONSTANT:
        expr = parse_constant();
        break;
    case TOKEN_STRING_LITERAL:
    case TOKEN_FUNC_NAME:
        expr = parse_string();
        break;
    case TOKEN_LPAREN:
        next_token();
        expr = parse_expression();
        expect_token(TOKEN_RPAREN);
        break;
    case TOKEN_GENERIC:
        expr = parse_generic_selection();
        break;
    default:
        parse_error("Expected primary expression");
    }
    return expr;
}

Expr *parse_constant()
{
    Expr *expr      = new_expr(EXPR_LITERAL);
    expr->u.literal = new_literal(current_token == TOKEN_I_CONSTANT   ? LITERAL_INT
                                  : current_token == TOKEN_F_CONSTANT ? LITERAL_FLOAT
                                                                      : LITERAL_ENUM);
    switch (current_token) {
    case TOKEN_I_CONSTANT:
        expr->u.literal->u.int_val = atoi(current_lexeme);
        break;
    case TOKEN_F_CONSTANT:
        expr->u.literal->u.float_val = atof(current_lexeme);
        break;
    case TOKEN_ENUMERATION_CONSTANT:
        expr->u.literal->u.enum_const = strdup(current_lexeme);
        break;
    }
    next_token();
    return expr;
}

Expr *parse_string()
{
    Expr *expr                    = new_expr(EXPR_LITERAL);
    expr->u.literal               = new_literal(LITERAL_STRING);
    expr->u.literal->u.string_val = strdup(current_lexeme);
    next_token();
    return expr;
}

Expr *parse_generic_selection()
{
    expect_token(TOKEN_GENERIC);
    expect_token(TOKEN_LPAREN);
    Expr *controlling_expr = parse_assignment_expression();
    expect_token(TOKEN_COMMA);
    GenericAssoc *associations = parse_generic_assoc_list();
    expect_token(TOKEN_RPAREN);
    Expr *expr                       = new_expr(EXPR_GENERIC);
    expr->u.generic.controlling_expr = controlling_expr;
    expr->u.generic.associations     = associations;
    return expr;
}

GenericAssoc *parse_generic_assoc_list()
{
    GenericAssoc *assoc = parse_generic_association();
    if (current_token == TOKEN_COMMA) {
        next_token();
        assoc->next = parse_generic_assoc_list();
    }
    return assoc;
}

GenericAssoc *parse_generic_association()
{
    GenericAssoc *assoc;
    if (current_token == TOKEN_DEFAULT) {
        next_token();
        expect_token(TOKEN_COLON);
        Expr *expr             = parse_assignment_expression();
        assoc                  = new_generic_assoc(GENERIC_ASSOC_DEFAULT);
        assoc->u.default_assoc = expr;
    } else {
        Type *type = parse_type_name();
        expect_token(TOKEN_COLON);
        Expr *expr               = parse_assignment_expression();
        assoc                    = new_generic_assoc(GENERIC_ASSOC_TYPE);
        assoc->u.type_assoc.type = type;
        assoc->u.type_assoc.expr = expr;
    }
    return assoc;
}

Expr *parse_postfix_expression()
{
    Expr *expr = parse_primary_expression();
    while (1) {
        if (current_token == TOKEN_LBRACKET) {
            next_token();
            Expr *index = parse_expression();
            expect_token(TOKEN_RBRACKET);
            Expr *new_expr              = new_expr(EXPR_BINARY_OP);
            new_expr->u.binary_op.op    = new_binary_op(BINARY_ADD);
            new_expr->u.binary_op.left  = expr;
            new_expr->u.binary_op.right = index;
            expr                        = new_expr;
        } else if (current_token == TOKEN_LPAREN) {
            next_token();
            Expr *args = NULL;
            if (current_token != TOKEN_RPAREN) {
                args = parse_argument_expression_list();
            }
            expect_token(TOKEN_RPAREN);
            Expr *new_expr        = new_expr(EXPR_CALL);
            new_expr->u.call.func = expr;
            new_expr->u.call.args = args;
            expr                  = new_expr;
        } else if (current_token == TOKEN_DOT) {
            next_token();
            expect_token(TOKEN_IDENTIFIER);
            Expr *new_expr                 = new_expr(EXPR_FIELD_ACCESS);
            new_expr->u.field_access.expr  = expr;
            new_expr->u.field_access.field = strdup(current_lexeme);
            next_token();
            expr = new_expr;
        } else if (current_token == TOKEN_PTR_OP) {
            next_token();
            expect_token(TOKEN_IDENTIFIER);
            Expr *new_expr               = new_expr(EXPR_PTR_ACCESS);
            new_expr->u.ptr_access.expr  = expr;
            new_expr->u.ptr_access.field = strdup(current_lexeme);
            next_token();
            expr = new_expr;
        } else if (current_token == TOKEN_INC_OP) {
            next_token();
            Expr *new_expr       = new_expr(EXPR_POST_INC);
            new_expr->u.post_inc = expr;
            expr                 = new_expr;
        } else if (current_token == TOKEN_DEC_OP) {
            next_token();
            Expr *new_expr       = new_expr(EXPR_POST_DEC);
            new_expr->u.post_dec = expr;
            expr                 = new_expr;
        } else {
            break;
        }
    }
    return expr;
}

Expr *parse_argument_expression_list()
{
    Expr *expr = parse_assignment_expression();
    if (current_token == TOKEN_COMMA) {
        next_token();
        expr->next = parse_argument_expression_list();
    }
    return expr;
}

Expr *parse_unary_expression()
{
    if (current_token == TOKEN_INC_OP) {
        next_token();
        Expr *expr                = parse_unary_expression();
        Expr *new_expr            = new_expr(EXPR_UNARY_OP);
        new_expr->u.unary_op.op   = new_unary_op(UNARY_PRE_INC);
        new_expr->u.unary_op.expr = expr;
        return new_expr;
    } else if (current_token == TOKEN_DEC_OP) {
        next_token();
        Expr *expr                = parse_unary_expression();
        Expr *new_expr            = new_expr(EXPR_UNARY_OP);
        new_expr->u.unary_op.op   = new_unary_op(UNARY_PRE_DEC);
        new_expr->u.unary_op.expr = expr;
        return new_expr;
    } else if (current_token == TOKEN_AMPERSAND || current_token == TOKEN_STAR ||
               current_token == TOKEN_PLUS || current_token == TOKEN_MINUS ||
               current_token == TOKEN_TILDE || current_token == TOKEN_NOT) {
        UnaryOp *op               = parse_unary_operator();
        Expr *expr                = parse_cast_expression();
        Expr *new_expr            = new_expr(EXPR_UNARY_OP);
        new_expr->u.unary_op.op   = op;
        new_expr->u.unary_op.expr = expr;
        return new_expr;
    } else if (current_token == TOKEN_SIZEOF) {
        next_token();
        if (current_token == TOKEN_LPAREN &&
            /* Peek for type_name */ 0) { // Simplified: assume expr
            expect_token(TOKEN_LPAREN);
            Type *type = parse_type_name();
            expect_token(TOKEN_RPAREN);
            Expr *new_expr          = new_expr(EXPR_SIZEOF_TYPE);
            new_expr->u.sizeof_type = type;
            return new_expr;
        } else {
            Expr *expr              = parse_unary_expression();
            Expr *new_expr          = new_expr(EXPR_SIZEOF_EXPR);
            new_expr->u.sizeof_expr = expr;
            return new_expr;
        }
    } else if (current_token == TOKEN_ALIGNOF) {
        next_token();
        expect_token(TOKEN_LPAREN);
        Type *type = parse_type_name();
        expect_token(TOKEN_RPAREN);
        Expr *new_expr      = new_expr(EXPR_ALIGNOF);
        new_expr->u.alignof = type;
        return new_expr;
    } else {
        return parse_postfix_expression();
    }
}

UnaryOp *parse_unary_operator()
{
    UnaryOp *op = new_unary_op(current_token == TOKEN_AMPERSAND ? UNARY_ADDRESS
                               : current_token == TOKEN_STAR    ? UNARY_DEREF
                               : current_token == TOKEN_PLUS    ? UNARY_PLUS
                               : current_token == TOKEN_MINUS   ? UNARY_NEG
                               : current_token == TOKEN_TILDE   ? UNARY_BIT_NOT
                                                                : UNARY_LOG_NOT);
    next_token();
    return op;
}

Expr *parse_cast_expression()
{
    if (current_token == TOKEN_LPAREN) {
        next_token();
        Type *type = parse_type_name();
        expect_token(TOKEN_RPAREN);
        Expr *expr            = parse_cast_expression();
        Expr *new_expr        = new_expr(EXPR_CAST);
        new_expr->u.cast.type = type;
        new_expr->u.cast.expr = expr;
        return new_expr;
    }
    return parse_unary_expression();
}

Expr *parse_multiplicative_expression()
{
    Expr *expr = parse_cast_expression();
    while (current_token == TOKEN_STAR || current_token == TOKEN_SLASH ||
           current_token == TOKEN_PERCENT) {
        BinaryOpKind op_kind = current_token == TOKEN_STAR    ? BINARY_MUL
                               : current_token == TOKEN_SLASH ? BINARY_DIV
                                                              : BINARY_MOD;
        next_token();
        Expr *right                 = parse_cast_expression();
        Expr *new_expr              = new_expr(EXPR_BINARY_OP);
        new_expr->u.binary_op.op    = new_binary_op(op_kind);
        new_expr->u.binary_op.left  = expr;
        new_expr->u.binary_op.right = right;
        expr                        = new_expr;
    }
    return expr;
}

Expr *parse_additive_expression()
{
    Expr *expr = parse_multiplicative_expression();
    while (current_token == TOKEN_PLUS || current_token == TOKEN_MINUS) {
        BinaryOpKind op_kind = current_token == TOKEN_PLUS ? BINARY_ADD : BINARY_SUB;
        next_token();
        Expr *right                 = parse_multiplicative_expression();
        Expr *new_expr              = new_expr(EXPR_BINARY_OP);
        new_expr->u.binary_op.op    = new_binary_op(op_kind);
        new_expr->u.binary_op.left  = expr;
        new_expr->u.binary_op.right = right;
        expr                        = new_expr;
    }
    return expr;
}

Expr *parse_shift_expression()
{
    Expr *expr = parse_additive_expression();
    while (current_token == TOKEN_LEFT_OP || current_token == TOKEN_RIGHT_OP) {
        BinaryOpKind op_kind =
            current_token == TOKEN_LEFT_OP ? BINARY_LEFT_SHIFT : BINARY_RIGHT_SHIFT;
        next_token();
        Expr *right                 = parse_additive_expression();
        Expr *new_expr              = new_expr(EXPR_BINARY_OP);
        new_expr->u.binary_op.op    = new_binary_op(op_kind);
        new_expr->u.binary_op.left  = expr;
        new_expr->u.binary_op.right = right;
        expr                        = new_expr;
    }
    return expr;
}

Expr *parse_relational_expression()
{
    Expr *expr = parse_shift_expression();
    while (current_token == TOKEN_LT || current_token == TOKEN_GT || current_token == TOKEN_LE_OP ||
           current_token == TOKEN_GE_OP) {
        BinaryOpKind op_kind = current_token == TOKEN_LT      ? BINARY_LT
                               : current_token == TOKEN_GT    ? BINARY_GT
                               : current_token == TOKEN_LE_OP ? BINARY_LE
                                                              : BINARY_GE;
        next_token();
        Expr *right                 = parse_shift_expression();
        Expr *new_expr              = new_expr(EXPR_BINARY_OP);
        new_expr->u.binary_op.op    = new_binary_op(op_kind);
        new_expr->u.binary_op.left  = expr;
        new_expr->u.binary_op.right = right;
        expr                        = new_expr;
    }
    return expr;
}

Expr *parse_equality_expression()
{
    Expr *expr = parse_relational_expression();
    while (current_token == TOKEN_EQ_OP || current_token == TOKEN_NE_OP) {
        BinaryOpKind op_kind = current_token == TOKEN_EQ_OP ? BINARY_EQ : BINARY_NE;
        next_token();
        Expr *right                 = parse_relational_expression();
        Expr *new_expr              = new_expr(EXPR_BINARY_OP);
        new_expr->u.binary_op.op    = new_binary_op(op_kind);
        new_expr->u.binary_op.left  = expr;
        new_expr->u.binary_op.right = right;
        expr                        = new_expr;
    }
    return expr;
}

Expr *parse_and_expression()
{
    Expr *expr = parse_equality_expression();
    while (current_token == TOKEN_AMPERSAND) {
        next_token();
        Expr *right                 = parse_equality_expression();
        Expr *new_expr              = new_expr(EXPR_BINARY_OP);
        new_expr->u.binary_op.op    = new_binary_op(BINARY_BIT_AND);
        new_expr->u.binary_op.left  = expr;
        new_expr->u.binary_op.right = right;
        expr                        = new_expr;
    }
    return expr;
}

Expr *parse_exclusive_or_expression()
{
    Expr *expr = parse_and_expression();
    while (current_token == TOKEN_CARET) {
        next_token();
        Expr *right                 = parse_and_expression();
        Expr *new_expr              = new_expr(EXPR_BINARY_OP);
        new_expr->u.binary_op.op    = new_binary_op(BINARY_BIT_XOR);
        new_expr->u.binary_op.left  = expr;
        new_expr->u.binary_op.right = right;
        expr                        = new_expr;
    }
    return expr;
}

Expr *parse_inclusive_or_expression()
{
    Expr *expr = parse_exclusive_or_expression();
    while (current_token == TOKEN_PIPE) {
        next_token();
        Expr *right                 = parse_exclusive_or_expression();
        Expr *new_expr              = new_expr(EXPR_BINARY_OP);
        new_expr->u.binary_op.op    = new_binary_op(BINARY_BIT_OR);
        new_expr->u.binary_op.left  = expr;
        new_expr->u.binary_op.right = right;
        expr                        = new_expr;
    }
    return expr;
}

Expr *parse_logical_and_expression()
{
    Expr *expr = parse_inclusive_or_expression();
    while (current_token == TOKEN_AND_OP) {
        next_token();
        Expr *right                 = parse_inclusive_or_expression();
        Expr *new_expr              = new_expr(EXPR_BINARY_OP);
        new_expr->u.binary_op.op    = new_binary_op(BINARY_LOG_AND);
        new_expr->u.binary_op.left  = expr;
        new_expr->u.binary_op.right = right;
        expr                        = new_expr;
    }
    return expr;
}

Expr *parse_logical_or_expression()
{
    Expr *expr = parse_logical_and_expression();
    while (current_token == TOKEN_OR_OP) {
        next_token();
        Expr *right                 = parse_logical_and_expression();
        Expr *new_expr              = new_expr(EXPR_BINARY_OP);
        new_expr->u.binary_op.op    = new_binary_op(BINARY_LOG_OR);
        new_expr->u.binary_op.left  = expr;
        new_expr->u.binary_op.right = right;
        expr                        = new_expr;
    }
    return expr;
}

Expr *parse_conditional_expression()
{
    Expr *expr = parse_logical_or_expression();
    if (current_token == TOKEN_QUESTION) {
        next_token();
        Expr *then_expr = parse_expression();
        expect_token(TOKEN_COLON);
        Expr *else_expr            = parse_conditional_expression();
        Expr *new_expr             = new_expr(EXPR_COND);
        new_expr->u.cond.condition = expr;
        new_expr->u.cond.then_expr = then_expr;
        new_expr->u.cond.else_expr = else_expr;
        expr                       = new_expr;
    }
    return expr;
}

Expr *parse_assignment_expression()
{
    Expr *expr = parse_conditional_expression();
    if (current_token == TOKEN_ASSIGN || current_token == TOKEN_MUL_ASSIGN ||
        current_token == TOKEN_DIV_ASSIGN || current_token == TOKEN_MOD_ASSIGN ||
        current_token == TOKEN_ADD_ASSIGN || current_token == TOKEN_SUB_ASSIGN ||
        current_token == TOKEN_LEFT_ASSIGN || current_token == TOKEN_RIGHT_ASSIGN ||
        current_token == TOKEN_AND_ASSIGN || current_token == TOKEN_XOR_ASSIGN ||
        current_token == TOKEN_OR_ASSIGN) {
        AssignOp *op              = parse_assignment_operator();
        Expr *value               = parse_assignment_expression();
        Expr *new_expr            = new_expr(EXPR_ASSIGN);
        new_expr->u.assign.target = expr;
        new_expr->u.assign.op     = op;
        new_expr->u.assign.value  = value;
        expr                      = new_expr;
    }
    return expr;
}

AssignOp *parse_assignment_operator()
{
    AssignOp *op = new_assign_op(current_token == TOKEN_ASSIGN         ? ASSIGN_SIMPLE
                                 : current_token == TOKEN_MUL_ASSIGN   ? ASSIGN_MUL
                                 : current_token == TOKEN_DIV_ASSIGN   ? ASSIGN_DIV
                                 : current_token == TOKEN_MOD_ASSIGN   ? ASSIGN_MOD
                                 : current_token == TOKEN_ADD_ASSIGN   ? ASSIGN_ADD
                                 : current_token == TOKEN_SUB_ASSIGN   ? ASSIGN_SUB
                                 : current_token == TOKEN_LEFT_ASSIGN  ? ASSIGN_LEFT
                                 : current_token == TOKEN_RIGHT_ASSIGN ? ASSIGN_RIGHT
                                 : current_token == TOKEN_AND_ASSIGN   ? ASSIGN_AND
                                 : current_token == TOKEN_XOR_ASSIGN   ? ASSIGN_XOR
                                                                       : ASSIGN_OR);
    next_token();
    return op;
}

Expr *parse_expression()
{
    Expr *expr = parse_assignment_expression();
    while (current_token == TOKEN_COMMA) {
        next_token();
        Expr *next = parse_assignment_expression();
        expr->next = next;
    }
    return expr;
}

Expr *parse_constant_expression()
{
    return parse_conditional_expression();
}

Declaration *parse_declaration()
{
    if (current_token == TOKEN_STATIC_ASSERT) {
        return parse_static_assert_declaration();
    }
    DeclSpec *specifiers = parse_declaration_specifiers();
    if (current_token == TOKEN_SEMICOLON) {
        next_token();
        Declaration *decl      = new_declaration(DECL_EMPTY);
        decl->u.var.specifiers = specifiers;
        return decl;
    }
    InitDeclarator *declarators = parse_init_declarator_list();
    expect_token(TOKEN_SEMICOLON);
    Declaration *decl       = new_declaration(DECL_VAR);
    decl->u.var.specifiers  = specifiers;
    decl->u.var.declarators = declarators;
    return decl;
}

DeclSpec *parse_declaration_specifiers()
{
    DeclSpec *ds = new_decl_spec();
    while (1) {
        if (current_token == TOKEN_TYPEDEF || current_token == TOKEN_EXTERN ||
            current_token == TOKEN_STATIC || current_token == TOKEN_THREAD_LOCAL ||
            current_token == TOKEN_AUTO || current_token == TOKEN_REGISTER) {
            ds->storage = parse_storage_class_specifier();
        } else if (current_token == TOKEN_VOID || current_token == TOKEN_CHAR ||
                   current_token == TOKEN_SHORT || current_token == TOKEN_INT ||
                   current_token == TOKEN_LONG || current_token == TOKEN_FLOAT ||
                   current_token == TOKEN_DOUBLE || current_token == TOKEN_SIGNED ||
                   current_token == TOKEN_UNSIGNED || current_token == TOKEN_BOOL ||
                   current_token == TOKEN_COMPLEX || current_token == TOKEN_IMAGINARY ||
                   current_token == TOKEN_STRUCT || current_token == TOKEN_UNION ||
                   current_token == TOKEN_ENUM || current_token == TOKEN_TYPEDEF_NAME ||
                   current_token == TOKEN_ATOMIC) {
            TypeSpec *ts = parse_type_specifier();
            append_list(&ds->type_specs, ts);
        } else if (current_token == TOKEN_CONST || current_token == TOKEN_RESTRICT ||
                   current_token == TOKEN_VOLATILE || current_token == TOKEN_ATOMIC) {
            TypeQualifier *q = parse_type_qualifier();
            append_list(&ds->qualifiers, q);
        } else if (current_token == TOKEN_INLINE || current_token == TOKEN_NORETURN) {
            FunctionSpec *fs = parse_function_specifier();
            append_list(&ds->func_specs, fs);
        } else if (current_token == TOKEN_ALIGNAS) {
            ds->align_spec = parse_alignment_specifier();
        } else {
            break;
        }
    }
    return ds;
}

InitDeclarator *parse_init_declarator_list()
{
    InitDeclarator *decl = parse_init_declarator();
    if (current_token == TOKEN_COMMA) {
        next_token();
        decl->next = parse_init_declarator_list();
    }
    return decl;
}

InitDeclarator *parse_init_declarator()
{
    Declarator *declarator = parse_declarator();
    Initializer *init      = NULL;
    if (current_token == TOKEN_ASSIGN) {
        next_token();
        init = parse_initializer();
    }
    return new_init_declarator(declarator, init);
}

StorageClass *parse_storage_class_specifier()
{
    StorageClassKind kind = current_token == TOKEN_TYPEDEF        ? STORAGE_CLASS_TYPEDEF
                            : current_token == TOKEN_EXTERN       ? STORAGE_CLASS_EXTERN
                            : current_token == TOKEN_STATIC       ? STORAGE_CLASS_STATIC
                            : current_token == TOKEN_THREAD_LOCAL ? STORAGE_CLASS_THREAD_LOCAL
                            : current_token == TOKEN_AUTO         ? STORAGE_CLASS_AUTO
                                                                  : STORAGE_CLASS_REGISTER;
    next_token();
    return new_storage_class(kind);
}

TypeSpec *parse_type_specifier()
{
    TypeSpec *ts;
    if (current_token == TOKEN_VOID) {
        ts          = new_type_spec(TYPE_SPEC_BASIC);
        ts->u.basic = new_type(TYPE_VOID);
        next_token();
    } else if (current_token == TOKEN_CHAR) {
        ts          = new_type_spec(TYPE_SPEC_BASIC);
        ts->u.basic = new_type(TYPE_CHAR);
        next_token();
    } else if (current_token == TOKEN_SHORT) {
        ts          = new_type_spec(TYPE_SPEC_BASIC);
        ts->u.basic = new_type(TYPE_SHORT);
        next_token();
    } else if (current_token == TOKEN_INT) {
        ts          = new_type_spec(TYPE_SPEC_BASIC);
        ts->u.basic = new_type(TYPE_INT);
        next_token();
    } else if (current_token == TOKEN_LONG) {
        ts          = new_type_spec(TYPE_SPEC_BASIC);
        ts->u.basic = new_type(TYPE_LONG);
        next_token();
    } else if (current_token == TOKEN_FLOAT) {
        ts          = new_type_spec(TYPE_SPEC_BASIC);
        ts->u.basic = new_type(TYPE_FLOAT);
        next_token();
    } else if (current_token == TOKEN_DOUBLE) {
        ts          = new_type_spec(TYPE_SPEC_BASIC);
        ts->u.basic = new_type(TYPE_DOUBLE);
        next_token();
    } else if (current_token == TOKEN_SIGNED) {
        ts                               = new_type_spec(TYPE_SPEC_BASIC);
        ts->u.basic                      = new_type(TYPE_INT);
        ts->u.basic->u.char_t.signedness = SIGNED_SIGNED;
        next_token();
    } else if (current_token == TOKEN_UNSIGNED) {
        ts                               = new_type_spec(TYPE_SPEC_BASIC);
        ts->u.basic                      = new_type(TYPE_INT);
        ts->u.basic->u.char_t.signedness = SIGNED_UNSIGNED;
        next_token();
    } else if (current_token == TOKEN_BOOL) {
        ts          = new_type_spec(TYPE_SPEC_BASIC);
        ts->u.basic = new_type(TYPE_BOOL);
        next_token();
    } else if (current_token == TOKEN_COMPLEX) {
        ts          = new_type_spec(TYPE_SPEC_BASIC);
        ts->u.basic = new_type(TYPE_COMPLEX);
        next_token();
    } else if (current_token == TOKEN_IMAGINARY) {
        ts          = new_type_spec(TYPE_SPEC_BASIC);
        ts->u.basic = new_type(TYPE_IMAGINARY);
        next_token();
    } else if (current_token == TOKEN_ATOMIC) {
        ts           = new_type_spec(TYPE_SPEC_ATOMIC);
        ts->u.atomic = parse_atomic_type_specifier();
    } else if (current_token == TOKEN_STRUCT || current_token == TOKEN_UNION) {
        Type *type = parse_struct_or_union_specifier();
        ts         = new_type_spec(type->kind == TYPE_STRUCT ? TYPE_SPEC_STRUCT : TYPE_SPEC_UNION);
        ts->u.struct_spec = type->u.struct_t;
    } else if (current_token == TOKEN_ENUM) {
        Type *type      = parse_enum_specifier();
        ts              = new_type_spec(TYPE_SPEC_ENUM);
        ts->u.enum_spec = type->u.enum_t;
    } else if (current_token == TOKEN_TYPEDEF_NAME) {
        ts                 = new_type_spec(TYPE_SPEC_TYPEDEF_NAME);
        ts->u.typedef_name = strdup(current_lexeme);
        next_token();
    } else {
        parse_error("Expected type specifier");
    }
    return ts;
}

Type *parse_struct_or_union_specifier()
{
    int su     = parse_struct_or_union();
    Type *type = new_type(su == TOKEN_STRUCT ? TYPE_STRUCT : TYPE_UNION);
    if (current_token == TOKEN_IDENTIFIER) {
        type->u.struct_t.name = strdup(current_lexeme);
        next_token();
    }
    if (current_token == TOKEN_LBRACE) {
        next_token();
        type->u.struct_t.fields = parse_struct_declaration_list();
        expect_token(TOKEN_RBRACE);
    }
    return type;
}

int parse_struct_or_union()
{
    if (current_token == TOKEN_STRUCT || current_token == TOKEN_UNION) {
        int su = current_token;
        next_token();
        return su;
    }
    parse_error("Expected struct or union");
    return 0;
}

Field *parse_struct_declaration_list()
{
    Field *fields = parse_struct_declaration();
    if (current_token != TOKEN_RBRACE) {
        fields->next = parse_struct_declaration_list();
    }
    return fields;
}

Field *parse_struct_declaration()
{
    if (current_token == TOKEN_STATIC_ASSERT) {
        parse_static_assert_declaration(); /* Ignore for now */
        return NULL;
    }
    TypeSpec *spec = parse_specifier_qualifier_list();
    if (current_token == TOKEN_SEMICOLON) {
        next_token();
        return new_field(NULL, spec->u.basic, NULL, true);
    }
    Declarator *decls = parse_struct_declarator_list();
    expect_token(TOKEN_SEMICOLON);
    return new_field(decls->u.named.name, spec->u.basic, NULL, false);
}

TypeSpec *parse_specifier_qualifier_list()
{
    TypeSpec *spec = new_type_spec(TYPE_SPEC_BASIC);
    spec->u.basic  = new_type(TYPE_VOID); /* Placeholder */
    while (1) {
        if (current_token == TOKEN_VOID || current_token == TOKEN_CHAR ||
            current_token == TOKEN_SHORT || current_token == TOKEN_INT ||
            current_token == TOKEN_LONG || current_token == TOKEN_FLOAT ||
            current_token == TOKEN_DOUBLE || current_token == TOKEN_SIGNED ||
            current_token == TOKEN_UNSIGNED || current_token == TOKEN_BOOL ||
            current_token == TOKEN_COMPLEX || current_token == TOKEN_IMAGINARY ||
            current_token == TOKEN_STRUCT || current_token == TOKEN_UNION ||
            current_token == TOKEN_ENUM || current_token == TOKEN_TYPEDEF_NAME ||
            current_token == TOKEN_ATOMIC) {
            TypeSpec *ts = parse_type_specifier();
            append_list(&spec->type_specs, ts);
        } else if (current_token == TOKEN_CONST || current_token == TOKEN_RESTRICT ||
                   current_token == TOKEN_VOLATILE || current_token == TOKEN_ATOMIC) {
            TypeQualifier *q = parse_type_qualifier();
            append_list(&spec->qualifiers, q);
        } else {
            break;
        }
    }
    return spec;
}

Declarator *parse_struct_declarator_list()
{
    Declarator *decl = parse_struct_declarator();
    if (current_token == TOKEN_COMMA) {
        next_token();
        decl->next = parse_struct_declarator_list();
    }
    return decl;
}

Declarator *parse_struct_declarator()
{
    if (current_token == TOKEN_COLON) {
        next_token();
        Expr *bitfield = parse_constant_expression();
        return new_declarator(DECLARATOR_NAMED); /* Placeholder */
    }
    Declarator *decl = parse_declarator();
    if (current_token == TOKEN_COLON) {
        next_token();
        Expr *bitfield = parse_constant_expression();
        /* Update decl with bitfield */
    }
    return decl;
}

Type *parse_enum_specifier()
{
    expect_token(TOKEN_ENUM);
    Type *type = new_type(TYPE_ENUM);
    if (current_token == TOKEN_IDENTIFIER) {
        type->u.enum_t.name = strdup(current_lexeme);
        next_token();
    }
    if (current_token == TOKEN_LBRACE) {
        next_token();
        type->u.enum_t.enumerators = parse_enumerator_list();
        if (current_token == TOKEN_COMMA)
            next_token();
        expect_token(TOKEN_RBRACE);
    }
    return type;
}

Enumerator *parse_enumerator_list()
{
    Enumerator *enumr = parse_enumerator();
    if (current_token == TOKEN_COMMA && current_token != TOKEN_RBRACE) {
        next_token();
        enumr->next = parse_enumerator_list();
    }
    return enumr;
}

Enumerator *parse_enumerator()
{
    expect_token(TOKEN_IDENTIFIER);
    Ident name = strdup(current_lexeme);
    next_token();
    Expr *value = NULL;
    if (current_token == TOKEN_ASSIGN) {
        next_token();
        value = parse_constant_expression();
    }
    return new_enumerator(name, value);
}

Type *parse_atomic_type_specifier()
{
    expect_token(TOKEN_ATOMIC);
    expect_token(TOKEN_LPAREN);
    Type *type = parse_type_name();
    expect_token(TOKEN_RPAREN);
    Type *atomic          = new_type(TYPE_ATOMIC);
    atomic->u.atomic.base = type;
    return atomic;
}

TypeQualifier *parse_type_qualifier()
{
    TypeQualifierKind kind = current_token == TOKEN_CONST      ? TYPE_QUALIFIER_CONST
                             : current_token == TOKEN_RESTRICT ? TYPE_QUALIFIER_RESTRICT
                             : current_token == TOKEN_VOLATILE ? TYPE_QUALIFIER_VOLATILE
                                                               : TYPE_QUALIFIER_ATOMIC;
    next_token();
    return new_type_qualifier(kind);
}

FunctionSpec *parse_function_specifier()
{
    FunctionSpecKind kind = current_token == TOKEN_INLINE ? FUNC_SPEC_INLINE : FUNC_SPEC_NORETURN;
    next_token();
    return new_function_spec(kind);
}

AlignmentSpec *parse_alignment_specifier()
{
    expect_token(TOKEN_ALIGNAS);
    expect_token(TOKEN_LPAREN);
    AlignmentSpec *as;
    if (current_token == TOKEN_VOID || current_token == TOKEN_CHAR || /* Type check simplified */
        current_token == TOKEN_STRUCT || current_token == TOKEN_UNION ||
        current_token == TOKEN_ENUM || current_token == TOKEN_TYPEDEF_NAME) {
        Type *type = parse_type_name();
        as         = new_alignment_spec(ALIGN_SPEC_TYPE);
        as->u.type = type;
    } else {
        Expr *expr = parse_constant_expression();
        as         = new_alignment_spec(ALIGN_SPEC_EXPR);
        as->u.expr = expr;
    }
    expect_token(TOKEN_RPAREN);
    return as;
}

Declarator *parse_declarator()
{
    Pointer *pointers = NULL;
    if (current_token == TOKEN_STAR) {
        pointers = parse_pointer();
    }
    Declarator *decl = parse_direct_declarator();
    if (pointers) {
        append_list(&decl->u.named.pointers, pointers);
    }
    return decl;
}

Declarator *parse_direct_declarator()
{
    Declarator *decl;
    if (current_token == TOKEN_IDENTIFIER) {
        decl               = new_declarator(DECLARATOR_NAMED);
        decl->u.named.name = strdup(current_lexeme);
        next_token();
    } else if (current_token == TOKEN_LPAREN) {
        next_token();
        decl = parse_declarator();
        expect_token(TOKEN_RPAREN);
    } else {
        parse_error("Expected identifier or '('");
    }
    while (1) {
        if (current_token == TOKEN_LBRACKET) {
            next_token();
            DeclaratorSuffix *suffix = new_declarator_suffix(SUFFIX_ARRAY);
            if (current_token == TOKEN_STATIC) {
                next_token();
                suffix->u.array.is_static = true;
            }
            TypeQualifier *qualifiers = NULL;
            if (current_token == TOKEN_CONST || current_token == TOKEN_RESTRICT ||
                current_token == TOKEN_VOLATILE || current_token == TOKEN_ATOMIC) {
                qualifiers = parse_type_qualifier_list();
            }
            Expr *size = NULL;
            if (current_token != TOKEN_RBRACKET && current_token != TOKEN_STAR) {
                size = parse_assignment_expression();
            }
            if (current_token == TOKEN_STAR) {
                next_token();
            }
            expect_token(TOKEN_RBRACKET);
            suffix->u.array.qualifiers = qualifiers;
            suffix->u.array.size       = size;
            append_list(&decl->u.named.suffixes, suffix);
        } else if (current_token == TOKEN_LPAREN) {
            next_token();
            DeclaratorSuffix *suffix = new_declarator_suffix(SUFFIX_FUNCTION);
            ParamList *params        = NULL;
            if (current_token != TOKEN_RPAREN) {
                params = parse_parameter_type_list();
            } else {
                params = new_param_list(true, false);
            }
            expect_token(TOKEN_RPAREN);
            suffix->u.function.params   = params;
            suffix->u.function.variadic = params->u.params && params->u.params->next;
            append_list(&decl->u.named.suffixes, suffix);
        } else {
            break;
        }
    }
    return decl;
}

Pointer *parse_pointer()
{
    expect_token(TOKEN_STAR);
    TypeQualifier *qualifiers = NULL;
    if (current_token == TOKEN_CONST || current_token == TOKEN_RESTRICT ||
        current_token == TOKEN_VOLATILE || current_token == TOKEN_ATOMIC) {
        qualifiers = parse_type_qualifier_list();
    }
    Pointer *pointer = new_pointer(qualifiers);
    if (current_token == TOKEN_STAR) {
        pointer->next = parse_pointer();
    }
    return pointer;
}

TypeQualifier *parse_type_qualifier_list()
{
    TypeQualifier *qual = parse_type_qualifier();
    if (current_token == TOKEN_CONST || current_token == TOKEN_RESTRICT ||
        current_token == TOKEN_VOLATILE || current_token == TOKEN_ATOMIC) {
        qual->next = parse_type_qualifier_list();
    }
    return qual;
}

ParamList *parse_parameter_type_list()
{
    ParamList *pl = new_param_list(false, false);
    pl->u.params  = parse_parameter_list();
    if (current_token == TOKEN_COMMA && current_token == TOKEN_ELLIPSIS) {
        next_token();
        expect_token(TOKEN_ELLIPSIS);
        Param *variadic = new_param(NULL, NULL);
        append_list(&pl->u.params, variadic);
    }
    return pl;
}

Param *parse_parameter_list()
{
    Param *param = parse_parameter_declaration();
    if (current_token == TOKEN_COMMA && current_token != TOKEN_ELLIPSIS) {
        next_token();
        param->next = parse_parameter_list();
    }
    return param;
}

Param *parse_parameter_declaration()
{
    DeclSpec *spec = parse_declaration_specifiers();
    if (current_token == TOKEN_IDENTIFIER || current_token == TOKEN_LPAREN ||
        current_token == TOKEN_STAR) {
        Declarator *decl = parse_declarator();
        return new_param(decl->u.named.name, spec->type_specs ? spec->type_specs->u.basic : NULL);
    } else if (current_token == TOKEN_LBRACKET) {
        Declarator *decl = parse_abstract_declarator();
        return new_param(NULL, spec->type_specs ? spec->type_specs->u.basic : NULL);
    }
    return new_param(NULL, spec->type_specs ? spec->type_specs->u.basic : NULL);
}

Ident parse_identifier_list()
{
    expect_token(TOKEN_IDENTIFIER);
    Ident id = strdup(current_lexeme);
    next_token();
    if (current_token == TOKEN_COMMA) {
        next_token();
        id->next = parse_identifier_list();
    }
    return id;
}

Type *parse_type_name()
{
    TypeSpec *spec = parse_specifier_qualifier_list();
    if (current_token == TOKEN_STAR || current_token == TOKEN_LPAREN ||
        current_token == TOKEN_LBRACKET) {
        parse_abstract_declarator();
    }
    return spec->u.basic;
}

Declarator *parse_abstract_declarator()
{
    Declarator *decl = new_declarator(DECLARATOR_ABSTRACT);
    if (current_token == TOKEN_STAR) {
        Pointer *pointers = parse_pointer();
        append_list(&decl->u.abstract.pointers, pointers);
        if (current_token == TOKEN_LPAREN || current_token == TOKEN_LBRACKET) {
            Declarator *direct = parse_direct_abstract_declarator();
            append_list(&decl->u.abstract.suffixes, direct->u.abstract.suffixes);
        }
    } else {
        decl = parse_direct_abstract_declarator();
    }
    return decl;
}

Declarator *parse_direct_abstract_declarator()
{
    Declarator *decl = new_declarator(DECLARATOR_ABSTRACT);
    if (current_token == TOKEN_LPAREN) {
        next_token();
        if (current_token != TOKEN_RPAREN) {
            Declarator *inner = parse_abstract_declarator();
            append_list(&decl->u.abstract.suffixes, inner->u.abstract.suffixes);
        }
        expect_token(TOKEN_RPAREN);
    } else if (current_token == TOKEN_LBRACKET) {
        next_token();
        DeclaratorSuffix *suffix = new_declarator_suffix(SUFFIX_ARRAY);
        if (current_token == TOKEN_STATIC) {
            next_token();
            suffix->u.array.is_static = true;
        }
        TypeQualifier *qualifiers = NULL;
        if (current_token == TOKEN_CONST || current_token == TOKEN_RESTRICT ||
            current_token == TOKEN_VOLATILE || current_token == TOKEN_ATOMIC) {
            qualifiers = parse_type_qualifier_list();
        }
        Expr *size = NULL;
        if (current_token != TOKEN_RBRACKET && current_token != TOKEN_STAR) {
            size = parse_assignment_expression();
        }
        if (current_token == TOKEN_STAR) {
            next_token();
        }
        expect_token(TOKEN_RBRACKET);
        suffix->u.array.qualifiers = qualifiers;
        suffix->u.array.size       = size;
        append_list(&decl->u.abstract.suffixes, suffix);
    } else if (current_token == TOKEN_LPAREN) {
        next_token();
        ParamList *params = NULL;
        if (current_token != TOKEN_RPAREN) {
            params = parse_parameter_type_list();
        } else {
            params = new_param_list(true, false);
        }
        expect_token(TOKEN_RPAREN);
        DeclaratorSuffix *suffix  = new_declarator_suffix(SUFFIX_FUNCTION);
        suffix->u.function.params = params;
        append_list(&decl->u.abstract.suffixes, suffix);
    }
    if (current_token == TOKEN_LBRACKET || current_token == TOKEN_LPAREN) {
        Declarator *next = parse_direct_abstract_declarator();
        append_list(&decl->u.abstract.suffixes, next->u.abstract.suffixes);
    }
    return decl;
}

Initializer *parse_initializer()
{
    if (current_token == TOKEN_LBRACE) {
        next_token();
        InitItem *items = parse_initializer_list();
        if (current_token == TOKEN_COMMA)
            next_token();
        expect_token(TOKEN_RBRACE);
        Initializer *init = new_initializer(INITIALIZER_COMPOUND);
        init->u.items     = items;
        return init;
    }
    Initializer *init = new_initializer(INITIALIZER_SINGLE);
    init->u.expr      = parse_assignment_expression();
    return init;
}

InitItem *parse_initializer_list()
{
    Designator *designation = NULL;
    if (current_token == TOKEN_LBRACKET || current_token == TOKEN_DOT) {
        designation = parse_designation();
    }
    Initializer *init = parse_initializer();
    InitItem *item    = new_init_item(designation, init);
    if (current_token == TOKEN_COMMA && current_token != TOKEN_RBRACE) {
        next_token();
        item->next = parse_initializer_list();
    }
    return item;
}

Designator *parse_designation()
{
    Designator *designators = parse_designator_list();
    expect_token(TOKEN_ASSIGN);
    return designators;
}

Designator *parse_designator_list()
{
    Designator *designator = parse_designator();
    if (current_token == TOKEN_LBRACKET || current_token == TOKEN_DOT) {
        designator->next = parse_designator_list();
    }
    return designator;
}

Designator *parse_designator()
{
    if (current_token == TOKEN_LBRACKET) {
        next_token();
        Expr *expr = parse_constant_expression();
        expect_token(TOKEN_RBRACKET);
        Designator *d = new_designator(DESIGNATOR_ARRAY);
        d->u.expr     = expr;
        return d;
    } else if (current_token == TOKEN_DOT) {
        next_token();
        expect_token(TOKEN_IDENTIFIER);
        Designator *d = new_designator(DESIGNATOR_FIELD);
        d->u.name     = strdup(current_lexeme);
        next_token();
        return d;
    }
    parse_error("Expected designator");
    return NULL;
}

Declaration *parse_static_assert_declaration()
{
    expect_token(TOKEN_STATIC_ASSERT);
    expect_token(TOKEN_LPAREN);
    Expr *condition = parse_constant_expression();
    expect_token(TOKEN_COMMA);
    expect_token(TOKEN_STRING_LITERAL);
    char *message = strdup(current_lexeme);
    next_token();
    expect_token(TOKEN_RPAREN);
    expect_token(TOKEN_SEMICOLON);
    Declaration *decl               = new_declaration(DECL_STATIC_ASSERT);
    decl->u.static_assert.condition = condition;
    decl->u.static_assert.message   = message;
    return decl;
}

Stmt *parse_statement()
{
    if (current_token == TOKEN_IDENTIFIER && /* Peek for ':' */ 0) {
        return parse_labeled_statement();
    } else if (current_token == TOKEN_CASE || current_token == TOKEN_DEFAULT) {
        return parse_labeled_statement();
    } else if (current_token == TOKEN_LBRACE) {
        return parse_compound_statement();
    } else if (current_token == TOKEN_IF || current_token == TOKEN_SWITCH) {
        return parse_selection_statement();
    } else if (current_token == TOKEN_WHILE || current_token == TOKEN_DO ||
               current_token == TOKEN_FOR) {
        return parse_iteration_statement();
    } else if (current_token == TOKEN_GOTO || current_token == TOKEN_CONTINUE ||
               current_token == TOKEN_BREAK || current_token == TOKEN_RETURN) {
        return parse_jump_statement();
    } else {
        return parse_expression_statement();
    }
}

Stmt *parse_labeled_statement()
{
    if (current_token == TOKEN_IDENTIFIER) {
        Ident label = strdup(current_lexeme);
        next_token();
        expect_token(TOKEN_COLON);
        Stmt *stmt               = parse_statement();
        Stmt *labeled            = new_stmt(STMT_LABELED);
        labeled->u.labeled.label = label;
        labeled->u.labeled.stmt  = stmt;
        return labeled;
    } else if (current_token == TOKEN_CASE) {
        next_token();
        Expr *expr = parse_constant_expression();
        expect_token(TOKEN_COLON);
        Stmt *stmt                  = parse_statement();
        Stmt *case_stmt             = new_stmt(STMT_CASE);
        case_stmt->u.case_stmt.expr = expr;
        case_stmt->u.case_stmt.stmt = stmt;
        return case_stmt;
    } else if (current_token == TOKEN_DEFAULT) {
        next_token();
        expect_token(TOKEN_COLON);
        Stmt *stmt                   = parse_statement();
        Stmt *default_stmt           = new_stmt(STMT_DEFAULT);
        default_stmt->u.default_stmt = stmt;
        return default_stmt;
    }
    parse_error("Expected labeled statement");
    return NULL;
}

Stmt *parse_compound_statement()
{
    expect_token(TOKEN_LBRACE);
    DeclOrStmt *items = NULL;
    if (current_token != TOKEN_RBRACE) {
        items = parse_block_item_list();
    }
    expect_token(TOKEN_RBRACE);
    Stmt *stmt       = new_stmt(STMT_COMPOUND);
    stmt->u.compound = items;
    return stmt;
}

DeclOrStmt *parse_block_item_list()
{
    DeclOrStmt *item = parse_block_item();
    if (current_token != TOKEN_RBRACE) {
        item->next = parse_block_item_list();
    }
    return item;
}

DeclOrStmt *parse_block_item()
{
    if (current_token == TOKEN_TYPEDEF || current_token == TOKEN_EXTERN ||
        current_token == TOKEN_STATIC || current_token == TOKEN_THREAD_LOCAL ||
        current_token == TOKEN_AUTO || current_token == TOKEN_REGISTER ||
        current_token == TOKEN_VOID || current_token == TOKEN_CHAR ||
        current_token == TOKEN_SHORT || current_token == TOKEN_INT || current_token == TOKEN_LONG ||
        current_token == TOKEN_FLOAT || current_token == TOKEN_DOUBLE ||
        current_token == TOKEN_SIGNED || current_token == TOKEN_UNSIGNED ||
        current_token == TOKEN_BOOL || current_token == TOKEN_COMPLEX ||
        current_token == TOKEN_IMAGINARY || current_token == TOKEN_STRUCT ||
        current_token == TOKEN_UNION || current_token == TOKEN_ENUM ||
        current_token == TOKEN_TYPEDEF_NAME || current_token == TOKEN_ATOMIC ||
        current_token == TOKEN_CONST || current_token == TOKEN_RESTRICT ||
        current_token == TOKEN_VOLATILE || current_token == TOKEN_INLINE ||
        current_token == TOKEN_NORETURN || current_token == TOKEN_ALIGNAS ||
        current_token == TOKEN_STATIC_ASSERT) {
        Declaration *decl = parse_declaration();
        DeclOrStmt *ds    = new_decl_or_stmt(DECL_OR_STMT_DECL);
        ds->u.decl        = decl;
        return ds;
    }
    Stmt *stmt     = parse_statement();
    DeclOrStmt *ds = new_decl_or_stmt(DECL_OR_STMT_STMT);
    ds->u.stmt     = stmt;
    return ds;
}

Stmt *parse_expression_statement()
{
    Expr *expr = NULL;
    if (current_token != TOKEN_SEMICOLON) {
        expr = parse_expression();
    }
    expect_token(TOKEN_SEMICOLON);
    Stmt *stmt   = new_stmt(STMT_EXPR);
    stmt->u.expr = expr;
    return stmt;
}

Stmt *parse_selection_statement()
{
    if (current_token == TOKEN_IF) {
        next_token();
        expect_token(TOKEN_LPAREN);
        Expr *condition = parse_expression();
        expect_token(TOKEN_RPAREN);
        Stmt *then_stmt = parse_statement();
        Stmt *else_stmt = NULL;
        if (current_token == TOKEN_ELSE) {
            next_token();
            else_stmt = parse_statement();
        }
        Stmt *stmt                = new_stmt(STMT_IF);
        stmt->u.if_stmt.condition = condition;
        stmt->u.if_stmt.then_stmt = then_stmt;
        stmt->u.if_stmt.else_stmt = else_stmt;
        return stmt;
    } else if (current_token == TOKEN_SWITCH) {
        next_token();
        expect_token(TOKEN_LPAREN);
        Expr *expr = parse_expression();
        expect_token(TOKEN_RPAREN);
        Stmt *body               = parse_statement();
        Stmt *stmt               = new_stmt(STMT_SWITCH);
        stmt->u.switch_stmt.expr = expr;
        stmt->u.switch_stmt.body = body;
        return stmt;
    }
    parse_error("Expected if or switch");
    return NULL;
}

Stmt *parse_iteration_statement()
{
    if (current_token == TOKEN_WHILE) {
        next_token();
        expect_token(TOKEN_LPAREN);
        Expr *condition = parse_expression();
        expect_token(TOKEN_RPAREN);
        Stmt *body                   = parse_statement();
        Stmt *stmt                   = new_stmt(STMT_WHILE);
        stmt->u.while_stmt.condition = condition;
        stmt->u.while_stmt.body      = body;
        return stmt;
    } else if (current_token == TOKEN_DO) {
        next_token();
        Stmt *body = parse_statement();
        expect_token(TOKEN_WHILE);
        expect_token(TOKEN_LPAREN);
        Expr *condition = parse_expression();
        expect_token(TOKEN_RPAREN);
        expect_token(TOKEN_SEMICOLON);
        Stmt *stmt                 = new_stmt(STMT_DO_WHILE);
        stmt->u.do_while.body      = body;
        stmt->u.do_while.condition = condition;
        return stmt;
    } else if (current_token == TOKEN_FOR) {
        next_token();
        expect_token(TOKEN_LPAREN);
        ForInit *init   = NULL;
        Expr *condition = NULL;
        Expr *update    = NULL;
        if (current_token == TOKEN_TYPEDEF || current_token == TOKEN_EXTERN ||
            current_token == TOKEN_STATIC || current_token == TOKEN_THREAD_LOCAL ||
            current_token == TOKEN_AUTO || current_token == TOKEN_REGISTER ||
            current_token == TOKEN_VOID || current_token == TOKEN_CHAR ||
            current_token == TOKEN_SHORT || current_token == TOKEN_INT ||
            current_token == TOKEN_LONG || current_token == TOKEN_FLOAT ||
            current_token == TOKEN_DOUBLE || current_token == TOKEN_SIGNED ||
            current_token == TOKEN_UNSIGNED || current_token == TOKEN_BOOL ||
            current_token == TOKEN_COMPLEX || current_token == TOKEN_IMAGINARY ||
            current_token == TOKEN_STRUCT || current_token == TOKEN_UNION ||
            current_token == TOKEN_ENUM || current_token == TOKEN_TYPEDEF_NAME ||
            current_token == TOKEN_ATOMIC || current_token == TOKEN_CONST ||
            current_token == TOKEN_RESTRICT || current_token == TOKEN_VOLATILE ||
            current_token == TOKEN_INLINE || current_token == TOKEN_NORETURN ||
            current_token == TOKEN_ALIGNAS || current_token == TOKEN_STATIC_ASSERT) {
            Declaration *decl = parse_declaration();
            init              = new_for_init(FOR_INIT_DECL);
            init->u.decl      = decl;
        } else {
            Stmt *expr_stmt = parse_expression_statement();
            init            = new_for_init(FOR_INIT_EXPR);
            init->u.expr    = expr_stmt->u.expr;
        }
        Stmt *cond_stmt = parse_expression_statement();
        condition       = cond_stmt->u.expr;
        if (current_token != TOKEN_RPAREN) {
            update = parse_expression();
        }
        expect_token(TOKEN_RPAREN);
        Stmt *body                 = parse_statement();
        Stmt *stmt                 = new_stmt(STMT_FOR);
        stmt->u.for_stmt.init      = init;
        stmt->u.for_stmt.condition = condition;
        stmt->u.for_stmt.update    = update;
        stmt->u.for_stmt.body      = body;
        return stmt;
    }
    parse_error("Expected while, do, or for");
    return NULL;
}

Stmt *parse_jump_statement()
{
    if (current_token == TOKEN_GOTO) {
        next_token();
        expect_token(TOKEN_IDENTIFIER);
        Ident label = strdup(current_lexeme);
        next_token();
        expect_token(TOKEN_SEMICOLON);
        Stmt *stmt         = new_stmt(STMT_GOTO);
        stmt->u.goto_label = label;
        return stmt;
    } else if (current_token == TOKEN_CONTINUE) {
        next_token();
        expect_token(TOKEN_SEMICOLON);
        return new_stmt(STMT_CONTINUE);
    } else if (current_token == TOKEN_BREAK) {
        next_token();
        expect_token(TOKEN_SEMICOLON);
        return new_stmt(STMT_BREAK);
    } else if (current_token == TOKEN_RETURN) {
        next_token();
        Expr *expr = NULL;
        if (current_token != TOKEN_SEMICOLON) {
            expr = parse_expression();
        }
        expect_token(TOKEN_SEMICOLON);
        Stmt *stmt   = new_stmt(STMT_RETURN);
        stmt->u.expr = expr;
        return stmt;
    }
    parse_error("Expected jump statement");
    return NULL;
}

Program *parse_translation_unit()
{
    Program *program = new_program();
    while (current_token != TOKEN_EOF) {
        ExternalDecl *decl = parse_external_declaration();
        append_list(&program->decls, decl);
    }
    return program;
}

ExternalDecl *parse_external_declaration()
{
    DeclSpec *spec = parse_declaration_specifiers();
    if (current_token == TOKEN_SEMICOLON || current_token == TOKEN_STATIC_ASSERT ||
        (current_token == TOKEN_IDENTIFIER && current_token == TOKEN_ASSIGN)) {
        Declaration *decl = parse_declaration();
        ExternalDecl *ed  = new_external_decl(EXTERNAL_DECL_DECLARATION);
        ed->u.declaration = decl;
        return ed;
    }
    Declaration *func         = parse_function_definition();
    ExternalDecl *ed          = new_external_decl(EXTERNAL_DECL_FUNCTION);
    ed->u.function.specifiers = spec;
    ed->u.function.declarator = func->u.var.declarators->declarator;
    ed->u.function.decls      = NULL; /* Simplified */
    ed->u.function.body       = NULL; /* Simplified */
    return ed;
}

Declaration *parse_function_definition()
{
    DeclSpec *spec     = parse_declaration_specifiers();
    Declarator *decl   = parse_declarator();
    Declaration *decls = NULL;
    if (current_token != TOKEN_LBRACE) {
        decls = parse_declaration_list();
    }
    Stmt *body              = parse_compound_statement();
    Declaration *func       = new_declaration(DECL_VAR); /* Placeholder */
    func->u.var.specifiers  = spec;
    func->u.var.declarators = new_init_declarator(decl, NULL);
    return func;
}

Declaration *parse_declaration_list()
{
    Declaration *decl = parse_declaration();
    if (current_token != TOKEN_LBRACE) {
        decl->next = parse_declaration_list();
    }
    return decl;
}

/* Main parsing function */
Program *parse(FILE *input)
{
    init_scanner(input);
    next_token();
    Program *program = parse_translation_unit();
    if (current_token != TOKEN_EOF) {
        parse_error("Expected end of file");
    }
    return program;
}
