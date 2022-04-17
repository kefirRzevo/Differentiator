#include "include/differentiator.h"


int main()
{
    char equation_file_path[] = "func.txt";
    Tree* equ_tree = nullptr;
    Tree* dif_tree = nullptr;
    tree_ctor(&equ_tree);
    parse(equ_tree, equation_file_path);
    tree_viz_dump(equ_tree);
    
    optimize(equ_tree);
    tree_viz_dump(equ_tree);

    dif_tree = differentiate(equ_tree, 'x');
    tree_viz_dump(dif_tree);

    optimize(dif_tree);
    tex_msg("Предлагаем читателю убедиться в том, что конечный ответ: ");
    tex_tree(dif_tree, 0);
    tree_viz_dump(dif_tree);

    tree_dtor(equ_tree);
    tree_dtor(dif_tree);
    return 0;
}
