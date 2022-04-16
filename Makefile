CXX=g++
#CXXFLAGS = -D NDEBUG -g -std=c++14 -fmax-errors=100 -Wall 				\
	   -Wextra -Weffc++  												\
	   -Waggressive-loop-optimizations -Wc++0x-compat -Wc++11-compat    \
	   -Wc++14-compat -Wcast-align -Wcast-qual -Wchar-subscripts        \
	   -Wconditionally-supported -Wconversion -Wctor-dtor-privacy       \
	   -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security \
	   -Wformat-signedness -Wformat=2 -Winline -Wlogical-op             \
	   -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd          \
	   -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls  \
	   -Wshadow -Wsign-conversion -Wsign-promo -Wstack-usage=8192       \
	   -Wstrict-null-sentinel -Wstrict-overflow=2                       \
	   -Wsuggest-attribute=noreturn -Wsuggest-final-methods             \
	   -Wsuggest-final-types -Wsuggest-override -Wswitch-default        \
	   -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code             \
	   -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix    \
	   -Wno-missing-field-initializers -Wno-narrowing                   \
	   -Wno-old-style-cast -Wno-varargs  -fcheck-new                    \
	   -fsized-deallocation -fstack-check -fstack-protector             \
	   -fstrict-overflow                          						\
	   -fno-omit-frame-pointer -fPIE  -fsanitize=address                \
	   -fsanitize=bool -fsanitize=bounds  -fsanitize=enum               \
	   -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero   \
	   -fsanitize=integer-divide-by-zero               					\
	   -fsanitize=nonnull-attribute  -fsanitize=null  -fsanitize=return \
	   -fsanitize=returns-nonnull-attribute                             \
	   -fsanitize=signed-integer-overflow -fsanitize=unreachable        \
	   -fsanitize=vla-bound -fsanitize=vptr -lm -pie 	            	\
	   -Wno-format-nonliteral 

# $(CXX) -g $(CXXFLAGS) -c  main.cpp src/akinator.cpp src/dump.cpp src/parser.cpp src/tree.cpp
#"${workspaceFolder}\\/main.cpp", "${workspaceFolder}\\/lib/hash.cpp", "${workspaceFolder}\\/lib/operators.cpp", "${workspaceFolder}/\\src/lib/tree_viz_dump.cpp", "${workspaceFolder}/\\lib/tree.cpp", "${workspaceFolder}/\\src/differentiator.cpp", "${workspaceFolder}/\\src/optimization.cpp", "${workspaceFolder}/\\src/parser.cpp"
	
all: 
	$(CXX) -g -O3 -pipe $(CXXFLAGS) -c lib/hash.cpp lib/operators.cpp lib/stack.cpp lib/tree_tex_dump.cpp lib/tree_viz_dump.cpp lib/tree.cpp src/differentiator.cpp src/optimization.cpp src/parser.cpp main.cpp
	$(CXX) -g -O3 -pipe $(CXXFLAGS) -o app hash.o operators.o stack.o tree_tex_dump.o tree_viz_dump.o tree.o differentiator.o optimization.o parser.o main.o

current:
	$(CXX) -g -O3 -pipe $(CXXFLAGS) -c lib/tree_tex_dump.cpp src/differentiator.cpp src/optimization.cpp

start:
	$(CXX) -g -O3 -pipe $(CXXFLAGS) -o app hash.o operators.o stack.o tree_tex_dump.o tree_viz_dump.o tree.o differentiator.o optimization.o parser.o main.o
	./app

clean:
	rm *.o

kill:
	rm logfiles/dump.html logfiles/dump.tex logfiles/res/*
	