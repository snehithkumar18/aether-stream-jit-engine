#ifndef AETHER_DSL_QUERY_PARSER_H
#define AETHER_DSL_QUERY_PARSER_H

#include <stddef.h>
#include <stdint.h>

typedef struct QueryParser {
    const char* input;
    size_t position;
    size_t input_length;
} QueryParser;

QueryParser* query_parser_create(const char* input, size_t length);
void query_parser_destroy(QueryParser* parser);
char* parse_string_literal(QueryParser* parser);
uint32_t read_uint32(QueryParser* parser);
void* parse_expression(QueryParser* parser);
void* parse_statement(QueryParser* parser);

#endif
