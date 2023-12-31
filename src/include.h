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

#ifndef INCLUDE_H
#define INCLUDE_H

#include "symtable.h"
#include "types.h"

extern const char *includeCode;

Status add_builtin_fn(SymTab *st);

#endif //INCLUDE_H
