As of now, our parser works by making a single pass over the python osurce code.
The two main classes in the program are Lexer and Parser.
For every line of python code, the Lexer will identify every token in the line and store them in a vector of tokens (i.e. tokenVector).
The Parser will then read every resulting vector of tokens, keeping track of variables and their values in an unordered_map, mapping variable names to an associated type and value.
Our main issue of concern with this program is that it treats the addition of an integer and a list element as list concatenation rather than simple addition.
when reading a + l[0], it treats the integers a, and l[0] as if they were both lists.
if a equaled 5 and l[0] equaled 1, the result would be [5, 0] instead of simply 6.

For some possible extra points, we implemented a way of supporting lists of strings and arithmetic operations other than addition.

example of string support:

a = ["abc", "def", "hij"]
b = a[1]
print(b)

would give a result of "def"

