#include "../include/parser.h"
#include "../include/operators.h"
#include "../include/logs.h"


char* iterator = nullptr;


static size_t get_file_size(FILE** const pp_file, char* textfile_path, char** pp_buf);
static void   skip_spaces(char* p_buf);
static bool   check_bin_opers(hash_t* oper, OperType oper1, OperType oper2);
static bool   check_uno_opers(hash_t* oper);
static char*  find_open_bracket(char* cur_position);
static Node*  get_data();

static Node* GetG();
static Node* GetE();
static Node* GetT();
static Node* GetP();
static Node* GetU();
static Node* GetN();


static size_t get_file_size(FILE** const pp_file, char* textfile_path, char** pp_buf)
{
    assert(pp_file);
    assert(textfile_path);
    assert(pp_buf);

    *pp_file = fopen(textfile_path, "r");
    FILE_CHECK(*pp_file);

    size_t current_position = (size_t)  ftell(*pp_file);
                                        fseek(*pp_file,  0L, SEEK_END);
    size_t file_size        = (size_t)  ftell(*pp_file) + 1;

    char* temp_buf_ptr = (char *)calloc(file_size, sizeof(char));
    CALLOC_CHECK(temp_buf_ptr);
    *pp_buf    = temp_buf_ptr;

    fseek(*pp_file, (long) current_position, SEEK_SET);
    
    return file_size;
}


static void skip_spaces(char* p_buf)
{
    assert(p_buf);

    char* fast_iterator = p_buf;
    char* slow_iterator = p_buf;

    while(*fast_iterator != '\0')
    {
        if(!isspace(*fast_iterator))
        {
            *slow_iterator = *fast_iterator;
            slow_iterator++;
        }
        fast_iterator++;
    }

    *slow_iterator = '\0';
}


static char* find_open_bracket(char* cur_position)
{
    assert(cur_position);

    while(*cur_position != '(' && *cur_position != '\0')
        cur_position++;
    
    return cur_position;
}


static bool check_bin_opers(hash_t* oper, OperType oper1, OperType oper2)
{
    assert(oper);

    *oper = qhash(iterator, 1);
    
    if(*oper == oper1 || *oper == oper2)
        return true;
    
    return false;
}


static bool check_uno_oper(hash_t* oper)
{
    assert(oper);

    char* end = find_open_bracket(iterator);
    *oper = qhash(iterator, end - iterator);

    switch(*oper)
    {
        case OPER_LN:  iterator = end; return true;
        case OPER_SIN: iterator = end; return true;
        case OPER_COS: iterator = end; return true;
        case OPER_SH:  iterator = end; return true;
        case OPER_CH:  iterator = end; return true;
        default:       return false;
    }

    return false;
}


static Node* get_data()
{
    Node* new_node = (Node* )calloc(1, sizeof(Node));
    CALLOC_CHECK(new_node);

    char*  endstr = nullptr;
    double number = strtod(iterator, &endstr);

    if(endstr != iterator)
    {
        new_node->Type         = NUM;
        new_node->Value.number = number;
        iterator               = endstr;
        return new_node;
    }

    if(isalpha(*iterator))
    {
        new_node->Type           = VAR;
        new_node->Value.variable = *iterator;
        iterator++;
        return new_node;   
    }

    free(new_node);
    return nullptr;
}


static Node* GetG()
{
    Node* root = GetE();

    if(*iterator != '\0')
    {
        fprintf(stderr, "Bad ending: %s\n", iterator);
        node_delete(root);
        return nullptr;
    }
    return root;
}


static Node* GetE()
{
    Node* new_node = GetT();
    hash_t oper    = 0;

    while(check_bin_opers(&oper, OPER_ADD, OPER_SUB))
    {
        iterator++;
        Node* oper_node  = new_oper_node((OperType)oper, nullptr, nullptr);
        oper_node->Left  = new_node;
        oper_node->Right = GetT();
        new_node = oper_node;
    }
    return new_node;
}


static Node* GetT()
{
    Node* new_node = GetP();
    hash_t oper    = 0;

    while(check_bin_opers(&oper, OPER_MUL, OPER_DIV))
    {
        iterator++;
        Node* oper_node  = new_oper_node((OperType)oper, nullptr, nullptr);
        oper_node->Left  = new_node;
        oper_node->Right = GetP();
        new_node = oper_node;
    }
    return new_node;
}


static Node* GetP()
{
    Node* new_node = GetU();
    hash_t oper    = 0;

    while(check_bin_opers(&oper, OPER_POW, OPER_POW))
    {
        iterator++;
        Node* oper_node  = new_oper_node((OperType)oper, nullptr, nullptr);
        oper_node->Left  = new_node;
        oper_node->Right = GetU();
        new_node = oper_node;
    }
    return new_node;
}


static Node* GetU()
{
    Node* new_node = nullptr;
    hash_t oper    = 0;

    if(*iterator == '(')
    {
        iterator++;
        new_node = GetE();
        if(*iterator != ')')
        {
            fprintf(stderr, "No closing bracket: %s\n", iterator);
            node_delete(new_node);
            return nullptr;
        }
        iterator++;
    }
    else if(!strncmp(iterator, "d(", 2))
    {
        iterator+=2;
        new_node = new_oper_node(OPER_DDX, nullptr, nullptr);
        new_node->Left = GetE();
        if(strncmp(iterator, ")d(", 3))
        {
            fprintf(stderr, "There should be )d(... : %s\n", iterator);
            node_delete(new_node);
            return nullptr;            
        }
        iterator+=3;
        new_node->Right = GetN();
        if(*iterator != ')')
        {
            fprintf(stderr, "No closing bracket : %s\n", iterator);
            node_delete(new_node);
            return nullptr;            
        }
        iterator++;
    }
    else if(check_uno_oper(&oper))
    {
        iterator++;
        new_node = new_oper_node((OperType)oper, nullptr, nullptr);
        new_node->Right = GetE();
        if(*iterator != ')')
        {
            fprintf(stderr, "No closing bracket : %s\n", iterator);
            node_delete(new_node);
            return nullptr;            
        }
        iterator++;
    }
    else
    {
        new_node = GetN();
    }
    return new_node;
}

static Node* GetN()
{
    Node* new_node = get_data();
    if(!new_node)
    {
        fprintf(stderr, "Bad data: %s\n", iterator);
        node_delete(new_node);
        return nullptr;
    }
    return new_node;
}


Tree* parse(Tree* p_tree, char* equation_file_path)
{
    assert(p_tree);
    assert(equation_file_path);

    char*  buf           = nullptr;
    size_t buf_size      =       0;
    FILE*  equation_file = nullptr;

    buf_size = get_file_size(&equation_file, equation_file_path, &buf);
    fread(buf, sizeof(char), buf_size, equation_file);
    skip_spaces(buf);

    iterator        = buf;
    Node* temp_node = GetG();
    free(buf);

    if(!temp_node)
    {
        fprintf(stderr, "There's an error in %s\n", equation_file_path);
        return nullptr;
    }

    p_tree->root = temp_node;
    p_tree->size = count_tree_size(temp_node);

    return p_tree;
}
