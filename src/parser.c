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
static SymTab st;


#define GET_NEW_TOKEN() scanner_destroy_token(&token); \
status = scanner_get_token(&token); \
if (status != SUCCESS) { break; }

#define ASSERT_TOKEN_TYPE(token_type) if (token.type != (token_type)) { break; }

#define ASSERT_NT(nt_function_return) if (!(nt_function_return)) { break; }

#define ASSERT_SUCCESS(fn_return) if ((status = (fn_return)) != SUCCESS) { break; }

bool nt_prog();
bool nt_prolog();
bool nt_prog_body();

bool nt_fn_decl();
bool nt_fn_decl_params(HTabPair *pair);
bool nt_fn_decl_params_next(HTabPair *pair);
bool nt_fn_def();
bool nt_fn_def_params(HTabPair *pair, bool isDeclared);
bool nt_fn_def_params_next(HTabPair *pair, bool isDeclared);
bool nt_fn_returns(HTabPair *pair, bool declared);
bool nt_fn_returns_next(HTabPair *pair, bool declared);

bool nt_fn_body(HTabPair *fnPair);

bool nt_var_decl();
bool nt_var_decl_assign();

bool nt_assignment(HTabPair *fnPair);

bool nt_if(HTabPair *fnPair);

bool nt_else(HTabPair *fnPair);

bool nt_while(HTabPair *fnPair);

bool nt_return(HTabPair *fnPair);

bool nt_r_value_list(bool emptyValid, HTabPair *fnPair);

bool nt_r_value_list_next(HTabPair *fnPair);
bool nt_l_value_list();
bool nt_l_value_list_next();
bool nt_fn_call();
bool nt_fn_call_params(HTabPair *callPair);
bool nt_fn_call_params_next(HTabPair *callPair);
bool nt_type();


bool nt_prog() {
    bool found = false;

    switch (token.type) {
        // pravidlo <prog> -> <prolog> <prog_body>
        case TOKEN_REQUIRE:
            ASSERT_NT(nt_prolog());
            ASSERT_NT(nt_prog_body());
            found = true;
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
            if (strcmp(token.str, "\"ifj21\"") != 0) {
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

        case TOKEN_IDENTIFIER:
            // <prog_body> -> <fn_call> <prog_body>
            found = nt_fn_call() && nt_prog_body();
            break;

        case TOKEN_EOF:
            // pravidlo <prog_body> -> eps
            // At the end of program check if all declared functions were defined
            ASSERT_SUCCESS(st_check_fn_defined(&st));
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

            // Checks if identifier already exists in symtable
            // If not creates one
            HTabPair *pair;
            ASSERT_SUCCESS(st_add(&st, token, &pair));

            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_COLON);
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_FUNCTION);
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_PAR_L);
            GET_NEW_TOKEN();
            if (!nt_fn_decl_params(pair)) {
                break;
            }
            ASSERT_TOKEN_TYPE(TOKEN_PAR_R);
            GET_NEW_TOKEN();
            if (!nt_fn_returns(pair, false)) {
                break;
            }

            found = true;
            break;

        default:
            break;
    }
    return found;
}

bool nt_fn_decl_params(HTabPair *pair) {
    bool found = false;

    switch (token.type) {
        // <fn_decl_params> -> <type> <fn_decl_params_next>
        case TOKEN_INTEGER_KW:
        case TOKEN_NUMBER_KW:
        case TOKEN_STRING_KW:
        case TOKEN_NIL:;

            // Adds parameter name into parameter list of a function
            ASSERT_SUCCESS(list_append(&pair->value.paramList, token_keyword_to_type(token.type)));

            GET_NEW_TOKEN();
            found = nt_fn_decl_params_next(pair);
            break;

        // <fn_decl_params> -> eps
        // there don't have to be any parameters
        default:
            found = true;
            break;
    }
    return found;
}

bool nt_fn_decl_params_next(HTabPair *pair) {
    bool found = false;

    switch (token.type) {
        case TOKEN_COMMA:
            // <fn_decl_params> -> TOKEN_COMMA <type> <fn_decl_params_next>
            GET_NEW_TOKEN();

            // Adds parameter name into parameter list of a function
            ASSERT_SUCCESS(list_append(&pair->value.paramList, token_keyword_to_type(token.type)));

            if (!nt_type()) {
                break;
            }
            // read another comma and parameter or eps
            found = nt_fn_decl_params_next(pair);
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
            // <fn_returns> <fn_body> TOKEN_END
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_IDENTIFIER);

            // Check if function is already declared
            HTabPair *fnPair = st_lookup(&st, token.str);
            bool isDeclared = fnPair != NULL;
            if (!isDeclared) {
                // Adds function name into symtable
                ASSERT_SUCCESS(st_add(&st, token, &fnPair));
                fnPair = st_lookup(&st, token.str);
            } else {
                // Function cannot be defined more than once
                if (fnPair->value.defined) {
                    status = ERR_SEMANTIC_DEF;
                    break;
                }
            }
            // Now the function is defined
            fnPair->value.defined = true;

            gen_print("LABEL %s\n", token.str);
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_PAR_L);
            GET_NEW_TOKEN();

            // Create local frame for this function
            st_push_frame(&st);

            gen_print("PUSHFRAME\n");
            ASSERT_NT(nt_fn_def_params(fnPair, isDeclared));
            ASSERT_TOKEN_TYPE(TOKEN_PAR_R);
            GET_NEW_TOKEN();
            ASSERT_NT(nt_fn_returns(fnPair, isDeclared));
            ASSERT_NT(nt_fn_body(fnPair));
            gen_print("POPFRAME\n");
            gen_print("RETURN\n");

            // Destroy local frame
            st_pop_frame(&st);

            ASSERT_TOKEN_TYPE(TOKEN_END);
            GET_NEW_TOKEN();
            found = true;
            break;

        default:
            break;
    }
    return found;
}

bool nt_fn_def_params(HTabPair *pair, bool isDeclared) {
    bool found = false;

    switch (token.type) {
        case TOKEN_IDENTIFIER:;
            // <fn_def_params> -> TOKEN_IDENTIFIER TOKEN_COLON <type> <fn_def_params_next>

            // Check for parameter name in symtable, add it if not found
            HTabPair *paramPair;
            ASSERT_SUCCESS(st_add(&st, token, &paramPair));

            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_COLON);
            GET_NEW_TOKEN();

            list_first(&pair->value.paramList);
            paramPair->value.varType = token_keyword_to_type(token.type);
            if (isDeclared) {
                // Check if parameter types from declaration match the ones from definition
                Type saved = list_get_active(&pair->value.paramList);
                if (saved != paramPair->value.varType) {
                    status = ERR_SEMANTIC_FUNC;
                    break;
                }
            } else {
                // Adds item to the list if the function is undeclared
                ASSERT_SUCCESS(list_append(&pair->value.paramList, token_keyword_to_type(token.type)));
            }

            // move the parameter to newly declared local variable
            gen_print("DEFVAR LF@$%u\n", paramPair->value.ID);
            gen_print("MOVE LF@$%u TF@%%param%u\n",
                      paramPair->value.ID,
                      list_active_index(&pair->value.paramList)
                      );

            if (!nt_type()) {
                break;
            }
            if (!nt_fn_def_params_next(pair, isDeclared)) {
                break;
            }
            found = true;
            break;


        default:
            // <fn_def_params> -> eps
            // there don't have to be any parameters

            // Check that the list is no longer active
            list_first(&pair->value.paramList);
            if (list_is_active(&pair->value.paramList)) {
                status = ERR_SEMANTIC_FUNC;
                break;
            }

            found = true;
            break;
    }
    return found;
}

bool nt_fn_def_params_next(HTabPair *pair, bool isDeclared) {
    bool found = false;

    switch (token.type) {
        case TOKEN_COMMA:
            // <fn_def_params_next> -> TOKEN_COMMA TOKEN_IDENTIFIER TOKEN_COLON <type> <fn_decl_params_next>
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_IDENTIFIER);

            // Check for parameter name in symtable, add it if not found
            HTabPair *paramPair;
            ASSERT_SUCCESS(st_add(&st, token, &paramPair));

            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_COLON);
            GET_NEW_TOKEN();

            paramPair->value.varType = token_keyword_to_type(token.type);
            if (isDeclared) {
                // Check if parameter types from declaration match the ones from definition
                list_next(&pair->value.paramList);
                Type saved = list_get_active(&pair->value.paramList);
                if (saved != paramPair->value.varType) {
                    status = ERR_SEMANTIC_FUNC;
                    break;
                }
            } else {
                // Adds item to the list if the function is undeclared
                ASSERT_SUCCESS(list_append(&pair->value.paramList, token_keyword_to_type(token.type)));
            }

            if (!nt_type()) {
                break;
            }
            // read another comma and parameter or eps
            if (!nt_fn_def_params_next(pair, isDeclared)) {
                break;
            }
            found = true;
            break;

        default:
            // <fn_def_params> -> eps
            // there don't have to be any parameters

            // Check that the list is no longer active
            list_next(&pair->value.paramList);
            if (list_is_active(&pair->value.paramList)) {
                status = ERR_SEMANTIC_FUNC;
                break;
            }

            found = true;
            break;
    }
    return found;
}

bool nt_fn_returns(HTabPair *pair, bool declared) {
    bool found = false;
    list_first(&pair->value.returnList);
    switch (token.type) {
        case TOKEN_COLON:
            // <fn_returns> -> TOKEN_COLON <type> <fn_returns_next>
            GET_NEW_TOKEN();

            Type returnType = token_keyword_to_type(token.type);
            if (declared) {
                // Check if return types from declaration match the ones from definition
                Type saved = list_get_active(&pair->value.returnList);
                if (saved != returnType) {
                    status = ERR_SEMANTIC_FUNC;
                    break;
                }
            }
            else {
                // Adds item to the list if the function is undeclared
                ASSERT_SUCCESS(list_append(&pair->value.returnList, token_keyword_to_type(token.type)));
            }

            if (!nt_type()) {
                break;
            }
            if (!nt_fn_returns_next(pair, declared)) {
                break;
            }
            found = true;
            break;

        default:
            // <fn_returns> -> eps
            // function can be without return

            // Check that the list is no longer active
            if (list_is_active(&pair->value.returnList)) {
                status = ERR_SEMANTIC_FUNC;
                break;
            }

            found = true;
            break;
    }
    return found;
}

bool nt_fn_returns_next(HTabPair *pair, bool declared) {
    bool found = false;

    switch (token.type) {
        case TOKEN_COMMA:
            // <fn_returns_next> -> TOKEN_COMMA <type> <fn_returns_next>
            GET_NEW_TOKEN();

            Type returnType = token_keyword_to_type(token.type);
            if (declared) {
                list_next(&pair->value.returnList);
                // Get type of the active element
                Type saved = list_get_active(&pair->value.returnList);
                // Check if parameter types from declaration match the ones from definition
                if (saved != returnType) {
                    status = ERR_SEMANTIC_FUNC;
                    break;
                }
            }
            else {
                // Adds item to the list if the function is undeclared
                ASSERT_SUCCESS(list_append(&pair->value.returnList, token_keyword_to_type(token.type)));
            }

            if (!nt_type()) {
                break;
            }
            if (!nt_fn_returns_next(pair, declared)) {
                break;
            }
            found = true;
            break;

        default:
            // <fn_returns_next> -> eps
            // function can be without return

            // Check that the list is no longer active
            list_next(&pair->value.returnList);
            if (list_is_active(&pair->value.returnList)) {
                status = ERR_SEMANTIC_FUNC;
                break;
            }

            found = true;
            break;
    }
    return found;
}

bool nt_fn_body(HTabPair *fnPair) {
    bool found = false;

    switch (token.type) {
        case TOKEN_LOCAL:
            // variable declaration
            ASSERT_NT(nt_var_decl());
            ASSERT_NT(nt_fn_body(fnPair));
            found = true;
            break;

        case TOKEN_IDENTIFIER:;
            // assignment or function call
            // <fn_body> -> nt_l_value_list TOKEN_ASSIGN <expr> <fn_body>

            // get next token to know what we're dealing with
            Token nextToken;
            ASSERT_SUCCESS(scanner_get_token(&nextToken));
            bool isFunction = (nextToken.type == TOKEN_PAR_L);
            scanner_unget_token(nextToken);

            if (isFunction) {
                ASSERT_NT(nt_fn_call());
                ASSERT_NT(nt_fn_body(fnPair));
            } else {
                ASSERT_NT(nt_assignment(fnPair));
                ASSERT_NT(nt_fn_body(fnPair));
            }
            found = true;
            break;

        case TOKEN_RETURN:
            ASSERT_NT(nt_return(fnPair));
            ASSERT_NT(nt_fn_body(fnPair)); // TODO maybe stop after return?
            found = true;
            break;

        case TOKEN_IF:
            found = nt_if(fnPair) && nt_fn_body(fnPair);
            break;

        case TOKEN_WHILE:
            found = nt_while(fnPair) && nt_fn_body(fnPair);
            break;

        default:
            found = true;
            break;
    }
    return found;
}

bool nt_var_decl() {
    bool found = false;

    switch (token.type) {
        case TOKEN_LOCAL:
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_IDENTIFIER);

            // Checks if identifier already exists in symtable
            // If not creates one
            HTabPair *varPair = st_lookup(&st, token.str);
            bool isDeclared = varPair != NULL;
            if (isDeclared) {
                // Variable cannot be declared more than once
                status = ERR_SEMANTIC_DEF;
                break;
            }
            // Adds variable name into symtable
            ASSERT_SUCCESS(st_add(&st, token, &varPair));
            varPair = st_lookup(&st, token.str);

            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_COLON);
            GET_NEW_TOKEN();
            varPair->value.varType = token_keyword_to_type(token.type);
            ASSERT_NT(nt_type());
            ASSERT_NT(nt_var_decl_assign());
            found = true;
            break;

        default:
            break;
    }
    return found;
}

bool nt_var_decl_assign() {
    bool found = false;

    switch (token.type) {
        case TOKEN_ASSIGN:
            GET_NEW_TOKEN();
            // This could be an expression or function call
            Token nextToken;
            ASSERT_SUCCESS(scanner_get_token(&nextToken));
            bool isFunction = (nextToken.type == TOKEN_PAR_L);
            scanner_unget_token(nextToken);

            if (isFunction) {
                ASSERT_NT(nt_fn_call());
            } else {
                ASSERT_NT(nt_expr(&token)); // TODO semantic check inside
            }
            found = true;
            break;

        default:
            // the variable does not have to be assigned, which makes it nil
            found = true;
            break;
    }
    return found;
}

bool nt_assignment(HTabPair *fnPair) {
    bool found = false;

    switch (token.type) {
        case TOKEN_IDENTIFIER:
            ASSERT_NT(nt_l_value_list());
            ASSERT_TOKEN_TYPE(TOKEN_ASSIGN);
            GET_NEW_TOKEN();

            // check if the identifier is a variable or a function
            // get next token to know what we're dealing with
            Token nextToken;
            ASSERT_SUCCESS(scanner_get_token(&nextToken));
            bool isFunction = (nextToken.type == TOKEN_PAR_L);
            scanner_unget_token(nextToken);

            if (isFunction) {
                ASSERT_NT(nt_fn_call());
            } else {
                ASSERT_NT(nt_r_value_list(false, fnPair));
            }

            found = true;
            break;

        default:
            break;
    }
    return found;
}

bool nt_if(HTabPair *fnPair) {
    bool found = false;

    switch (token.type) {
        case TOKEN_IF:
            // <if> -> TOKEN_IF <expr> TOKEN_THEN <fn_body> <else> TOKEN_END
            GET_NEW_TOKEN();
            ASSERT_NT(nt_expr(&token));
            ASSERT_TOKEN_TYPE(TOKEN_THEN);
            GET_NEW_TOKEN();
            ASSERT_NT(nt_fn_body(fnPair));
            ASSERT_NT(nt_else(fnPair));
            ASSERT_TOKEN_TYPE(TOKEN_END);
            GET_NEW_TOKEN();
            found = true;
            break;

        default:
            break;
    }
    return found;
}

bool nt_else(HTabPair *fnPair) {
    bool found = false;

    switch (token.type) {
        case TOKEN_ELSE:
            // <else> -> TOKEN_ELSE <fn_body>
            GET_NEW_TOKEN();
            ASSERT_NT(nt_fn_body(fnPair));
            found = true;
            break;

        default:
            // <else> -> eps
            found = true;
            break;
    }
    return found;
}

bool nt_while(HTabPair *fnPair) {
    bool found = false;

    switch (token.type) {
        case TOKEN_WHILE:
            // <while> -> TOKEN_WHILE <expr> TOKEN_DO <fn_body> TOKEN_END
            GET_NEW_TOKEN();
            ASSERT_NT(nt_expr(&token));
            ASSERT_TOKEN_TYPE(TOKEN_DO);
            GET_NEW_TOKEN();
            ASSERT_NT(nt_fn_body(fnPair));
            ASSERT_TOKEN_TYPE(TOKEN_END);
            GET_NEW_TOKEN();
            found = true;
            break;

        default:
            break;
    }
    return found;
}

bool nt_return(HTabPair *fnPair) {
    bool found = false;

    switch (token.type) {
        case TOKEN_RETURN:
            // <return> -> TOKEN_RETURN <r_value_list>
            GET_NEW_TOKEN();
            ASSERT_NT(nt_r_value_list(true, fnPair));
            found = true;
            break;

        default:
            break;
    }
    return found;
}

bool nt_r_value_list(bool emptyValid, HTabPair *fnPair) {
    bool found = false;

    switch (token.type) {
        case TOKEN_IDENTIFIER:
        case TOKEN_INTEGER_LIT:
        case TOKEN_DOUBLE_LIT:
        case TOKEN_STRING_LIT:
        case TOKEN_NIL:;
            // Cannot be a function
            // <r_value_list> -> <r_value> <r_value_list_next>
            ASSERT_NT(nt_expr(&token));
            // this is supposed to compare against expression
            /*
            Type rValueType = st_token_to_type(&st, token);
            list_first(&fnPair->value.returnList);
            Type savedValue = list_get_active(&fnPair->value.returnList);
            // Check if parameter type matches the one from definition / declaration
            if (!can_assign(rValueType, savedValue)) {
                status = ERR_SEMANTIC_FUNC;
                break;
            }
*/

            ASSERT_NT(nt_r_value_list_next(fnPair));
            found = true;
            break;

        default:
            list_first(&fnPair->value.returnList);
            if (list_is_active(&fnPair->value.returnList)) {
                status = ERR_SEMANTIC_FUNC;
                break;
            }
            if (emptyValid) {
                found = true;
            }
            break;
    }
    return found;
}

bool nt_r_value_list_next(HTabPair *fnPair) {
    bool found = false;

    switch (token.type) {
        case TOKEN_COMMA:
            GET_NEW_TOKEN();
            ASSERT_NT(nt_expr(&token));
            // this is supposed to compare against expression
            /*
            Type rValueType = st_token_to_type(&st, token);
            list_next(&fnPair->value.returnList);
            Type savedValue = list_get_active(&fnPair->value.returnList);
            // Check if parameter type matches the one from definition / declaration
            if (!can_assign(rValueType, savedValue)) {
                status = ERR_SEMANTIC_FUNC;
                break;
            }
            */
            ASSERT_NT(nt_r_value_list_next(fnPair));
            found = true;
            break;

        default:
            // can be empty
            list_next(&fnPair->value.returnList);
            if (list_is_active(&fnPair->value.returnList)) {
                status = ERR_SEMANTIC_FUNC;
                break;
            }
            found = true;
            break;
    }
    return found;
}

bool nt_l_value_list() {
    bool found = false;

    switch (token.type) {
        case TOKEN_IDENTIFIER:
            // todo semantics
            GET_NEW_TOKEN();
            ASSERT_NT(nt_l_value_list_next());
            found = true;
            break;

        default:
            break;
    }
    return found;
}

bool nt_l_value_list_next() {
    bool found = false;

    switch (token.type) {
        case TOKEN_COMMA:
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_IDENTIFIER);
            GET_NEW_TOKEN();
            ASSERT_NT(nt_l_value_list_next());
            found = true;
            break;

        default:
            found = true;
            break;
    }
    return found;
}


bool nt_fn_call() {
    bool found = false;

    switch (token.type) {
        case TOKEN_IDENTIFIER:;
            // <fn_call> -> TOKEN_IDENTIFIER TOKEN_PAR_L <fn_call_params> TOKEN_PAR_R

            // Check whether the function is declared or defined
            HTabPair *callPair = st_lookup(&st, token.str);
            bool isDeclared = callPair != NULL;
            if (!isDeclared) {
                // Cannot call an undeclared function
                status = ERR_SEMANTIC_DEF;
                break;
            }

            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TOKEN_PAR_L);
            GET_NEW_TOKEN();
            gen_print("CREATEFRAME\n");
            if (!nt_fn_call_params(callPair)) {
                break;
            }
            ASSERT_TOKEN_TYPE(TOKEN_PAR_R);
            GET_NEW_TOKEN()
            found = true;
            break;

        default:
            break;
    }
    return found;
}

bool nt_fn_call_params(HTabPair *callPair) {
    bool found = false;

    switch (token.type) {
        // <nt_fn_call_params> -> TOKEN_IDENTIFIER <fn_call_params_next>
        case TOKEN_IDENTIFIER: // This is not needed - only inside function body
        case TOKEN_INTEGER_LIT:
        case TOKEN_DOUBLE_LIT:
        case TOKEN_STRING_LIT:
        case TOKEN_NIL:;

            // Set activity to the first item
            list_first(&callPair->value.paramList);
            // Get type of the first item in paramList
            Type saved = list_get_active(&callPair->value.paramList);
            // Check if parameter type matches the one from definition / declaration
            if (!can_assign(saved, st_token_to_type(&st, token))) {
                status = ERR_SEMANTIC_FUNC;
                break;
            }

            // move the argument into the temporary frame
            gen_print("MOVE TF@%%param%d ", list_active_index(&callPair->value.paramList));
            if (token.type == TOKEN_IDENTIFIER) {
                gen_print("LF@%s", token.str);
            } else {
                gen_print_literal(token);
            }
            gen_print("\n");

            GET_NEW_TOKEN();
            found = nt_fn_call_params_next(callPair);
            break;

        default:
            // <fn_decl_params> -> eps
            // there don't have to be any parameters

            // Check that the list is no longer active
            list_first(&callPair->value.paramList);
            if (list_is_active(&callPair->value.paramList)) {
                status = ERR_SEMANTIC_FUNC;
                break;
            }

            found = true;
            break;
    }
    return found;
}

bool nt_fn_call_params_next(HTabPair *callPair) {
    bool found = false;

    switch (token.type) {
        case TOKEN_COMMA:
            // <fn_decl_params> -> TOKEN_COMMA <type> <fn_decl_params_next>
            GET_NEW_TOKEN();

            // Set activity to the next item
            list_next(&callPair->value.paramList);
            // Get type of the active item in paramList
            Type saved = list_get_active(&callPair->value.paramList);
            // Check if parameter type matches the one from definition / declaration
            if (!can_assign(saved, st_token_to_type(&st, token))) {
                status = ERR_SEMANTIC_FUNC;
                break;
            }

            // move the argument into the temporary frame
            gen_print("MOVE TF@%%param%d ", list_active_index(&callPair->value.paramList));
            if (token.type == TOKEN_IDENTIFIER) {
                gen_print("LF@%s", token.str);
            } else {
                gen_print_literal(token);
            }
            gen_print("\n");

            GET_NEW_TOKEN();
            // read another comma and parameter or eps
            found = nt_fn_call_params_next(callPair);
            break;

        default:
            // <fn_decl_params> -> eps
            // there don't have to be any parameters

            // Check that the list is no longer active
            list_next(&callPair->value.paramList);
            if (list_is_active(&callPair->value.paramList)) {
                status = ERR_SEMANTIC_FUNC;
                break;
            }

            found = true;
            break;
    }
    return found;
}

bool nt_type() {
    bool found = false;

    switch (token.type) {
        case TOKEN_INTEGER_KW:
        case TOKEN_NUMBER_KW:
        case TOKEN_STRING_KW:
        case TOKEN_NIL:
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

    st_init(&st);
    gen_init(stdout);
    bool valid = nt_prog();
    gen_destroy();
    st_destroy(&st);

    if (status == SUCCESS && !valid) {
        char *fstring = "Syntax error on line %u:%u:\nUnexpected token `%s`\n\n";
        fprintf(stderr, fstring, token.lineNumber, token.characterNumber, token.str);
        return ERR_SYNTAX;
    }

    // TODO delete later
    if (status != SUCCESS) {
        fprintf(stderr, "error on line %u:%u:\n\n", token.lineNumber, token.characterNumber);
    }
    return status;
}

Status parser_destroy() {
    return SUCCESS;
}
