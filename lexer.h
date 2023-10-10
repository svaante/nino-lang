#pragma once

#include <stdlib.h>

/**
 * @brief The different types of tokens.
 */
enum TokenType {
    TOKEN_KEYWORD,
    TOKEN_SEPARATOR,
    TOKEN_IDENTIFIER,
    TOKEN_INT,
    TOKEN_OPERATOR,
};

/**
 * @brief The names of the different types of tokens.
 */
extern char** TokenNames;

/**
 * @brief Token object that contains the type and the text of the token.
 */
typedef struct Token {
    enum TokenType type;
    char* text;
} Token;

/**
 * @brief A simple list of tokens.
 */
typedef struct TokenList {
    Token* tokens;
    size_t length;
} TokenList;


/**
 * @brief Lexes the input string and returns a list of tokens.
 *
 * @param input The input string to lex.
 * @return TokenList* A pointer to the list of tokens.
 */
TokenList* lex(char* input);

// Debugging function

void _print_token(Token* token);