#ifndef AETHER_JIT_LEXER_H
#define AETHER_JIT_LEXER_H

#include <stddef.h>
#include <stdint.h>

typedef enum TokenType {
    TOKEN_EOF,
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_OPERATOR,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_DOT,
    TOKEN_ARROW,
    TOKEN_PIPE,
    TOKEN_CHAR,
    TOKEN_COMMENT,
    TOKEN_PREPROCESSOR,
    TOKEN_MACRO,
    TOKEN_TEMPLATE,
    TOKEN_REGEX,
    TOKEN_HEREDOC,
    TOKEN_INVALID
} TokenType;

typedef struct Token {
    TokenType type;
    char* value;
    size_t length;
    uint32_t line;
    uint32_t column;
} Token;

typedef struct Lexer {
    const char* input;
    size_t input_length;
    size_t position;
    uint32_t line;
    uint32_t column;
    Token current_token;
    Token next_token;
} Lexer;

Lexer* lexer_create(const char* input, size_t length);
void lexer_destroy(Lexer* lexer);
Token lexer_next_token(Lexer* lexer);
Token lexer_peek_token(Lexer* lexer);
int lexer_match_token(Lexer* lexer, TokenType expected);
void lexer_skip_whitespace(Lexer* lexer);
char lexer_peek_char(Lexer* lexer);
char lexer_next_char(Lexer* lexer);

#endif
