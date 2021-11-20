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

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

Status parser_init();

Status parser_run();

Status parser_destroy();

#endif // PARSER_H
