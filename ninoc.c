#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "code-gen.h"
#include "lexer.h"
#include "parser.h"

char* load_file(char* file_name) {
    FILE* file = fopen(file_name, "r");
    assert(file && "File could not be opened.");

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char* file_content = malloc(file_size + 1);
    assert(file_content && "Could not allocate memory for file content.");

    fread(file_content, 1, file_size, file);
    file_content[file_size] = '\0';

    fclose(file);
    return file_content;
}

void write_file(char* file_name, char* content) {
    FILE* file = fopen(file_name, "w");
    assert(file && "File could not be opened.");

    fwrite(content, 1, strlen(content), file);

    fclose(file);
}

void compile(char* code, char* output_file_name) {
    struct stat st = {0};
    if (stat("build", &st) == -1) {
        mkdir("build", 0700);
    }

    char* build_file_name = "build/build.ll";
    write_file(build_file_name, code);

    char* command = malloc(strlen(build_file_name) + strlen(output_file_name) + 100);
    sprintf(command, "clang -o %s %s -Wno-override-module", output_file_name, build_file_name);

    system(command);
}

char* get_file_name_without_extension(char* file_path) {
    char* file_path_cpy = malloc(strlen(file_path) + 1);
    strcpy(file_path_cpy, file_path);

    char* file_name = strrchr(file_path_cpy, '/');
    if (file_name == NULL) {
        file_name = file_path_cpy;
    } else {
        file_name++;
    }
    char* dot = strrchr(file_name, '.');
    if (dot == NULL || dot == file_name) {
        return file_name;
    } else {
        *dot = '\0';
        return file_name;
    }
}

int main(int argc, char* argv[]) {
    assert(argc > 1 && "No source file provided.");
    char* source_file_name = argv[1];
    char* destination_file_name = malloc(100);
    if (argc == 1) {
        printf("Usage: ninoc <source file> [destination file]\n");
    } else if (argc > 2) {
        strcpy(destination_file_name, argv[2]);
    } else {
        strcpy(destination_file_name, get_file_name_without_extension(source_file_name));
    }

    char* source_code = load_file(source_file_name);

    TokenList* tokens = lex(source_code);

    ASTList* ast_list = parse(tokens);

    char* llvm_ir = code_gen(ast_list);

    compile(llvm_ir, destination_file_name);
}