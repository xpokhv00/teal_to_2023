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
    parser_run();
    scanner_destroy();
}
