#include "../include/differentiator.h"
#include "../include/logs.h"


static int variable_location(Node* p_node, char variable);

static Node* find_variable(Node* p_node, char variable);

static Node* node_dif(Node* n, char variable);

static Node* new_num_node(double num);

static Node* new_oper_node(OperType type, Node* lnode, Node* rnode);


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


static Node* new_num_node(double num)
{
    Node* p_new_node = (Node *)calloc(1, sizeof(Node));
    CALLOC_CHECK(p_new_node);

    p_new_node->Type         = NUM;
    p_new_node->Value.number = num;

    return p_new_node;
}


static Node* new_oper_node(OperType type, Node* lnode, Node* rnode)
{
    Node* p_new_node = (Node *)calloc(1, sizeof(Node));
    CALLOC_CHECK(p_new_node);

    p_new_node->Type            = OPER;
    p_new_node->Value.oper_type = type;
    p_new_node->Left            = lnode;
    p_new_node->Right           = rnode;

    return p_new_node;
}


#define L           n->Left
#define R           n->Right
#define C(X)        node_cpy(X)
#define D(X)        node_dif(X, variable)
#define NUM(X)      new_num_node(X)

#define ADD(X, Y)   new_oper_node(OPER_ADD, X, Y)
#define SUB(X, Y)   new_oper_node(OPER_SUB, X, Y)
#define MUL(X, Y)   new_oper_node(OPER_MUL, X, Y)
#define DIV(X, Y)   new_oper_node(OPER_DIV, X, Y)
#define OPER(OP, X, Y)    new_oper_node(OP, X, Y)

static Node* node_dif(Node* n, char variable)
{
    assert(n);

    NodeValue temp = {};
    switch(n->Type)
    {
        case NUM:   return new_num_node(0);

        case VAR:   if(n->Value.variable == variable)
                        return new_num_node(1);
                    else    
                        return new_num_node(0);

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
                    }
    }
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

    Tree* new_tree = nullptr;
    tree_ctor(&new_tree);

    new_tree->root = node_dif(p_tree->root, variable);
    new_tree->size = count_tree_size(new_tree->root);
    return new_tree;
}
