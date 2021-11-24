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
                          "JUMP %start0\n"
                          "################################# WRITE\n"
                          "LABEL write\n"
                          "PUSHFRAME\n"
                          "\n"
                          "WRITE LF@param0\n"
                          "\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "\n"
                          "\n"
                          "################################# WRITE ENDS\n"
                          "\n"
                          "\n"
                          "################################# READS\n"
                          "LABEL reads\n"
                          "PUSHFRAME\n"
                          "DEFVAR LF@return\n"
                          "DEFVAR LF@readstr\n"
                          "\n"
                          "READ LF@readstr string\n"
                          "\n"
                          "# test : write LF@readstr\n"
                          "\n"
                          "MOVE LF@return LF@readstr\n"
                          "\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "############################### END READS\n"
                          "\n"
                          "############################### READI\n"
                          "LABEL readi\n"
                          "PUSHFRAME\n"
                          "DEFVAR LF@return\n"
                          "DEFVAR LF@readstr\n"
                          "DEFVAR LF@rettype\n"
                          "\n"
                          "READ LF@readstr int\n"
                          "TYPE LF@rettype LF@readstr\n"
                          "\n"
                          "JUMPIFEQ %readiNil nil@nil LF@rettype\n"
                          "\n"
                          "MOVE LF@return LF@readstr\n"
                          "\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "\n"
                          "LABEL %readiNil\n"
                          "\n"
                          "MOVE LF@return nil@nil\n"
                          "\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "\n"
                          "############################### END READI\n"
                          "\n"
                          "############################### READN\n"
                          "LABEL readn\n"
                          "PUSHFRAME\n"
                          "DEFVAR LF@return\n"
                          "DEFVAR LF@readstr\n"
                          "DEFVAR LF@rettype\n"
                          "\n"
                          "READ LF@readstr float\n"
                          "TYPE LF@rettype LF@readstr\n"
                          "\n"
                          "JUMPIFEQ %readnNil nil@nil LF@rettype\n"
                          "\n"
                          "MOVE LF@return LF@readstr\n"
                          "\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "\n"
                          "LABEL %readnNil\n"
                          "\n"
                          "MOVE LF@return nil@nil\n"
                          "\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "############################### END READN\n"
                          "\n"
                          "############################### TOINTEGER\n"
                          "LABEL tointeger\n"
                          "PUSHFRAME\n"
                          "DEFVAR LF@return\n"
                          "DEFVAR LF@rettype\n"
                          "\n"
                          "TYPE LF@rettype LF@param0\n"
                          "JUMPIFEQ %tointegernil nil@nil LF@rettype\n"
                          "\n"
                          "FLOAT2INT LF@return LF@param0\n"
                          "\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "\n"
                          "LABEL %tointegernil\n"
                          "MOVE LF@return nil@nil\n"
                          "\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "############################### END TOINTIGER\n"
                          "\n"
                          "############################### SUBSTR\n"
                          "LABEL substr\n"
                          "PUSHFRAME\n"
                          "DEFVAR LF@areParamsCorrect\n"
                          "DEFVAR LF@strlen\n"
                          "DEFVAR LF@return\n"
                          "STRLEN LF@strlen LF@param0\n"
                          "\n"
                          "LT LF@areParamsCorrect LF@param1 int@1\n"
                          "JUMPIFEQ %substrempty LF@areParamsCorrect bool@true\n"
                          "\n"
                          "GT LF@areParamsCorrect LF@param1 LF@strlen\n"
                          "JUMPIFEQ %substrempty LF@areParamsCorrect bool@true\n"
                          "\n"
                          "LT LF@areParamsCorrect LF@param2 int@1\n"
                          "JUMPIFEQ %substrempty LF@areParamsCorrect bool@true\n"
                          "\n"
                          "LT LF@areParamsCorrect LF@param2 LF@param1\n"
                          "JUMPIFEQ %substrempty LF@areParamsCorrect bool@true\n"
                          "\n"
                          "GT LF@areParamsCorrect LF@param2 LF@strlen\n"
                          "JUMPIFEQ %substrempty LF@areParamsCorrect bool@true\n"
                          "\n"
                          "JUMPIFEQ %substrnil LF@param0 nil@nil\n"
                          "JUMPIFEQ %substrnil LF@param1 nil@nil\n"
                          "JUMPIFEQ %substrnil LF@param2 nil@nil\n"
                          "\n"
                          "# if we got here, everything is correct\n"
                          "\n"
                          "DEFVAR LF@tmp\n"
                          "SUB LF@param1 LF@param1 int@1\n"
                          "SUB LF@param2 LF@param2 int@1\n"
                          "MOVE LF@return string@\n"
                          "#while cycle, concat return and tmp value, tmp == getchar on position i, until i != j\n"
                          "\n"
                          "LABEL %substrwhile\n"
                          "GETCHAR LF@tmp LF@param0 LF@param1\n"
                          "CONCAT LF@return LF@return LF@tmp\n"
                          "ADD LF@param1 LF@param1 int@1\n"
                          "LT LF@areParamsCorrect LF@param1 LF@param2\n"
                          "JUMPIFEQ %substrwhile LF@areParamsCorrect bool@true\n"
                          "\n"
                          "\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "\n"
                          "LABEL %substrnil\n"
                          "EXIT int@8\n"
                          "\n"
                          "LABEL %substrempty\n"
                          "MOVE LF@return string@\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "############################### END SUBSTR\n"
                          "\n"
                          "############################### ORD\n"
                          "LABEL ord\n"
                          "PUSHFRAME\n"
                          "DEFVAR LF@return\n"
                          "DEFVAR LF@check\n"
                          "DEFVAR LF@strlen\n"
                          "\n"
                          "JUMPIFEQ %ordfail LF@param0 nil@nil\n"
                          "JUMPIFEQ %ordfail LF@param1 nil@nil\n"
                          "STRLEN LF@strlen LF@param0\n"
                          "\n"
                          "GT LF@check LF@param1 LF@strlen\n"
                          "JUMPIFEQ %ordnil LF@check bool@true\n"
                          "\n"
                          "STRI2INT LF@return LF@param0 LF@param1\n"
                          "\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "\n"
                          "\n"
                          "LABEL %ordnil\n"
                          "MOVE LF@return nil@nil\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "\n"
                          "LABEL %ordfail\n"
                          "EXIT int@8\n"
                          "############################### END ORD\n"
                          "\n"
                          "\n"
                          "\n"
                          "############################### CHR\n"
                          "LABEL chr\n"
                          "PUSHFRAME\n"
                          "DEFVAR LF@return\n"
                          "DEFVAR LF@tmp\n"
                          "\n"
                          "LT LF@tmp LF@param0 int@0\n"
                          "JUMPIFEQ %chrdnil LF@tmp bool@true\n"
                          "GT LF@tmp LF@param0 int@255\n"
                          "JUMPIFEQ %chrdnil LF@tmp bool@true\n"
                          "JUMPIFEQ %chrerror LF@param0 nil@nil\n"
                          "\n"
                          "INT2CHAR LF@return LF@param0\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "\n"
                          "LABEL %chrnil\n"
                          "MOVE LF@return nil@nil\n"
                          "POPFRAME\n"
                          "RETURN\n"
                          "\n"
                          "LABEL %chrerror\n"
                          "EXIT int@8\n"
                          "############################### END CHR\n";

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

