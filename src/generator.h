/**
 * @file generator.h
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

Status gen_init(FILE *outFile);
void gen_buffer_start();
void gen_buffer_stop();
Status gen_print(const char *format, ...);
Status gen_prepend(const char *format, ...);
void gen_destroy();

#endif //GENERATOR_H
