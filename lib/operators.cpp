#include "../include/operators.h"


bool get_oper_type(hash_t hash, Node* p_node)
{
    switch(hash)
    {
        case OPER_ADD:  p_node->Value.oper_type = OPER_ADD;   return true;
        case OPER_SUB:  p_node->Value.oper_type = OPER_SUB;   return true;
        case OPER_MUL:  p_node->Value.oper_type = OPER_MUL;   return true;
        case OPER_DIV:  p_node->Value.oper_type = OPER_DIV;   return true;
        case OPER_POW:  p_node->Value.oper_type = OPER_POW;   return true;
        case OPER_LN:   p_node->Value.oper_type = OPER_LN;    return true;
        case OPER_SIN:  p_node->Value.oper_type = OPER_SIN;   return true;
        case OPER_COS:  p_node->Value.oper_type = OPER_COS;   return true;
        case OPER_SH:   p_node->Value.oper_type = OPER_SH;    return true;
        case OPER_CH:   p_node->Value.oper_type = OPER_CH;    return true;
        case OPER_DDX:  p_node->Value.oper_type = OPER_DDX;   return true;
    }
    return false;
}


void oper_name(char* name, Node* p_node)
{
    switch(p_node->Value.oper_type)
    {
        case OPER_ADD:  strcpy(name, "+"  );   break;
        case OPER_SUB:  strcpy(name, "-"  );   break;
        case OPER_MUL:  strcpy(name, "*"  );   break;
        case OPER_DIV:  strcpy(name, "/"  );   break;
        case OPER_POW:  strcpy(name, "^"  );   break;
        case OPER_LN:   strcpy(name, "ln" );   break;
        case OPER_SIN:  strcpy(name, "sin");   break;
        case OPER_COS:  strcpy(name, "cos");   break;
        case OPER_SH:   strcpy(name, "sh" );   break;
        case OPER_CH:   strcpy(name, "ch" );   break;
        case OPER_DDX:  strcpy(name, "d"  );   break;
    }
}
