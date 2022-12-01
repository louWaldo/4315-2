As of now, our parser works by making a single pass over the python osurce code. The two main classes in the program are Lexer and Parser. For every line of python code, the Lexer will identify every token in the line and store them in a vector of tokens. Every line of source code will have its own dedicated vector of tokens. Each of these respective vectors will be brought together into a two-dimensional vector of tokens. 
The Parser will then read every resulting vector of tokens, keeping track of variables and their values in an unordered_map, mapping variable names to an associated type and value. We then build an abstract syntax tree that will be evaluated recursively, outputting terminal symbols present in the symbol table upon every "print" enocuntered.
We are only able to pass 5 out of the 10 test cases for phase two. Our phase one submission was not very good, having only passed 5 of those cases as well. Before even addressing phase two, we dedicated our time in ensuring we could pass all 10 phase one test cases since those are much easier to implement a solution for.
