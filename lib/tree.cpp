#include "../include/tree.h" 
#include "../include/logs.h"


Tree* tree_ctor(Tree** p_tree)
{
    Tree* new_tree  = (Tree* )calloc(1, sizeof(Tree));
    CALLOC_CHECK(new_tree);

    *p_tree         = new_tree;
    (*p_tree)->size = 0;

    return new_tree;
}


Node* node_delete(Node* tree_node)
{
    if(tree_node != nullptr)
    {
        node_delete(tree_node->Left );
        node_delete(tree_node->Right);

        free(tree_node);
    }
    return nullptr;
}


Tree* tree_dtor(Tree* p_tree)
{
    assert(p_tree);

    p_tree->root = node_delete(p_tree->root);
    free(p_tree);
    return nullptr;
}


Node* node_cpy(Node* p_node)
{
    if(!p_node)
    {
        return nullptr;
    }

    Node* new_node = (Node* )calloc(1, sizeof(Node));
    CALLOC_CHECK(new_node);

    new_node->Type  = p_node->Type;
    new_node->Value = p_node->Value;

    if(p_node->Left)
    {
        new_node->Left  = node_cpy(p_node->Left);
    }
    if(p_node->Right)
    {
        new_node->Right = node_cpy(p_node->Right);
    }
    return new_node;
}


void tree_visit(Node* p_node, void(*function)(Node*))
{
    if(!p_node)
        return;

    function(p_node);

    if(p_node->Left)
        tree_visit(p_node->Left,  function);
    if(p_node->Right)
        tree_visit(p_node->Right, function);
}


size_t count_tree_size(Node* p_node)
{
    if(p_node->Left && p_node->Right)
        return count_tree_size(p_node->Left) + count_tree_size(p_node->Right);

    if(p_node->Left)
        count_tree_size(p_node->Left);
    if(p_node->Right)
        count_tree_size(p_node->Right);
    
    return 1;
}
