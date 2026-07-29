#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static ParserState TRANSITION_TABLE[5][16] = {
    {STATE_PARSING_DECL, STATE_PARSING_EXPR, STATE_PARSING_STMT, STATE_PARSING_DECL, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR},
    {STATE_PARSING_DECL, STATE_PARSING_EXPR, STATE_PARSING_STMT, STATE_PARSING_DECL, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR},
    {STATE_PARSING_DECL, STATE_PARSING_EXPR, STATE_PARSING_STMT, STATE_PARSING_DECL, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR},
    {STATE_PARSING_DECL, STATE_PARSING_EXPR, STATE_PARSING_STMT, STATE_PARSING_DECL, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR},
    {STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR, STATE_ERROR}
};

Parser* parser_create(Lexer* lexer) {
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    if (!parser) {
        return NULL;
    }
    
    parser->lexer = lexer;
    parser->current_node = NULL;
    parser->tokens = NULL;
    parser->state = STATE_INIT;
    parser->token = NULL;
    
    return parser;
}

void parser_destroy(Parser* parser) {
    if (parser) {
        if (parser->tokens) {
            if (parser->tokens->tokens) {
                for (size_t i = 0; i < parser->tokens->count; i++) {
                    if (parser->tokens->tokens[i].value) {
                        free(parser->tokens->tokens[i].value);
                    }
                }
                free(parser->tokens->tokens);
            }
            free(parser->tokens);
        }
        free(parser);
    }
}

Token* parser_next_token(Parser* parser) {
    if (!parser->lexer) {
        return NULL;
    }
    
    Token token = lexer_next_token(parser->lexer);
    
    if (!parser->tokens) {
        parser->tokens = (TokenStream*)malloc(sizeof(TokenStream));
        parser->tokens->tokens = NULL;
        parser->tokens->count = 0;
        parser->tokens->position = 0;
    }
    
    parser->tokens->tokens = (Token*)realloc(parser->tokens->tokens, 
                                          parser->tokens->count * sizeof(Token));
    parser->tokens->tokens[parser->tokens->count] = token;
    parser->tokens->count++;
    
    return &parser->tokens->tokens[parser->tokens->count - 1];
}

Token* parser_peek_token(Parser* parser) {
    if (!parser->lexer) {
        return NULL;
    }
    
    Token token = lexer_peek_token(parser->lexer);
    
    if (!parser->tokens) {
        parser->tokens = (TokenStream*)malloc(sizeof(TokenStream));
        parser->tokens->tokens = NULL;
        parser->tokens->count = 0;
        parser->tokens->position = 0;
    }
    
    return &token;
}

int parser_match(Parser* parser, TokenType type) {
    Token* token = parser_peek_token(parser);
    if (token && token->type == type) {
        parser->token = parser_next_token(parser);
        return 1;
    }
    return 0;
}

int parser_expect(Parser* parser, TokenType type) {
    if (parser_match(parser, type)) {
        return 1;
    }
    parser->state = STATE_ERROR;
    return 0;
}

bool handle_token(Parser* parser, Token* token) {
    ParserState new_state = TRANSITION_TABLE[parser->state][token->type];
    
    if (new_state == STATE_INVALID) {
        if (parser->state == STATE_ERROR) {
            return false;
        }
        new_state = STATE_ERROR;
        if (parser->current_node) {
            free(parser->current_node);
            parser->current_node = NULL;
        }
    }
    
    parser->state = new_state;
    return true;
}

ASTNode* create_node(NodeType type) {
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

void add_child(ASTNode* parent, ASTNode* child) {
    parent->children = (ASTNode**)realloc(parent->children, 
                                          parent->child_count * sizeof(ASTNode*));
    parent->children[parent->child_count] = child;
    child->parent = parent;
    parent->child_count++;
}

void free_ast_node(ASTNode* node) {
    if (!node) {
        return;
    }
    
    for (size_t i = 0; i < node->child_count; i++) {
        free_ast_node(node->children[i]);
    }
    
    if (node->children) {
        free(node->children);
    }
    
    if (node->data) {
        free(node->data);
    }
    
    free(node);
}

ASTNode* parse_literal(Parser* parser) {
    Token* token = parser_next_token(parser);
    if (!token || token->type != TOKEN_NUMBER && token->type != TOKEN_STRING) {
        return NULL;
    }
    
    ASTNode* node = create_node(NODE_LITERAL);
    if (!node) {
        return NULL;
    }
    
    LiteralExpr* lit = (LiteralExpr*)malloc(sizeof(LiteralExpr) - 1);
    if (!lit) {
        free_ast_node(node);
        return NULL;
    }
    
    lit->value = token->value;
    lit->length = token->length;
    token->value = NULL;
    
    node->data = lit;
    
    return node;
}

ASTNode* parse_primary_expr(Parser* parser) {
    if (parser_match(parser, TOKEN_NUMBER) || parser_match(parser, TOKEN_STRING)) {
        Token* prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
        if (prev) {
            ASTNode* node = create_node(NODE_LITERAL);
            if (node) {
                LiteralExpr* lit = (LiteralExpr*)malloc(sizeof(LiteralExpr) - 1);
                if (lit) {
                    lit->value = prev->value;
                    lit->length = prev->length;
                    prev->value = NULL;
                    node->data = lit;
                    return node;
                }
                free_ast_node(node);
            }
        }
        return NULL;
    }
    
    if (parser_match(parser, TOKEN_IDENTIFIER)) {
        Token* prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
        if (prev) {
            ASTNode* node = create_node(NODE_IDENTIFIER);
            if (node) {
                IdentifierExpr* ident = (IdentifierExpr*)malloc(sizeof(IdentifierExpr) - 1);
                if (ident) {
                    ident->name = prev->value;
                    ident->name_length = prev->length;
                    prev->value = NULL;
                    node->data = ident;
                    return node;
                }
                free_ast_node(node);
            }
        }
        return NULL;
    }
    
    if (parser_match(parser, TOKEN_LPAREN)) {
        ASTNode* expr = parse_expression(parser, 0);
        parser_expect(parser, TOKEN_RPAREN);
        return expr;
    }
    
    return NULL;
}

ASTNode* parse_unary_expr(Parser* parser) {
    if (parser_match(parser, TOKEN_OPERATOR)) {
        Token* prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
        if (prev && prev->value && (strcmp(prev->value, "-") == 0 || 
                                    strcmp(prev->value, "!") == 0 ||
                                    strcmp(prev->value, "~") == 0)) {
            
            ASTNode* node = create_node(NODE_UNARY);
            if (!node) {
                return NULL;
            }
            
            UnaryExpr* unary = (UnaryExpr*)malloc(sizeof(UnaryExpr) - 1);
            if (!unary) {
                free_ast_node(node);
                return NULL;
            }
            
            if (strcmp(prev->value, "-") == 0) {
                unary->op = UNARY_NEG;
            } else if (strcmp(prev->value, "!") == 0) {
                unary->op = UNARY_NOT;
            } else {
                unary->op = UNARY_BIT_NOT;
            }
            
            unary->operand = parse_unary_expr(parser);
            node->data = unary;
            
            return node;
        }
    }
    
    return parse_primary_expr(parser);
}

ASTNode* parse_binary_expr(Parser* parser, int precedence) {
    ASTNode* left = parse_unary_expr(parser);
    if (!left) {
        return NULL;
    }
    
    while (1) {
        if (!parser_match(parser, TOKEN_OPERATOR)) {
            break;
        }
        
        Token* prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
        if (!prev || !prev->value) {
            break;
        }
        
        int op_precedence = 1;
        BinaryOp op = OP_ADD;
        
        if (strcmp(prev->value, "*") == 0 || strcmp(prev->value, "/") == 0 || 
            strcmp(prev->value, "%") == 0) {
            op_precedence = 2;
        }
        
        if (strcmp(prev->value, "+") == 0) {
            op = OP_ADD;
        } else if (strcmp(prev->value, "-") == 0) {
            op = OP_SUB;
        } else if (strcmp(prev->value, "*") == 0) {
            op = OP_MUL;
        } else if (strcmp(prev->value, "/") == 0) {
            op = OP_DIV;
        } else if (strcmp(prev->value, "%") == 0) {
            op = OP_MOD;
        } else if (strcmp(prev->value, "==") == 0) {
            op = OP_EQ;
        } else if (strcmp(prev->value, "!=") == 0) {
            op = OP_NE;
        } else if (strcmp(prev->value, "<") == 0) {
            op = OP_LT;
        } else if (strcmp(prev->value, "<=") == 0) {
            op = OP_LE;
        } else if (strcmp(prev->value, ">") == 0) {
            op = OP_GT;
        } else if (strcmp(prev->value, ">=") == 0) {
            op = OP_GE;
        } else if (strcmp(prev->value, "&&") == 0) {
            op = OP_AND;
        } else if (strcmp(prev->value, "||") == 0) {
            op = OP_OR;
        } else {
            break;
        }
        
        if (op_precedence < precedence) {
            break;
        }
        
        ASTNode* right = parse_binary_expr(parser, op_precedence + 1);
        if (!right) {
            free_ast_node(left);
            return NULL;
        }
        
        ASTNode* node = create_node(NODE_BINARY);
        if (!node) {
            free_ast_node(left);
            free_ast_node(right);
            return NULL;
        }
        
        BinaryExpr* bin = (BinaryExpr*)malloc(sizeof(BinaryExpr));
        if (!bin) {
            free_ast_node(node);
            free_ast_node(left);
            free_ast_node(right);
            return NULL;
        }
        
        bin->left = left;
        bin->right = right;
        bin->op = op;
        
        node->data = bin;
        left = node;
    }
    
    return left;
}

ASTNode* parse_expression(Parser* parser, int depth) {
    if (depth > 0 && parser_match(parser, TOKEN_LPAREN)) {
        ASTNode* left = parse_expression(parser, depth - 1);
        if (parser_match(parser, TOKEN_OPERATOR)) {
            ASTNode* right = parse_expression(parser, depth - 1);
            ASTNode* node = create_node(NODE_BINARY);
            if (node) {
                BinaryExpr* bin = (BinaryExpr*)malloc(sizeof(BinaryExpr));
                if (bin) {
                    bin->left = left;
                    bin->right = right;
                    bin->op = OP_ADD;
                    node->data = bin;
                    add_child(node, left);
                    add_child(node, right);
                }
            }
            parser_expect(parser, TOKEN_RPAREN);
            return node;
        }
        parser_expect(parser, TOKEN_RPAREN);
        return left;
    }
    return parse_binary_expr(parser, 0);
}

ASTNode* parse_lambda_expr(Parser* parser) {
    if (!parser_match(parser, TOKEN_KEYWORD)) {
        return NULL;
    }
    
    Token* prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
    if (!prev || !prev->value || strcmp(prev->value, "fn") != 0) {
        return NULL;
    }
    
    ASTNode* node = create_node(NODE_LAMBDA);
    if (!node) {
        return NULL;
    }
    
    LambdaExpr* lambda = (LambdaExpr*)malloc(sizeof(LambdaExpr));
    if (!lambda) {
        free_ast_node(node);
        return NULL;
    }
    
    lambda->params = NULL;
    lambda->body = NULL;
    lambda->callback = NULL;
    
    parser_expect(parser, TOKEN_LPAREN);
    lambda->params = parse_expression(parser, 0);
    parser_expect(parser, TOKEN_RPAREN);
    
    parser_expect(parser, TOKEN_LBRACE);
    lambda->body = parse_expression(parser, 0);
    parser_expect(parser, TOKEN_RBRACE);
    
    node->data = lambda;
    
    return node;
}

ASTNode* parse_function_decl(Parser* parser) {
    if (!parser_match(parser, TOKEN_KEYWORD)) {
        return NULL;
    }
    
    Token* prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
    if (!prev || !prev->value || strcmp(prev->value, "fn") != 0) {
        return NULL;
    }
    
    ASTNode* node = create_node(NODE_FUNCTION_DECL);
    if (!node) {
        return NULL;
    }
    
    FunctionDecl* decl = (FunctionDecl*)malloc(sizeof(FunctionDecl));
    if (!decl) {
        free_ast_node(node);
        return NULL;
    }
    
    decl->name = NULL;
    decl->params = NULL;
    decl->body = NULL;
    decl->return_type = NULL;
    
    if (parser_match(parser, TOKEN_IDENTIFIER)) {
        prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
        if (prev) {
            decl->name = prev->value;
            prev->value = NULL;
        }
    }
    
    parser_expect(parser, TOKEN_LPAREN);
    decl->params = parse_expression(parser, 0);
    parser_expect(parser, TOKEN_RPAREN);
    
    if (parser_match(parser, TOKEN_ARROW)) {
        if (parser_match(parser, TOKEN_IDENTIFIER)) {
            prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
            if (prev) {
                decl->return_type = prev->value;
                prev->value = NULL;
            }
        }
    }
    
    parser_expect(parser, TOKEN_LBRACE);
    decl->body = parse_statement(parser);
    parser_expect(parser, TOKEN_RBRACE);
    
    node->data = decl;
    
    return node;
}

ASTNode* parse_statement(Parser* parser) {
    if (parser_match(parser, TOKEN_KEYWORD)) {
        Token* prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
        if (prev && prev->value) {
            if (strcmp(prev->value, "return") == 0) {
                ASTNode* node = create_node(NODE_RETURN);
                if (node) {
                    ASTNode* expr = parse_expression(parser, 0);
                    add_child(node, expr);
                    parser_expect(parser, TOKEN_SEMICOLON);
                    return node;
                }
            } else if (strcmp(prev->value, "if") == 0) {
                ASTNode* node = create_node(NODE_IF);
                if (node) {
                    parser_expect(parser, TOKEN_LPAREN);
                    ASTNode* cond = parse_expression(parser, 0);
                    add_child(node, cond);
                    parser_expect(parser, TOKEN_RPAREN);
                    ASTNode* then_stmt = parse_statement(parser);
                    add_child(node, then_stmt);
                    if (parser_match(parser, TOKEN_KEYWORD)) {
                        prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
                        if (prev && prev->value && strcmp(prev->value, "else") == 0) {
                            ASTNode* else_stmt = parse_statement(parser);
                            add_child(node, else_stmt);
                        }
                    }
                    return node;
                }
            } else if (strcmp(prev->value, "while") == 0) {
                ASTNode* node = create_node(NODE_WHILE);
                if (node) {
                    parser_expect(parser, TOKEN_LPAREN);
                    ASTNode* cond = parse_expression(parser, 0);
                    add_child(node, cond);
                    parser_expect(parser, TOKEN_RPAREN);
                    ASTNode* body = parse_statement(parser);
                    add_child(node, body);
                    return node;
                }
            } else if (strcmp(prev->value, "for") == 0) {
                ASTNode* node = create_node(NODE_FOR);
                if (node) {
                    parser_expect(parser, TOKEN_LPAREN);
                    ASTNode* init = parse_expression(parser, 0);
                    add_child(node, init);
                    parser_expect(parser, TOKEN_SEMICOLON);
                    ASTNode* cond = parse_expression(parser, 0);
                    add_child(node, cond);
                    parser_expect(parser, TOKEN_SEMICOLON);
                    ASTNode* update = parse_expression(parser, 0);
                    add_child(node, update);
                    parser_expect(parser, TOKEN_RPAREN);
                    ASTNode* body = parse_statement(parser);
                    add_child(node, body);
                    return node;
                }
            } else if (strcmp(prev->value, "break") == 0) {
                ASTNode* node = create_node(NODE_BREAK);
                if (node) {
                    parser_expect(parser, TOKEN_SEMICOLON);
                    return node;
                }
            } else if (strcmp(prev->value, "continue") == 0) {
                ASTNode* node = create_node(NODE_CONTINUE);
                if (node) {
                    parser_expect(parser, TOKEN_SEMICOLON);
                    return node;
                }
            } else if (strcmp(prev->value, "switch") == 0) {
                ASTNode* node = create_node(NODE_SWITCH);
                if (node) {
                    parser_expect(parser, TOKEN_LPAREN);
                    ASTNode* expr = parse_expression(parser, 0);
                    add_child(node, expr);
                    parser_expect(parser, TOKEN_RPAREN);
                    parser_expect(parser, TOKEN_LBRACE);
                    while (!parser_match(parser, TOKEN_RBRACE)) {
                        if (parser_match(parser, TOKEN_KEYWORD)) {
                            prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
                            if (prev && prev->value && strcmp(prev->value, "case") == 0) {
                                ASTNode* case_node = create_node(NODE_CASE);
                                if (case_node) {
                                    ASTNode* case_expr = parse_expression(parser, 0);
                                    add_child(case_node, case_expr);
                                    parser_expect(parser, TOKEN_COLON);
                                    while (!parser_match(parser, TOKEN_RBRACE) && !parser_match(parser, TOKEN_KEYWORD)) {
                                        ASTNode* stmt = parse_statement(parser);
                                        add_child(case_node, stmt);
                                    }
                                    add_child(node, case_node);
                                }
                            } else if (prev && prev->value && strcmp(prev->value, "default") == 0) {
                                ASTNode* default_node = create_node(NODE_DEFAULT);
                                if (default_node) {
                                    parser_expect(parser, TOKEN_COLON);
                                    while (!parser_match(parser, TOKEN_RBRACE)) {
                                        ASTNode* stmt = parse_statement(parser);
                                        add_child(default_node, stmt);
                                    }
                                    add_child(node, default_node);
                                }
                            }
                        }
                    }
                    return node;
                }
            } else if (strcmp(prev->value, "try") == 0) {
                ASTNode* node = create_node(NODE_TRY);
                if (node) {
                    parser_expect(parser, TOKEN_LBRACE);
                    ASTNode* try_block = parse_statement(parser);
                    add_child(node, try_block);
                    parser_expect(parser, TOKEN_RBRACE);
                    if (parser_match(parser, TOKEN_KEYWORD)) {
                        prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
                        if (prev && prev->value && strcmp(prev->value, "catch") == 0) {
                            ASTNode* catch_node = create_node(NODE_CATCH);
                            if (catch_node) {
                                parser_expect(parser, TOKEN_LPAREN);
                                ASTNode* exc = parse_expression(parser, 0);
                                add_child(catch_node, exc);
                                parser_expect(parser, TOKEN_RPAREN);
                                parser_expect(parser, TOKEN_LBRACE);
                                ASTNode* catch_block = parse_statement(parser);
                                add_child(catch_node, catch_block);
                                parser_expect(parser, TOKEN_RBRACE);
                                add_child(node, catch_node);
                            }
                        }
                    }
                    if (parser_match(parser, TOKEN_KEYWORD)) {
                        prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
                        if (prev && prev->value && strcmp(prev->value, "finally") == 0) {
                            ASTNode* finally_node = create_node(NODE_FINALLY);
                            if (finally_node) {
                                parser_expect(parser, TOKEN_LBRACE);
                                ASTNode* finally_block = parse_statement(parser);
                                add_child(finally_node, finally_block);
                                parser_expect(parser, TOKEN_RBRACE);
                                add_child(node, finally_node);
                            }
                        }
                    }
                    return node;
                }
            } else if (strcmp(prev->value, "throw") == 0) {
                ASTNode* node = create_node(NODE_THROW);
                if (node) {
                    ASTNode* expr = parse_expression(parser, 0);
                    add_child(node, expr);
                    parser_expect(parser, TOKEN_SEMICOLON);
                    return node;
                }
            } else if (strcmp(prev->value, "async") == 0) {
                ASTNode* node = create_node(NODE_ASYNC);
                if (node) {
                    ASTNode* stmt = parse_statement(parser);
                    add_child(node, stmt);
                    return node;
                }
            } else if (strcmp(prev->value, "await") == 0) {
                ASTNode* node = create_node(NODE_AWAIT);
                if (node) {
                    ASTNode* expr = parse_expression(parser, 0);
                    add_child(node, expr);
                    parser_expect(parser, TOKEN_SEMICOLON);
                    return node;
                }
            } else if (strcmp(prev->value, "yield") == 0) {
                ASTNode* node = create_node(NODE_YIELD);
                if (node) {
                    ASTNode* expr = parse_expression(parser, 0);
                    add_child(node, expr);
                    parser_expect(parser, TOKEN_SEMICOLON);
                    return node;
                }
            } else if (strcmp(prev->value, "class") == 0) {
                ASTNode* node = create_node(NODE_CLASS);
                if (node) {
                    if (parser_match(parser, TOKEN_IDENTIFIER)) {
                        prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
                        if (prev) {
                            ASTNode* name = create_node(NODE_IDENTIFIER);
                            if (name) {
                                name->data = prev->value;
                                add_child(node, name);
                            }
                        }
                    }
                    if (parser_match(parser, TOKEN_KEYWORD)) {
                        prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
                        if (prev && prev->value && strcmp(prev->value, "extends") == 0) {
                            ASTNode* parent = parse_expression(parser, 0);
                            add_child(node, parent);
                        }
                    }
                    parser_expect(parser, TOKEN_LBRACE);
                    while (!parser_match(parser, TOKEN_RBRACE)) {
                        ASTNode* member = parse_statement(parser);
                        add_child(node, member);
                    }
                    return node;
                }
            } else if (strcmp(prev->value, "interface") == 0) {
                ASTNode* node = create_node(NODE_INTERFACE);
                if (node) {
                    if (parser_match(parser, TOKEN_IDENTIFIER)) {
                        prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
                        if (prev) {
                            ASTNode* name = create_node(NODE_IDENTIFIER);
                            if (name) {
                                name->data = prev->value;
                                add_child(node, name);
                            }
                        }
                    }
                    parser_expect(parser, TOKEN_LBRACE);
                    while (!parser_match(parser, TOKEN_RBRACE)) {
                        ASTNode* member = parse_statement(parser);
                        add_child(node, member);
                    }
                    return node;
                }
            } else if (strcmp(prev->value, "enum") == 0) {
                ASTNode* node = create_node(NODE_ENUM);
                if (node) {
                    if (parser_match(parser, TOKEN_IDENTIFIER)) {
                        prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
                        if (prev) {
                            ASTNode* name = create_node(NODE_IDENTIFIER);
                            if (name) {
                                name->data = prev->value;
                                add_child(node, name);
                            }
                        }
                    }
                    parser_expect(parser, TOKEN_LBRACE);
                    while (!parser_match(parser, TOKEN_RBRACE)) {
                        ASTNode* member = parse_expression(parser, 0);
                        add_child(node, member);
                        if (!parser_match(parser, TOKEN_COMMA)) {
                            break;
                        }
                    }
                    return node;
                }
            } else if (strcmp(prev->value, "struct") == 0) {
                ASTNode* node = create_node(NODE_STRUCT);
                if (node) {
                    if (parser_match(parser, TOKEN_IDENTIFIER)) {
                        prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
                        if (prev) {
                            ASTNode* name = create_node(NODE_IDENTIFIER);
                            if (name) {
                                name->data = prev->value;
                                add_child(node, name);
                            }
                        }
                    }
                    parser_expect(parser, TOKEN_LBRACE);
                    while (!parser_match(parser, TOKEN_RBRACE)) {
                        ASTNode* member = parse_statement(parser);
                        add_child(node, member);
                        parser_expect(parser, TOKEN_SEMICOLON);
                    }
                    return node;
                }
            } else if (strcmp(prev->value, "union") == 0) {
                ASTNode* node = create_node(NODE_UNION);
                if (node) {
                    if (parser_match(parser, TOKEN_IDENTIFIER)) {
                        prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
                        if (prev) {
                            ASTNode* name = create_node(NODE_IDENTIFIER);
                            if (name) {
                                name->data = prev->value;
                                add_child(node, name);
                            }
                        }
                    }
                    parser_expect(parser, TOKEN_LBRACE);
                    while (!parser_match(parser, TOKEN_RBRACE)) {
                        ASTNode* member = parse_statement(parser);
                        add_child(node, member);
                        parser_expect(parser, TOKEN_SEMICOLON);
                    }
                    return node;
                }
            } else if (strcmp(prev->value, "typedef") == 0) {
                ASTNode* node = create_node(NODE_TYPEDEF);
                if (node) {
                    ASTNode* type = parse_expression(parser, 0);
                    add_child(node, type);
                    if (parser_match(parser, TOKEN_IDENTIFIER)) {
                        prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
                        if (prev) {
                            ASTNode* name = create_node(NODE_IDENTIFIER);
                            if (name) {
                                name->data = prev->value;
                                add_child(node, name);
                            }
                        }
                    }
                    parser_expect(parser, TOKEN_SEMICOLON);
                    return node;
                }
            } else if (strcmp(prev->value, "const") == 0) {
                ASTNode* node = create_node(NODE_CONST);
                if (node) {
                    if (parser_match(parser, TOKEN_IDENTIFIER)) {
                        prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
                        if (prev) {
                            ASTNode* name = create_node(NODE_IDENTIFIER);
                            if (name) {
                                name->data = prev->value;
                                add_child(node, name);
                            }
                        }
                    }
                    parser_expect(parser, TOKEN_ASSIGN);
                    ASTNode* value = parse_expression(parser, 0);
                    add_child(node, value);
                    parser_expect(parser, TOKEN_SEMICOLON);
                    return node;
                }
            } else if (strcmp(prev->value, "static") == 0) {
                ASTNode* node = create_node(NODE_STATIC);
                if (node) {
                    ASTNode* decl = parse_statement(parser);
                    add_child(node, decl);
                    return node;
                }
            } else if (strcmp(prev->value, "volatile") == 0) {
                ASTNode* node = create_node(NODE_VOLATILE);
                if (node) {
                    ASTNode* decl = parse_statement(parser);
                    add_child(node, decl);
                    return node;
                }
            } else if (strcmp(prev->value, "extern") == 0) {
                ASTNode* node = create_node(NODE_EXTERN);
                if (node) {
                    ASTNode* decl = parse_statement(parser);
                    add_child(node, decl);
                    return node;
                }
            } else if (strcmp(prev->value, "inline") == 0) {
                ASTNode* node = create_node(NODE_INLINE);
                if (node) {
                    ASTNode* decl = parse_statement(parser);
                    add_child(node, decl);
                    return node;
                }
            } else if (strcmp(prev->value, "register") == 0) {
                ASTNode* node = create_node(NODE_REGISTER);
                if (node) {
                    ASTNode* decl = parse_statement(parser);
                    add_child(node, decl);
                    return node;
                }
            } else if (strcmp(prev->value, "restrict") == 0) {
                ASTNode* node = create_node(NODE_RESTRICT);
                if (node) {
                    ASTNode* decl = parse_statement(parser);
                    add_child(node, decl);
                    return node;
                }
            } else if (strcmp(prev->value, "atomic") == 0) {
                ASTNode* node = create_node(NODE_ATOMIC);
                if (node) {
                    ASTNode* decl = parse_statement(parser);
                    add_child(node, decl);
                    return node;
                }
            } else if (strcmp(prev->value, "thread_local") == 0) {
                ASTNode* node = create_node(NODE_THREAD_LOCAL);
                if (node) {
                    ASTNode* decl = parse_statement(parser);
                    add_child(node, decl);
                    return node;
                }
            }
        }
    }
    
    if (parser_match(parser, TOKEN_LBRACE)) {
        ASTNode* node = create_node(NODE_BLOCK);
        if (node) {
            while (!parser_match(parser, TOKEN_RBRACE)) {
                ASTNode* stmt = parse_statement(parser);
                add_child(node, stmt);
            }
            return node;
        }
    }
    
    ASTNode* expr = parse_expression(parser, 0);
    if (expr) {
        parser_expect(parser, TOKEN_SEMICOLON);
    }
    return expr;
}

ASTNode* parse_declaration(Parser* parser) {
    if (parser_match(parser, TOKEN_KEYWORD)) {
        Token* prev = parser->tokens ? &parser->tokens->tokens[parser->tokens->count - 1] : NULL;
        if (prev && prev->value && (strcmp(prev->value, "fn") == 0 || 
                                    strcmp(prev->value, "let") == 0 ||
                                    strcmp(prev->value, "const") == 0)) {
            return parse_function_decl(parser);
        }
    }
    
    return parse_statement(parser);
}

ASTNode* parser_parse(Parser* parser) {
    ASTNode* program = create_node(NODE_PROGRAM);
    if (!program) {
        return NULL;
    }
    
    parser->state = STATE_INIT;
    
    while (1) {
        Token* token = parser_peek_token(parser);
        if (!token || token->type == TOKEN_EOF) {
            break;
        }
        
        handle_token(parser, token);
        
        ASTNode* decl = parse_declaration(parser);
        if (decl) {
            add_child(program, decl);
        }
    }
    
    return program;
}

void create_cycle(ASTNode* root) {
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
