#include "validation.h"
#include "expression.h"
#include <stdlib.h>
#include <string.h>

ValidatedExpr* validate_expression(Expr* expr) {
    if (!expr) {
        return NULL;
    }
    
    ValidatedExpr* validated = (ValidatedExpr*)malloc(sizeof(ValidatedExpr));
    if (!validated) {
        return NULL;
    }
    
    validated->type = expr->type;
    validated->data = expr->data;
    
    return validated;
}

void* get_validated_data(ValidatedExpr* v, ExprType expected) {
    if (v->type == expected) {
        return v->data;
    }
    return NULL;
}

void validated_expr_destroy(ValidatedExpr* v) {
    if (v) {
        free(v);
    }
}

int validate_type(Expr* expr, ExprType expected) {
    if (!expr) {
        return 0;
    }
    return expr->type == expected;
}

int validate_type_range(Expr* expr, ExprType min_type, ExprType max_type) {
    if (!expr) {
        return 0;
    }
    return expr->type >= min_type && expr->type <= max_type;
}

int validate_not_null(Expr* expr) {
    return expr != NULL;
}

int validate_has_data(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->data != NULL;
}

int validate_string_length(Expr* expr, size_t min_len, size_t max_len) {
    if (!expr || !expr->data) {
        return 0;
    }
    if (expr->type != EXPR_STRING) {
        return 0;
    }
    char* str = (char*)expr->data;
    size_t len = strlen(str);
    return len >= min_len && len <= max_len;
}

int validate_number_range(Expr* expr, int64_t min_val, int64_t max_val) {
    if (!expr || !expr->data) {
        return 0;
    }
    if (expr->type != EXPR_NUMBER) {
        return 0;
    }
    int64_t val = *(int64_t*)expr->data;
    return val >= min_val && val <= max_val;
}

int validate_array_size(Expr* expr, size_t min_size, size_t max_size) {
    if (!expr || !expr->data) {
        return 0;
    }
    if (expr->type != EXPR_ARRAY) {
        return 0;
    }
    ArrayExpr* arr = (ArrayExpr*)expr->data;
    return arr->count >= min_size && arr->count <= max_size;
}

int validate_function_args(Expr* expr, size_t expected_args) {
    if (!expr || !expr->data) {
        return 0;
    }
    if (expr->type != EXPR_FUNCTION) {
        return 0;
    }
    FunctionExpr* func = (FunctionExpr*)expr->data;
    return func->arg_count == expected_args;
}

int validate_identifier_chars(Expr* expr) {
    if (!expr || !expr->data) {
        return 0;
    }
    if (expr->type != EXPR_IDENTIFIER) {
        return 0;
    }
    char* id = (char*)expr->data;
    if (!isalpha(id[0]) && id[0] != '_') {
        return 0;
    }
    for (size_t i = 1; id[i]; i++) {
        if (!isalnum(id[i]) && id[i] != '_') {
            return 0;
        }
    }
    return 1;
}

int validate_no_side_effects(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type != EXPR_ASSIGN && expr->type != EXPR_CALL;
}

int validate_constant(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_NUMBER || expr->type == EXPR_STRING || 
           expr->type == EXPR_BOOLEAN || expr->type == EXPR_NULL;
}

int validate_lvalue(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_IDENTIFIER || expr->type == EXPR_ARRAY_ACCESS ||
           expr->type == EXPR_MEMBER_ACCESS;
}

int validate_rvalue(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type != EXPR_ASSIGN;
}

int validate_binary_op(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type >= EXPR_ADD && expr->type <= EXPR_LOGICAL_OR;
}

int validate_unary_op(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_NEGATE || expr->type == EXPR_NOT ||
           expr->type == EXPR_BITWISE_NOT || expr->type == EXPR_PRE_INC ||
           expr->type == EXPR_PRE_DEC || expr->type == EXPR_POST_INC ||
           expr->type == EXPR_POST_DEC;
}

int validate_comparison_op(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type >= EXPR_EQ && expr->type <= EXPR_GE;
}

int validate_logical_op(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_LOGICAL_AND || expr->type == EXPR_LOGICAL_OR;
}

int validate_bitwise_op(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type >= EXPR_BITWISE_AND && expr->type <= EXPR_BITWISE_XOR;
}

int validate_shift_op(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_LEFT_SHIFT || expr->type == EXPR_RIGHT_SHIFT;
}

int validate_arithmetic_op(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type >= EXPR_ADD && expr->type <= EXPR_MOD;
}

int validate_assignment_op(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_ASSIGN || expr->type == EXPR_ADD_ASSIGN ||
           expr->type == EXPR_SUB_ASSIGN || expr->type == EXPR_MUL_ASSIGN ||
           expr->type == EXPR_DIV_ASSIGN || expr->type == EXPR_MOD_ASSIGN ||
           expr->type == EXPR_AND_ASSIGN || expr->type == EXPR_OR_ASSIGN ||
           expr->type == EXPR_XOR_ASSIGN || expr->type == EXPR_LEFT_SHIFT_ASSIGN ||
           expr->type == EXPR_RIGHT_SHIFT_ASSIGN;
}

int validate_statement(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type >= EXPR_IF && expr->type <= EXPR_TRY;
}

int validate_declaration(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_VAR_DECL || expr->type == EXPR_CONST_DECL ||
           expr->type == EXPR_FUNC_DECL || expr->type == EXPR_CLASS_DECL;
}

int validate_block(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_BLOCK;
}

int validate_loop(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_WHILE || expr->type == EXPR_DO_WHILE ||
           expr->type == EXPR_FOR || expr->type == EXPR_FOR_IN ||
           expr->type == EXPR_FOR_OF;
}

int validate_conditional(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_IF || expr->type == EXPR_SWITCH ||
           expr->type == EXPR_TERNARY;
}

int validate_jump(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_BREAK || expr->type == EXPR_CONTINUE ||
           expr->type == EXPR_RETURN || expr->type == EXPR_THROW;
}

int validate_try_catch(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_TRY;
}

int validate_async(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_ASYNC || expr->type == EXPR_AWAIT ||
           expr->type == EXPR_YIELD;
}

int validate_object(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_OBJECT;
}

int validate_array(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_ARRAY;
}

int validate_function(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_FUNCTION || expr->type == EXPR_ARROW_FUNCTION ||
           expr->type == EXPR_METHOD;
}

int validate_class(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_CLASS_DECL || expr->type == EXPR_CLASS_EXPR;
}

int validate_interface(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_INTERFACE_DECL;
}

int validate_enum(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_ENUM_DECL;
}

int validate_struct(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_STRUCT_DECL;
}

int validate_union(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_UNION_DECL;
}

int validate_typedef(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_TYPEDEF_DECL;
}

int validate_namespace(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_NAMESPACE_DECL;
}

int validate_import(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_IMPORT || expr->type == EXPR_EXPORT;
}

int validate_module(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_MODULE_DECL;
}

int validate_annotation(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_ANNOTATION;
}

int validate_decorator(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_DECORATOR;
}

int validate_generic(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_GENERIC_TYPE || expr->type == EXPR_GENERIC_FUNCTION;
}

int validate_template(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_TEMPLATE_STRING || expr->type == EXPR_TEMPLATE_EXPR;
}

int validate_regex(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_REGEX;
}

int validate_literal(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_NUMBER || expr->type == EXPR_STRING ||
           expr->type == EXPR_BOOLEAN || expr->type == EXPR_NULL ||
           expr->type == EXPR_UNDEFINED || expr->type == EXPR_REGEX;
}

int validate_complex_expr(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_TERNARY || expr->type == EXPR_SPREAD ||
           expr->type == EXPR_REST || expr->type == EXPR_DESTRUCTURING;
}

int validate_scope(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_BLOCK || expr->type == EXPR_FUNCTION ||
           expr->type == EXPR_CLASS_DECL;
}

int validate_closure(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_FUNCTION || expr->type == EXPR_ARROW_FUNCTION;
}

int validate_pure(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return validate_no_side_effects(expr) && validate_constant(expr);
}

int validate_immutable(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type != EXPR_ASSIGN && expr->type != EXPR_POST_INC &&
           expr->type != EXPR_POST_DEC && expr->type != EXPR_PRE_INC &&
           expr->type != EXPR_PRE_DEC;
}

int validate_safe(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type != EXPR_NULL && expr->type != EXPR_UNDEFINED;
}

int validate_defined(Expr* expr) {
    return expr != NULL;
}

int validate_undefined(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_UNDEFINED;
}

int validate_null(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_NULL;
}

int validate_boolean(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_BOOLEAN;
}

int validate_numeric(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_NUMBER;
}

int validate_string_type(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_STRING;
}

int validate_symbol(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_SYMBOL;
}

int validate_bigint(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_BIGINT;
}

int validate_date(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_DATE;
}

int validate_time(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_TIME;
}

int validate_datetime(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_DATETIME;
}

int validate_duration(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_DURATION;
}

int validate_period(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_PERIOD;
}

int validate_range(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_RANGE;
}

int validate_tuple(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_TUPLE;
}

int validate_set(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_SET;
}

int validate_map(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_MAP;
}

int validate_record(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_RECORD;
}

int validate_variant(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_VARIANT;
}

int validate_option(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_OPTION;
}

int validate_result(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_RESULT;
}

int validate_either(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_EITHER;
}

int validate_future(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_FUTURE;
}

int validate_promise(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_PROMISE;
}

int validate_stream(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_STREAM;
}

int validate_observable(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_OBSERVABLE;
}

int validate_iterator(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_ITERATOR;
}

int validate_generator(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_GENERATOR;
}

int validate_async_iter(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_ASYNC_ITERATOR;
}

int validate_async_gen(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_ASYNC_GENERATOR;
}

int validate_proxy(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_PROXY;
}

int validate_reflection(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_REFLECTION;
}

int validate_metadata(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_METADATA;
}

int validate_attribute(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_ATTRIBUTE;
}

int validate_property(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_PROPERTY;
}

int validate_method(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_METHOD;
}

int validate_accessor(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_GETTER || expr->type == EXPR_SETTER;
}

int validate_indexer(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type == EXPR_INDEXER;
}

int validate_operator(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type >= EXPR_ADD && expr->type <= EXPR_RIGHT_SHIFT_ASSIGN;
}

int validate_expression(Expr* expr) {
    if (!expr) {
        return 0;
    }
    return expr->type >= EXPR_NUMBER && expr->type <= EXPR_ASYNC_GENERATOR;
}
