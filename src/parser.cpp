#include "../include/parser.h"
#include "../include/operators.h"
#include "../include/logs.h"


char* iterator = nullptr;


static size_t get_file_size(FILE** const pp_file, char* textfile_path, char** pp_buf);
static Node*  get_nodes(Tree* p_tree);
static bool   get_data( Node* p_node);
static void   skip_spaces( char* p_buf);
static char*  find_bracket(char* cur_position);


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

    char* temp_buf_ptr      = (char *)  calloc(file_size, sizeof(char));
    CALLOC_CHECK(temp_buf_ptr);
    *pp_buf = temp_buf_ptr;

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


static char* find_bracket(char* cur_position)
{
    assert(cur_position);

    while(*cur_position != '(' && *cur_position != ')' && *cur_position != '\0')
        cur_position++;
    
    return cur_position;
}


static bool get_data(Node* p_node)
{
    assert(p_node);

    char*  end      = find_bracket(iterator);
    char*  endstr   = nullptr;
    double temp_num = strtod(iterator, &endstr);

    if(endstr != iterator)
    {
        p_node->Type         = NUM;
        p_node->Value.number = temp_num;
        iterator             = endstr;
        return 1;
    }

    if(get_oper_type(qhash(iterator, end - iterator), p_node))
    {
        p_node->Type = OPER;
        iterator = end;
        return 1;
    }

    if(isalpha(*iterator) && *iterator != 'd')
    {
        p_node->Type           = VAR;
        p_node->Value.variable = *iterator;
        iterator++;
        return 1;   
    }

    return 0;
}


//#define $$ printf("%d: %s\n", __LINE__, iterator);

static Node* get_nodes(Tree* p_tree)
{
    if(*iterator != '(')
    {
        return nullptr;
    }

    iterator++;

    Node* new_node = (Node *)calloc(1, sizeof(Node));
    CALLOC_CHECK(new_node);
    p_tree->size++;
   
    if(*iterator == '(')
    {
        new_node->Left = get_nodes(p_tree);

        if(!new_node->Left)
        {
            node_delete(new_node);
            return nullptr;
        }
    }

    if(!get_data(new_node))
    {
        node_delete(new_node);
        return nullptr;
    }

    if(*iterator == '(')
    {
        if(new_node->Type != OPER)
        {
            node_delete(new_node);
            return nullptr;
        }

        new_node->Right = get_nodes(p_tree);

        if(!new_node->Right)
        {
            node_delete(new_node);
            return nullptr;
        }
    }

    if(*iterator != ')')
    {
        node_delete(new_node);
        return nullptr;
    }

    iterator++;
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

    iterator = buf;
    Node* temp_node = get_nodes(p_tree);
    free(buf);

    if(!temp_node)
    {
        fprintf(stderr, "There's an error in %s\n", equation_file_path);
        return nullptr;
    }
    p_tree->root = temp_node;

    return p_tree;
}
