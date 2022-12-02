#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <filesystem>
#define OPERATOR "OPERATOR"
#define NEW_LINE "NEW_LINE"
#define INDENT "INDENT"
#define EQUALS "EQUAL"
#define EQUAL_EQUAL "EQUAL_EQUAL"
#define COLON "COLON"
#define COMMA "COMMA"
#define SPACE "SPACE"
#define ASSIGMENT "ASSIGMENT"
#define OPERATION "OPERATION"
#define INT "INT"
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
#define ADD "ADD"
#define LEFT_PAREN "LEFT_PAREN"
#define RIGHT_PAREN "RIGHT_PAREN"
#define LEFT_BRACK "LEFT_BRACK"
#define RIGHT_BRACK "RIGHT_BRACK"
#define TRUE "TRUE"
#define FALSE "FALSE"
#define IDENTIFIER "IDENTIFIER"
#define STRING "STRING"
#define PRINT "PRINT"
#define IF "IF"
#define ELSE "ELSE"
#define FUNCTION_DEFINITION "FUNCTION_DEFINITION"
#define RETURN "RETURN"
#define NEW_CHUNK "NEW_CHUNK"
#define COMMENT "COMMENT"

class token 
{
public:
    std::string type;
    std::string value;
};

int fun_if_else = 0; 

class Node
{
public:
    int line;
    std::string type;
    std::string value;
    std::string symbol;
    std::vector<Node> children;
    std::vector<Node> parameters;
    std::vector<Node> expression;
};

class Symbol
{
public:
    std::string type;
    int integer;
    std::string str;
    bool boolean;
    std::vector<int> list;
    Node function;
    int line;
};

std::vector<std::vector<token>> lexer(std::string);
std::unordered_map<std::string, Symbol> symTable;
Node runParser(std::vector<std::vector<token>> tokens);
Symbol evaulate(Node node);
Node evaluate(std::vector<std::vector<token>> tokens, int line, int tokenIndex, token currToken) ;




int main(int argc, char* argv[]) 
{
    //load contents of python file into std::string contents
    std::string input_file_name = argv[1];
    std::ifstream input(input_file_name);
    std::string contents;
    contents.assign( (std::istreambuf_iterator<char>(input) ),(std::istreambuf_iterator<char>()    ) );
    
    std::vector<std::vector<token>> tokens = lexer(contents);

    Node* root = new Node;
    Node* ast = new Node;
    Symbol* sym = new Symbol;

    *root = runParser(tokens);

    for (int i=0; i < root->children.size(); i++) 
    {
        evaulate(root->children[i]);
    }
    return 0;
}


Node runParser(std::vector<std::vector<token>> tokens) 
{
    Node* root = new Node;
    int line = 0; 
    int tokenIndex = 0; 
    token currToken; 

    while (line < tokens.size()) 
    {
        currToken = tokens[line][tokenIndex]; 
        Node* node = new Node;
        *node = evaluate(tokens, line, tokenIndex, currToken); 
        root->children.push_back(*node); 
        if(fun_if_else == 0)
        {
            line++;
        }
        else
        {
            line = fun_if_else + line;
        }

        fun_if_else = 0; 
    }

    return *root; 
}

Node evaluate(std::vector<std::vector<token>> tokens, int line, int tokenIndex, token currToken) 
{
    Node* node = new Node;
    if (currToken.type == IDENTIFIER) 
    {

        node->type = IDENTIFIER; 
        node->symbol = currToken.value;

        if (tokens[line].size() == 1 || tokenIndex == tokens[line].size() - 1 || tokens[line][tokenIndex + 1].type == NEW_CHUNK)
        {
            return *node;
        }
        
        currToken = tokens[line][tokenIndex+1];

        if (currToken.type == EQUALS) 
        {

            node->type = ASSIGMENT; 
            Node expression = evaluate(tokens, line, tokenIndex+2, tokens[line][tokenIndex+2]); //Parse expression after equal sign

            node->expression.push_back(expression); 
            return *node; 
        }


        if (currToken.type == ADD) 
        {

            Node operation = Node();
            operation.type = OPERATION; 
            operation.value = "+"; 

            operation.parameters.push_back(*node); 
            operation.parameters.push_back(evaluate(tokens, line, tokenIndex+2, tokens[line][tokenIndex+2])); //Add expression after ADD sign to parameters

            return operation;
        }

        if (currToken.type == LEFT_PAREN) 
        {

            Node functionCall = Node();
            functionCall.type = FUNCTION_CALL; 
            functionCall.symbol = node->symbol; 
            int tokenIncrement = 2;
            currToken = tokens[line][tokenIndex + 2]; 
            while (currToken.type != RIGHT_PAREN) 
            {
                //Ignore commas
                if (currToken.type == COMMA) 
                {
                    tokenIncrement++;
                    currToken = tokens[line][tokenIndex+tokenIncrement];
                    continue;
                }
                Node expression = evaluate(tokens, line, tokenIndex+tokenIncrement, currToken); //parse parameter
                functionCall.parameters.push_back(expression); 
                tokenIncrement ++; 
                currToken = tokens[line][tokenIndex+tokenIncrement]; 
            }
            return functionCall;
        }

        
        if (currToken.type == RIGHT_PAREN) 
        {
            return *node;
        }

        if (currToken.type == LEFT_BRACK) 
        {
            Node listIndex = Node();
            listIndex.type = LIST_INDEX; 
            listIndex.symbol = node->symbol; 
            listIndex.parameters.push_back(evaluate(tokens, line, tokenIndex+2, tokens[line][tokenIndex+2])); 
            int tokenIncrement = 2; 
            while (currToken.type != RIGHT_BRACK) 
            { 
                if (tokens[line].size() > tokenIndex + tokenIncrement) 
                {
                    currToken = tokens[line][tokenIndex+tokenIncrement];
                    tokenIncrement++;
                } 
                else 
                {
                    std::cout << "no closing bracket\n";
                }
            }

            //Check if there are more tokens after the close bracket
            if (tokens[line].size() > tokenIndex+tokenIncrement) 
            {
                currToken = tokens[line][tokenIndex+tokenIncrement];
            } else 
            {
                return listIndex;
            }
            
            if (currToken.type == EQUALS) 
            {
                node->type = ASSIGMENT;
                currToken = tokens[line][tokenIndex+tokenIncrement+1];
                listIndex.expression.push_back(evaluate(tokens, line, tokenIndex+tokenIncrement+1, currToken));
                node->expression.push_back(listIndex);
                return *node;
            }

            if (currToken.type == ADD) 
            { 
                Node operation = Node();
                operation.type = OPERATION;
                operation.value = currToken.value;

                operation.parameters.push_back(listIndex);
                operation.parameters.push_back(evaluate(tokens, line, tokenIndex+tokenIncrement+1, tokens[line][tokenIndex+tokenIncrement+1]));

                return operation;
            }

            return listIndex;
        }

        if (currToken.type == OPERATOR) 
        {

            Node operation = Node();
            operation.type = OPERATION;
            operation.value = currToken.value;

            operation.parameters.push_back(*node);
            operation.parameters.push_back(evaluate(tokens, line, tokenIndex+2, tokens[line][tokenIndex+2]));

            return operation;
        }

        if (currToken.type == LEFT_PAREN) 
        {

            Node functionCall = Node();
            functionCall.type = FUNCTION_CALL;
            functionCall.symbol = node->symbol;

            int tokenIncrement = 1;
            while (tokenIndex+2+tokenIncrement < tokens[line].size()) 
            {
                currToken = tokens[line][tokenIndex+2+tokenIncrement];
                if (currToken.type == RIGHT_PAREN) 
                {
                    break;
                }

                functionCall.parameters.push_back(evaluate(tokens, line, tokenIndex+2+tokenIncrement, currToken));
            }

            return functionCall;
        }

        return *node;
    }

    if (currToken.type == LEFT_PAREN) 
    {

        Node expression = Node();
        expression.type = EXPRESSION;

        int tokenIncrement = 1;
        while (currToken.type != RIGHT_PAREN) 
        {
            currToken = tokens[line][tokenIndex+tokenIncrement];

            if (currToken.type == COMMA) 
            {
                tokenIncrement++;
                currToken = tokens[line][tokenIndex+tokenIncrement];
            }

            if (currToken.type == RIGHT_PAREN) 
            {
                break;
            }

            Node node = evaluate(tokens, line, tokenIndex+tokenIncrement, currToken);
            expression.expression.push_back(node);
            tokenIncrement++;

            for (int i = tokenIndex+tokenIncrement; i < tokens[line].size(); i++) 
            {
                if (tokens[line][i].type == COMMA || tokens[line][i].type == RIGHT_PAREN) {
                    break;
                }
                tokenIncrement ++;
            }
            
        }

        return expression;
    }

    if (currToken.type == LEFT_BRACK) 
    {
        Node list = Node();
        list.type = LIST;
        int tokenIncrement = 1;

        while (currToken.type != RIGHT_BRACK) 
        {
            currToken = tokens[line][tokenIndex+tokenIncrement];

            if (currToken.type == COMMA) 
            {
                tokenIncrement++;
                currToken = tokens[line][tokenIndex+tokenIncrement];
            }

            if (currToken.type == RIGHT_BRACK)
            {
                break;
            } 

            Node element = evaluate(tokens, line, tokenIndex+tokenIncrement, currToken);
            list.expression.push_back(element);
            tokenIncrement++;
        }


        tokenIncrement ++;
        if (tokenIndex + tokenIncrement < tokens[line].size()) 
        {
            currToken = tokens[line][tokenIndex+tokenIncrement];
        } 
        else 
        {
            return list;
        }

        if (currToken.type == ADD && tokens[line][tokenIndex+tokenIncrement+1].type != NEW_CHUNK) 
        {
            Node operation = Node();
            operation.type = OPERATION;
            operation.value = currToken.value;

            operation.parameters.push_back(list);
            operation.parameters.push_back(evaluate(tokens, line, tokenIndex+tokenIncrement+1, tokens[line][tokenIndex+tokenIncrement+1]));

            return operation;
        }
    }

    if (currToken.type == INT) 
    {

        node->type = INT;
        node->value = currToken.value;

        if (tokens[line].size() == 1 || tokenIndex == tokens[line].size() - 1)
        {
            return *node;
        }

        currToken = tokens[line][tokenIndex+1];

        if (currToken.type == ADD) 
        {

            Node operation = Node();
            operation.type = OPERATION;
            operation.value = "+";

            operation.parameters.push_back(*node);
            operation.parameters.push_back(evaluate(tokens, line, tokenIndex+2, tokens[line][tokenIndex+2]));

            return operation;
        }

        return *node;
    }

    if (currToken.type == STRING) 
    {
        node->type = STRING;
        node->value = currToken.value;
        return *node;
    }

    if (currToken.type == TRUE || currToken.type == FALSE) 
    {
        node->type = BOOLEAN;
        node->value = currToken.value;
        return *node;
    }

    if (currToken.type == PRINT) 
    {

        Node print = Node();
        print.type = FUNCTION_CALL;
        print.symbol = currToken.value;

        print.expression.push_back(evaluate(tokens, line, tokenIndex+1, tokens[line][tokenIndex+1]));
        return print;
    }

    
    if (currToken.type == IF) 
    {

        Node ifStatement = Node();
        ifStatement.type = FUNCTION_CALL;
        ifStatement.symbol = currToken.value;

        ifStatement.parameters.push_back(evaluate(tokens, line, tokenIndex+1, tokens[line][tokenIndex+1]));

        int tokenIncrement = 2;
        for (int i = tokenIndex+tokenIncrement; i<tokens[line].size(); i++) 
        {
            currToken = tokens[line][i];
            if (currToken.type == COLON) 
            {
                break;
            } 
        }
        
        line ++;
        currToken = tokens[line][tokenIndex];
        while (currToken.type != NEW_CHUNK) 
        {
            fun_if_else ++;
            Node node = evaluate(tokens, line, tokenIndex, currToken);
            ifStatement.children.push_back(node);
            token lastElement = tokens[line][tokens[line].size()-1];
            line += 1 + node.children.size();
            if (lastElement.type == NEW_CHUNK) 
            {
                break;
            }
            currToken = tokens[line][tokenIndex];
        }
        
        currToken = tokens[line][tokenIndex];
        if (currToken.type == ELSE) 
        {
            Node elseStatement = Node();
            elseStatement.type = FUNCTION_CALL;
            elseStatement.symbol = currToken.value;

            line ++;
            fun_if_else ++;
            currToken = tokens[line][tokenIndex];
            while (currToken.type != NEW_CHUNK) 
            {
                fun_if_else ++;
                if(line > tokens.size()) break;
                Node node = evaluate(tokens, line, tokenIndex, currToken);
                elseStatement.children.push_back(node);
                token lastElement = tokens[line][tokens[line].size()-1];
                line += 1 + node.children.size();
                if (lastElement.type == NEW_CHUNK) break;
                if (tokens.size() > line) 
                {
                    currToken = tokens[line][tokenIndex];
                }
            }

            ifStatement.children.push_back(elseStatement);
        }
        
        return ifStatement;
    }

    if (currToken.type == FUNCTION_DEFINITION) 
    {

        Node function = Node();
        function.type = FUNCTION;
        int tokenIncrement = 1;
        currToken = tokens[line][tokenIndex+tokenIncrement];
        function.symbol = currToken.value;
        tokenIncrement ++;
        currToken = tokens[line][tokenIndex+tokenIncrement];
        while (currToken.type != COLON) 
        {
            if (currToken.type == IDENTIFIER) 
            {
                Node parameter = Node();
                parameter.type = PARAMETER;
                parameter.symbol = currToken.value;
                function.parameters.push_back(parameter);
            } 

            tokenIncrement ++;
            currToken = tokens[line][tokenIndex+tokenIncrement];
        }
        
        line ++;
        fun_if_else ++;
        currToken = tokens[line][tokenIndex];
        while (currToken.type != NEW_CHUNK) 
        {
            fun_if_else ++;
            Node funcStatement = evaluate(tokens, line, tokenIndex, currToken);
            function.children.push_back(funcStatement);
            token lastElement = tokens[line][tokens[line].size()-1];
            line += 1 + funcStatement.children.size();
            if (lastElement.type == NEW_CHUNK) break;
            if (tokens.size() > line) 
            {
                currToken = tokens[line][tokenIndex];
            }
        }
        return function;
    }

    if (currToken.type == NEW_CHUNK) 
    {
        if (tokens[line].size() > 1) 
        {
            return evaluate(tokens, line, tokenIndex+1, tokens[line][tokenIndex+1]);
        } 
        else 
        {
            return Node();
        }
    }

    if (currToken.type == RETURN) 
    {

        Node returnNode = Node();
        returnNode.type = RETURN;

        currToken = tokens[line][tokenIndex+1];
        returnNode.expression.push_back(evaluate(tokens, line, tokenIndex+1, currToken));
        return returnNode;
    }
    
    Node error = Node();
    error.type = ERROR;
    error.value = "Unrecognized statement";
    return error;
}



Symbol evaulate(Node node) 
{
    if (node.type == ASSIGMENT) 
    {

        Symbol symbol = Symbol();
        std::string symbolName = node.symbol;
        symbol.line = node.line;


        symbol = evaulate(node.expression[0]);
        symTable[symbolName] = symbol;

        return symbol;
    }

    if (node.type == FUNCTION) 
    {
        Symbol function = Symbol();
        std::string functionName = node.symbol;
        function.function = node;
        function.line = node.line;
        function.type = "function";
        symTable[functionName] = function;
    }

    if (node.type == FUNCTION_CALL) 
    {
        if (node.symbol == "print") 
        {
            for (int i=0; i<node.expression[0].expression.size(); i++) 
            {
                Symbol toPrint = evaulate(node.expression[0].expression[i]);
                if (toPrint.type == INT)
                {
                    int intPrint = toPrint.integer;
                    std::cout << intPrint << " ";
                }
                if (toPrint.type == STRING)
                { 
                    std::string strPrint = toPrint.str;
                    std::cout << strPrint << " ";
                }
                if (toPrint.type == LIST) 
                {
                    std::cout << "[";
                    for (int i = 0; i < toPrint.list.size() -1 ; i++) 
                    {
                        std::cout << toPrint.list[i] << ", ";
                    }
                    int last = toPrint.list.size()-1;
                    std::cout << toPrint.list[last];
                    std::cout << "] ";
                }
            }

            std::cout << std::endl;
        }

        if (node.symbol == "if") 
        {
            Symbol condition = evaulate(node.parameters[0]);
            
            if (condition.boolean) 
            {
                for (int i = 0; i < node.children.size(); i++) 
                {
                    evaulate(node.children[i]);
                }
            } 
            else if (node.expression.size() > 0) 
            {
                evaulate(node.expression[0]);
            }
        }

        if (node.symbol == "else") 
        {
            for (int i = 0; i < node.children.size(); i++)
            {
                evaulate(node.children[i]);
            }
        }

        if (symTable.find(node.symbol) != symTable.end()) 
        {
            Node function = symTable[node.symbol].function;

            for (int i = 0; i < function.parameters.size(); i++) 
            {
                std::string parameterName = function.parameters[i].symbol;
                Symbol parameterValue = evaulate(node.parameters[i]);
                symTable[parameterName] = parameterValue;
            }

            for (int i = 0; i < function.children.size(); i++) 
            {
                if (function.children[i].type == RETURN) 
                {
                    return evaulate(function.children[i]);
                }
                evaulate(function.children[i]);
            }
            return Symbol();
        }
    }

    if (node.type == OPERATION) 
    {
        Symbol term1 = evaulate(node.parameters[0]);
        Symbol term2 = evaulate(node.parameters[1]);
        Symbol result = Symbol();
        result.line = node.line;

        if (node.value == "+") 
        {
            if (term1.type == LIST && term2.type == LIST) 
            {
                Symbol res = Symbol();
                res.type = LIST;
                res.line = node.line;
                std::vector<int> list;
                for (int i = 0; i < term1.list.size(); i++) 
                {
                    list.push_back(term1.list[i]);
                }
                for (int i = 0; i < term2.list.size(); i++) 
                {
                    list.push_back(term2.list[i]);
                }
                res.list = list;
                return res;
            }

            if (term1.type == STRING && term2.type == STRING) 
            {
                Symbol res = Symbol();
                res.type = STRING;
                res.line = node.line;
                std::string str = "";
                for (int i = 0; i < term1.str.size(); i++) 
                {
                    str += term1.str[i];
                }
                for (int i = 0; i < term2.str.size(); i++) 
                {
                    str += term2.str[i];
                }
                res.str = str;
                return res;
            }

            if(term1.type == INT && term2.type == INT)
            {
                result.type = INT;
                result.integer = term1.integer + term2.integer;
                return result;
            }

            if((term1.type == INT && term2.type != INT) || (term1.type != INT && term2.type == INT))
            {
                std::cout << "error, cannot add number to non-number\n";
            }
            if((term1.type == LIST && term2.type != LIST) || (term1.type != LIST && term2.type == LIST))
            {
                std::cout << "error, cannot add list to non-list\n";
            }
            
        }

        if (node.value == "==") 
        {
            result.type = BOOLEAN;
            if (term1.type == NULL_ID) 
            {
                result.boolean = false;
            } 
            if (term2.type == NULL_ID)
            {
                result.boolean = false;
            } 
            else 
            {
                result.boolean = term1.integer == term2.integer;
            }
            return result;
        }

        if (node.value == ">") 
        {
            result.type = BOOLEAN;
            result.boolean = term1.integer > term2.integer;
            return result;
        }

        if (node.value == "<") 
        {
            result.type = BOOLEAN;
            result.boolean = term1.integer < term2.integer;
            return result;
        }

        if (node.value == ">=") 
        {
            result.type = BOOLEAN;
            result.boolean = term1.integer >= term2.integer;
            return result;
        }

        if (node.value == "<=") 
        {
            result.type = BOOLEAN;
            result.boolean = term1.integer <= term2.integer;
            return result;
        }

        if (node.value == "!=") 
        {
            result.type = BOOLEAN;
            result.boolean = term1.integer != term2.integer;
            return result;
        }

    }

    if (node.type == EXPRESSION) 
    {
        return evaulate(node.expression[0]);
    }

    if (node.type == INT) 
    {
        Symbol symbol = Symbol();
        symbol.type = "INT";
        symbol.integer = stoi(node.value);
        symbol.line = node.line;
        return symbol;
    }

    if (node.type == STRING) 
    {
        Symbol symbol = Symbol();
        symbol.type = "STRING";
        symbol.str = node.value;
        symbol.line = node.line;
        return symbol;
    }

    if (node.type == BOOLEAN) 
    {
        Symbol symbol = Symbol();
        symbol.type = "BOOLEAN";
        symbol.boolean = node.value == "true";
        symbol.line = node.line;
        return symbol;
    }

    if (node.type == LIST) 
    {
        Symbol symbol = Symbol();
        symbol.type = "LIST";
        symbol.line = node.line;
        for (int i=0; i<node.expression.size(); i++) 
        {
            Symbol listElement = evaulate(node.expression[i]);
            symbol.list.push_back(listElement.integer);
        }
        return symbol;
    }

    if (node.type == NULL_ID) 
    {
        Symbol symbol = Symbol();
        symbol.type = "NULL";
        symbol.line = node.line;
        return symbol;
    }

    if (node.type == LIST_INDEX) 
    {
        if (symTable.find(node.symbol) != symTable.end()) 
        {
            Symbol symbol = symTable[node.symbol];
            if (node.expression.size() > 0) 
            {
                Symbol parameter = evaulate(node.parameters[0]);
                Symbol expression = evaulate(node.expression[0]);
                symbol.list[parameter.integer] = expression.integer;
                return symbol;
            } 
            else 
            {
                Symbol result = Symbol();
                result.type = INT;
                result.line = node.line;
                result.integer = symbol.list[evaulate(node.parameters[0]).integer];
                return result;
            }
        } 
    }

    if (node.type == RETURN) 
    {
        return evaulate(node.expression[0]);
    }

    if (node.type == IDENTIFIER) 
    {
        if(symTable.find(node.symbol) != symTable.end()) 
        {
            return symTable[node.symbol];
        } 
    }
    return Symbol();
}

std::vector<std::vector<token>> lexer(std::string input) 
{
    std::vector<std::vector<token>> tokens; 
    std::vector<token> lineTokens; 
    int currIndex = 0; 
    int indentationLevel = 0; 
    int indentationLength = 0; 
    bool isIndent = false; 
    int lastIndentationLength = 0; 
    while (currIndex < input.length()) 
    {
        std::string subStr = ""; 
        subStr += input[currIndex]; 

        
        if (input[currIndex] == '\n') 
        {
            if(isIndent) 
            {
                std::string indentation = "";

                if (indentationLevel == 1 && indentationLength == 0)
                {
                    int index = 1;
                    while (input[currIndex+index] == ' ') 
                    {
                        index ++;
                        indentationLength ++;
                    }
                }

                for (int i = 1; i <= indentationLength * indentationLevel; i++) 
                {
                    if (input[currIndex+i] == ' ') 
                    {
                        indentation += input[currIndex+i];
                    } 
                    else 
                    {
                        break;
                    }
                }
                if (indentation.length() != 0 && indentation.length() % (indentationLength*indentationLevel) == 0) 
                {
                    currIndex += 1 + indentation.length();
                    if (lineTokens.size() > 0) tokens.push_back(lineTokens);
                    lineTokens.clear();
                    continue;
                }
                
                if (input[currIndex+1] == '\n') 
                {
                    currIndex ++;
                    continue;
                }
                if (indentation.length() == 0 && indentationLevel > 1) 
                {
                    for (int i = 1; i < indentationLevel; i++) 
                    {
                        std::vector<token> breakLine;
                        token t;
                        t.type = NEW_CHUNK;
                        t.value = NEW_CHUNK;
                        breakLine.push_back(t);
                        tokens.push_back(breakLine);
                        indentationLevel --;
                    }
                }
                
                indentationLevel --;
                if (indentationLevel == 0) isIndent = false; 
                token t;
                t.type = NEW_CHUNK;
                t.value = "NEW_CHUNK";
                lineTokens.push_back(t);
                indentationLength = indentationLength * indentationLevel; 
                currIndex += 1 + indentation.length(); 
                tokens.push_back(lineTokens); 
                lineTokens.clear(); 
                continue;
            }

            currIndex ++; 
            if (lineTokens.size()>0) 
            {
                tokens.push_back(lineTokens);
                lineTokens.clear();
            }
            continue;
        }

        //check for comment or whitespace
        if(input[currIndex] == '#' || input[currIndex] == ' ') 
        {
            currIndex ++;
            continue;
        }

        //check for comma
        if(input[currIndex] == ',') {
            token t;
            t.type = COMMA;
            t.value = ",";
            lineTokens.push_back(t);
            currIndex ++;
            continue;
        }

        //check for colon
        if(input[currIndex] == ':') {
            token t;
            t.type = COLON;
            t.value = ":";
            lineTokens.push_back(t);
            currIndex ++;
            continue;
        }

        //equal and equal_equal
        if (input[currIndex] == '=') 
        {
            currIndex ++;
            if (input[currIndex] == '=') 
            {
                token t;
                t.type = OPERATOR;
                t.value = "==";
                lineTokens.push_back(t);
                currIndex ++;
                continue;
            } 
            else 
            {
                token t;
                t.type = EQUALS;
                t.value = "=";
                lineTokens.push_back(t);
                continue;
            }
        }

        //greater, greater than
        if (input[currIndex] == '>') 
        {
            currIndex ++;
            if (input[currIndex] == '=') 
            {
                token t;
                t.type = OPERATOR;
                t.value = ">=";
                lineTokens.push_back(t);
                currIndex ++;
                continue;
            } 
            else 
            {
                token t;
                t.type = OPERATOR;
                t.value = ">";
                continue;
            }
        }

        //less, less than
        if (input[currIndex] == '<') 
        {
            currIndex ++;
            if (input[currIndex] == '=') 
            {
                token t;
                t.type = OPERATOR;
                t.value = "<=";
                lineTokens.push_back(t);
                currIndex ++;
                continue;
            } 
            else 
            {
                token t;
                t.type = OPERATOR;
                t.value = "<";
                lineTokens.push_back(t);
                continue;
            }
        }

        //check not equal
        if (input[currIndex] == '!') 
        {
            currIndex ++;
            if (input[currIndex] == '=') 
            {
                token t;
                t.type = OPERATOR;
                t.value = "!=";
                lineTokens.push_back(t);
                currIndex ++;
                continue;
            } 
        }

        //check add
        if (input[currIndex] == '+') 
        {
            token t;
            t.type = ADD;
            t.value = "+";
            lineTokens.push_back(t);
            currIndex ++;
            continue;
        }

        //check for parentheses
        if (input[currIndex] == '(') 
        {
            token t;
            t.type = LEFT_PAREN;
            t.value = "(";
            lineTokens.push_back(t);
            currIndex ++;
            continue;
        }

        if (input[currIndex] == ')') 
        {
            token t;
            t.type = RIGHT_PAREN;
            t.value = ")";
            lineTokens.push_back(t);
            currIndex ++;
            continue;
        }

        //checking for bracket
        if (input[currIndex] == '[') 
        {
            token t;
            t.type = LEFT_BRACK;
            t.value = "[";
            lineTokens.push_back(t);
            currIndex ++;
            continue;
        }

        if (input[currIndex] == ']') 
        {
            token t;
            t.type = RIGHT_BRACK;
            t.value = "]";
            lineTokens.push_back(t);
            currIndex ++;
            continue;
        }

        //checking for string
        if (input[currIndex] == '\"') 
        {
            std::string str = "";
            currIndex ++;
            while (input[currIndex] != '\"') 
            {
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
        if (isdigit(subStr[0])) 
        {
            std::string number = "";
            while (isdigit(input[currIndex]) && currIndex < input.length()) 
            {
                number += input[currIndex];
                currIndex ++;
            }

            token t;
            t.type = INT;
            t.value = number;
            lineTokens.push_back(t);
            continue;
        }

        //checking for identifier
        if (isalpha(subStr[0])) 
        {
            std::string identifier = "";
            while ((isalpha(input[currIndex]) || isdigit(input[currIndex])) && currIndex < input.length()) 
            {
                identifier += input[currIndex];
                currIndex ++;
            }

            if (identifier == "print") 
            {
                token t;
                t.type = PRINT;
                t.value = "print";
                lineTokens.push_back(t);
            
            } 
            else if (identifier == "if")
            {
                isIndent = true;
                indentationLevel ++;
                token t;
                t.type = IF;
                t.value = "if";
                lineTokens.push_back(t);
            } 
            else if (identifier == "else") 
            {
                isIndent = true;
                indentationLevel ++;
                token t;
                t.type = ELSE;
                t.value = "else";
                lineTokens.push_back(t);
            } 
            else if (identifier == "def") 
            {
                indentationLevel ++;
                isIndent = true;
                token t;
                t.type = FUNCTION_DEFINITION;
                t.value = "def";
                lineTokens.push_back(t);
            } 
            else if (identifier == "return") 
            {
                token t;
                t.type = RETURN;
                t.value = "return";
                lineTokens.push_back(t);
            } 
            else if (identifier == "True") 
            {
                token t;
                t.type = TRUE;
                t.value = "true";
                lineTokens.push_back(t);
            } 
            else if (identifier == "False") 
            {
                token t;
                t.type = FALSE;
                t.value = "false";
                lineTokens.push_back(t);
            }
            
            else 
            {
                token t;
                t.type = IDENTIFIER;
                t.value = identifier;
                lineTokens.push_back(t);
            }
            continue;
        }        
    }

    //add line tokens to tokens
    if (lineTokens.size() > 0) {
        tokens.push_back(lineTokens);
    }
    return tokens;
}
