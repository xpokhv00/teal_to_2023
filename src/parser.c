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


#define GET_NEW_TOKEN() scanner_destroy_token(&token); \
status = scanner_get_token(&token); \
if (status != SUCCESS) { break; }

#define ASSERT_TOKEN_TYPE(token_type) if (token.type != (token_type)) { break; }

bool nt_prog();
bool nt_prolog();
bool nt_prog_body();
bool nt_fn_decl();
bool nt_fn_decl_params();
bool nt_fn_decl_params_next();
bool nt_fn_def();
bool nt_fn_def_params();
bool nt_fn_def_params_next();
bool nt_fn_returns();
bool nt_fn_returns_next();
bool nt_type();


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
            GET_NEW_TOKEN();
            // the next token must be string literal "ifj21"
            ASSERT_TOKEN_TYPE(TOKEN_STRING_LIT);
            if (strcmp(token.str, "ifj21") != 0) {
                status = ERR_SEMANTIC_OTHER;
                break;
            }
            GET_NEW_TOKEN();

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
        case TOKEN_GLOBAL:
            // <prog_body> -> <fn_decl> <prog_body>
            found = nt_fn_decl() && nt_prog_body();
            break;

        case TOKEN_FUNCTION:
            // <prog_body> -> <fn_def> <prog_body>
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
        // TOKEN_PAR_L <fn_decl_params> TOKEN_PAR_R <fn_returns>
        // example global foo : function(string) : string
        case TOKEN_GLOBAL:
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_IDENTIFIER);
            // TODO semantic check, add into symbol table ...
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_COLON);
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_FUNCTION);
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_PAR_L);
            GET_NEW_TOKEN();
            if (!nt_fn_decl_params()) {
                break;
            }
            ASSERT_TOKEN_TYPE(TOKEN_PAR_R);
            GET_NEW_TOKEN();
            if (!nt_fn_returns()) {
                break;
            }
            // TODO semantic check with symbol table ...

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
        // <fn_decl_params> -> <type> <fn_decl_params_next>
        case TOKEN_INTEGER_KW:
        case TOKEN_NUMBER_KW:
        case TOKEN_STRING_KW:
            GET_NEW_TOKEN();
            found = nt_fn_decl_params_next();
            break;

        // <fn_decl_params> -> eps
        // there don't have to be any parameters
        default:
            found = true;
            break;
    }
    return found;
}

bool nt_fn_decl_params_next() {
    bool found = false;

    switch (token.type) {
        case TOKEN_COMMA:
            // <fn_decl_params> -> TOKEN_COMMA <type> <fn_decl_params_next>
            GET_NEW_TOKEN();

            if (!nt_type()) {
                break;
            }
            // TODO semantic check for type
            // read another comma and parameter or eps
            found = nt_fn_decl_params_next();
            break;

        default:
            // <fn_decl_params> -> eps
            // there don't have to be any parameters
            found = true;
            break;
    }
    return found;
}

bool nt_fn_def() {
    bool found = false;

    //  function bar(param : string) : string
    //      return foo (param)
    //  end
    switch (token.type) {
        case TOKEN_FUNCTION:
            // <fn_def> -> TOKEN_FUNCTION TOKEN_IDENTIFIER
            // TOKEN_PAR_L <fn_def_params> TOKEN_PAR_R
            // <fn_returns> <fn_body>
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_IDENTIFIER);
            // TODO semantic check, add into symbol table ...
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_PAR_L);
            GET_NEW_TOKEN();
            if (!nt_fn_def_params()) {
                break;
            }
            // TODO semantic check of params
            ASSERT_TOKEN_TYPE(TOKEN_PAR_R);
            GET_NEW_TOKEN();
            if (!nt_fn_returns()) {
                break;
            }

            // TODO continue here, body is missing

            found = true;
            break;

        default:
            break;
    }
    return found;
}

bool nt_fn_def_params() {
    bool found = false;

    switch (token.type) {
        case TOKEN_IDENTIFIER:
            // <fn_def_params> -> TOKEN_IDENTIFIER TOKEN_COLON <type> <fn_def_params_next>
            // TODO symtable lookup
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_COLON);
            GET_NEW_TOKEN();
            if (!nt_type()) {
                break;
            }
            // TODO semantic check for correct type
            if (!nt_fn_def_params_next()) {
                break;
            }
            found = true;
            break;


        default:
            // <fn_def_params> -> eps
            // there don't have to be any parameters
            found = true;
            break;
    }
    return found;
}

bool nt_fn_def_params_next() {
    bool found = false;

    switch (token.type) {
        case TOKEN_COMMA:
            // <fn_def_params_next> -> TOKEN_COMMA TOKEN_IDENTIFIER TOKEN_COLON <type> <fn_decl_params_next>
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_IDENTIFIER);
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_COLON);
            GET_NEW_TOKEN();
            if (!nt_type()) {
                break;
            }
            // TODO semantic check for type

            // read another comma and parameter or eps
            if (!nt_fn_def_params_next()) {
                break;
            }
            found = true;
            break;

        default:
            // <fn_def_params> -> eps
            // there don't have to be any parameters
            found = true;
            break;
    }
    return found;
}


bool nt_fn_returns() {
    bool found = false;

    switch (token.type) {
        case TOKEN_COLON:
            // <fn_returns> -> TOKEN_COLON <type> <fn_returns_next>
            GET_NEW_TOKEN();
            if (!nt_type()) {
                break;
            }
            if (!nt_fn_returns_next()) {
                break;
            }
            found = true;
            break;

        default:
            // <fn_returns> -> eps
            // function can be without return
            found = true;
            break;
    }
    return found;
}

bool nt_fn_returns_next() {
    bool found = false;

    switch (token.type) {
        case TOKEN_COMMA:
            // <fn_returns_next> -> TOKEN_COMMA <type> <fn_returns_next>
            GET_NEW_TOKEN();
            if (!nt_type()) {
                break;
            }
            if (!nt_fn_returns_next()) {
                break;
            }
            found = true;
            break;

        default:
            // <fn_returns_next> -> eps
            // function can be without return
            found = true;
            break;
    }
    return found;
}

bool nt_type() {
    bool found = false;

    switch (token.type) {

        case TOKEN_INTEGER_KW:
            // <type> -> TOKEN_INTEGER_KW
        case TOKEN_NUMBER_KW:
            // <type> -> TOKEN_NUMBER_KW
        case TOKEN_STRING_KW:
            // <type> -> TOKEN_STRING_KW
            // TODO some semantic checks?
            GET_NEW_TOKEN();
            found = true;

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
            char *fstring = "Syntax error on line %u:%u:\nUnexpected token `%s`\n\n";
            fprintf(stderr, fstring, token.lineNumber, token.characterNumber, token.str);
            return ERR_SYNTAX;
        }
    }

    return SUCCESS;
}

Status parser_destroy() {
    return SUCCESS;
}