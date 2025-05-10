#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include <stddef.h>

/* Forward declarations for recursive types */
typedef struct Type Type;
typedef struct TypeQualifier TypeQualifier;
typedef struct Field Field;
typedef struct Enumerator Enumerator;
typedef struct ParamList ParamList;
typedef struct Param Param;
typedef struct Declaration Declaration;
typedef struct DeclSpec DeclSpec;
typedef struct StorageClass StorageClass;
typedef struct TypeSpec TypeSpec;
typedef struct FunctionSpec FunctionSpec;
typedef struct AlignmentSpec AlignmentSpec;
typedef struct InitDeclarator InitDeclarator;
typedef struct Declarator Declarator;
typedef struct Pointer Pointer;
typedef struct DeclaratorSuffix DeclaratorSuffix;
typedef struct Initializer Initializer;
typedef struct InitItem InitItem;
typedef struct Designator Designator;
typedef struct Expr Expr;
typedef struct Literal Literal;
typedef struct UnaryOp UnaryOp;
typedef struct BinaryOp BinaryOp;
typedef struct AssignOp AssignOp;
typedef struct GenericAssoc GenericAssoc;
typedef struct Stmt Stmt;
typedef struct DeclOrStmt DeclOrStmt;
typedef struct ForInit ForInit;
typedef struct Program Program;
typedef struct ExternalDecl ExternalDecl;

/* Identifier */
typedef char *Ident;

/* Types */
typedef enum {
    TYPE_VOID,
    TYPE_BOOL,
    TYPE_CHAR,
    TYPE_SHORT,
    TYPE_INT,
    TYPE_LONG,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_COMPLEX,
    TYPE_IMAGINARY,
    TYPE_POINTER,
    TYPE_ARRAY,
    TYPE_FUNCTION,
    TYPE_STRUCT,
    TYPE_UNION,
    TYPE_ENUM,
    TYPE_TYPEDEF_NAME,
    TYPE_ATOMIC
} TypeKind;

typedef enum { SIGNED_SIGNED, SIGNED_UNSIGNED } Signedness;

struct Type {
    TypeKind kind;
    union {
        struct {
            Signedness signedness;
        } char_t; /* for Char, Short, Int, Long */
        struct {
            Type *base;
        } complex; /* for Complex, Imaginary */
        struct {
            Type *target;
            TypeQualifier *qualifiers;
        } pointer;
        struct {
            Type *element;
            Expr *size;
            TypeQualifier *qualifiers;
        } array;
        struct {
            Type *returnType;
            ParamList *params;
            bool variadic;
        } function;
        struct {
            Ident name;
            Field *fields;
        } struct_t; /* optional name */
        struct {
            Ident name;
            Enumerator *enumerators;
        } enum_t;
        struct {
            Ident name;
        } typedef_name;
        struct {
            Type *base;
        } atomic;
    } u;
    TypeQualifier *qualifiers; /* attributes */
};

typedef enum {
    TYPE_QUALIFIER_CONST,
    TYPE_QUALIFIER_RESTRICT,
    TYPE_QUALIFIER_VOLATILE,
    TYPE_QUALIFIER_ATOMIC
} TypeQualifierKind;

struct TypeQualifier {
    TypeQualifierKind kind;
    TypeQualifier *next; /* linked list */
};

struct Field {
    bool is_anonymous;
    union {
        struct {
            Ident name;
            Type *type;
            Expr *bitfield;
        } named; /* optional name, bitfield */
        struct {
            Type *type;
        } anonymous;
    } u;
    Field *next; /* linked list */
};

struct Enumerator {
    Ident name;
    Expr *value;      /* optional */
    Enumerator *next; /* linked list */
};

/* Parameter List */
struct ParamList {
    bool is_empty;
    bool is_ident_list;
    union {
        Param *params;
        Ident *idents; /* K&R style, linked list of Ident */
    } u;
};

struct Param {
    Ident name; /* optional */
    Type *type;
    Param *next; /* linked list */
};

/* Declarations */
typedef enum { DECL_VAR, DECL_STATIC_ASSERT, DECL_EMPTY } DeclarationKind;

struct Declaration {
    DeclarationKind kind;
    union {
        struct {
            DeclSpec *specifiers;
            InitDeclarator *declarators;
        } var;
        struct {
            Expr *condition;
            char *message;
        } static_assert;
    } u;
    Declaration *next; /* linked list for declaration_list */
};

struct DeclSpec {
    StorageClass *storage; /* optional */
    TypeQualifier *qualifiers;
    TypeSpec *type_specs;
    FunctionSpec *func_specs;
    AlignmentSpec *align_spec; /* optional */
};

typedef enum {
    STORAGE_CLASS_TYPEDEF,
    STORAGE_CLASS_EXTERN,
    STORAGE_CLASS_STATIC,
    STORAGE_CLASS_THREAD_LOCAL,
    STORAGE_CLASS_AUTO,
    STORAGE_CLASS_REGISTER
} StorageClassKind;

struct StorageClass {
    StorageClassKind kind;
};

typedef enum {
    TYPE_SPEC_BASIC,
    TYPE_SPEC_STRUCT,
    TYPE_SPEC_UNION,
    TYPE_SPEC_ENUM,
    TYPE_SPEC_TYPEDEF_NAME,
    TYPE_SPEC_ATOMIC
} TypeSpecKind;

struct TypeSpec {
    TypeSpecKind kind;
    union {
        Type *basic;
        struct {
            Ident name;
            Field *fields;
        } struct_spec; /* optional name */
        struct {
            Ident name;
            Enumerator *enumerators;
        } enum_spec;
        struct {
            Ident name;
        } typedef_name;
        struct {
            Type *type;
        } atomic;
    } u;
    TypeSpec *next; /* linked list */
};

typedef enum { FUNC_SPEC_INLINE, FUNC_SPEC_NORETURN } FunctionSpecKind;

struct FunctionSpec {
    FunctionSpecKind kind;
    FunctionSpec *next; /* linked list */
};

typedef enum { ALIGN_SPEC_TYPE, ALIGN_SPEC_EXPR } AlignmentSpecKind;

struct AlignmentSpec {
    AlignmentSpecKind kind;
    union {
        Type *type;
        Expr *expr;
    } u;
};

struct InitDeclarator {
    Declarator *declarator;
    Initializer *init;    /* optional */
    InitDeclarator *next; /* linked list */
};

typedef enum { DECLARATOR_NAMED, DECLARATOR_ABSTRACT } DeclaratorKind;

struct Declarator {
    DeclaratorKind kind;
    union {
        struct {
            Ident name;
            Pointer *pointers;
            DeclaratorSuffix *suffixes;
        } named;
        struct {
            Pointer *pointers;
            DeclaratorSuffix *suffixes;
        } abstract;
    } u;
};

struct Pointer {
    TypeQualifier *qualifiers;
    Pointer *next; /* linked list */
};

typedef enum { SUFFIX_ARRAY, SUFFIX_FUNCTION } DeclaratorSuffixKind;

struct DeclaratorSuffix {
    DeclaratorSuffixKind kind;
    union {
        struct {
            Expr *size;
            TypeQualifier *qualifiers;
            bool is_static;
        } array;
        struct {
            ParamList *params;
            bool variadic;
        } function;
    } u;
    DeclaratorSuffix *next; /* linked list */
};

typedef enum { INITIALIZER_SINGLE, INITIALIZER_COMPOUND } InitializerKind;

struct Initializer {
    InitializerKind kind;
    union {
        Expr *expr;
        InitItem *items;
    } u;
};

struct InitItem {
    Designator *designators;
    Initializer *init;
    InitItem *next; /* linked list */
};

typedef enum { DESIGNATOR_ARRAY, DESIGNATOR_FIELD } DesignatorKind;

struct Designator {
    DesignatorKind kind;
    union {
        Expr *expr; /* array index */
        Ident name; /* field name */
    } u;
    Designator *next; /* linked list */
};

/* Expressions */
typedef enum {
    EXPR_LITERAL,
    EXPR_VAR,
    EXPR_UNARY_OP,
    EXPR_BINARY_OP,
    EXPR_ASSIGN,
    EXPR_COND,
    EXPR_CAST,
    EXPR_CALL,
    EXPR_COMPOUND_LITERAL,
    EXPR_FIELD_ACCESS,
    EXPR_PTR_ACCESS,
    EXPR_POST_INC,
    EXPR_POST_DEC,
    EXPR_SIZEOF_EXPR,
    EXPR_SIZEOF_TYPE,
    EXPR_ALIGNOF,
    EXPR_GENERIC
} ExprKind;

struct Expr {
    ExprKind kind;
    union {
        Literal *literal;
        Ident var;
        struct {
            UnaryOp *op;
            Expr *expr;
        } unary_op;
        struct {
            BinaryOp *op;
            Expr *left;
            Expr *right;
        } binary_op;
        struct {
            Expr *target;
            AssignOp *op;
            Expr *value;
        } assign;
        struct {
            Expr *condition;
            Expr *then_expr;
            Expr *else_expr;
        } cond;
        struct {
            Type *type;
            Expr *expr;
        } cast;
        struct {
            Expr *func;
            Expr *args;
        } call;
        struct {
            Type *type;
            Initializer *init;
        } compound_literal;
        struct {
            Expr *expr;
            Ident field;
        } field_access;
        struct {
            Expr *expr;
            Ident field;
        } ptr_access;
        Expr *post_inc;
        Expr *post_dec;
        Expr *sizeof_expr;
        Type *sizeof_type;
        Type * alignof;
        struct {
            Expr *controlling_expr;
            GenericAssoc *associations;
        } generic;
    } u;
    Type *type; /* attributes */
    Expr *next; /* linked list for argument lists */
};

typedef enum { LITERAL_INT, LITERAL_FLOAT, LITERAL_CHAR, LITERAL_STRING, LITERAL_ENUM } LiteralKind;

struct Literal {
    LiteralKind kind;
    union {
        int int_val;
        float float_val;
        char char_val;
        char *string_val;
        Ident enum_const;
    } u;
};

typedef enum {
    UNARY_ADDRESS,
    UNARY_DEREF,
    UNARY_PLUS,
    UNARY_NEG,
    UNARY_BIT_NOT,
    UNARY_LOG_NOT,
    UNARY_PRE_INC,
    UNARY_PRE_DEC
} UnaryOpKind;

struct UnaryOp {
    UnaryOpKind kind;
};

typedef enum {
    BINARY_MUL,
    BINARY_DIV,
    BINARY_MOD,
    BINARY_ADD,
    BINARY_SUB,
    BINARY_LEFT_SHIFT,
    BINARY_RIGHT_SHIFT,
    BINARY_LT,
    BINARY_GT,
    BINARY_LE,
    BINARY_GE,
    BINARY_EQ,
    BINARY_NE,
    BINARY_BIT_AND,
    BINARY_BIT_XOR,
    BINARY_BIT_OR,
    BINARY_LOG_AND,
    BINARY_LOG_OR
} BinaryOpKind;

struct BinaryOp {
    BinaryOpKind kind;
};

typedef enum {
    ASSIGN_SIMPLE,
    ASSIGN_MUL,
    ASSIGN_DIV,
    ASSIGN_MOD,
    ASSIGN_ADD,
    ASSIGN_SUB,
    ASSIGN_LEFT,
    ASSIGN_RIGHT,
    ASSIGN_AND,
    ASSIGN_XOR,
    ASSIGN_OR
} AssignOpKind;

struct AssignOp {
    AssignOpKind kind;
};

typedef enum { GENERIC_ASSOC_TYPE, GENERIC_ASSOC_DEFAULT } GenericAssocKind;

struct GenericAssoc {
    GenericAssocKind kind;
    union {
        struct {
            Type *type;
            Expr *expr;
        } type_assoc;
        Expr *default_assoc;
    } u;
    GenericAssoc *next; /* linked list */
};

/* Statements */
typedef enum {
    STMT_EXPR,
    STMT_COMPOUND,
    STMT_IF,
    STMT_SWITCH,
    STMT_WHILE,
    STMT_DO_WHILE,
    STMT_FOR,
    STMT_GOTO,
    STMT_CONTINUE,
    STMT_BREAK,
    STMT_RETURN,
    STMT_LABELED,
    STMT_CASE,
    STMT_DEFAULT
} StmtKind;

struct Stmt {
    StmtKind kind;
    union {
        Expr *expr; /* optional */
        DeclOrStmt *compound;
        struct {
            Expr *condition;
            Stmt *then_stmt;
            Stmt *else_stmt;
        } if_stmt;
        struct {
            Expr *expr;
            Stmt *body;
        } switch_stmt;
        struct {
            Expr *condition;
            Stmt *body;
        } while_stmt;
        struct {
            Stmt *body;
            Expr *condition;
        } do_while;
        struct {
            ForInit *init;
            Expr *condition;
            Expr *update;
            Stmt *body;
        } for_stmt;
        Ident goto_label;
        struct {
            Ident label;
            Stmt *stmt;
        } labeled;
        struct {
            Expr *expr;
            Stmt *stmt;
        } case_stmt;
        Stmt *default_stmt;
    } u;
};

typedef enum { DECL_OR_STMT_DECL, DECL_OR_STMT_STMT } DeclOrStmtKind;

struct DeclOrStmt {
    DeclOrStmtKind kind;
    union {
        Declaration *decl;
        Stmt *stmt;
    } u;
    DeclOrStmt *next; /* linked list */
};

typedef enum { FOR_INIT_EXPR, FOR_INIT_DECL } ForInitKind;

struct ForInit {
    ForInitKind kind;
    union {
        Expr *expr;
        Declaration *decl;
    } u;
};

/* Program Structure */
struct Program {
    ExternalDecl *decls;
};

typedef enum { EXTERNAL_DECL_FUNCTION, EXTERNAL_DECL_DECLARATION } ExternalDeclKind;

struct ExternalDecl {
    ExternalDeclKind kind;
    union {
        struct {
            DeclSpec *specifiers;
            Declarator *declarator;
            Declaration *decls;
            Stmt *body;
        } function;
        Declaration *declaration;
    } u;
    ExternalDecl *next; /* linked list */
};

#endif /* AST_H */
