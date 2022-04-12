#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "parser.h"
#include "optimizer.h"


Tree* differentiate(Tree* p_tree, char variable);


#endif