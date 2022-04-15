#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "parser.h"
#include "optimizer.h"


extern FILE* LOGFILE;
const  char  LOGFILE_PATH[] = "logfiles/dump.tex";


Tree* differentiate(Tree* p_tree, char variable);

FILE* tex_open();
void  tex_tree(Tree* p_tree, bool phrase);
void  tex_msg(char message[]);


#endif