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

static unsigned callNumber = 0;


bool nt_prog() {
    bool found = false;

    switch (token.type) {
        // pravidlo <prog> -> <prolog> <prog_body>
        default:
            ASSERT_NT(nt_prolog());
            ASSERT_NT(nt_prog_body());
            found = true;
            break;
    }
    return found;
}

bool nt_prolog() {
    bool found = false;

    switch (token.type) {
        // pravidlo <prolog> -> TOKEN_REQUIRE TOKEN_STRING_LIT
        default:
            ASSERT_SUCCESS(add_builtin_fn(&st));
            gen_print("%s", includeCode);

            found = true;
            break;
    }
    return found;
}

bool nt_prog_body() {
    bool found = false;

    switch (token.type) {
//        case TOKEN_GLOBAL:
//            // <prog_body> -> <fn_decl> <prog_body>
//            found = nt_fn_decl() && nt_prog_body();
//            break;

        case TYPE_FUNCTION:
            // <prog_body> -> <fn_def> <prog_body>
            found = nt_fn_def() && nt_prog_body();
            break;

        case TYPE_ID:;
            // <prog_body> -> <fn_call> <prog_body>
            // the interpreter has to jump from call to call
            // it has to skip the definitions inbetween
            gen_print("LABEL %%start%u\n", callNumber);
            callNumber++;
            ASSERT_NT(nt_fn_call(true));
            gen_print("JUMP %%start%u\n", callNumber);
            ASSERT_NT(nt_prog_body());

            found = true;
            break;

        case TYPE_EOF:
            // pravidlo <prog_body> -> eps
            // The last label, after which the execution is ended
            gen_print("LABEL %%start%u\n", callNumber);
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
        case TYPE_GLOBAL:
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TYPE_ID);

            // Checks if identifier already exists in symtable
            // If not creates one
            HTabPair *pair;
            ASSERT_SUCCESS(st_add(&st, token, &pair));

            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TYPE_COLON);
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TYPE_FUNCTION);
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TYPE_LEFT_BRACKET);
            GET_NEW_TOKEN();
            if (!nt_fn_decl_params(pair)) {
                break;
            }
            ASSERT_TOKEN_TYPE(TYPE_RIGHT_BRACKET);
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
        case TYPE_INTEGER_KW:
        case TYPE_NUMBER_KW:
        case TYPE_STRING_KW:
        case TYPE_NIL:;

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
        case TYPE_COMMA:
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
            // there don't have to be any more parameters
            found = true;
            break;
    }
    return found;
}

bool nt_fn_def() {
    bool found = false;

    switch (token.type) {
        case TYPE_FUNCTION:
            // <fn_def> -> TOKEN_FUNCTION TOKEN_IDENTIFIER
            // TOKEN_PAR_L <fn_def_params> TOKEN_PAR_R
            // <fn_returns> <fn_body> TOKEN_END
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TYPE_ID);

            // Check if function is already declared
            HTabPair *fnPair = st_lookup(&st, token.str, false);
            bool isDeclared = fnPair != NULL;
            if (!isDeclared) {
                // Adds function name into symtable
                ASSERT_SUCCESS(st_add(&st, token, &fnPair));
                fnPair = st_lookup(&st, token.str, false);
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
            ASSERT_TOKEN_TYPE(TYPE_LEFT_BRACKET);
            GET_NEW_TOKEN();

            // Create local frame for this function
            st_push_frame(&st);

            gen_print("PUSHFRAME\n");
            ASSERT_NT(nt_fn_def_params(fnPair, isDeclared));
            ASSERT_TOKEN_TYPE(TYPE_RIGHT_BRACKET);
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TYPE_LEFT_CURLY_BRACKET);
            GET_NEW_TOKEN();
            ASSERT_NT(nt_fn_returns(fnPair, isDeclared));
            ASSERT_NT(nt_fn_body(fnPair));

            // Implicit return of all values NIL
            unsigned numReturns = list_count(&fnPair->value.returnList);
            for (unsigned i=0; i<numReturns; i++) {
                gen_print("PUSHS nil@nil\n");
            }
            // the end of the function
            gen_print("LABEL %%end_of_fn_%s\n", fnPair->key);
            gen_print("POPFRAME\n");
            gen_print("RETURN\n");

            // Destroy local frame
            st_pop_frame(&st);

            ASSERT_TOKEN_TYPE(TYPE_RIGHT_CURLY_BRACKET);
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
        case TYPE_ID:;
            // <fn_def_params> -> TOKEN_IDENTIFIER TOKEN_COLON <type> <fn_def_params_next>

            // Check for parameter name in symtable, add it if not found
            HTabPair *paramPair;
            ASSERT_SUCCESS(st_add(&st, token, &paramPair));

            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TYPE_COLON);
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
            gen_print("MOVE LF@$%u LF@%%param%u\n",
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

            // Check that the parameter list is no longer active
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
        case TYPE_COMMA:
            // <fn_def_params_next> -> TOKEN_COMMA TOKEN_IDENTIFIER TOKEN_COLON <type> <fn_decl_params_next>
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TYPE_ID);

            // Check for parameter name in symtable, add it if not found
            HTabPair *paramPair;
            ASSERT_SUCCESS(st_add(&st, token, &paramPair));

            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TYPE_COLON);
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

            // move the parameter to newly declared local variable
            gen_print("DEFVAR LF@$%u\n", paramPair->value.ID);
            gen_print("MOVE LF@$%u LF@%%param%u\n",
                      paramPair->value.ID,
                      list_active_index(&pair->value.paramList)
            );

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
            // there don't have to be any more parameters

            // Check that the parameter list is no longer active
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

bool nt_fn_returns(HTabPair *pair, bool isDeclared) {
    bool found = false;

    switch (token.type) {
        case TYPE_COLON:
            // <fn_returns> -> TOKEN_COLON <type> <fn_returns_next>
            GET_NEW_TOKEN();

            Type returnType = token_keyword_to_type(token.type);
            if (isDeclared) {
                // Check if return types from declaration match the ones from definition
                list_first(&pair->value.returnList);
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
            if (!nt_fn_returns_next(pair, isDeclared)) {
                break;
            }
            found = true;
            break;

        default:
            // <fn_returns> -> eps
            // function can be without return

            // Check that the return list is no longer active
            list_first(&pair->value.returnList);
            if (list_is_active(&pair->value.returnList)) {
                status = ERR_SEMANTIC_FUNC;
                break;
            }

            found = true;
            break;
    }
    return found;
}

bool nt_fn_returns_next(HTabPair *pair, bool isDeclared) {
    bool found = false;

    switch (token.type) {
        case TYPE_COMMA:
            // <fn_returns_next> -> TOKEN_COMMA <type> <fn_returns_next>
            GET_NEW_TOKEN();

            Type returnType = token_keyword_to_type(token.type);
            if (isDeclared) {
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
            if (!nt_fn_returns_next(pair, isDeclared)) {
                break;
            }
            found = true;
            break;

        default:
            // <fn_returns_next> -> eps
            // function can be without any further returns

            // Check that the return list is no longer active
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
        case TYPE_VAR:
            // variable declaration
            ASSERT_NT(nt_var_decl());
            ASSERT_NT(nt_fn_body(fnPair));
            found = true;
            break;

        case TYPE_ID:;
            // assignment or function call
            // <fn_body> -> <assignment> <fn_body>
            // <fn_body> -> <fn_call> <fn_body>

            // Get next token to know what we're dealing with
            Token nextToken;
            ASSERT_SUCCESS(scanner_get_token(&nextToken));
            bool isFunction = (nextToken.type == TYPE_LEFT_BRACKET);
            scanner_unget_token(nextToken);

            if (isFunction) {
                ASSERT_NT(nt_fn_call(false));
                ASSERT_NT(nt_fn_body(fnPair));
            } else {
                ASSERT_NT(nt_assignment(fnPair));
                ASSERT_NT(nt_fn_body(fnPair));
            }
            found = true;
            break;

        case TYPE_RETURN:
            ASSERT_NT(nt_return(fnPair));
            ASSERT_NT(nt_fn_body(fnPair)); // TODO maybe stop after return?
            found = true;
            break;

        case TYPE_IF:
            found = nt_if(fnPair) && nt_fn_body(fnPair);
            break;

        case TYPE_WHILE:
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
        case TYPE_VAR:
            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TYPE_ID);

            // Save the identifier for later, when we define the variable
            Token idToken = token;
            status = scanner_get_token(&token);
            if (status != SUCCESS) {
                break;
            }
            ASSERT_TOKEN_TYPE(TYPE_COLON);
            GET_NEW_TOKEN();

            // Save the type for later, when we define the variable
            Type varType = token_keyword_to_type(token.type);
            if (varType == TYPE_NONE) {
                status = ERR_SYNTAX;
                break;
            }
            GET_NEW_TOKEN();

            Type assignedType;
            ASSERT_NT(nt_var_decl_assign(&assignedType));

            // Only after the expression is evaluated,
            // we can add the new variable into the symbol table
            // and use DEFVAR to define it
            // Adds variable name into symtable
            HTabPair *varPair;
            ASSERT_SUCCESS(st_add(&st, idToken, &varPair));
            scanner_destroy_token(&idToken);
            // Set type of new variable
            varPair->value.varType = varType;

            // Generate appropriate code
            gen_prepend("DEFVAR LF@$%u\n", varPair->value.ID);

            if (!can_assign(varPair->value.varType, assignedType)) {
                status = ERR_SEMANTIC_ASSIGN;
                break;
            }

            gen_print("POPS LF@$%u\n", varPair->value.ID);

            found = true;
            break;

        default:
            break;
    }
    return found;
}

bool nt_var_decl_assign(Type *assignedType) {
    bool found = false;
    Type returnType;

    switch (token.type) {
        case TYPE_ASSIGN:
            GET_NEW_TOKEN();

            bool isFunction = false;

            if (token.type == TYPE_ID) {
                // This could be an expression or function call
                Token nextToken;
                ASSERT_SUCCESS(scanner_get_token(&nextToken));
                isFunction = (nextToken.type == TYPE_LEFT_BRACKET);
                scanner_unget_token(nextToken);
            }

            if (isFunction) {
                // Check if there is at least one return, and that it is the correct type
                HTabPair *fnPair = st_lookup(&st, token.str, false);
                if (fnPair == NULL) {
                    // Cannot use function that does not exist
                    status = ERR_SEMANTIC_DEF;
                    break;
                }
                unsigned numReturns = list_count(&fnPair->value.returnList);
                if (numReturns == 0) {
                    status = ERR_SEMANTIC_FUNC;
                    break;
                }

                list_first(&fnPair->value.returnList);
                returnType = list_get_active(&fnPair->value.returnList);

                ASSERT_NT(nt_fn_call(false));
                for (unsigned i=0; i<numReturns-1; i++) {
                    gen_print("POPS GF@_\n");
                }
            } else {
                ASSERT_NT(nt_expr(&token, &st, &status, &returnType));
            }

            found = true;
            break;

        default:
            // the variable does not have to be assigned, which makes it nil
            gen_print("PUSHS nil@nil\n");
            returnType = NIL;
            found = true;
            break;
    }
    if (assignedType) {
        *assignedType = returnType;
    }
    return found;
}

bool nt_assignment(HTabPair *fnPair) {
    bool found = false;

    switch (token.type) {
        case TYPE_ID:;
            TypeList dstList = list_init();

            // Tokens of l-values will be stored here
            SymStack generateLater;
            symstack_init(&generateLater);
            ASSERT_NT(nt_l_value_list(fnPair, &dstList, &generateLater));
            ASSERT_TOKEN_TYPE(TYPE_ASSIGN);
            GET_NEW_TOKEN();

            bool isFunction = false;
            unsigned actualReturns;
            unsigned neededReturns;
            TypeList srcList = list_init();
            HTabPair *calledFnPair;

            // In order to be a function, the token must be an identifier
            if (token.type == TYPE_ID) {
                // Check if the identifier is a variable or a function
                // Get next token to know what we're dealing with
                Token nextToken;
                ASSERT_SUCCESS(scanner_get_token(&nextToken));
                isFunction = (nextToken.type == TYPE_LEFT_BRACKET);
                scanner_unget_token(nextToken);
            }

            if (isFunction) {
                calledFnPair = st_lookup(&st, token.str, false);
                actualReturns = list_count(&calledFnPair->value.returnList);
                neededReturns = symstack_count(&generateLater);
                ASSERT_NT(nt_fn_call(false));
            }
            else {
                ASSERT_NT(nt_r_value_list(false, fnPair, &srcList));
                actualReturns = list_count(&srcList);
                neededReturns = symstack_count(&generateLater);
            }

            // Semantic check needed for code generation
            if (actualReturns < neededReturns) {
                status = isFunction ? ERR_SEMANTIC_FUNC : ERR_SEMANTIC_OTHER;
                break;
            }
            else {
                if (isFunction) {
                    if (!list_can_assign(&dstList, &calledFnPair->value.returnList)) {
                        status = ERR_SEMANTIC_FUNC;
                        break;
                    }
                }
                else {
                    if (!list_can_assign(&dstList, &srcList)) {
                        status = ERR_SEMANTIC_ASSIGN;
                        break;
                    }
                }
            }
            list_destroy(&srcList);
            list_destroy(&dstList);

            // if there are excess variables on the right, we will have to pop them
            unsigned popCount = actualReturns - neededReturns;
            for (unsigned i=0; i<popCount; i++) {
                gen_print("POPS GF@_\n");
            }
            while (!symstack_is_empty(&generateLater)) {
                gen_print("POPS ");
                Token varToken = symstack_pop(&generateLater).token;
                gen_print_var(varToken, &st);
                scanner_destroy_token(&varToken);
                gen_print("\n");
            }
            symstack_destroy(&generateLater);

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
        case TYPE_IF:
            // <if> -> TOKEN_IF <expr> TOKEN_THEN <fn_body> <else> TOKEN_END
            GET_NEW_TOKEN();
            ASSERT_NT(nt_expr(&token, &st, &status, NULL));

            // CODE GENERATION
            int thenLabel = gen_new_label();
            int elseLabel = gen_new_label();
            int endLabel = gen_new_label();

            gen_conditional(thenLabel, elseLabel);

            gen_print("LABEL %%%d\n", thenLabel);

            ASSERT_TOKEN_TYPE(TYPE_THEN);
            GET_NEW_TOKEN();

            st_push_frame(&st);
            ASSERT_NT(nt_fn_body(fnPair));
            st_pop_frame(&st);

            gen_print("JUMP %%%d\n", endLabel);
            gen_print("LABEL %%%d\n", elseLabel);

            ASSERT_NT(nt_else(fnPair));
            ASSERT_TOKEN_TYPE(TYPE_RIGHT_CURLY_BRACKET);

            gen_print("LABEL %%%d\n", endLabel);

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
        case TYPE_ELSE:
            // <else> -> TOKEN_ELSE <fn_body>
            GET_NEW_TOKEN();
            st_push_frame(&st);
            ASSERT_NT(nt_fn_body(fnPair));
            st_pop_frame(&st);
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
        case TYPE_WHILE:
            // <while> -> TOKEN_WHILE <expr> TOKEN_DO <fn_body> TOKEN_END
            GET_NEW_TOKEN();

            // CODE GENERATION
            int evalLabel = gen_new_label();
            int doLabel = gen_new_label();
            int endLabel = gen_new_label();

            // special buffering for while
            // DEFVAR instructions will be printed here, effectively
            gen_buffer_start();
            gen_print("LABEL %%%d\n", evalLabel);

            ASSERT_NT(nt_expr(&token, &st, &status, NULL));

            gen_conditional(doLabel, endLabel);
            gen_print("LABEL %%%d\n", doLabel);

            // execute the body of the while
            ASSERT_TOKEN_TYPE(TYPE_DO);
            GET_NEW_TOKEN();
            st_push_frame(&st);
            ASSERT_NT(nt_fn_body(fnPair));
            st_pop_frame(&st);

            gen_buffer_stop();
            // evaluate the condition
            gen_print("JUMP %%%d\n", evalLabel);
            // end the cycle
            gen_print("LABEL %%%d\n", endLabel);

            ASSERT_TOKEN_TYPE(TYPE_RIGHT_CURLY_BRACKET);
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
        case TYPE_RETURN:
            // <return> -> TOKEN_RETURN <r_value_list>
            GET_NEW_TOKEN();
            TypeList typeList = list_init();
            ASSERT_NT(nt_r_value_list(true, fnPair, &typeList));

            // Error when returning more returns than zou are supposed to return
            if (list_count(&typeList) > list_count(&fnPair->value.returnList)) {
                status = ERR_SEMANTIC_FUNC;
                break;
            }
            // Push nil into list if fewer returns than expected
            if (list_count(&typeList) < list_count(&fnPair->value.returnList)) {
                // Implicit return of all values NIL
                unsigned numNils = list_count(&typeList) - list_count(&fnPair->value.returnList);
                for (unsigned i=0; i<numNils; i++) {
                    gen_print("PUSHS nil@nil\n");
                    list_append(&typeList, NIL);
                }
            }
            // Check identity of typelists
            if (!list_can_assign(&fnPair->value.returnList, &typeList)) {
                status = ERR_SEMANTIC_DEF;
                break;
            }
            list_destroy(&typeList);

            // Go to the end of the function
            gen_print("JUMP %%end_of_fn_%s\n", fnPair->key);
            found = true;
            break;

        default:
            break;
    }
    return found;
}

bool nt_r_value_list(bool emptyValid, HTabPair *fnPair, TypeList *srcList) {
    bool found = false;

    switch (token.type) {
        case TYPE_ID:
        case TYPE_INTEGER_LIT:
        case TYPE_DOUBLE_LIT:
        case TYPE_STRING_LIT:
        case TYPE_NIL:
        case TYPE_LEFT_BRACKET:;
            // Cannot be a function
            // <r_value_list> -> <expr> <r_value_list_next>
            Type exprType;
            ASSERT_NT(nt_expr(&token, &st, &status, &exprType));

            if (srcList) {
                list_append(srcList, exprType);
            }

            ASSERT_NT(nt_r_value_list_next(fnPair, srcList));
            found = true;
            break;

        default:
            if (emptyValid) {
                found = true;
            }
            break;
    }
    return found;
}

bool nt_r_value_list_next(HTabPair *fnPair, TypeList *srcList) {
    bool found = false;

    switch (token.type) {
        case TYPE_COMMA:
            GET_NEW_TOKEN();
            Type exprType;
            ASSERT_NT(nt_expr(&token, &st, &status, &exprType));

            if (srcList) {
                list_append(srcList, exprType);
            }

            ASSERT_NT(nt_r_value_list_next(fnPair, srcList));
            found = true;
            break;

        default:
            // can be empty
            found = true;
            break;
    }
    return found;
}

bool nt_l_value_list(HTabPair *fnPair, TypeList *dstList, SymStack *generateLater) {
    bool found = false;

    switch (token.type) {
        case TYPE_ID:;

            // Add variable types from left side to list
            Type lValueType = st_token_to_type(&st, token);
            list_append(dstList, lValueType);

            // Add for generation
            Symbol s = { .token = token };
            symstack_push(generateLater, s);

            // WARNING: do NOT use SCANNER_GET_TOKEN() macro
            // the token needs to stay allocated until the code is generated
            status = scanner_get_token(&token);
            if (status != SUCCESS) {
                break;
            }

            ASSERT_NT(nt_l_value_list_next(fnPair, dstList, generateLater));
            found = true;
            break;

        default:
            break;
    }
    return found;
}

bool nt_l_value_list_next(HTabPair *fnPair, TypeList *dstList, SymStack *generateLater) {
    bool found = false;

    switch (token.type) {
        case TYPE_COMMA:
            GET_NEW_TOKEN();

            // Add variable types from left side to list
            Type lValueType = st_token_to_type(&st, token);
            list_append(dstList, lValueType);

            // Add for generation
            Symbol s = { .token = token };
            symstack_push(generateLater, s);

            ASSERT_TOKEN_TYPE(TYPE_ID);
            // WARNING: do NOT use SCANNER_GET_TOKEN() macro
            // the token needs to stay allocated until the code is generated
            status = scanner_get_token(&token);
            if (status != SUCCESS) {
                break;
            }

            ASSERT_NT(nt_l_value_list_next(fnPair, dstList, generateLater));
            found = true;
            break;

        default:
            found = true;
            break;
    }
    return found;
}

bool nt_fn_call(bool discardReturn) {
    bool found = false;

    switch (token.type) {
        case TYPE_ID:;
            // <fn_call> -> TOKEN_IDENTIFIER TOKEN_PAR_L <fn_call_params> TOKEN_PAR_R

            // Get the pair of called function
            HTabPair *callPair = st_lookup(&st, token.str, false);

            GET_NEW_TOKEN();
            ASSERT_TOKEN_TYPE(TYPE_LEFT_BRACKET);
            GET_NEW_TOKEN();

            // Check whether the function is declared or defined
            bool isDeclared = callPair != NULL;
            if (!isDeclared) {
                // Cannot call an undeclared function
                status = ERR_SEMANTIC_DEF;
                break;
            }

            if (!callPair->value.specialFn) {
                gen_print("CREATEFRAME\n");
            }
            ASSERT_NT(nt_fn_call_params(callPair));
            if (!callPair->value.specialFn) {
                gen_print("CALL %s\n", callPair->key);
            }

            if (discardReturn) {
                unsigned count = list_count(&callPair->value.returnList);
                for (unsigned i=0; i<count; i++) {
                    gen_print("POPS GF@_\n");
                }
            }

            ASSERT_TOKEN_TYPE(TYPE_RIGHT_BRACKET);
            GET_NEW_TOKEN();
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
        case TYPE_ID: // This is not needed - only inside function body
        case TYPE_INTEGER_LIT:
        case TYPE_DOUBLE_LIT:
        case TYPE_STRING_LIT:
        case TYPE_NIL:;

            if (!callPair->value.specialFn) {
                // Set activity to the first item
                list_first(&callPair->value.paramList);
                // Get type of the first item in paramList
                Type saved = list_get_active(&callPair->value.paramList);
                // Check if parameter type matches the one from definition / declaration
                if (!can_assign(saved, st_token_to_type(&st, token))) {
                    status = ERR_SEMANTIC_FUNC;
                    break;
                }
            }
            else {
                if (!strcmp(callPair->key, "write")) {
                    ASSERT_SUCCESS(gen_write(token, &st));
                }
            }

            // move the argument into the temporary frame
            if (!callPair->value.specialFn) {
                gen_print("DEFVAR TF@%%param%d\n", list_active_index(&callPair->value.paramList));
                gen_print("MOVE TF@%%param%d ", list_active_index(&callPair->value.paramList));
                gen_print_value(token, &st);
                gen_print("\n");
            }

            GET_NEW_TOKEN();
            found = nt_fn_call_params_next(callPair);
            break;

        default:
            // <fn_call_params> -> eps
            // there don't have to be any parameters

            // write function will not be checked
            if (!callPair->value.specialFn) {
                // Check that the list is no longer active
                list_first(&callPair->value.paramList);
                if (list_is_active(&callPair->value.paramList)) {
                    status = ERR_SEMANTIC_FUNC;
                    break;
                }
            }

            found = true;
            break;
    }
    return found;
}

bool nt_fn_call_params_next(HTabPair *callPair) {
    bool found = false;

    switch (token.type) {
        case TYPE_COMMA:
            // <fn_decl_params> -> TOKEN_COMMA <type> <fn_decl_params_next>
            GET_NEW_TOKEN();

            // Do this only for write function
            if (!callPair->value.specialFn) {
                // Set activity to the next item
                list_next(&callPair->value.paramList);
                // Get type of the active item in paramList
                Type saved = list_get_active(&callPair->value.paramList);
                // Check if parameter type matches the one from definition / declaration
                if (!can_assign(saved, st_token_to_type(&st, token))) {
                    status = ERR_SEMANTIC_FUNC;
                    break;
                }
            }
            else {
                if (!strcmp(callPair->key, "write")) {
                    ASSERT_SUCCESS(gen_write(token, &st));
                }
            }

            // move the argument into the temporary frame
            if (!callPair->value.specialFn) {
                gen_print("DEFVAR TF@%%param%d\n", list_active_index(&callPair->value.paramList));
                gen_print("MOVE TF@%%param%d ", list_active_index(&callPair->value.paramList));
                gen_print_value(token, &st);
                gen_print("\n");
            }

            GET_NEW_TOKEN();
            // read another comma and parameter or eps
            found = nt_fn_call_params_next(callPair);
            break;

        default:
            // <fn_decl_params> -> eps
            // there don't have to be any more parameters

            // Do this only for write function
            if (!callPair->value.specialFn) {
                // Check that the list is no longer active
                list_next(&callPair->value.paramList);
                if (list_is_active(&callPair->value.paramList)) {
                    status = ERR_SEMANTIC_FUNC;
                    break;
                }
            }

            found = true;
            break;
    }
    return found;
}

bool nt_type() {
    bool found = false;

    switch (token.type) {
        case TYPE_INTEGER_KW:
        case TYPE_NUMBER_KW:
        case TYPE_STRING_KW:
        case TYPE_NIL:
            GET_NEW_TOKEN();
            found = true;

        default:
            break;
    }
    return found;
}

// The only function called from main
Status parser_run() {
    status = scanner_get_token(&token);
    if (status != SUCCESS) {
        return status;
    }

    st_init(&st);
    gen_init(stdout);
    // run the parser
    bool valid = nt_prog();

    gen_destroy();
    st_destroy(&st);

    if (status == SUCCESS && !valid) {
        char *fstring = "Syntax error on line %u:%u:\nUnexpected token `%s`\n\n";
        fprintf(stderr, fstring, token.lineNumber, token.characterNumber, token.str);
        return ERR_SYNTAX;
    }

    if (status != SUCCESS) {
        fprintf(stderr, "error on line %u:%u:\n\n", token.lineNumber, token.characterNumber);
    }

    scanner_destroy_token(&token);
    return status;
}
