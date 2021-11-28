/**
 * @file include.h
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

#include "include.h"

const char *includeCode = ".IFJcode21\n"
                          "DEFVAR GF@_\n"
                          "DEFVAR GF@cond1\n"
                          "DEFVAR GF@cond2\n"
                          "DEFVAR GF@swap1\n"
                          "DEFVAR GF@swap2\n"
                          "DEFVAR GF@a\n"
                          "DEFVAR GF@b\n"
                          "DEFVAR GF@c\n"
                          "DEFVAR GF@d\n"
                          "JUMP %start0\n"
                          "LABEL %ERR_RT_NIL\n"
                          "EXIT int@8\n"
                          "LABEL %ERR_RT_DIVZERO\n"
                          "EXIT int@9\n"
                          "\n"
                          "################################# READS\n"
                          "LABEL reads\n"
                          "PUSHFRAME\n"
                          "DEFVAR LF@return\n"
                          "DEFVAR LF@readstr\n"
                          "READ LF@readstr string\n"
                          "MOVE LF@return LF@readstr\n"
                          "PUSHS LF@return\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "\n"
                          "############################### READI\n"
                          "LABEL readi\n"
                          "PUSHFRAME\n"
                          "DEFVAR LF@return\n"
                          "DEFVAR LF@readstr\n"
                          "DEFVAR LF@rettype\n"
                          "READ LF@readstr int\n"
                          "TYPE LF@rettype LF@readstr\n"
                          "JUMPIFEQ %readiNil nil@nil LF@rettype\n"
                          "MOVE LF@return LF@readstr\n"
                          "PUSHS LF@return\n"
                          "JUMP %end_readi\n"
                          "\n"
                          "LABEL %readiNil\n"
                          "MOVE LF@return nil@nil\n"
                          "PUSHS LF@return\n"
                          "LABEL %end_readi\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "\n"
                          "############################### READN\n"
                          "LABEL readn\n"
                          "PUSHFRAME\n"
                          "DEFVAR LF@return\n"
                          "DEFVAR LF@readstr\n"
                          "DEFVAR LF@rettype\n"
                          "READ LF@readstr float\n"
                          "TYPE LF@rettype LF@readstr\n"
                          "JUMPIFEQ %readnNil nil@nil LF@rettype\n"
                          "MOVE LF@return LF@readstr\n"
                          "PUSHS LF@return\n"
                          "JUMP %end_readn\n"
                          "\n"
                          "LABEL %readnNil\n"
                          "MOVE LF@return nil@nil\n"
                          "PUSHS LF@return\n"
                          "LABEL %end_readn\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "\n"
                          "############################### TOINTEGER\n"
                          "LABEL tointeger\n"
                          "PUSHFRAME\n"
                          "DEFVAR LF@return\n"
                          "DEFVAR LF@rettype\n"
                          "TYPE LF@rettype LF@%param0\n"
                          "JUMPIFEQ %tointegernil nil@nil LF@rettype\n"
                          "FLOAT2INT LF@return LF@%param0\n"
                          "PUSHS LF@return\n"
                          "JUMP %end_tointeger\n"
                          "\n"
                          "LABEL %tointegernil\n"
                          "MOVE LF@return nil@nil\n"
                          "PUSHS LF@return\n"
                          "LABEL %end_tointeger\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "\n"
                          "############################### SUBSTR\n"
                          "LABEL substr\n"
                          "PUSHFRAME\n"
                          "DEFVAR LF@areParamsCorrect\n"
                          "DEFVAR LF@strlen\n"
                          "DEFVAR LF@return\n"
                          "\n"
                          "JUMPIFEQ %ERR_RT_NIL LF@%param0 nil@nil\n"
                          "JUMPIFEQ %ERR_RT_NIL LF@%param1 nil@nil\n"
                          "JUMPIFEQ %ERR_RT_NIL LF@%param2 nil@nil\n"
                          "\n"
                          "STRLEN LF@strlen LF@%param0\n"
                          "\n"
                          "LT LF@areParamsCorrect LF@%param1 int@1\n"
                          "JUMPIFEQ %substrempty LF@areParamsCorrect bool@true\n"
                          "\n"
                          "GT LF@areParamsCorrect LF@%param2 LF@strlen\n"
                          "JUMPIFEQ %substrempty LF@areParamsCorrect bool@true\n"
                          "\n"
                          "LT LF@areParamsCorrect LF@%param2 LF@%param1\n"
                          "JUMPIFEQ %substrempty LF@areParamsCorrect bool@true\n"
                          "\n"
                          "# if we got here, everything is correct\n"
                          "DEFVAR LF@character\n"
                          "SUB LF@%param1 LF@%param1 int@1\n"
                          "SUB LF@%param2 LF@%param2 int@1\n"
                          "MOVE LF@return string@\n"
                          "\n"
                          "# while (i<j) {i++;}\n"
                          "DEFVAR LF@condition\n"
                          "LABEL %substr_while_start\n"
                          "GT LF@condition LF@%param1 LF@%param2\n"
                          "JUMPIFEQ %substr_while_end LF@condition bool@true\n"
                          "GETCHAR LF@character LF@%param0 LF@%param1\n"
                          "CONCAT LF@return LF@return LF@character\n"
                          "ADD LF@%param1 LF@%param1 int@1\n"
                          "JUMP %substr_while_start\n"
                          "\n"
                          "LABEL %substr_while_end\n"
                          "PUSHS LF@return\n"
                          "JUMP %end_substr\n"
                          "\n"
                          "LABEL %substrempty\n"
                          "MOVE LF@return string@\n"
                          "PUSHS LF@return\n"
                          "LABEL %end_substr\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "\n"
                          "############################### ORD\n"
                          "LABEL ord\n"
                          "PUSHFRAME\n"
                          "DEFVAR LF@return\n"
                          "DEFVAR LF@check\n"
                          "DEFVAR LF@strlen\n"
                          "JUMPIFEQ %ordfail LF@%param0 nil@nil\n"
                          "JUMPIFEQ %ordfail LF@%param1 nil@nil\n"
                          "STRLEN LF@strlen LF@%param0\n"
                          "GT LF@check LF@%param1 LF@strlen\n"
                          "JUMPIFEQ %ordnil LF@check bool@true\n"
                          "STRI2INT LF@return LF@%param0 LF@%param1\n"
                          "PUSHS LF@return\n"
                          "JUMP %end_ord\n"
                          "\n"
                          "LABEL %ordnil\n"
                          "MOVE LF@return nil@nil\n"
                          "PUSHS LF@return\n"
                          "LABEL %end_ord\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "\n"
                          "LABEL %ordfail\n"
                          "EXIT int@8\n"
                          "\n"
                          "############################### CHR\n"
                          "LABEL chr\n"
                          "PUSHFRAME\n"
                          "DEFVAR LF@return\n"
                          "DEFVAR LF@tmp\n"
                          "LT LF@tmp LF@%param0 int@0\n"
                          "JUMPIFEQ %chrdnil LF@tmp bool@true\n"
                          "GT LF@tmp LF@%param0 int@255\n"
                          "JUMPIFEQ %chrdnil LF@tmp bool@true\n"
                          "JUMPIFEQ %ERR_RT_NIL LF@%param0 nil@nil\n"
                          "INT2CHAR LF@return LF@%param0\n"
                          "PUSHS LF@return\n"
                          "JUMP %end_chr\n"
                          "\n"
                          "LABEL %chrnil\n"
                          "MOVE LF@return nil@nil\n"
                          "PUSHS LF@return\n"
                          "LABEL %end_chr\n"
                          "POPFRAME\n"
                          "RETURN\n";

#define SET_ATRIBUTES()   pair->value.varType = TYPE_NONE; \
                       pair->value.ID = st->idCounter++;\
                       pair->value.defined = true;

Status add_builtin_fn(SymTab *st) {
    // Builtin function reads
    HTabPair *pair = htab_insert(st->top, "reads");
    if (pair == NULL) {
        return ERR_INTERNAL;
    }
    SET_ATRIBUTES();
    list_append((&pair->value.returnList), STRING);

    // Builtin function readi
    pair = htab_insert(st->top, "readi");
    if (pair == NULL) {
        return ERR_INTERNAL;
    }
    SET_ATRIBUTES();
    list_append((&pair->value.returnList), INTEGER);

    // Builtin function readn
    pair = htab_insert(st->top, "readn");
    if (pair == NULL) {
        return ERR_INTERNAL;
    }
    SET_ATRIBUTES();
    list_append((&pair->value.returnList), NUMBER);

    // Builtin function write
    pair = htab_insert(st->top, "write");
    if (pair == NULL) {
        return ERR_INTERNAL;
    }
    SET_ATRIBUTES();
    pair->value.specialFn = true;

    // Builtin function tointeger
    pair = htab_insert(st->top, "tointeger");
    if (pair == NULL) {
        return ERR_INTERNAL;
    }
    SET_ATRIBUTES();
    list_append((&pair->value.paramList), NUMBER);
    list_append((&pair->value.returnList), INTEGER);

    // Builtin function substr
    pair = htab_insert(st->top, "substr");
    if (pair == NULL) {
        return ERR_INTERNAL;
    }
    SET_ATRIBUTES();
    list_append((&pair->value.paramList), STRING);
    list_append((&pair->value.paramList), NUMBER);
    list_append((&pair->value.paramList), NUMBER);
    list_append((&pair->value.returnList), STRING);

    // Builtin function ord
    pair = htab_insert(st->top, "ord");
    if (pair == NULL) {
        return ERR_INTERNAL;
    }
    SET_ATRIBUTES();
    list_append((&pair->value.paramList), STRING);
    list_append((&pair->value.paramList), INTEGER);
    list_append((&pair->value.returnList), INTEGER);

    // Builtin function chr
    pair = htab_insert(st->top, "chr");
    if (pair == NULL) {
        return ERR_INTERNAL;
    }
    SET_ATRIBUTES();
    list_append((&pair->value.paramList), INTEGER);
    list_append((&pair->value.returnList), STRING);

    return SUCCESS;
}

