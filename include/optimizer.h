#ifndef OPTIMIZER_H
#define OPTIMIZER_H


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"

const double EPS = 1E-05;
const double E   = 2.71828182845;
const double P   = 3.14159265358;


Tree* optimize(Tree* p_tree);


#endif