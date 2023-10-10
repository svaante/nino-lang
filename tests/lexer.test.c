#include "../lexer.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

void compare_tokens(Token* expected, Token* actual) {
    assert(expected->type == actual->type);
    assert(strncmp(expected->text, actual->text, strlen(expected->text)) == 0);
}

void test_variable_assignment() {
    printf("Testing assignment...\n");
    char* input = "let x = 5;";

    TokenList* expected = malloc(sizeof(TokenList));
    expected->length = 5;
    expected->tokens = malloc(5 * sizeof(Token));
    expected->tokens[0] = (Token){TOKEN_KEYWORD, "let"};
    expected->tokens[1] = (Token){TOKEN_IDENTIFIER, "x"};
    expected->tokens[2] = (Token){TOKEN_OPERATOR, "="};
    expected->tokens[3] = (Token){TOKEN_INT, "5"};
    expected->tokens[4] = (Token){TOKEN_SEPARATOR, ";"};

    TokenList* tokens = lex(input);

    for (size_t i = 0; i < tokens->length; i++) {
        _print_token(&tokens->tokens[i]);
    }
    assert(tokens->length == 5);

    compare_tokens(&expected->tokens[0], &tokens->tokens[0]);  // "let"
    compare_tokens(&expected->tokens[1], &tokens->tokens[1]);  // "x"
    compare_tokens(&expected->tokens[2], &tokens->tokens[2]);  // "="
    compare_tokens(&expected->tokens[3], &tokens->tokens[3]);  // "5"
    compare_tokens(&expected->tokens[4], &tokens->tokens[4]);  // ";"

    printf("Passed!\n\n");
}

void test_function_declaration() {
    printf("Testing function declaration...\n");
    char* input = "let add = (x, y) => x + y;";
    TokenList* expected = malloc(sizeof(TokenList));
    expected->length = 11;
    expected->tokens = malloc(11 * sizeof(Token));
    expected->tokens[0] = (Token){TOKEN_KEYWORD, "let"};
    expected->tokens[1] = (Token){TOKEN_IDENTIFIER, "add"};
    expected->tokens[2] = (Token){TOKEN_OPERATOR, "="};
    expected->tokens[3] = (Token){TOKEN_SEPARATOR, "("};
    expected->tokens[4] = (Token){TOKEN_IDENTIFIER, "x"};
    expected->tokens[5] = (Token){TOKEN_SEPARATOR, ","};
    expected->tokens[6] = (Token){TOKEN_IDENTIFIER, "y"};
    expected->tokens[7] = (Token){TOKEN_SEPARATOR, ")"};
    expected->tokens[8] = (Token){TOKEN_OPERATOR, "=>"};
    expected->tokens[9] = (Token){TOKEN_IDENTIFIER, "x"};
    expected->tokens[10] = (Token){TOKEN_OPERATOR, "+"};
    expected->tokens[11] = (Token){TOKEN_IDENTIFIER, "y"};
    expected->tokens[12] = (Token){TOKEN_SEPARATOR, ";"};

    TokenList* tokens = lex(input);

    for (size_t i = 0; i < tokens->length; i++) {
        _print_token(&tokens->tokens[i]);
    }

    assert(tokens->length == 13);

    compare_tokens(&expected->tokens[0], &tokens->tokens[0]);    // "let"
    compare_tokens(&expected->tokens[1], &tokens->tokens[1]);    // "add"
    compare_tokens(&expected->tokens[2], &tokens->tokens[2]);    // "="
    compare_tokens(&expected->tokens[3], &tokens->tokens[3]);    // "("
    compare_tokens(&expected->tokens[4], &tokens->tokens[4]);    // "x"
    compare_tokens(&expected->tokens[5], &tokens->tokens[5]);    // ","
    compare_tokens(&expected->tokens[6], &tokens->tokens[6]);    // "y"
    compare_tokens(&expected->tokens[7], &tokens->tokens[7]);    // ")"
    compare_tokens(&expected->tokens[8], &tokens->tokens[8]);    // "=>"
    compare_tokens(&expected->tokens[9], &tokens->tokens[9]);    // "x"
    compare_tokens(&expected->tokens[10], &tokens->tokens[10]);  // "+"
    compare_tokens(&expected->tokens[11], &tokens->tokens[11]);  // "y"
    compare_tokens(&expected->tokens[12], &tokens->tokens[12]);  // ";"

    printf("Passed!\n\n");
}

int main() {
    test_variable_assignment();
    test_function_declaration();

    return 0;
}