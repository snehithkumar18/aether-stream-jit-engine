#ifndef AETHER_JIT_PARSER_H
#define AETHER_JIT_PARSER_H

#include "lexer.h"
#include <stddef.h>

typedef enum NodeType {
    NODE_INVALID,
    NODE_PROGRAM,
    NODE_FUNCTION_DECL,
    NODE_FUNCTION_CALL,
    NODE_LAMBDA,
    NODE_BLOCK,
    NODE_RETURN,
    NODE_IF,
    NODE_ELSE,
    NODE_WHILE,
    NODE_FOR,
    NODE_BREAK,
    NODE_CONTINUE,
    NODE_SWITCH,
    NODE_CASE,
    NODE_DEFAULT,
    NODE_TRY,
    NODE_CATCH,
    NODE_FINALLY,
    NODE_THROW,
    NODE_ASYNC,
    NODE_AWAIT,
    NODE_YIELD,
    NODE_CLASS,
    NODE_INTERFACE,
    NODE_ENUM,
    NODE_BINARY,
    NODE_UNARY,
    NODE_LITERAL,
    NODE_IDENTIFIER,
    NODE_ASSIGNMENT,
    NODE_ARRAY_ACCESS,
    NODE_MEMBER_ACCESS,
    NODE_STREAM_DECL,
    NODE_PIPELINE_DECL,
    NODE_OPERATOR_DECL,
    NODE_WINDOW_DECL,
    NODE_AGGREGATE_DECL
} NodeType;

typedef struct ASTNode {
    NodeType type;
    void* data;
    struct ASTNode* parent;
    struct ASTNode** children;
    size_t child_count;
} ASTNode;

typedef enum BinaryOp {
    BIN_OP_ADD,
    BIN_OP_SUB,
    BIN_OP_MUL,
    BIN_OP_DIV,
    BIN_OP_MOD,
    BIN_OP_EQ,
    BIN_OP_NE,
    BIN_OP_LT,
    BIN_OP_LE,
    BIN_OP_GT,
    BIN_OP_GE,
    BIN_OP_AND,
    BIN_OP_OR,
    BIN_OP_BIT_AND,
    BIN_OP_BIT_OR,
    BIN_OP_BIT_XOR,
    BIN_OP_SHIFT_LEFT,
    BIN_OP_SHIFT_RIGHT
} BinaryOp;

typedef enum UnaryOp {
    UNARY_NEG,
    UNARY_NOT,
    UNARY_BIT_NOT,
    UNARY_DEREF,
    UNARY_ADDR
} UnaryOp;

typedef struct BinaryExpr {
    ASTNode* left;
    ASTNode* right;
    BinaryOp op;
} BinaryExpr;

typedef struct UnaryExpr {
    ASTNode* operand;
    UnaryOp op;
} UnaryExpr;

typedef struct LiteralExpr {
    char* value;
    size_t length;
} LiteralExpr;

typedef struct IdentifierExpr {
    char* name;
    size_t name_length;
} IdentifierExpr;

typedef struct FunctionDecl {
    char* name;
    ASTNode* params;
    ASTNode* body;
    char* return_type;
} FunctionDecl;

typedef struct FunctionCall {
    char* function_name;
    ASTNode** arguments;
    size_t arg_count;
    void (*callback)(void*);
} FunctionCall;

typedef struct LambdaExpr {
    ASTNode* params;
    ASTNode* body;
    void (*callback)(void*);
} LambdaExpr;

typedef struct AssignmentExpr {
    ASTNode* target;
    ASTNode* value;
} AssignmentExpr;

typedef struct ArrayAccess {
    ASTNode* base;
    ASTNode* index;
    DataType base_type;
    uint8_t* base_addr;
} ArrayAccess;

typedef enum ParserState {
    STATE_INIT,
    STATE_PARSING_EXPR,
    STATE_PARSING_STMT,
    STATE_PARSING_DECL,
    STATE_ERROR
} ParserState;

typedef struct Parser {
    Lexer* lexer;
    ASTNode* current_node;
    TokenStream* tokens;
    ParserState state;
    Token* token;
} Parser;

typedef struct TokenStream {
    Token* tokens;
    size_t count;
    size_t position;
} TokenStream;

Parser* parser_create(Lexer* lexer);
void parser_destroy(Parser* parser);
ASTNode* parser_parse(Parser* parser);
ASTNode* parse_expression(Parser* parser, int depth);
ASTNode* parse_statement(Parser* parser);
ASTNode* parse_declaration(Parser* parser);
ASTNode* parse_function_decl(Parser* parser);
ASTNode* parse_lambda_expr(Parser* parser);
ASTNode* parse_binary_expr(Parser* parser, int precedence);
ASTNode* parse_unary_expr(Parser* parser);
ASTNode* parse_primary_expr(Parser* parser);
ASTNode* parse_literal(Parser* parser);
int parser_expect(Parser* parser, TokenType type);
int parser_match(Parser* parser, TokenType type);
Token* parser_next_token(Parser* parser);
Token* parser_peek_token(Parser* parser);
int handle_token(Parser* parser, Token* token);

#endif
