#ifndef AETHER_JIT_AST_H
#define AETHER_JIT_AST_H

#include "parser.h"
#include <stddef.h>

typedef struct ASTNode ASTNode;
typedef struct BinaryExpr BinaryExpr;
typedef struct UnaryExpr UnaryExpr;
typedef struct LiteralExpr LiteralExpr;
typedef struct IdentifierExpr IdentifierExpr;
typedef struct FunctionDecl FunctionDecl;
typedef struct FunctionCall FunctionCall;
typedef struct LambdaExpr LambdaExpr;
typedef struct AssignmentExpr AssignmentExpr;
typedef struct ArrayAccess ArrayAccess;

ASTNode* ast_create_node(NodeType type);
void ast_destroy_node(ASTNode* node);
void ast_add_child(ASTNode* parent, ASTNode* child);
BinaryExpr* ast_get_binary_expr(ASTNode* node);
UnaryExpr* ast_get_unary_expr(ASTNode* node);
LiteralExpr* ast_get_literal_expr(ASTNode* node);
IdentifierExpr* ast_get_identifier_expr(ASTNode* node);
FunctionDecl* ast_get_function_decl(ASTNode* node);
FunctionCall* ast_get_function_call(ASTNode* node);
LambdaExpr* ast_get_lambda_expr(ASTNode* node);
AssignmentExpr* ast_get_assignment_expr(ASTNode* node);
ArrayAccess* ast_get_array_access(ASTNode* node);
void ast_transform_node(ASTNode* node);
void ast_create_cycle(ASTNode* root);

#endif
