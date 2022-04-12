#include <stdio.h>

#include "include/differentiator.h"


int main()
{
    char equation_file_path[] = "func.txt";
    Tree equation_tree = {};
    Tree* dif_tree = nullptr;

    parse(&equation_tree, equation_file_path);
    tree_viz_dump(&equation_tree);
    dif_tree = differentiate(&equation_tree, 'x');
    tree_viz_dump(dif_tree);
    optimize(dif_tree);
    tree_viz_dump(dif_tree);

    node_delete(equation_tree.root);
    tree_dtor(dif_tree);
    return 0;
}
