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
#include "scanner.h"

void test_scanner(){

    Token *token = malloc(sizeof(Token)); // TODO this is wrong, allocating should be scanner's responsibility
    token->str = malloc(sizeof(char) * 500);
    int status;
    while (true){

        status = scanner_get_token(token);
        printf("status - %d | tokenType - %d | tokenStr - %s\n", status, token->type, token->str);

    }

}
int main() {
    scanner_init(stdin);

    test_scanner();

    scanner_destroy();
}
