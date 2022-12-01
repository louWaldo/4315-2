#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <filesystem>
using namespace std;

#define NEW_LINE  "NEW_LINE"
#define INDENT  "INDENT"
#define EQUALS  "EQUAL"
#define COLON  "COLON"
#define SEMICOLON  "SEMICOLON"
#define COMMA  "COMMA"
#define SPACE  "SPACE"

#define PLUS  "PLUS"

#define OPERATOR  "OPERATOR"

#define OPEN_PARENTHESES  "OPEN_PARENTHESES"
#define CLOSE_PARENTHESES  "CLOSE_PARENTHESES"
#define OPEN_BRACKET  "OPEN_BRACKET"
#define CLOSE_BRACKET  "CLOSE_BRACKET"
#define OPEN_BRACE  "OPEN_BRACE"
#define CLOSE_BRACE  "CLOSE_BRACE"

#define NUMBER  "NUMBER"
#define TRUE  "TRUE"
#define FALSE  "FALSE"
#define NULL_IDENTIFIER  "NULL"
#define IDENTIFIER  "IDENTIFIER"
#define STRING  "STRING"

#define PRINT  "PRINT"
#define IF  "IF"
#define ELSE  "ELSE"
//#define LENGTH  "LENGTH"

#define FUNCTION_DEFINITION  "FUNCTION_DEFINITION"
#define RETURN  "RETURN"

#define BLOCK_START  "BLOCK_START"
#define BLOCK_END  "BLOCK_END"

#define COMMENT  "COMMENT"



#define PROGRAM "PROGRAM"
#define ASSIGMENT "ASSIGMENT"
#define OPERATION "OPERATION"
#define NUMBER "NUMBER"
#define STRING "STRING"
#define BOOLEAN "BOOLEAN"
#define NULL_ID "NULL"
#define LIST "LIST"
#define LIST_INDEX "LIST_INDEX"
#define IDENTIFIER "IDENTIFIER"
#define FUNCTION_CALL "FUNCTION_CALL"
#define EXPRESSION "EXPRESSION"
#define BODY "BODY"
#define FUNCTION "FUNCTION"
#define PARAMETER "PARAMETER"
#define RETURN "RETURN"
#define ERROR "ERROR"
//#define LENGTH "LENGTH"
using namespace std;

//Each Token represents a word, a symbol or a number
struct token 
{
    string type;
    string value;
};

vector<vector<token>> lexer(string input);
//LEXER (TOKENIZER)
vector<vector<token>> lexer(string input) {
    vector<vector<token>> tokens; //2D vector of tokens where each vector represents a line
    vector<token> lineTokens; //Vector of tokens for each line
    int currIndex = 0; //Stores current index of input string (Entire file)
    int indentationLevel = 0; //Stores indentation level of current line depending on if statements and function definitions
    int indentationLength = 0; //Stores length of indentation of current line (number of spaces used for indentation)
    bool isIndent = false; //Boolean to help check for indentation when switching to new lines
    int lastIndentationLength = 0; //Stores length of indentation of previous line (number of spaces used for indentation)

    while (currIndex < input.length()) {
        string subStr = ""; 
        subStr += input[currIndex]; //Adds current character to lookahead

        //NEW LINE
        if (input[currIndex] == '\n') {
            if(isIndent) {
                //Check for proper indentation on the next line
                    //Obtain length of indentation
                string indentation = "";

                //If indentation level is 1 and the indentation length is 0 (no spaces). Obtain indentation length since this is the first indented line
                if (indentationLevel == 1 && indentationLength == 0) {
                    int index = 1;
                    while (input[currIndex+index] == ' ') {
                        index ++;
                        indentationLength ++;
                    }
                }

                //Get indentation length of next line
                for (int i = 1; i <= indentationLength * indentationLevel; i++) {
                    if (input[currIndex+i] == ' ') {
                        indentation += input[currIndex+i];
                    } else {
                        break;
                    }
                }
                    //Check indentation length corresponds to the current indentation level
                if (indentation.length() != 0 && indentation.length() % (indentationLength*indentationLevel) == 0) {
                    currIndex += 1 + indentation.length();
                    if (lineTokens.size() > 0) tokens.push_back(lineTokens);
                    lineTokens.clear();
                    continue;
                }
                //If there is directly a next line ignore but increase line counter
                if (input[currIndex+1] == '\n') {
                    currIndex ++;
                    continue;
                }
                //if indentation is 0 and the indentation level is greater than one. This means that several levels of indentation ended so add corresponding breaking points.
                if (indentation.length() == 0 && indentationLevel > 1) {
                    for (int i = 1; i < indentationLevel; i++) {
                        vector<token> breakLine;
                        token t;
                        t.type = BLOCK_END;
                        t.value = BLOCK_END;
                        breakLine.push_back(t);
                        tokens.push_back(breakLine);
                        indentationLevel --;
                    }
                }
                
                indentationLevel --; //Decrease indentation level
                if (indentationLevel == 0) isIndent = false; //If indentation level is 0 then there is no indentation
                token t;
                t.type = BLOCK_END;
                t.value = "BLOCK_END";
                lineTokens.push_back(t);
                indentationLength = indentationLength * indentationLevel; //Update indentation length
                currIndex += 1 + indentation.length(); //Increase current index
                tokens.push_back(lineTokens); //Add line tokens to tokens vector
                lineTokens.clear(); //Clear line tokens vector
                continue;
            }

            //if there is no indentation proceed as normal
            currIndex ++; 
            if (lineTokens.size()>0) {
                tokens.push_back(lineTokens);
                lineTokens.clear();
            }
            continue;
        }

        //COMMENT
        if(input[currIndex] == '#') {
            currIndex ++;
            while (input[currIndex] != '\n' && currIndex <= input.length()) {
                currIndex ++;
            }
            continue;
        }

        //SPACE
        if (input[currIndex] == ' ') {
            currIndex ++;
            continue;
            
        }

        //SEMICOLON
        if (input[currIndex] == ';') {
            //ignore space
            currIndex ++;
            continue;
        }

        //COMMA
        if(input[currIndex] == ',') {
            token t;
            t.type = COMMA;
            t.value = ",";
            lineTokens.push_back(t);
            currIndex ++;
            continue;
        }

        //COLON 
        if(input[currIndex] == ':') {
            token t;
            t.type = COLON;
            t.value = ":";
            lineTokens.push_back(t);
            currIndex ++;
            continue;
        }

        //EQUALS, and IS EQUAL operators
        if (input[currIndex] == '=') {
            currIndex ++;
            if (input[currIndex] == '=') {
                token t;
                t.type = OPERATOR;
                t.value = "==";
                lineTokens.push_back(t);
                currIndex ++;
                continue;
            } else {
                token t;
                t.type = EQUALS;
                t.value = "=";
                lineTokens.push_back(t);
                continue;
            }
        }

        //GREATER and GREATER THAN operators
        if (input[currIndex] == '>') {
            currIndex ++;
            if (input[currIndex] == '=') {
                token t;
                t.type = OPERATOR;
                t.value = ">=";
                lineTokens.push_back(t);
                currIndex ++;
                continue;
            } else {
                token t;
                t.type = OPERATOR;
                t.value = ">";
                continue;
            }
        }

        //LESS and LESS THAN operators
        if (input[currIndex] == '<') {
            currIndex ++;
            if (input[currIndex] == '=') {
                token t;
                t.type = OPERATOR;
                t.value = "<=";
                lineTokens.push_back(t);
                currIndex ++;
                continue;
            } else {
                token t;
                t.type = OPERATOR;
                t.value = "<";
                lineTokens.push_back(t);
                continue;
            }
        }

        //NOT_EQUAL operator
        if (input[currIndex] == '!') {
            currIndex ++;
            if (input[currIndex] == '=') {
                token t;
                t.type = OPERATOR;
                t.value = "!=";
                lineTokens.push_back(t);
                currIndex ++;
                continue;
            } 
        }

        //PLUS
        if (input[currIndex] == '+') {
            token t;
            t.type = PLUS;
            t.value = "+";
            lineTokens.push_back(t);
            currIndex ++;
            continue;
        }

        //PARENTHESIS
        if (input[currIndex] == '(') {
            token t;
            t.type = OPEN_PARENTHESES;
            t.value = "(";
            lineTokens.push_back(t);
            currIndex ++;
            continue;
        }

        if (input[currIndex] == ')') {
            token t;
            t.type = CLOSE_PARENTHESES;
            t.value = ")";
            lineTokens.push_back(t);
            currIndex ++;
            continue;
        }

        //BRACKETS
        if (input[currIndex] == '[') {
            token t;
            t.type = OPEN_BRACKET;
            t.value = "[";
            lineTokens.push_back(t);
            currIndex ++;
            continue;
        }

        if (input[currIndex] == ']') {
            token t;
            t.type = CLOSE_BRACKET;
            t.value = "]";
            lineTokens.push_back(t);
            currIndex ++;
            continue;
        }

        //STRING
        if (input[currIndex] == '\"') {
            string str = "";
            currIndex ++;
            while (input[currIndex] != '\"') {
                str += input[currIndex];
                currIndex ++;
            }
            token t;
            t.type = STRING;
            t.value = str;
            lineTokens.push_back(t);
            currIndex ++;
            continue;
        }

        //INTEGER
        if (isdigit(subStr[0])) {
            //obtain integer
            string number = "";
            while (isdigit(input[currIndex]) && currIndex < input.length()) {
                number += input[currIndex];
                currIndex ++;
            }

            //create integer token
            token t;
            t.type = NUMBER;
            t.value = number;
            lineTokens.push_back(t);
            continue;
        }

        //IDENTIFIER
        if (isalpha(subStr[0])) {
            //obtain identifier
            string identifier = "";
            while ((isalpha(input[currIndex]) || isdigit(input[currIndex]) || input[currIndex] == '_') && currIndex < input.length()) {
                identifier += input[currIndex];
                currIndex ++;
            }

            //check if identifier is a keyword
            if (identifier == "print") {
                token t;
                t.type = PRINT;
                t.value = "print";
                lineTokens.push_back(t);
            
            } else if (identifier == "if") {
                isIndent = true;
                indentationLevel ++;
                token t;
                t.type = IF;
                t.value = "if";
                lineTokens.push_back(t);
            } else if (identifier == "else") {
                isIndent = true;
                indentationLevel ++;
                token t;
                t.type = ELSE;
                t.value = "else";
                lineTokens.push_back(t);
            } else if (identifier == "def") {
                indentationLevel ++;
                isIndent = true;
                token t;
                t.type = FUNCTION_DEFINITION;
                t.value = "def";
                lineTokens.push_back(t);
            } else if (identifier == "return") {
                token t;
                t.type = RETURN;
                t.value = "return";
                lineTokens.push_back(t);
            } else if (identifier == "True") {
                token t;
                t.type = TRUE;
                t.value = "true";
                lineTokens.push_back(t);
            } else if (identifier == "False") {
                token t;
                t.type = FALSE;
                t.value = "false";
                lineTokens.push_back(t);
            } else if (identifier == "None") {
                token t;
                t.type = NULL_IDENTIFIER;
                t.value = "none";
                lineTokens.push_back(t);
            } else {
                token t;
                t.type = IDENTIFIER;
                t.value = identifier;
                lineTokens.push_back(t);
            }
            continue;
        }

        //ERROR
        std::cout << "error";
        
    }

    //add line tokens to tokens
    if (lineTokens.size() > 0) {
        tokens.push_back(lineTokens);
    }
    return tokens;
}
