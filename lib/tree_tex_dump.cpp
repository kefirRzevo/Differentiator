#include "../include/differentiator.h"
#include "../include/tree.h"
#include "../include/logs.h"
#include "../include/stack.h"

FILE *LOGFILE          = nullptr;
static size_t N_DUMPS  = 0;
stack CHANGE_STK       = {};


static void compile_tex();
static void close_tex();

static void tex_eq_start(bool phraze);
static void tex_eq_end(   );
static void left_bracket( );
static void right_bracket();

static void tex_mul( Node* left_node, Node* right_node);
static void tex_div( Node* left_node, Node* right_node);
static void tex_pow( Node* left_node, Node* right_node);
static void tex_uno( Node* n);
static void tex_dif( Node* n);
static void tex_data(Node* n);
static void tex_node(Node* n);

static void  fill_substitutions(Node* n, Tree* p_tree);
static Node* find_node(Node *n);


static const char* CRINGE[] = 
{
    "По теореме о причёсывании ежа: ",
    "Согласно теореме о бутерброде с ветчиной: ",
    "Теорема волшебной палочки гласит, что: "
    "По теореме о двух миллиционерах: ",
    "Как известно, по теореме Пифагора: ",
    "Очевидно, что по критерию Сильвестра: ",
    "Слава Украине, героям слава: ",
    "По теореме об изменении энтропии: ",
    "Гаусс еще в \\RomanNumeralCaps{14} веке посчитал, что: ",
    "Из неравенства Клаузиуса вытекает, что: ",
    "По теореме Лиувилля о сохранении фазового объёма: ",
    "А по теореме Лиувилля об интеграле уравнения Гамильтона — Якоби: ",
    "А Флуктуационно-диссипационная теорема гласит, что: ",
    "А вот если вспомнить теорему Пенроуза — Хокинга о сингулярности, то: ",
};


#define L       n->Left
#define R       n->Right
#define T       n->Type
#define LT      left_node->Type
#define RT      right_node->Type
#define N       n->Value.number  
#define V       n->Value.variable
#define O       n->Value.oper_type
#define LO      left_node->Value.oper_type
#define RO      right_node->Value.oper_type


FILE* tex_open()
{
    if(!LOGFILE)
    {
        LOGFILE = fopen(LOGFILE_PATH, "w");
        FILE_CHECK(LOGFILE);

        fprintf(LOGFILE,    "\\documentclass[12pt]{article}                             \n"
                            "\\usepackage[utf8]{inputenc}                               \n"
                            "\\usepackage[russian]{babel}                               \n"
                            "\\usepackage{geometry}                                     \n"
                            "\\usepackage{amsmath}                                      \n"
                            "\\geometry{a4paper}                                        \n"
                            "\\usepackage{graphicx}                                     \n"
                            "\\begin{document}                                          \n"
                            "\\begin{titlepage}                                         \n"
                            "\t\\begin{center}                                          \n"
                            "\t\t\\Huge                                                 \n"
                            "\t\tСан-Юрьевич                                            \n"
                            "\t\t\\vspace*{1cm}                                         \n"
                            "                                                           \n"              
                            "\t\t\\textbf{Дифференциальный зачёт}                       \n"
                            "                                                           \n"
                            "\t\t\\vspace{0.5cm}                                        \n"
                            "\t\t\\vspace{1.5cm}                                        \n"
                            "\t\t\\includegraphics[width = 10 cm]{imgs/petrovich.png}   \n"
                            "\t\t\\begin{minipage}{10cm}                                \n"
                            "\t\t\\vspace*{2cm}                                         \n"
                            "\t\t\t\\begin{center}                                      \n"
                            "\t\t\t\tИз МФТИ, с любовью                                 \n"
                            "\t\t\t\\end{center}                                        \n"
                            "\t\t\\includegraphics[width = 3 cm]{imgs/with_love.png}    \n"
                            "\t\t\\end{minipage}                                        \n"
                            "                                                           \n"
                            "\t\\end{center}                                            \n"                    
                            "\\end{titlepage}                                           \n"
                            "\\newpage                                                  \n"
                            "\\Huge                                                     \n"
                            "\tПосчитаем производную\\\\                                \n"                    
		                    "\\newline                                                  \n"
                            "\\normalfont                                               \n"
		                    "\\normalsize                                               \n");
    }
    atexit(close_tex);
    //atexit(compile_tex);
    return LOGFILE;
}


void tex_tree(Tree* p_tree, bool phrase)
{
    assert(p_tree);
    StackCtor(&CHANGE_STK);
    
    fill_substitutions(p_tree->root, p_tree);

    tex_eq_start(phrase);
    tex_node(p_tree->root);
    tex_eq_end();

    if(CHANGE_STK.size)
        tex_msg("Где, \n");

    while (CHANGE_STK.size) 
    {
        Node* temp = nullptr;
        StackPop(&CHANGE_STK, &temp);

        tex_eq_start(0);
        tex_node(temp);
        tex_msg(" = ");
        tex_node(temp->Right);
        tex_eq_end();

        free(temp);
    }
    StackDtor(&CHANGE_STK);
}


void tex_msg(char message[])
{
    assert(message);

    fprintf(LOGFILE, "%s\n", message);
}


static void compile_tex()
{
    char cmd[64] = {0}; 
    sprintf(cmd, "pdflatex -output-directory tex %s ", LOGFILE_PATH);
    system(cmd);
}


static void close_tex()
{
    fprintf(LOGFILE, "\\end{document}\n");
    if(!LOGFILE)
    {
        fclose(LOGFILE);
        LOGFILE = nullptr;
    }
}



static void tex_eq_start(bool phraze)
{
    if(phraze)
        fprintf(LOGFILE, "%s ", CRINGE[rand() % (sizeof(CRINGE) / sizeof(*CRINGE))]);

    fprintf(LOGFILE, "\\begin{equation}\n\t");
}


static void tex_eq_end()
{
    fprintf(LOGFILE, "\n\\end{equation}\n");
}


static void left_bracket()
{
    fprintf(LOGFILE, "\\left( ");
}


static void right_bracket()
{
    fprintf(LOGFILE, "\\right) ");
}


//#----------NOW FUNCTIONS-----------#//


static void tex_data(Node* n)
{
    assert(n);

    switch(T) 
    {
        case VAR:   fprintf(LOGFILE, "%" VAR_SPEC "",  V);
                    break;
        case NUM:   fprintf(LOGFILE, "%" NUM_SPEC "",  N);
                    break;
        default:    break;
    }
}


static void tex_mul(Node* left_node, Node* right_node)
{
    assert(left_node);
    assert(right_node);

    if(LT == OPER && LO == OPER_ADD || LO == OPER_SUB)
    {
        left_bracket();
        tex_node(left_node);
        right_bracket();
    }
    else
    {
        tex_node(left_node);
    }

    fprintf(LOGFILE, "\\cdot ");

    if(RT == OPER && RO == OPER_ADD || RO == OPER_SUB)
    {
        left_bracket();
        tex_node(right_node);
        right_bracket();
    }
    else
    {
        tex_node(right_node);
    }
}


static void tex_div(Node* left_node, Node* right_node)
{
    assert(left_node);
    assert(right_node);

    fprintf(LOGFILE, "\\frac{");
    tex_node(left_node);
    fprintf(LOGFILE, "}{");
    tex_node(right_node);
    fprintf(LOGFILE, "}");
}


static void tex_pow(Node* left_node, Node* right_node)
{
    assert(left_node);
    assert(right_node);

    if (LT == OPER) 
    {
        left_bracket();
        tex_node(left_node);
        right_bracket();
    }
    else 
    {
        tex_node(left_node);
    }

    fprintf(LOGFILE, "^{");
    tex_node(right_node);
    fprintf(LOGFILE, "}");
}


static void tex_uno(Node* n)
{
    assert(n);

    switch(O)
    {
        case OPER_LN:   fprintf(LOGFILE, "\\ln {");   
                        tex_node(R);
                        break;
        case OPER_SIN:  fprintf(LOGFILE, "\\sin {");   
                        tex_node(R);
                        break;
        case OPER_COS:  fprintf(LOGFILE, "\\cos {");   
                        tex_node(R);
                        break;
        case OPER_SH:   fprintf(LOGFILE, "\\sinh {");   
                        tex_node(R);
                        break;
        case OPER_CH:   fprintf(LOGFILE, "\\cosh {");   
                        tex_node(R);
                        break;
        default:        return;
    }
    fprintf(LOGFILE, "}");
}


static void tex_dif(Node* n)
{
    assert(n);

    if(R)
        fprintf(LOGFILE, "\\frac{\\partial}{\\partial %c}", R->Value.variable);

    left_bracket();
    tex_node(L);
    right_bracket();
}


static void fill_substitutions(Node* n, Tree* p_tree)
{
    assert(n);

    size_t size = count_tree_size(n);

    if (size > 10 && size < 20 && p_tree->size > 30) 
    {
        Node* new_node  = (Node* )calloc(1, sizeof(Node));
        new_node->Type  = VAR;
        new_node->Right = n;
        new_node->Value.variable = 'A' + CHANGE_STK.size;

        StackPush(&CHANGE_STK, new_node);
        return;
    }

    if(L)
        fill_substitutions(L, p_tree);
    
    if(R)
        fill_substitutions(R, p_tree);
}


static Node* find_node(Node *n)
{
    assert(n);

    if (!CHANGE_STK.size)
        return nullptr;

    Node* found = nullptr;
    for (size_t i = CHANGE_STK.size; i > 0; i--) 
    {
        if((CHANGE_STK.data[i - 1])->Right == n) 
        {
            found = CHANGE_STK.data[i - 1];
            break;
        }
    }

    return found;
}


static void tex_node(Node *n)
{
    assert(n);

    if (CHANGE_STK.size) 
    {
        Node* changed_node = find_node(n);
        if (changed_node) 
        {
            tex_data(changed_node);
            return;
        }
    }

    if (T != OPER) 
    {
        tex_data(n);
        return;
    }

    switch (O) 
    {
        case OPER_ADD:  tex_node(L);
                        fprintf(LOGFILE, " + ");
                        tex_node(R);
                        break;
        case OPER_SUB:  tex_node(L);
                        fprintf(LOGFILE, " - ");
                        tex_node(R);
                        break;
        case OPER_MUL:  tex_mul(L, R);
                        break;
        case OPER_DIV:  tex_div(L, R);
                        break;
        case OPER_POW:  tex_pow(L, R);
                        break;
        case OPER_DDX:  tex_dif(n);
                        break;
        default:        tex_uno(n);
                        break;
    }

}


#undef L
#undef R
#undef T
#undef LT
#undef RT
#undef N
#undef V
#undef O
#undef LO
#undef RO
