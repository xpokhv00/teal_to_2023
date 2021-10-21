/**
 * @file scanner.c
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

#include "scanner.h"

// The file, that is being read
static FILE *inFile;

// Count lines from the start of file and store it in tokens
// this is for easier debugging
static unsigned lineCount;
// characters form the start of the line
static unsigned characterCount;

// if a letter is read after the token has ended, it is kept here
// until the next reading
static int nextLetter;

Status scanner_init(FILE *in) {
    inFile = in;
    return SUCCESS;
}

Status scanner_get_token(Token *pToken, bool end) {

}

Status scanner_destroy() {
    // deallocate all the resources
}

