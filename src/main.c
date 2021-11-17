/**
 * @file main.c
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

#include "main.h"
Token *TokenInit(){
    Token *Token;
    if ((Token = malloc(sizeof(Token))) == NULL){
        return NULL;
    }
    if ((Token->str = malloc(500)) == NULL){
        free(Token);
        return NULL;
    }
    return Token;
}
void test_scanner(){

    Token *token = TokenInit();
    int status = 1;
    while (status != SCANNER_EOF){
        status = scanner_get_token(token);
        printf("status - %d | tokenType - %d | tokenStr - %s\n", status, token->type, token->str);

    }

}

void test_parser() {
    parser_init();
    parser_run();
    parser_destroy();
}

int main() {
    scanner_init(stdin);

    //test_scanner();
    Status s = parser_run();
    if (s == SUCCESS) {
        fprintf(stderr, "Compilation was successful!\n");
    } else {
        char *errorStrings[] = {
                [SUCCESS] = "", // never happens
                [ERR_LEXICAL] = "Lexical",
                [ERR_SYNTAX] = "Syntax",
                [ERR_SEMANTIC_DEF] = "Semantic",
                [ERR_SEMANTIC_ASSIGN] = "Semantic",
                [ERR_SEMANTIC_FUNC] = "Semantic",
                [ERR_SEMANTIC_EXPR] = "Semantic",
                [ERR_SEMANTIC_OTHER] = "Semantic",
                [ERR_RUNTIME_NIL] = "Runtime",
                [ERR_RUNTIME_DIV_ZERO] = "Runtime",
                [ERR_INTERNAL] = "Internal",
        };
        fprintf(stderr, "Compilation failed with %s error\n", errorStrings[s]);
    }
    scanner_destroy();
}
