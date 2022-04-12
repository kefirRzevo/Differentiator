#include <math.h>

#include "../include/optimizer.h"
#include "../include/logs.h"


static Node*  kill_constants(Node* n);

static Node*  kill_obvious(Node* n);

static bool   is_equal(Node* n, double y);

static Node*  optimize_visit(Node* n);

static void   my_round(double* x);


#define L           n->Left
#define R           n->Right
#define T           n->Type
#define N           n->Value.number  
#define V           n->Value.variable
#define O           n->Value.oper_type
#define LT          n->Left->Type
#define RT          n->Right->Type
#define LNUM        n->Left->Value.number 
#define RNUM        n->Right->Value.number


static bool is_equal(Node* n, double y)
{
    assert(n);

    if(T == NUM)
        if(fabs(N - y) < EPS)
            return true;

    return false;
}


static void my_round(double* x)
{
    assert(x);

    if(fabs(*x) < EPS)
        *x = 0;
}


static Node* kill_obvious(Node* n)
{
    assert(n);

    if(T != OPER)
        return n;

    if(!(L && R))
        return n;

    Node* new_n = n;

    if(is_equal(L, 1))
        switch(O)
        {
            case OPER_MUL:  free(L);
                            free(n);
                            return new_n->Right;
            case OPER_POW:  free(L);
                            node_delete(R);
                            L = nullptr;
                            R = nullptr;
                            T = NUM;
                            N = 1;
                            return n;
            default:        return n;
        }
    if(is_equal(R, 1))
        switch(O)
        {
            case OPER_MUL:
            case OPER_DIV:  free(R);
                            free(n);
                            return new_n->Left;
            case OPER_POW:  free(R);
                            free(n);
                            return new_n->Left;
            default:        return n;
        }
    if(is_equal(L, 0))
        switch(O)
        {
            case OPER_ADD:  free(L);
                            free(n);
                            return new_n->Right;
            case OPER_SUB:  O = OPER_MUL;
                            LNUM = -1;
                            return n;
            case OPER_DIV:  
            case OPER_MUL:  
            case OPER_POW:  free(L);
                            node_delete(R);
                            L = nullptr;
                            R = nullptr;
                            T = NUM;
                            N = 0;
                            return n;
            default:        return n; 
        }
    if(is_equal(R, 0))
        switch(O)
        {
            case OPER_ADD:  
            case OPER_SUB:  free(R);
                            free(n);
                            return new_n->Left;
            case OPER_DIV:  fprintf(stderr, "DIV BY 0\n");
                            node_delete(n);
                            return nullptr;
            case OPER_MUL:  free(R);
                            node_delete(L);
                            L = nullptr;
                            R = nullptr;
                            T = NUM;
                            N = 0;
                            return n;
            case OPER_POW:  free(R);
                            node_delete(L);
                            L = nullptr;
                            R = nullptr;
                            T = NUM;
                            N = 1;
                            return n;
            default:        return n; 
        }
    return n;
}

static Node* kill_constants(Node* n)
{
    assert(n);

    if(T == VAR)
    {
        if(V == 'e')
        {
            T = NUM;
            N = E;
            return n;
        }
        if(V == 'p')
        {
            T = NUM;
            N = P;
            return n;
        }
    }
    
    if(T != OPER)
        return n;
    
    if(L)
        L = kill_constants(L);
    if(R)
        R = kill_constants(R);

    if(L && R)
    {
        if(LT == NUM && RT == NUM)
        {
            switch(O)
            {
                case OPER_ADD:    N = LNUM + RNUM;        break;
                case OPER_SUB:    N = LNUM - RNUM;        break;
                case OPER_MUL:    N = LNUM * RNUM;        break;
                case OPER_DIV:    N = LNUM / RNUM;        break;
                case OPER_POW:    N = pow(LNUM, RNUM);    break;
                default:          return n;
            } 
            my_round(&N);
            T = NUM;
            free(L);
            free(R);
            L = nullptr;
            R = nullptr;
            return n;
        }
    }

    if(!L && R)
    {
        if(RT == NUM)
        {
            switch(O)
            {
                case OPER_LN:     N = log(RNUM);    break;
                case OPER_SIN:    N = sin(RNUM);    break;
                case OPER_COS:    N = cos(RNUM);    break;
                case OPER_SH:     N = sinh(RNUM);   break;
                case OPER_CH:     N = cosh(RNUM);   break;
                default:          return n;
            }
            my_round(&N);
            T = NUM;
            free(R);
            R = nullptr;
        }
    }
    return n;
}


static Node* optimize_visit(Node* n)
{
    assert(n);

    n = kill_constants(n);
    n = kill_obvious(n);

    if(L)
        L = optimize_visit(L);
    if(R)
        R = optimize_visit(R);

    return n;
}


#undef L
#undef R
#undef T
#undef N
#undef V
#undef O
#undef LT
#undef RT
#undef LNUM
#undef RNUM


Tree* optimize(Tree* p_tree)
{
    assert(p_tree);

    size_t tree_size_before = p_tree->size;
    size_t tree_size_after  = 0;

    while(tree_size_before != tree_size_after)
    {
        tree_size_before = tree_size_after;
        p_tree->root     = optimize_visit(p_tree->root);
        tree_size_after  = count_tree_size(p_tree->root);
    }

    p_tree->size = count_tree_size(p_tree->root);
    return p_tree;
}