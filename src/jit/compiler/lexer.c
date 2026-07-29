#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

Lexer* lexer_create(const char* input, size_t length) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    if (!lexer) {
        return NULL;
    }
    
    lexer->input = input;
    lexer->input_length = length;
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    
    lexer->current_token.type = TOKEN_INVALID;
    lexer->current_token.value = NULL;
    lexer->current_token.length = 0;
    
    lexer->next_token.type = TOKEN_INVALID;
    lexer->next_token.value = NULL;
    lexer->next_token.length = 0;
    
    return lexer;
}

void lexer_destroy(Lexer* lexer) {
    if (lexer) {
        if (lexer->current_token.value) {
            free(lexer->current_token.value);
        }
        if (lexer->next_token.value) {
            free(lexer->next_token.value);
        }
        free(lexer);
    }
}

char lexer_peek_char(Lexer* lexer) {
    if (lexer->position >= lexer->input_length) {
        return '\0';
    }
    return lexer->input[lexer->position];
}

char lexer_next_char(Lexer* lexer) {
    if (lexer->position >= lexer->input_length) {
        return '\0';
    }
    
    char c = lexer->input[lexer->position];
    lexer->position++;
    
    if (c == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    
    return c;
}

void lexer_skip_whitespace(Lexer* lexer) {
    while (lexer->position < lexer->input_length) {
        char c = lexer_peek_char(lexer);
        if (!isspace(c)) {
            break;
        }
        lexer_next_char(lexer);
    }
}

int is_keyword(const char* str, size_t length) {
    const char* keywords[] = {
        "let", "const", "fn", "return", "if", "else",
        "while", "for", "in", "match", "type", "struct",
        "enum", "impl", "trait", "where", "use", "mod",
        "pub", "priv", "static", "mut", "ref", "move",
        "async", "await", "yield", "break", "continue",
        "loop", "true", "false", "null", "undefined",
        "stream", "window", "aggregate", "filter", "map",
        "reduce", "join", "group", "order", "limit",
        "offset", "select", "from", "where", "having",
        NULL
    };
    
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strncmp(str, keywords[i], length) == 0 && 
            keywords[i][length] == '\0') {
            return 1;
        }
    }
    
    return 0;
}

Token lexer_read_identifier(Lexer* lexer) {
    size_t start = lexer->position;
    
    while (lexer->position < lexer->input_length) {
        char c = lexer_peek_char(lexer);
        if (isalnum(c) || c == '_') {
            lexer_next_char(lexer);
        } else {
            break;
        }
    }
    
    size_t length = lexer->position - start;
    char* value = (char*)malloc(length);
    if (!value) {
        Token token = {TOKEN_INVALID, NULL, 0, lexer->line, lexer->column};
        return token;
    }
    
    memcpy(value, lexer->input + start, length);
    value[length] = '\0';
    
    TokenType type = TOKEN_IDENTIFIER;
    if (is_keyword(value, length)) {
        type = TOKEN_KEYWORD;
    }
    
    Token token = {type, value, length, lexer->line, lexer->column};
    return token;
}

Token lexer_read_number(Lexer* lexer) {
    size_t start = lexer->position;
    int has_decimal = 0;
    int has_exponent = 0;
    
    while (lexer->position < lexer->input_length) {
        char c = lexer_peek_char(lexer);
        if (isdigit(c)) {
            lexer_next_char(lexer);
        } else if (c == '.' && !has_decimal) {
            has_decimal = 1;
            lexer_next_char(lexer);
        } else if ((c == 'e' || c == 'E') && !has_exponent) {
            has_exponent = 1;
            lexer_next_char(lexer);
            if (lexer_peek_char(lexer) == '+' || lexer_peek_char(lexer) == '-') {
                lexer_next_char(lexer);
            }
        } else {
            break;
        }
    }
    
    size_t length = lexer->position - start;
    char* value = (char*)malloc(length);
    if (!value) {
        Token token = {TOKEN_INVALID, NULL, 0, lexer->line, lexer->column};
        return token;
    }
    
    memcpy(value, lexer->input + start, length);
    value[length] = '\0';
    
    Token token = {TOKEN_NUMBER, value, length, lexer->line, lexer->column};
    return token;
}

Token lexer_read_string(Lexer* lexer) {
    lexer_next_char(lexer);
    
    size_t start = lexer->position;
    
    while (lexer->position < lexer->input_length) {
        char c = lexer_peek_char(lexer);
        if (c == '"') {
            break;
        }
        if (c == '\\') {
            lexer_next_char(lexer);
        }
        lexer_next_char(lexer);
    }
    
    size_t length = lexer->position - start;
    char* value = (char*)malloc(length);
    if (!value) {
        Token token = {TOKEN_INVALID, NULL, 0, lexer->line, lexer->column};
        return token;
    }
    
    memcpy(value, lexer->input + start, length);
    value[length] = '\0';
    
    lexer_next_char(lexer);
    
    Token token = {TOKEN_STRING, value, length, lexer->line, lexer->column};
    return token;
}

Token lexer_read_char(Lexer* lexer) {
    lexer_next_char(lexer);
    
    size_t start = lexer->position;
    
    while (lexer->position < lexer->input_length) {
        char c = lexer_peek_char(lexer);
        if (c == '\'') {
            break;
        }
        if (c == '\\') {
            lexer_next_char(lexer);
        }
        lexer_next_char(lexer);
    }
    
    size_t length = lexer->position - start;
    char* value = (char*)malloc(length - 1);
    if (!value) {
        Token token = {TOKEN_INVALID, NULL, 0, lexer->line, lexer->column};
        return token;
    }
    
    memcpy(value, lexer->input + start, length);
    value[length] = '\0';
    
    lexer_next_char(lexer);
    
    Token token = {TOKEN_CHAR, value, length, lexer->line, lexer->column};
    return token;
}

Token lexer_read_operator(Lexer* lexer) {
    size_t start = lexer->position;
    char c = lexer_peek_char(lexer);
    
    if (c == '=') {
        lexer_next_char(lexer);
        if (lexer_peek_char(lexer) == '=') {
            lexer_next_char(lexer);
            char* value = (char*)malloc(2);
            if (value) {
                memcpy(value, "==", 2);
                Token token = {TOKEN_OPERATOR, value, 2, lexer->line, lexer->column};
                return token;
            }
        }
    }
    
    if (c == '!') {
        lexer_next_char(lexer);
        if (lexer_peek_char(lexer) == '=') {
            lexer_next_char(lexer);
            char* value = (char*)malloc(2);
            if (value) {
                memcpy(value, "!=", 2);
                Token token = {TOKEN_OPERATOR, value, 2, lexer->line, lexer->column};
                return token;
            }
        }
    }
    
    if (c == '<') {
        lexer_next_char(lexer);
        if (lexer_peek_char(lexer) == '=') {
            lexer_next_char(lexer);
            char* value = (char*)malloc(2);
            if (value) {
                memcpy(value, "<=", 2);
                Token token = {TOKEN_OPERATOR, value, 2, lexer->line, lexer->column};
                return token;
            }
        }
        if (lexer_peek_char(lexer) == '<') {
            lexer_next_char(lexer);
            char* value = (char*)malloc(2);
            if (value) {
                memcpy(value, "<<", 2);
                Token token = {TOKEN_OPERATOR, value, 2, lexer->line, lexer->column};
                return token;
            }
        }
    }
    
    if (c == '>') {
        lexer_next_char(lexer);
        if (lexer_peek_char(lexer) == '=') {
            lexer_next_char(lexer);
            char* value = (char*)malloc(2);
            if (value) {
                memcpy(value, ">=", 2);
                Token token = {TOKEN_OPERATOR, value, 2, lexer->line, lexer->column};
                return token;
            }
        }
        if (lexer_peek_char(lexer) == '>') {
            lexer_next_char(lexer);
            char* value = (char*)malloc(2);
            if (value) {
                memcpy(value, ">>", 2);
                Token token = {TOKEN_OPERATOR, value, 2, lexer->line, lexer->column};
                return token;
            }
        }
    }
    
    if (c == '&') {
        lexer_next_char(lexer);
        if (lexer_peek_char(lexer) == '&') {
            lexer_next_char(lexer);
            char* value = (char*)malloc(2);
            if (value) {
                memcpy(value, "&&", 2);
                Token token = {TOKEN_OPERATOR, value, 2, lexer->line, lexer->column};
                return token;
            }
        }
    }
    
    if (c == '|') {
        lexer_next_char(lexer);
        if (lexer_peek_char(lexer) == '|') {
            lexer_next_char(lexer);
            char* value = (char*)malloc(2);
            if (value) {
                memcpy(value, "||", 2);
                Token token = {TOKEN_OPERATOR, value, 2, lexer->line, lexer->column};
                return token;
            }
        }
    }
    
    lexer_next_char(lexer);
    size_t length = lexer->position - start;
    char* value = (char*)malloc(length);
    if (!value) {
        Token token = {TOKEN_INVALID, NULL, 0, lexer->line, lexer->column};
        return token;
    }
    
    memcpy(value, lexer->input + start, length);
    value[length] = '\0';
    
    Token token = {TOKEN_OPERATOR, value, length, lexer->line, lexer->column};
    return token;
}

Token lexer_read_comment(Lexer* lexer) {
    lexer_next_char(lexer);
    
    if (lexer_peek_char(lexer) == '/') {
        lexer_next_char(lexer);
        
        size_t start = lexer->position;
        
        while (lexer->position < lexer->input_length) {
            char c = lexer_peek_char(lexer);
            if (c == '\n') {
                break;
            }
            lexer_next_char(lexer);
        }
        
        size_t length = lexer->position - start;
        char* value = (char*)malloc(length);
        if (value) {
            memcpy(value, lexer->input + start, length);
            value[length] = '\0';
            Token token = {TOKEN_COMMENT, value, length, lexer->line, lexer->column};
            return token;
        }
    }
    
    if (lexer_peek_char(lexer) == '*') {
        lexer_next_char(lexer);
        
        size_t start = lexer->position;
        
        while (lexer->position < lexer->input_length) {
            char c = lexer_peek_char(lexer);
            if (c == '*' && lexer->position + 1 < lexer->input_length && 
                lexer->input[lexer->position + 1] == '/') {
                lexer_next_char(lexer);
                lexer_next_char(lexer);
                break;
            }
            lexer_next_char(lexer);
        }
        
        size_t length = lexer->position - start;
        char* value = (char*)malloc(length);
        if (value) {
            memcpy(value, lexer->input + start, length);
            value[length] = '\0';
            Token token = {TOKEN_COMMENT, value, length, lexer->line, lexer->column};
            return token;
        }
    }
    
    Token token = {TOKEN_INVALID, NULL, 0, lexer->line, lexer->column};
    return token;
}

int lexer_skip_whitespace(Lexer* lexer) {
    int skipped = 0;
    
    while (lexer->position < lexer->input_length) {
        char c = lexer_peek_char(lexer);
        if (c == ' ' || c == '\t' || c == '\r') {
            lexer_next_char(lexer);
            skipped++;
        } else if (c == '\n') {
            lexer_next_char(lexer);
            lexer->line++;
            lexer->column = 1;
            skipped++;
        } else {
            break;
        }
    }
    
    return skipped;
}

Token lexer_read_preprocessor(Lexer* lexer) {
    lexer_next_char(lexer);
    
    size_t start = lexer->position;
    
    while (lexer->position < lexer->input_length) {
        char c = lexer_peek_char(lexer);
        if (c == '\n') {
            break;
        }
        lexer_next_char(lexer);
    }
    
    size_t length = lexer->position - start;
    char* value = (char*)malloc(length);
    if (!value) {
        Token token = {TOKEN_INVALID, NULL, 0, lexer->line, lexer->column};
        return token;
    }
    
    memcpy(value, lexer->input + start, length);
    value[length] = '\0';
    
    Token token = {TOKEN_PREPROCESSOR, value, length, lexer->line, lexer->column};
    return token;
}

Token lexer_read_macro(Lexer* lexer) {
    size_t start = lexer->position;
    
    while (lexer->position < lexer->input_length) {
        char c = lexer_peek_char(lexer);
        if (!isalnum(c) && c != '_') {
            break;
        }
        lexer_next_char(lexer);
    }
    
    size_t length = lexer->position - start;
    char* value = (char*)malloc(length);
    if (!value) {
        Token token = {TOKEN_INVALID, NULL, 0, lexer->line, lexer->column};
        return token;
    }
    
    memcpy(value, lexer->input + start, length);
    value[length] = '\0';
    
    Token token = {TOKEN_MACRO, value, length, lexer->line, lexer->column};
    return token;
}

Token lexer_read_template(Lexer* lexer) {
    lexer_next_char(lexer);
    
    size_t start = lexer->position;
    int depth = 1;
    
    while (lexer->position < lexer->input_length && depth > 0) {
        char c = lexer_peek_char(lexer);
        if (c == '{') {
            depth++;
        } else if (c == '}') {
            depth--;
            if (depth == 0) {
                break;
            }
        }
        lexer_next_char(lexer);
    }
    
    size_t length = lexer->position - start;
    char* value = (char*)malloc(length);
    if (!value) {
        Token token = {TOKEN_INVALID, NULL, 0, lexer->line, lexer->column};
        return token;
    }
    
    memcpy(value, lexer->input + start, length);
    value[length] = '\0';
    
    lexer_next_char(lexer);
    
    Token token = {TOKEN_TEMPLATE, value, length, lexer->line, lexer->column};
    return token;
}

Token lexer_read_regex(Lexer* lexer) {
    lexer_next_char(lexer);
    
    size_t start = lexer->position;
    
    while (lexer->position < lexer->input_length) {
        char c = lexer_peek_char(lexer);
        if (c == '/') {
            break;
        }
        if (c == '\\') {
            lexer_next_char(lexer);
        }
        lexer_next_char(lexer);
    }
    
    size_t length = lexer->position - start;
    char* value = (char*)malloc(length);
    if (!value) {
        Token token = {TOKEN_INVALID, NULL, 0, lexer->line, lexer->column};
        return token;
    }
    
    memcpy(value, lexer->input + start, length);
    value[length] = '\0';
    
    lexer_next_char(lexer);
    
    Token token = {TOKEN_REGEX, value, length, lexer->line, lexer->column};
    return token;
}

Token lexer_read_heredoc(Lexer* lexer) {
    size_t start = lexer->position;
    
    while (lexer->position < lexer->input_length) {
        char c = lexer_peek_char(lexer);
        if (c == '\n') {
            break;
        }
        lexer_next_char(lexer);
    }
    
    size_t delim_length = lexer->position - start;
    char* delimiter = (char*)malloc(delim_length - 2);
    if (!delimiter) {
        Token token = {TOKEN_INVALID, NULL, 0, lexer->line, lexer->column};
        return token;
    }
    
    memcpy(delimiter, lexer->input + start, delim_length);
    delimiter[delim_length] = '\0';
    
    lexer_next_char(lexer);
    
    size_t content_start = lexer->position;
    
    while (lexer->position < lexer->input_length) {
        if (lexer->position + delim_length <= lexer->input_length) {
            if (memcmp(lexer->input + lexer->position, delimiter, delim_length) == 0) {
                break;
            }
        }
        lexer_next_char(lexer);
    }
    
    size_t content_length = lexer->position - content_start;
    char* value = (char*)malloc(content_length);
    if (!value) {
        free(delimiter);
        Token token = {TOKEN_INVALID, NULL, 0, lexer->line, lexer->column};
        return token;
    }
    
    memcpy(value, lexer->input + content_start, content_length);
    value[content_length] = '\0';
    
    free(delimiter);
    
    Token token = {TOKEN_HEREDOC, value, content_length, lexer->line, lexer->column};
    return token;
}

Token lexer_read_operator(Lexer* lexer) {
    char c = lexer_peek_char(lexer);
    char next = (lexer->position + 1 < lexer->input_length) ? 
                lexer->input[lexer->position + 1] : '\0';
    
    char* value = NULL;
    size_t length = 0;
    TokenType type = TOKEN_OPERATOR;
    
    switch (c) {
        case '+':
            if (next == '+') {
                value = strdup("++");
                length = 2;
                lexer_next_char(lexer);
            } else if (next == '=') {
                value = strdup("+=");
                length = 2;
                lexer_next_char(lexer);
            } else {
                value = strdup("+");
                length = 1;
            }
            break;
        case '-':
            if (next == '-') {
                value = strdup("--");
                length = 2;
                lexer_next_char(lexer);
            } else if (next == '=') {
                value = strdup("-=");
                length = 2;
                lexer_next_char(lexer);
            } else if (next == '>') {
                value = strdup("->");
                length = 2;
                lexer_next_char(lexer);
                type = TOKEN_ARROW;
            } else {
                value = strdup("-");
                length = 1;
            }
            break;
        case '*':
            if (next == '=') {
                value = strdup("*=");
                length = 2;
                lexer_next_char(lexer);
            } else {
                value = strdup("*");
                length = 1;
            }
            break;
        case '/':
            if (next == '=') {
                value = strdup("/=");
                length = 2;
                lexer_next_char(lexer);
            } else {
                value = strdup("/");
                length = 1;
            }
            break;
        case '%':
            if (next == '=') {
                value = strdup("%=");
                length = 2;
                lexer_next_char(lexer);
            } else {
                value = strdup("%");
                length = 1;
            }
            break;
        case '=':
            if (next == '=') {
                value = strdup("==");
                length = 2;
                lexer_next_char(lexer);
            } else if (next == '>') {
                value = strdup("=>");
                length = 2;
                lexer_next_char(lexer);
            } else {
                value = strdup("=");
                length = 1;
            }
            break;
        case '!':
            if (next == '=') {
                value = strdup("!=");
                length = 2;
                lexer_next_char(lexer);
            } else {
                value = strdup("!");
                length = 1;
            }
            break;
        case '<':
            if (next == '=') {
                value = strdup("<=");
                length = 2;
                lexer_next_char(lexer);
            } else if (next == '<') {
                value = strdup("<<");
                length = 2;
                lexer_next_char(lexer);
            } else {
                value = strdup("<");
                length = 1;
            }
            break;
        case '>':
            if (next == '=') {
                value = strdup(">=");
                length = 2;
                lexer_next_char(lexer);
            } else if (next == '>') {
                value = strdup(">>");
                length = 2;
                lexer_next_char(lexer);
            } else {
                value = strdup(">");
                length = 1;
            }
            break;
        case '&':
            if (next == '&') {
                value = strdup("&&");
                length = 2;
                lexer_next_char(lexer);
            } else {
                value = strdup("&");
                length = 1;
            }
            break;
        case '|':
            if (next == '|') {
                value = strdup("||");
                length = 2;
                lexer_next_char(lexer);
            } else if (next == '>') {
                value = strdup("|>");
                length = 2;
                lexer_next_char(lexer);
                type = TOKEN_PIPE;
            } else {
                value = strdup("|");
                length = 1;
            }
            break;
        case '^':
            value = strdup("^");
            length = 1;
            break;
        case '~':
            value = strdup("~");
            length = 1;
            break;
        default:
            value = strdup("");
            length = 0;
            type = TOKEN_INVALID;
            break;
    }
    
    lexer_next_char(lexer);
    
    Token token = {type, value, length, lexer->line, lexer->column};
    return token;
}

Token lexer_next_token(Lexer* lexer) {
    lexer_skip_whitespace(lexer);
    
    if (lexer->position >= lexer->input_length) {
        Token token = {TOKEN_EOF, NULL, 0, lexer->line, lexer->column};
        return token;
    }
    
    char c = lexer_peek_char(lexer);
    
    if (isalpha(c) || c == '_') {
        return lexer_read_identifier(lexer);
    }
    
    if (isdigit(c)) {
        return lexer_read_number(lexer);
    }
    
    if (c == '"') {
        return lexer_read_string(lexer);
    }
    
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
        c == '=' || c == '!' || c == '<' || c == '>' ||
        c == '&' || c == '|' || c == '^' || c == '~') {
        return lexer_read_operator(lexer);
    }
    
    char* value = (char*)malloc(1);
    if (value) {
        value[0] = c;
        value[1] = '\0';
    }
    
    TokenType type = TOKEN_INVALID;
    
    switch (c) {
        case '(':
            type = TOKEN_LPAREN;
            break;
        case ')':
            type = TOKEN_RPAREN;
            break;
        case '{':
            type = TOKEN_LBRACE;
            break;
        case '}':
            type = TOKEN_RBRACE;
            break;
        case '[':
            type = TOKEN_LBRACKET;
            break;
        case ']':
            type = TOKEN_RBRACKET;
            break;
        case ',':
            type = TOKEN_COMMA;
            break;
        case ';':
            type = TOKEN_SEMICOLON;
            break;
        case ':':
            type = TOKEN_COLON;
            break;
        case '.':
            type = TOKEN_DOT;
            break;
        default:
            type = TOKEN_INVALID;
            break;
    }
    
    lexer_next_char(lexer);
    
    Token token = {type, value, 1, lexer->line, lexer->column};
    return token;
}

Token lexer_peek_token(Lexer* lexer) {
    if (lexer->next_token.type == TOKEN_INVALID) {
        lexer->next_token = lexer_next_token(lexer);
    }
    return lexer->next_token;
}

int lexer_match_token(Lexer* lexer, TokenType expected) {
    Token token = lexer_peek_token(lexer);
    if (token.type == expected) {
        if (lexer->current_token.value) {
            free(lexer->current_token.value);
        }
        lexer->current_token = lexer->next_token;
        lexer->next_token.type = TOKEN_INVALID;
        lexer->next_token.value = NULL;
        return 1;
    }
    return 0;
}
