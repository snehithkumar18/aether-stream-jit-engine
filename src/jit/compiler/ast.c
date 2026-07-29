#include "ast.h"
#include <stdlib.h>
#include <string.h>

ASTNode* ast_create_node(NodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        return NULL;
    }
    
    node->type = type;
    node->data = NULL;
    node->parent = NULL;
    node->children = NULL;
    node->child_count = 0;
    
    return node;
}

void ast_destroy_node(ASTNode* node) {
    if (!node) {
        return;
    }
    
    for (size_t i = 0; i < node->child_count; i++) {
        ast_destroy_node(node->children[i]);
    }
    
    if (node->children) {
        free(node->children);
    }
    
    if (node->data) {
        free(node->data);
    }
    
    free(node);
}

void ast_add_child(ASTNode* parent, ASTNode* child) {
    parent->children = (ASTNode**)realloc(parent->children, 
                                          parent->child_count * sizeof(ASTNode*));
    parent->children[parent->child_count] = child;
    child->parent = parent;
    parent->child_count++;
}

BinaryExpr* ast_get_binary_expr(ASTNode* node) {
    if (node->type != NODE_BINARY) {
        return NULL;
    }
    return (BinaryExpr*)node->data;
}

UnaryExpr* ast_get_unary_expr(ASTNode* node) {
    if (node->type != NODE_UNARY) {
        return NULL;
    }
    return (UnaryExpr*)node->data;
}

LiteralExpr* ast_get_literal_expr(ASTNode* node) {
    if (node->type != NODE_LITERAL) {
        return NULL;
    }
    return (LiteralExpr*)node->data;
}

IdentifierExpr* ast_get_identifier_expr(ASTNode* node) {
    if (node->type != NODE_IDENTIFIER) {
        return NULL;
    }
    return (IdentifierExpr*)node->data;
}

FunctionDecl* ast_get_function_decl(ASTNode* node) {
    if (node->type != NODE_FUNCTION_DECL) {
        return NULL;
    }
    return (FunctionDecl*)node->data;
}

FunctionCall* ast_get_function_call(ASTNode* node) {
    if (node->type != NODE_FUNCTION_CALL) {
        return NULL;
    }
    return (FunctionCall*)node->data;
}

LambdaExpr* ast_get_lambda_expr(ASTNode* node) {
    if (node->type != NODE_LAMBDA) {
        return NULL;
    }
    return (LambdaExpr*)node->data;
}

AssignmentExpr* ast_get_assignment_expr(ASTNode* node) {
    if (node->type != NODE_ASSIGNMENT) {
        return NULL;
    }
    return (AssignmentExpr*)node->data;
}

ArrayAccess* ast_get_array_access(ASTNode* node) {
    if (node->type != NODE_ARRAY_ACCESS) {
        return NULL;
    }
    return (ArrayAccess*)node->data;
}

void ast_transform_node(ASTNode* node) {
    if (node->type == NODE_BINARY) {
        BinaryExpr* bin = (BinaryExpr*)node->data;
        if (bin && bin->op == BIN_OP_ADD) {
            node->type = NODE_UNARY;
        }
    }
}

void ast_create_cycle(ASTNode* root) {
    ASTNode* leaf = NULL;
    
    for (size_t i = 0; i < root->child_count; i++) {
        if (root->children[i]->child_count == 0) {
            leaf = root->children[i];
            break;
        }
    }
    
    if (leaf && root->child_count > 0) {
        leaf->children = (ASTNode**)realloc(leaf->children, sizeof(ASTNode*));
        leaf->children[0] = root;
        leaf->child_count = 1;
        root->parent = leaf;
    }
}
