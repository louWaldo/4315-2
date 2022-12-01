#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <filesystem>
#include "lexer.h"

//AST Node Types


using namespace std;






int linesToSkip = 0; //Used with function definitions, if and else statements so that those lines are nor parsed several times

//Each AST node can have children like the program root node or functions. Paarameters like conditions on if statements and arguments in functions. And Expressions which involves assigments. operations, function calls and so on.
class Node
{
    public:
        string type;
        string value;
        string symbol;
        vector<Node> children;
        vector<Node> parameters;
        vector<Node> expression;
        int line;
        Node parseTree(vector<vector<token>> tokens);
        Node parseStatement(vector<vector<token>> tokens, int line, int tokenIndex, token currToken);

};

//Interpreter Variables functions and types. Used for evaluation of the AST
class Symbol
{
    public:
        string type;
        int integer;
        string str;
        bool boolean;
        vector<int> list;
        Node function;
        int line;
};

unordered_map<string, Symbol> variables; //Stores all variables from the outer scope

//Function Declarations
Symbol traverse(Node node);

int main(int argc, char* argv[]) 
{
    std::string input_file_name = argv[1];
    std::ifstream input(input_file_name);
    //load contents of python file into std::string contents
    std::string contents;
    contents.assign( (std::istreambuf_iterator<char>(input) ),(std::istreambuf_iterator<char>()    ) );
    
    vector<vector<token>> tokens = lexer(contents);

    Node* root = new Node;
    Node* ast = new Node;

    *root = ast->parseTree(tokens);

    for (int i=0; i < root->children.size(); i++) 
    {
        traverse(root->children[i]);
    }
    return 0;
}






//TREE PARSING
Node Node::parseTree(vector<vector<token>> tokens) 
{

    //create root node: <program>
    Node* root = new Node;
    root->type = "PROGRAM";
    
    int line = 0; //current line
    int tokenIndex = 0; //current token index
    
    token currToken; //current token

    //Iterate through all children in program
    while (line < tokens.size()) 
    {
        currToken = tokens[line][tokenIndex]; //Get current token
        Node* node = new Node;
        *node = parseStatement(tokens, line, tokenIndex, currToken); //Parse token
        root->children.push_back(*node); //Add statement to root
        if(linesToSkip == 0)
        {
            line++;
        }
        else
        {
            line = linesToSkip + line;
        }

        linesToSkip = 0; //Reset linesToSkip
        tokenIndex = 0; //Reset tokenIndex
    }

    return *root; //Return AST
}

//Recursive AST parse function
Node Node::parseStatement(vector<vector<token>> tokens, int line, int tokenIndex, token currToken) 
{
    //Create AST node
    Node* node = new Node;

    //<IDENTIFIER>
    //If token is an identifier
    if (currToken.type == IDENTIFIER) 
    {

        node->type = IDENTIFIER; //Set type
        node->symbol = currToken.value; //Set symbol

        //If the line only contains an identifier or if the identifier is the last element in the line, return identifier
        if (tokens[line].size() == 1 || tokenIndex == tokens[line].size() - 1 || tokens[line][tokenIndex + 1].type == BLOCK_END)
        {
            return *node;
        }
        
        //Obtain following token to the identifier
        currToken = tokens[line][tokenIndex+1];

        //<IDENTIFIER><EQUALS>
        //if following token is an equal sign
        if (currToken.type == EQUALS) 
        {

            node->type = ASSIGMENT; //Set type
            Node expression = parseStatement(tokens, line, tokenIndex+2, tokens[line][tokenIndex+2]); //Parse expression after equal sign

            node->expression.push_back(expression); //Add expression to assigment node
            return *node; 
        }

        //<IDENTIFIER><PLUS>
        //if following token is a plus sign
        if (currToken.type == PLUS) 
        {

            Node operation = Node();
            operation.type = OPERATION; //Set type
            operation.value = "+"; //Set value

            operation.parameters.push_back(*node); //Add identifier to parameters
            operation.parameters.push_back(parseStatement(tokens, line, tokenIndex+2, tokens[line][tokenIndex+2])); //Add expression after plus sign to parameters

            return operation;
        }

        //<IDENTIFIER><OPEN_PARENTHESIS>
        //if following token is an open parentheses
        if (currToken.type == OPEN_PARENTHESES) 
        {

            Node functionCall = Node();
            functionCall.type = FUNCTION_CALL; //Set type
            functionCall.symbol = node->symbol; //Set symbol
            int tokenIncrement = 2; //Increment to skip open parentheses
            currToken = tokens[line][tokenIndex+tokenIncrement]; //Get next token
            while (currToken.type != CLOSE_PARENTHESES) 
            {
                //Ignore commas
                if (currToken.type == COMMA) 
                {
                    tokenIncrement++;
                    currToken = tokens[line][tokenIndex+tokenIncrement];
                    continue;
                }
                Node expression = parseStatement(tokens, line, tokenIndex+tokenIncrement, currToken); //parse parameter
                functionCall.parameters.push_back(expression); //Add parameter to function call
                tokenIncrement ++; //Increment to skip parameter
                currToken = tokens[line][tokenIndex+tokenIncrement]; //Get next token
            }
            return functionCall;
        }

        //<IDENTIFIER><CLOSE_PARENTHESIS>
        //if following token is a close parentheses
        if (currToken.type == CLOSE_PARENTHESES) 
        {
            return *node;
        }

        //<IDENTIFIER><OPEN_BRACKET>
        if (currToken.type == OPEN_BRACKET) 
        {
            Node listIndex = Node();
            listIndex.type = LIST_INDEX; //Set type
            listIndex.symbol = node->symbol; //Set symbol
            listIndex.parameters.push_back(parseStatement(tokens, line, tokenIndex+2, tokens[line][tokenIndex+2])); //Add expression after open bracket to parameters

            int tokenIncrement = 2; //Increment to skip open bracket

            while (currToken.type != CLOSE_BRACKET) 
            { 
                if (tokens[line].size() > tokenIndex + tokenIncrement) 
                {
                    currToken = tokens[line][tokenIndex+tokenIncrement];
                    tokenIncrement++;
                } 
                else 
                {
                    cout << "no closing bracket\n";
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
            
            //<IDENTIFIER><OPEN_BRACKET>...<CLOSE_BRACKET><EQUALS>
            //If following token is an equal sign
            if (currToken.type == EQUALS) 
            {
                node->type = ASSIGMENT;
                currToken = tokens[line][tokenIndex+tokenIncrement+1];
                listIndex.expression.push_back(parseStatement(tokens, line, tokenIndex+tokenIncrement+1, currToken));
                node->expression.push_back(listIndex);
                return *node;
            }

            //<IDENTIFIER><OPEN_BRACKET>...<CLOSE_BRACKET><PLUS>
            if (currToken.type == PLUS) 
            { 
                Node operation = Node();
                operation.type = OPERATION;
                operation.value = currToken.value;

                operation.parameters.push_back(listIndex);
                operation.parameters.push_back(parseStatement(tokens, line, tokenIndex+tokenIncrement+1, tokens[line][tokenIndex+tokenIncrement+1]));

                return operation;
            }

            return listIndex;
        }

        //<IDENTIFIER><OPERATOR>
        if (currToken.type == OPERATOR) 
        {

            Node operation = Node();
            operation.type = OPERATION;
            operation.value = currToken.value;

            operation.parameters.push_back(*node);
            operation.parameters.push_back(parseStatement(tokens, line, tokenIndex+2, tokens[line][tokenIndex+2]));

            return operation;
        }

        //<IDENTIFIER><OPEN_PARENTHESIS>
        if (currToken.type == OPEN_PARENTHESES) 
        {

            Node functionCall = Node();
            functionCall.type = FUNCTION_CALL;
            functionCall.symbol = node->symbol;

            //If the function call has parameters
            int tokenIncrement = 1;
            while (tokenIndex+2+tokenIncrement < tokens[line].size()) 
            {
                currToken = tokens[line][tokenIndex+2+tokenIncrement];
                //if end of parameters reached
                if (currToken.type == CLOSE_PARENTHESES) break;

                functionCall.parameters.push_back(parseStatement(tokens, line, tokenIndex+2+tokenIncrement, currToken));
            }

            return functionCall;
        }

        return *node;
    }

    //<OPEN_PARENTHESIS>
    //If token is an open parenthesis
    if (currToken.type == OPEN_PARENTHESES) 
    {

        Node expression = Node();
        expression.type = EXPRESSION;

        int tokenIncrement = 1;
        while (currToken.type != CLOSE_PARENTHESES) 
        {
            //Get next token
            currToken = tokens[line][tokenIndex+tokenIncrement];

            //Check if it is a comma if it is skip it
            if (currToken.type == COMMA) 
            {
                tokenIncrement++;
                currToken = tokens[line][tokenIndex+tokenIncrement];
            }

            //Check if it is a close parenthesis if it is break
            if (currToken.type == CLOSE_PARENTHESES) break;

            //Parse next token
            Node node = parseStatement(tokens, line, tokenIndex+tokenIncrement, currToken);
            
            //Add to parameters
            expression.expression.push_back(node);
            tokenIncrement++;

            //move to the next comma or closing parenthesis
            for (int i = tokenIndex+tokenIncrement; i < tokens[line].size(); i++) 
            {
                if (tokens[line][i].type == COMMA || tokens[line][i].type == CLOSE_PARENTHESES) {
                    break;
                }
                tokenIncrement ++;
            }
            
        }

        return expression;
    }

    //<OPEN BRACKET>
    //If token is an open bracket
    if (currToken.type == OPEN_BRACKET) 
    {

        Node list = Node();
        list.type = LIST;
        int tokenIncrement = 1;

        while (currToken.type != CLOSE_BRACKET) 
        {
            //Get token
            currToken = tokens[line][tokenIndex+tokenIncrement];

            //Check if it is a comma if it is skip it
            if (currToken.type == COMMA) 
            {
                tokenIncrement++;
                currToken = tokens[line][tokenIndex+tokenIncrement];
            }

            //Check if it is a close bracket if it is break
            if (currToken.type == CLOSE_BRACKET) break;

            //Parse next token
            Node element = parseStatement(tokens, line, tokenIndex+tokenIncrement, currToken);

            //Add to list expression
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

        if (currToken.type == PLUS && tokens[line][tokenIndex+tokenIncrement+1].type != BLOCK_END) 
        {
            Node operation = Node();
            operation.type = OPERATION;
            operation.value = currToken.value;

            operation.parameters.push_back(list);
            operation.parameters.push_back(parseStatement(tokens, line, tokenIndex+tokenIncrement+1, tokens[line][tokenIndex+tokenIncrement+1]));

            return operation;
        }
    }

    //<NUMBER>
    //If token is a number
    if (currToken.type == NUMBER) 
    {

        node->type = NUMBER;
        node->value = currToken.value;

        //If the line only contains a number or if the number is the last element in the line
        if (tokens[line].size() == 1 || tokenIndex == tokens[line].size() - 1)
        {
            return *node;
        }

        currToken = tokens[line][tokenIndex+1];

        //<NUMBER><PLUS>
        if (currToken.type == PLUS) 
        {

            Node operation = Node();
            operation.type = OPERATION;
            operation.value = "+";

            operation.parameters.push_back(*node);
            operation.parameters.push_back(parseStatement(tokens, line, tokenIndex+2, tokens[line][tokenIndex+2]));

            return operation;
        }

        return *node;
    }

    //<STRING>
    //If token is a string
    if (currToken.type == STRING) 
    {
        node->type = STRING;
        node->value = currToken.value;
        return *node;
    }

    //<BOOLEAN>
    //If token is a boolean
    if (currToken.type == TRUE || currToken.type == FALSE) 
    {
        node->type = BOOLEAN;
        node->value = currToken.value;
        return *node;
    }

    //<NULL>
    //If token is null
    if (currToken.type == NULL_IDENTIFIER) 
    {

        node->type = NULL_ID;
        node->value = currToken.value;

        return *node;
    }

    //<PRINT>
    //If token is print
    if (currToken.type == PRINT) 
    {

        Node print = Node();
        print.type = FUNCTION_CALL;
        print.symbol = currToken.value;

        print.expression.push_back(parseStatement(tokens, line, tokenIndex+1, tokens[line][tokenIndex+1]));
        return print;
    }

    
    //<IF>
    if (currToken.type == IF) 
    {

        Node ifStatement = Node();
        ifStatement.type = FUNCTION_CALL;
        ifStatement.symbol = currToken.value;

        ifStatement.parameters.push_back(parseStatement(tokens, line, tokenIndex+1, tokens[line][tokenIndex+1]));

        //Check for colon and new line
        int tokenIncrement = 2;
        for (int i = tokenIndex+tokenIncrement; i<tokens[line].size(); i++) 
        {
            currToken = tokens[line][i];
            if (currToken.type == COLON) 
            {
                break;
            } 
        }
        
        //Add as childs next lines until end block found
        line ++;
        currToken = tokens[line][tokenIndex];
        while (currToken.type != BLOCK_END) 
        {
            linesToSkip ++;
            Node node = parseStatement(tokens, line, tokenIndex, currToken);
            ifStatement.children.push_back(node);
            token lastElement = tokens[line][tokens[line].size()-1];
            line += 1 + node.children.size();
            if (lastElement.type == BLOCK_END) 
            {
                break;
            }
            currToken = tokens[line][tokenIndex];
        }
        
        //If next token is Else, add else as expression
        currToken = tokens[line][tokenIndex];
        if (currToken.type == ELSE) 
        {
            Node elseStatement = Node();
            elseStatement.type = FUNCTION_CALL;
            elseStatement.symbol = currToken.value;

            line ++;
            linesToSkip ++;
            currToken = tokens[line][tokenIndex];
            while (currToken.type != BLOCK_END) 
            {
                linesToSkip ++;
                if(line > tokens.size()) break;
                Node node = parseStatement(tokens, line, tokenIndex, currToken);
                elseStatement.children.push_back(node);
                token lastElement = tokens[line][tokens[line].size()-1];
                line += 1 + node.children.size();
                if (lastElement.type == BLOCK_END) break;
                if (tokens.size() > line) 
                {
                    currToken = tokens[line][tokenIndex];
                }
            }

            ifStatement.children.push_back(elseStatement);
        }
        
        return ifStatement;
    }

    //<FUNCTION_DEFINITION>
    if (currToken.type == FUNCTION_DEFINITION) 
    {

        Node function = Node();
        function.type = FUNCTION;

        //Get funcion name
        int tokenIncrement = 1;
        currToken = tokens[line][tokenIndex+tokenIncrement];

        function.symbol = currToken.value;

        //Get parameters
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
        linesToSkip ++;
        currToken = tokens[line][tokenIndex];
        while (currToken.type != BLOCK_END) 
        {
            linesToSkip ++;
            Node funcStatement = parseStatement(tokens, line, tokenIndex, currToken);
            function.children.push_back(funcStatement);
            token lastElement = tokens[line][tokens[line].size()-1];
            line += 1 + funcStatement.children.size();
            if (lastElement.type == BLOCK_END) break;
            if (tokens.size() > line) 
            {
                currToken = tokens[line][tokenIndex];
            }
        }
        return function;
    }

    //<BLOCK_END>
    if (currToken.type == BLOCK_END) 
    {
        if (tokens[line].size() > 1) 
        {
            return parseStatement(tokens, line, tokenIndex+1, tokens[line][tokenIndex+1]);
        } 
        else 
        {
            return Node();
        }
    }

    //<RETURN>
    if (currToken.type == RETURN) 
    {

        Node returnNode = Node();
        returnNode.type = RETURN;

        currToken = tokens[line][tokenIndex+1];
        returnNode.expression.push_back(parseStatement(tokens, line, tokenIndex+1, currToken));


        return returnNode;
    }
    
    Node error = Node();
    error.type = ERROR;
    error.value = "Unrecognized statement";
    return error;
}



Symbol traverse(Node node) 
{
    unordered_map<string, Symbol> variables2 = variables;

    if (node.type == ASSIGMENT) 
    {

        Symbol symbol = Symbol();
        string symbolName = node.symbol;
        symbol.line = node.line;

        if (node.expression.size() == 0) cout << "ERROR: Assigment without expression at line: " << node.line << endl;

        symbol = traverse(node.expression[0]);
        variables[symbolName] = symbol;

        return symbol;
    }

    if (node.type == FUNCTION) 
    {
        Symbol function = Symbol();
        string functionName = node.symbol;
        function.function = node;
        function.line = node.line;
        function.type = "function";
        variables[functionName] = function;
    }

    if (node.type == FUNCTION_CALL) 
    {
        if (node.symbol == "print") 
        {
            if (node.expression.size() == 0) cout << "ERROR: Print without expression at line: " << node.line << endl;
            for (int i=0; i<node.expression[0].expression.size(); i++) 
            {
                Symbol toPrint = traverse(node.expression[0].expression[i]);
                if (toPrint.type == NUMBER) cout << toPrint.integer << " ";
                if (toPrint.type == STRING) cout << toPrint.str << " ";
                if (toPrint.type == LIST) 
                {
                    cout << "[";
                    for (int i=0; i<toPrint.list.size(); i++) 
                    {
                        cout << toPrint.list[i];
                        if (i != toPrint.list.size() - 1) cout << ", ";
                    }
                    cout << "] ";
                }
            }

            cout << endl;
        }

        if (node.symbol == "if") 
        {
            Symbol condition = traverse(node.parameters[0]);
            
            if (condition.boolean) 
            {
                //If condition is true
                for (int i = 0; i < node.children.size(); i++) 
                {
                    traverse(node.children[i]);
                }
            } 
            else if (node.expression.size() > 0) 
            {
                //if condition is false
                traverse(node.expression[0]);
            }
        }

        if (node.symbol == "else") 
        {
            for (int i = 0; i < node.children.size(); i++)
            {
                traverse(node.children[i]);
            }
        }

        if (variables.find(node.symbol) != variables.end()) 
        {
            Node function = variables[node.symbol].function;

            //Get parameters
            for (int i = 0; i < function.parameters.size(); i++) 
            {
                string parameterName = function.parameters[i].symbol;
                Symbol parameterValue = traverse(node.parameters[i]);
                variables[parameterName] = parameterValue;
            }

            //Execute function
            for (int i = 0; i < function.children.size(); i++) 
            {
                if (function.children[i].type == RETURN) 
                {
                    return traverse(function.children[i]);
                }
                traverse(function.children[i]);
            }
            return Symbol();
        }
    }

    if (node.type == OPERATION) 
    {
        Symbol term1 = traverse(node.parameters[0]);
        Symbol term2 = traverse(node.parameters[1]);


        Symbol result = Symbol();
        result.line = node.line;

        //SUM
        if (node.value == "+") 
        {
            if (term1.type == LIST && term2.type == LIST) 
            {
                Symbol newList = Symbol();
                newList.type = LIST;
                newList.line = node.line;
                vector<int> list;
                for (int i = 0; i < term1.list.size(); i++) 
                {
                    list.push_back(term1.list[i]);
                }
                for (int i = 0; i < term2.list.size(); i++) 
                {
                    list.push_back(term2.list[i]);
                }
                newList.list = list;
                return newList;
            }

            result.type = NUMBER;
            result.integer = term1.integer + term2.integer;
            return result;
        }

        //COMPARISON
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

        //GREATER THAN
        if (node.value == ">") 
        {
            result.type = BOOLEAN;
            result.boolean = term1.integer > term2.integer;
            return result;
        }

        //LESS THAN
        if (node.value == "<") 
        {
            result.type = BOOLEAN;
            result.boolean = term1.integer < term2.integer;
            return result;
        }

        //GREATER THAN OR EQUAL
        if (node.value == ">=") 
        {
            result.type = BOOLEAN;
            result.boolean = term1.integer >= term2.integer;
            return result;
        }

        //LESS THAN OR EQUAL
        if (node.value == "<=") 
        {
            result.type = BOOLEAN;
            result.boolean = term1.integer <= term2.integer;
            return result;
        }

        //DIFFERENT
        if (node.value == "!=") 
        {
            result.type = BOOLEAN;
            result.boolean = term1.integer != term2.integer;
            return result;
        }

        cout << "ERROR: Operation not implemented at line: " << node.line << endl;
    }

    if (node.type == EXPRESSION) 
    {
        return traverse(node.expression[0]);
    }

    if (node.type == NUMBER) 
    {
        Symbol symbol = Symbol();
        symbol.type = "NUMBER";
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
            Symbol listElement = traverse(node.expression[i]);
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
        if (variables.find(node.symbol) != variables.end()) 
        {
            Symbol symbol = variables[node.symbol];
            if (node.expression.size() > 0) 
            {
                Symbol parameter = traverse(node.parameters[0]);
                Symbol expression = traverse(node.expression[0]);
                symbol.list[parameter.integer] = expression.integer;
                return symbol;
            } 
            else 
            {
                Symbol result = Symbol();
                result.type = NUMBER;
                result.line = node.line;
                result.integer = symbol.list[traverse(node.parameters[0]).integer];
                return result;
            }
        } 
    }

    if (node.type == RETURN) 
    {
        return traverse(node.expression[0]);
    }

    if (node.type == IDENTIFIER) 
    {
        if(variables.find(node.symbol) != variables.end()) 
        {
            return variables[node.symbol];
        } 
    }
    return Symbol();
}