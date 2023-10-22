#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"

char functions[1000] = {0};

char* printing_header =
    "@.int_str = private unnamed_addr constant [4 x i8] c\"%d\\0A\\00\"\n"
    "declare i32 @printf(i8*, ...)\n\n";

/** Appends a unique id to the identifier */
void append_id(char* identifier) {
    static int id = 0;
    sprintf(identifier + strlen(identifier), "%d", id);
    id++;
}

void printing_function(char* identifier, char* output) {
    char fmt_val_id[100] = "fmt_val";
    append_id(fmt_val_id);

    char fmt_id[100] = "fmt";
    append_id(fmt_id);

    sprintf(output + strlen(output),
            "  \n"
            "  %%%s = load i32, i32* %%%s\n"
            "  %%%s = getelementptr inbounds [4 x i8], [4 x i8]* @.int_str, i32 0, i32 0\n"
            "  call i32 (i8*, ...) @printf(i8* %%%s, i32 %%%s)\n\n",
            fmt_val_id, identifier, fmt_id, fmt_id, fmt_val_id);
}

/**
 * @brief Build an expression
 *
 * @param identifier the identifier given by the parent expression
 * @param expression the expression to build
 * @param output the string to append the generated code to
 *
 * @return the identifier of the expression
 */
char* build_expression(char* identifier, Expression* expression, char* output) {
    if (expression->type == AST_INTEGER_LITERAL) {
        char* type = expression->data.Literal.type_name;
        char* value = expression->data.Literal.value;

        sprintf(output + strlen(output),
                "  %%%s = alloca %s\n"
                "  store %s %s, %s* %%%s\n\n",
                identifier, type,
                type, value, type, identifier);
        return identifier;
    }

    if (expression->type == AST_IDENTIFIER) {
        char* identifier = expression->data.Identifier.value;
        return identifier;
    }

    if (expression->type == AST_BINARY_OPERATION) {
        // The id to be used for the left and right expressions if they do not return a identifier
        char left_expr_id[100] = {0};
        strcpy(left_expr_id, identifier);
        append_id(left_expr_id);

        char* right_expr_id = malloc(100 * sizeof(char));
        strcpy(right_expr_id, identifier);
        append_id(right_expr_id);

        char* left_ptr_id = build_expression(left_expr_id, expression->data.BinaryOperation.left, output);
        char* right_ptr_id = build_expression(right_expr_id, expression->data.BinaryOperation.right, output);

        char* left_load_id = malloc(100 * sizeof(char));
        strcpy(left_load_id, identifier);
        append_id(left_load_id);

        char* right_load_id = malloc(100 * sizeof(char));
        strcpy(right_load_id, identifier);
        append_id(right_load_id);

        char* result_id = malloc(100 * sizeof(char));
        strcpy(result_id, identifier);
        append_id(result_id);

        char* operator= expression->data.BinaryOperation.operator;
        char* operation;
        if (strcmp(operator, "+") == 0) {
            operation = "add";
        } else if (strcmp(operator, "-") == 0) {
            operation = "sub";
        } else if (strcmp(operator, "*") == 0) {
            operation = "mul";
        } else if (strcmp(operator, "/") == 0) {
            operation = "sdiv";
        } else {
            printf("Error: Unknown operator %s\n", operator);
            return NULL;
        }

        sprintf(output + strlen(output),
                "  %%%s = load i32, i32* %%%s\n"
                "  %%%s = load i32, i32* %%%s\n"
                "  %%%s = %s i32 %%%s, %%%s\n"
                "  %%%s = alloca %s\n"
                "  store %s %%%s, %s* %%%s\n\n",
                left_load_id, left_ptr_id,
                right_load_id, right_ptr_id,
                result_id, operation, left_load_id, right_load_id,
                identifier, "i32",
                "i32", result_id, "i32", identifier);
        return identifier;
    }

    if (expression->type == AST_FUNCTION) {
        /*              Here we need to declare a function.
         * When declaring a function, the arguments are passed by value. In order
         * for the other operators to use them, they need to be loaded into new
         * variables. The arguments are given a temporary name, and then their
         * value is stored in the user-defined name.
         *
         * define i32 @x(type1 arg1, type2 arg2){
         *    ret i32 arg
         * };
         */

        char* arguments = malloc(100 * sizeof(char));
        char* argument_translated_id = malloc(100 * sizeof(char));

        for (int i = 0; i < (int)expression->data.Function.num_parameters; i++) {
            FunctionParameter parameter = expression->data.Function.parameters[i];
            char* parameter_id = malloc(100 * sizeof(char));
            strcpy(parameter_id, identifier);
            append_id(parameter_id);

            sprintf(arguments + strlen(arguments), "%s %%%s", parameter.type, parameter_id);
            sprintf(argument_translated_id + strlen(argument_translated_id), 
            "  %%%s = alloca %s\n"
            "  store %s %%%s, %s* %%%s\n\n",
            parameter.identifier, parameter.type,
            parameter.type, parameter_id, parameter.type, parameter.identifier);

            if (i != (int)expression->data.Function.num_parameters - 1) {
                strcat(arguments, ", ");
            }
        }
        sprintf(functions + strlen(functions),
                "define i32 @%s(%s){\n", identifier, arguments);

        sprintf(functions + strlen(functions), "%s\n", argument_translated_id);

        char* result_id = build_expression(identifier, expression->data.Function.expression, functions);
        char* translate_result_id = malloc(100 * sizeof(char));
        strcpy(translate_result_id, identifier);
        append_id(translate_result_id);

        sprintf(functions + strlen(functions),
                "  %%%s = load i32, i32* %%%s\n"
                "  ret i32 %%%s\n"
                "}\n\n",
                translate_result_id, result_id, translate_result_id);

        return identifier;
    }

    if (expression->type == AST_FUNCTION_CALL) {
        char* arguments = malloc(100 * sizeof(char));
        for (int i = 0; i < (int)expression->data.FunctionCall.num_arguments; i++) {
            char* result_id = malloc(100 * sizeof(char));
            strcpy(result_id, identifier);
            append_id(result_id);
            Expression* argument = &expression->data.FunctionCall.arguments[i];
            char* argument_id = build_expression(result_id, argument, output);
            char* loaded_id = malloc(100 * sizeof(char));
            strcpy(loaded_id, identifier);
            append_id(loaded_id);
            sprintf(output + strlen(output),
                    "   %%%s = load i32, i32* %%%s\n",
                    loaded_id, argument_id);

            sprintf(arguments + strlen(arguments), "i32 %%%s", loaded_id);
            if (i != (int)expression->data.FunctionCall.num_arguments - 1) {
                strcat(arguments, ", ");
            }
        }

        char* result_id = malloc(100 * sizeof(char));
        strcpy(result_id, identifier);
        append_id(result_id);

        sprintf(output + strlen(output),
                "   %%%s = alloca i32\n"
                "   %%%s =  call i32 @%s(%s)\n\n"
                "   store i32 %%%s, i32* %%%s\n\n",
                identifier, result_id, expression->data.FunctionCall.identifier, arguments, result_id, identifier);
        return identifier;
    }

    return NULL;
}

void build_print(Print* print, char* output) {
    Expression* expression = print->expression;
    char* identifier = malloc(100 * sizeof(char));
    strcpy(identifier, "print");
    append_id(identifier);

    char* load_id = build_expression(identifier, expression, output);

    printing_function(load_id, output);
}

void build_let(Declaration* declaration, char* output) {
    Expression* expression = declaration->expression;
    build_expression(declaration->identifier, expression, output);
}

char* code_gen(ASTList* ast_list) {
    strcat(functions, printing_header);

    char* main = malloc(10000 * sizeof(char));
    strcat(main,
           "define i32 @main() {\n"
           "entry:\n\n");

    for (int i = 0; i < (int)ast_list->length; i++) {
        ASTNode* node = ast_list->items[i];
        if (node->type == AST_DECLARATION) {
            build_let(&node->data.Declaration, main);
        }
        if (node->type == AST_PRINT) {
            build_print(&node->data.Print, main);
        }
        if (node->type == AST_EXPRESSION) {
            build_expression(node->data.Expression.data.FunctionCall.identifier, &node->data.Expression, main);
        }
    }

    strcat(main,
           "  ret i32 0\n"
           "}\n"
           "\n");

    char* code = malloc(10000 * sizeof(char));
    strcat(code, functions);
    strcat(code, main);

    return code;
}
