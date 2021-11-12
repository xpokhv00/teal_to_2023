/**
 * @file parser.c
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

#include "parser.h"

// current input token
static Token token;
// if anything fails, it will be stored here
static Status status;

// gets the token of the required type
// and gets a new one from the scanner
// returns false, if type does not match
bool eatToken(TokenType type) {
    if (token.type != type) {
        return false;
    }
    scanner_destroy_token(&token);
    status = scanner_get_token(&token);
    if (status != SUCCESS) {
        // this might not be the best practise
        return false;
    }
    return true;
}

bool nt_prog();
bool nt_prolog();
bool nt_prog_body();
bool nt_fn_decl();
bool nt_fn_decl_params();
bool nt_fn_def();


bool nt_prog() {
    bool found = false;

    switch (token.type) {
        // pravidlo <prog> -> <prolog> <prog_body>
        case TOKEN_REQUIRE:
            found = nt_prolog() && nt_prog_body();
            break;

        default:
            break;
    }
    return found;
}

bool nt_prolog() {
    bool found = false;

    switch (token.type) {
        // pravidlo <prolog> -> TOKEN_REQUIRE TOKEN_STRING_LIT
        case TOKEN_REQUIRE:
            scanner_destroy_token(&token);
            status = scanner_get_token(&token);
            if (status != SUCCESS) {
                break;
            }
            // the next token must be string literal "ifj21"
            if (token.type != TOKEN_STRING_LIT) {
                break;
            }
            if (strcmp(token.str, "ifj21") != 0) {
                status = ERR_SEMANTIC_OTHER;
                break;
            }
            found = true;
            break;

        default:
            break;
    }
    return found;
}

bool nt_prog_body() {
    bool found = false;

    switch (token.type) {
        // pravidlo <prog_body> -> <fn_decl> <prog_body>
        case TOKEN_GLOBAL:
            found = nt_fn_decl() && nt_prog_body();
            break;

        case TOKEN_FUNCTION:
            found = nt_fn_def() && nt_prog_body();
            break;

        // pravidlo <prog_body> -> eps
        case TOKEN_EOF:
            found = true;
            break;

        default:
            break;
    }
    return found;
}

bool nt_fn_decl() {
    bool found = false;

    switch (token.type) {
        // pravidlo <fn_decl> ->
        // TOKEN_GLOBAL TOKEN_IDENTIFIER TOKEN_COLON TOKEN_FUNCTION
        // TOKEN_PAR_L <fn_decl_params> TOKEN_PAR_R TOKEN_COLON <type>
        // example global foo : function(string) : string
        case TOKEN_GLOBAL:
            // TOKEN_GLOBAL
            scanner_destroy_token(&token);
            status = scanner_get_token(&token);
            if (status != SUCCESS) {
                break;
            }

            // TOKEN_IDENTIFIER
            if (token.type != TOKEN_STRING_LIT) {
                break;
            }
            // TODO semantic check, add into symbol table ...
            scanner_destroy_token(&token);
            status = scanner_get_token(&token);
            if (status != SUCCESS) {
                break;
            }

            if (!eatToken(TOKEN_COLON)) {
                break;
            }
            if (!eatToken(TOKEN_FUNCTION)) {
                break;
            }
            if (!eatToken(TOKEN_PAR_L)) {
                break;
            }
            if (!nt_fn_decl_params()) {
                break;
            }


            found = true;
            break;

        default:
            break;
    }
    return found;
}

bool nt_fn_decl_params() {
    bool found = false;

    switch (token.type) {
        case TOKEN_INTEGER_KW:
        case TOKEN_NUMBER_KW:
        case TOKEN_STRING_KW:
            scanner_destroy_token(&token);
            status = scanner_get_token(&token);
            if (status != SUCCESS) {
                break;
            }
            // TODO multiple parameters
            found = true;
            break;

        default:
            found = true;
            break;
    }
    return found;
}

bool nt_fn_def() {
    bool found = false;

    switch (token.type) {
        case TOKEN_FUNCTION:
            // TODO
            break;

        default:
            break;
    }
    return found;
}



Status parser_init() {
    return SUCCESS;
}

Status parser_run() {
    status = scanner_get_token(&token);
    if (status != SUCCESS) {
        return status;
    }

    bool valid = nt_prog();

    if (status == SUCCESS) {
        if (valid) {
            return SUCCESS;
        } else {
            // TODO maybe print out some error message?
            return ERR_SYNTAX;
        }
    }

    return SUCCESS;
}

Status parser_destroy() {
    return SUCCESS;
}