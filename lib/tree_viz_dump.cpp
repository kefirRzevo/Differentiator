#include "../include/tree.h"
#include "../include/operators.h"
#include "../include/logs.h"


static const char* LOG_PATH = "logfiles/dump";
static const char* DOT_PATH = "logfiles/res/tree";
static size_t N_DUMPS       = 0;
static FILE*  LOG           = nullptr;


static FILE* dump_init(const char* const dump_path);

static void  dump_close();

static FILE* open_dot_file(const char* const dot_path);

static void  close_dot_file(FILE* const dotfile);

static void  make_one_dump(Tree* p_tree, char* img_path);

static void  print_nodes(Node* node, FILE* const dotfile);


static void dump_close()
{
    fprintf(LOG, "\n\t</body>\n"
                     "</html>\n");
}


static FILE* dump_init(const char* const dump_path)
{
    assert(dump_path);

    if(!LOG)
    {
        LOG = fopen(dump_path, "w");
        FILE_CHECK(LOG);

        fprintf(LOG,    "<!doctype html>                                                             \n"
                        "<html>                                                                      \n"
                        "\t<head lang='en'>                                                          \n"
                        "\t\t<meta charset='utf-8'>                                                  \n"
                        "\t\t<meta name='viewport' content='width=device-width'>                     \n"
                        "\t\t<title>List dump</title>                                                \n"
                        "\t\t<style>                                                                 \n"
                        "\t\t.list                                                                   \n"
                        "\t\t{                                                                       \n"
                        "\t\t\twidth: max-content;                                                   \n"
                        "\t\t\tfont-size: 20px;                                                      \n"
                        "\t\t\tpadding: 10px 20px;                                                   \n"
                        "\t\t}                                                                       \n"
                        "\t\t.img                                                                    \n"
                        "\t\t{                                                                       \n"
                        "\t\t\toverflow-x: auto;                                                     \n"
                        "\t\t}                                                                       \n"
                        "\t\t</style>                                                                \n"
                        "\t</head>                                                                   \n"
                        "\t<body>");
    }
    return LOG;
}


static void make_one_dump(Tree* p_tree, char* img_path)
{
    fprintf(LOG,    "\n\t\t<hr width = '100%%'>\n"
                        "\t\t<div class = 'list'>\n"
                        "\t\t\t<pre>DUMP %zu</pre>\n"
                        "\t\t\t<pre>List address: %p</pre>\n"
                        "\t\t\t<pre>Size:         %zu</pre>\n"
                        "\t\t</div>\n"
                        "\t\t<div class = 'img'>\n"
                        "\t\t\t<img src = \"../%s\" height = 500px>\n"
                        "\t\t</div>", N_DUMPS + 1, p_tree, p_tree->size, img_path);
}


Tree* tree_viz_dump(Tree* p_tree)
{
    assert(p_tree);

    char cmd_string[64]     = "";
    char dotfile_string[32] = "";
    char dump_path[32]      = "";
    char img_path[32]       = "";


    sprintf(dotfile_string, "%s%zu.dot",            DOT_PATH, N_DUMPS);
    sprintf(img_path,       "%s_img%zu.svg",        DOT_PATH, N_DUMPS);
    sprintf(cmd_string,     "dot %s -Tsvg -o %s",   dotfile_string, img_path);
    sprintf(dump_path,      "%s.html",              LOG_PATH);


    FILE* dotfile = open_dot_file(dotfile_string);
    FILE_CHECK(dotfile);
    print_nodes(p_tree->root, dotfile);
    close_dot_file(dotfile);
    system(cmd_string);

    LOG = dump_init(dump_path);
    make_one_dump(p_tree, img_path);
    if(!N_DUMPS)
        atexit(dump_close);
    N_DUMPS++;
    return p_tree;
}


static FILE* open_dot_file(const char* const dot_path)
{
    assert(dot_path);

    FILE* dotfile   = nullptr;
    dotfile         = fopen(dot_path, "w");
    FILE_CHECK(dotfile);
    
    fprintf(dotfile,  "\tdigraph\n"
                      "\t{\n"
                      "\t\tedge [color     = darkgrey]\n"
                      "\t\tedge [arrowsize = 1.6]\n"
                      "\t\tedge [penwidth  = 1.2]\n"
                      "\n"
                      "\t\tgraph [penwidth = 2]\n" 
                      "\t\tgraph [ranksep  = 1.3]\n" 
                      "\t\tgraph [nodesep  = 0.5]\n"
                      "\t\tgraph [style    = \"filled\"]\n"
                      "\t\tgraph [color    = green, fillcolor = lightgreen]\n"
                      "\n"
                      "\t\tnode [penwidth = 2]\n" 
                      "\t\tnode [shape    = box]\n" 
                      "\t\tnode [color    = black, fillcolor = white]\n"
                      "\n"
                      "\t\tcompound  = true;\n"
                      "\t\tnewrank   = true;\n\n");
    return dotfile;
}


static void close_dot_file(FILE* const dotfile)
{
    assert(dotfile);

    fprintf(dotfile, "\t}");
    fclose(dotfile);
}



static void print_nodes(Node* node, FILE* const dotfile)
{
    assert(dotfile);
    assert(node);

    char oper[5] = {}; 

    switch(node->Type)
    {
        case NUM:   fprintf(dotfile, "\t\tNode%p[label = \"%" NUM_SPEC "\"]\n",  node, node->Value.number);
                    break;
        case OPER:  oper_name(oper, node);
                    fprintf(dotfile, "\t\tNode%p[label = \"%" OPER_SPEC "\"]\n", node, oper);
                    break;
        case VAR:   fprintf(dotfile, "\t\tNode%p[label = \"%" VAR_SPEC "\"]\n",  node, node->Value.variable);
                    break;
    }

    if(node->Left)
    {
        fprintf(dotfile,    "\t\tNode%p -> Node%p\n", node, node->Left);
        fprintf(dotfile, "\n");
        print_nodes(node->Left,  dotfile);
    }
    if(node->Right)
    {
        fprintf(dotfile,    "\t\tNode%p -> Node%p\n", node, node->Right);
        fprintf(dotfile, "\n");
        print_nodes(node->Right, dotfile);
    }
}
