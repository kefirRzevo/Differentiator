#include "../include/differentiator.h"
#include "../include/logs.h"


static int   variable_location(Node* p_node, char variable);

static Node* find_variable(Node* p_node, char variable);

static Node* find_operator(Node* p_node, OperType oper);

static Node* dif_node(Node* n, char variable, Tree* p_tree);

static Node* node_dif(Node* n, Tree* p_tree);

static Node* oper_dif(Node* n);


enum var_location
{
    OUT   = 0,
    LEFT  = 1,
    RIGHT = 2,
    BOTH  = 3,
};


static Node* find_variable(Node* p_node, char variable)
{
    Node* found = nullptr;

    if(p_node->Type == VAR)
        if(p_node->Value.variable == variable)
            found = p_node;
    
    if(p_node->Left  && !found)
        found = find_variable(p_node->Left,  variable);
    if(p_node->Right && !found)
        found = find_variable(p_node->Right, variable);

    return found;
}


static Node* find_operator(Node* p_node, OperType oper)
{
    Node* found = nullptr;

    if(p_node->Type == OPER)
        if(p_node->Value.oper_type == oper)
            found = p_node;
    
    if(p_node->Left  && !found)
        found = find_operator(p_node->Left,  oper);
    if(p_node->Right && !found)
        found = find_operator(p_node->Right, oper);

    return found;
}


static int variable_location(Node* p_node, char variable)
{
    assert(p_node);

    int var_in_left  = 0;
    int var_in_right = 0;

    if(find_variable(p_node->Left,  variable))
        var_in_left  = 1;
    if(find_variable(p_node->Right, variable))
        var_in_right = 2;

    return var_in_left + var_in_right;
}


#define L           n->Left
#define R           n->Right
#define C(X)        node_cpy(X)
#define D(X)        new_oper_node(OPER_DDX, X, new_var_node(variable))
#define NUM(X)      new_num_node(X)

#define ADD(X, Y)   new_oper_node(OPER_ADD, X, Y)
#define SUB(X, Y)   new_oper_node(OPER_SUB, X, Y)
#define MUL(X, Y)   new_oper_node(OPER_MUL, X, Y)
#define DIV(X, Y)   new_oper_node(OPER_DIV, X, Y)
#define OPER(OP, X, Y)    new_oper_node(OP, X, Y)

static Node* oper_dif(Node* n)
{
    assert(n);
    assert(R->Type == VAR);

    char variable = R->Value.variable;
    n = L;

    switch(n->Type)
    {
        case NUM:   return NUM(0);

        case VAR:   if(n->Value.variable == variable)
                        return NUM(1);
                    else    
                        return NUM(0);

        case OPER:  switch(n->Value.oper_type)
                    {
                        case OPER_ADD:  return  ADD(D(L), D(R));
                        case OPER_SUB:  return  SUB(D(L), D(R));
                        case OPER_MUL:  return  ADD(MUL(D(L), C(R)), MUL(C(L), D(R)) );
                        case OPER_DIV:  return  DIV(SUB(MUL(D(L), C(R)), MUL(C(L), D(R))), OPER(OPER_POW, C(R), NUM(2)));
                        case OPER_POW:  switch(variable_location(n, variable))
                                        {
                                            case OUT:   return NUM(0);
                                            case LEFT:  return MUL(D(L), MUL(C(R), OPER(OPER_POW, C(L), SUB(C(R), NUM(1)) )  )   );
                                            case RIGHT: return MUL(D(R), MUL(OPER(OPER_LN, nullptr, C(L)), OPER(OPER_POW, C(L), C(R)) )  );
                                            case BOTH:  return MUL(OPER(OPER_POW, C(L), C(R)), ADD(MUL(D(R), OPER(OPER_LN, nullptr, C(L))), DIV(MUL(C(R), D(L)), C(L)) )  );
                                        }
                        case OPER_LN:   return  DIV(D(R), C(R));
                        case OPER_SIN:  return  MUL(D(R), OPER(OPER_COS, nullptr, C(R)));
                        case OPER_COS:  return  MUL(NUM(-1), MUL(D(R), OPER(OPER_SIN, nullptr, C(R))));
                        case OPER_SH:   return  MUL(D(R), OPER(OPER_CH, nullptr, C(R)) );
                        case OPER_CH:   return  MUL(D(R), OPER(OPER_SH, nullptr, C(R)) );
                        case OPER_DDX:  return  nullptr;
                    }
        default:    return nullptr;
    }
}


static void fill_node(Node* n1, Node* n2)
{
    assert(n1);
    assert(n2);

    n1->Left  = n2->Left;
    n1->Right = n2->Right;
    n1->Type  = n2->Type;

    memcpy(&n1->Value, &n2->Value, sizeof(NodeValue));
    free(n2);
}


static Node* node_dif(Node* n, Tree* p_tree)
{
    assert(n);
    assert(p_tree);

    hash_t hash = count_hash(p_tree->root);
    if(hash != p_tree->hash)
    {
        tex_tree(p_tree, 1);
        p_tree->hash = count_hash(p_tree->root);
        p_tree->size = count_tree_size(p_tree->root);
    }

    if(n->Type == OPER && n->Value.oper_type == OPER_DDX)
    {
        if(L->Type == OPER && L->Value.oper_type == OPER_DDX)
            L = node_dif(L, p_tree);

        Node* new_node = oper_dif(n);
        free(R);
        fill_node(n, new_node);
    }
    
    if(L)
        L = node_dif(L, p_tree);
    if(R)
        R = node_dif(R, p_tree);

    return n;
}

#undef L     
#undef R     
#undef C  
#undef D  
#undef NUM

#undef ADD
#undef SUB
#undef MUL
#undef DIV
#undef OPER


Tree* differentiate(Tree* p_tree, char variable)
{
    assert(p_tree);
    tex_open();

    Tree* new_tree = nullptr;
    tree_ctor(&new_tree);

    Node* old_root = p_tree->root;
    Node* var_node = new_var_node(variable);
    Node* new_root = new_oper_node(OPER_DDX, p_tree->root, var_node);
    p_tree->root   = new_root;

    p_tree->hash   = 0;
    new_tree->root = node_dif(new_root, p_tree);
    new_tree->size = count_tree_size(new_tree->root);
    p_tree->root   = old_root;

    return new_tree;
}
