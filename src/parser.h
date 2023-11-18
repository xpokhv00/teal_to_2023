/**
 * @file parser.h
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include "scanner.h"
#include "expression.h"
#include "types.h"
#include "symtable.h"
#include "generator.h"
#include "include.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

Status parser_run();

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

bool nt_var_decl_assign(Type *assignedType);

bool nt_assignment(HTabPair *fnPair);

bool nt_if(HTabPair *fnPair);

bool nt_else(HTabPair *fnPair);

bool nt_while(HTabPair *fnPair);

bool nt_return(HTabPair *fnPair);

bool nt_r_value_list(bool emptyValid, HTabPair *fnPair, TypeList *listAssign);

bool nt_r_value_list_next(HTabPair *fnPair, TypeList *listAssign);

bool nt_l_value_list(HTabPair *fnPair, TypeList *dstList, SymStack *generateLater);

bool nt_l_value_list_next(HTabPair *fnPair, TypeList *dstList, SymStack *generateLater);

bool nt_fn_call(bool discardReturn);
bool nt_fn_call_params(HTabPair *callPair);
bool nt_fn_call_params_next(HTabPair *callPair);
bool nt_type();

#endif // PARSER_H
