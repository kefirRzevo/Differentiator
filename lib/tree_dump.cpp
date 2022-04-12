#include "../include/tree.h"
#include "../include/operators.h"
#include "../include/logs.h"


static const char* LOGFILE_PATH = "logfiles/dump";
static const char*     DOT_PATH = "logfiles/res/tree";
static size_t N_DUMPS           = 0;
static FILE*  logfile           = nullptr;


static FILE* dump_init(const char* const dump_path);

static void  dump_close();

static void  make_one_dump(Tree* p_tree, char* img_path);

static void  print_nodes(Node* tree_node, FILE* const dotfile);

static FILE* open_dot_txt(const char* const dot_path);

static void  close_dot_txt(FILE* const dotfile);


static void dump_close()
{
    fprintf(logfile, "\t</body>\n"
                     "</html>\n");
}


static FILE* dump_init(const char* const dump_path)
{
    assert(dump_path);

    if(logfile == nullptr)
    {
        logfile = fopen(dump_path, "w");
        FILE_CHECK(logfile);

        fprintf(logfile,    "<!doctype html>                                                             \n"
                            "<html>                                                                      \n"
                            "\t<head lang='en'>                                                          \n"
                            "\t\t<meta charset='utf-8'>                                                  \n"
                            "\t\t<meta name='viewport' content='width=device-width'>                     \n"
                            "\t\t<title>List dump</title>                                                \n"
                            "\t\t<style>                                                                 \n"
                            "\t\tbody {                                                                  \n"
                            "\t\t}                                                                       \n"
                            "\t\t.list {                                                                 \n"
                            "\t\t\twidth: max-content;                                                   \n"
                            "\t\t\tfont-size: 20px;                                                      \n"
                            "\t\t\tpadding: 10px 20px;                                                   \n"
                            "\t\t}                                                                       \n"
                            "\t\t.img {                                                                  \n"
                            "\t\t\toverflow-x: auto;                                                     \n"
                            "\t\t}                                                                       \n"
                            "\t\t</style>                                                                \n"
                            "\t</head>                                                                   \n"
                            "\t<body>                                                                    \n");
    }
    return logfile;
}


static void make_one_dump(Tree* p_tree, char* img_path)
{
    fprintf(logfile,    "\t\t<hr width = '100%%'>\n"
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
    sprintf(dump_path,      "%s.html",              LOGFILE_PATH);


    FILE* dotfile = open_dot_txt(dotfile_string);
    FILE_CHECK(dotfile);
    print_nodes(p_tree->root, dotfile);
    close_dot_txt(dotfile);

    system(cmd_string);

    logfile = dump_init(dump_path);
    make_one_dump(p_tree, img_path);
    atexit(dump_close);
    N_DUMPS++;
    return p_tree;
}


static FILE* open_dot_txt(const char* const dot_path)
{
    assert(dot_path);

    FILE* dotfile   = nullptr;
    dotfile         = fopen(dot_path, "w");
    FILE_CHECK(dotfile);
    
    fprintf(dotfile,  "digraph G                                                                                          \n"
                      "{                                                                                                  \n"
                      "\t\tbgcolor  = \"#FFFFFF\"                                                                         \n"
                      "\t\tcompound = true                                                                                \n"
                      "\t\tnewrank  = true                                                                                \n"
                      "\t\tgraph[splines = polyline, nodesep = 0.3, ranksep = 1, rankdir = TB]                            \n"
                      "\t\tnode[shape = record, style = \"filled\", penwidth = 2, color = \"#000000\", margin = 0.04,     \n"
                      "\t\tfontcolor = \"#000000\", fontsize = 12, fillcolor = \"#FFFFFF\", fontname = \"Arial\"]         \n"
                      "\t\tedge[arrowsize = 0.7, style = solid, penwidth = 1.1, color = \"#CCCCCC\"]                    \n\n");
    return dotfile;
}


static void close_dot_txt(FILE* const dotfile)
{
    assert(dotfile);

    fprintf(dotfile, "}");
    fclose(dotfile);
}


static void print_nodes(Node* tree_node, FILE* const dotfile)
{
    assert(dotfile);

    if (tree_node == nullptr)
    {
	    return; 
    }

    char temp_str[5] = {}; 
    switch(tree_node->Type)
    {
        case NUM:   fprintf(dotfile, "\t\tNode%p[label = \"%.2" NUM_SPEC " \"]\n", tree_node, tree_node->Value.number);
                    break;
        case OPER:  oper_name(temp_str, tree_node);
                    fprintf(dotfile, "\t\tNode%p[label = \"%" OPER_SPEC "\"]\n", tree_node, temp_str);
                    break;
        case VAR:   fprintf(dotfile, "\t\tNode%p[label = \"%" VAR_SPEC " \"]\n", tree_node, tree_node->Value.variable);
    }

    if(tree_node->Left != nullptr)
    {
        fprintf(dotfile,    "\t\tNode%p -> Node%p[color = \"black\", fontname = \"Arial\"]\n\n", tree_node, tree_node->Left);
        print_nodes(tree_node->Left,  dotfile);
    }
    if(tree_node->Right != nullptr)
    {
        fprintf(dotfile,    "\t\tNode%p -> Node%p[color = \"black\", fontname = \"Arial\"]\n\n", tree_node, tree_node->Right);
        print_nodes(tree_node->Right, dotfile);
    }

    fprintf(dotfile, "\n");
}
