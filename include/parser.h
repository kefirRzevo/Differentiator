#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <ctype.h>

#include "tree.h"


Tree* parse(Tree* p_tree, char* equation_file_path);


#endif