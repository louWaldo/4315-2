#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>

//defining all tokens
#define INT "INT"
#define ADD "PLUS"
#define MINUS "MINUS"
#define ASSIGNMENT "EQUAL"
#define MULTIPLY "MULTIPLY"
#define DIVIDE "DIVIDE"
#define COLON "COLON"
#define COMMA "COMMA"
#define L_PARENTH "LEFT_PAREN"
#define R_PARENTH "RIGHT_PAREN"
#define L_BRACK "LEFT_BRACKET"
#define R_BRACK "RIGHT_BRACKET"
#define IDENTIFIER "IDENTIFIER"
#define IF "if"
#define ELIF "elif"
#define ELSE "else"
#define PRINT "print"
#define NOT_EQUAL "NOT_EQUAL"
#define EQUAL_EQUAL "EQUAL_EQUAL"
#define LESS_THAN "LESS_THAN"
#define LESS_THAN_EQUAL "LESS_THAN_OR_EQUAL"
#define GREATER_THAN "GREATER_THAN"
#define GREATER_THAN_EQUALS "GREATER_THAN_OR_EQUAL"
#define STRING "STRING"
#define LIST "LIST"

class Token
{
public:
    std::string type;
    std::string value;
    Token();
};

class Lexer
{
public:
    std::vector<Token>* tokenVector;
    Lexer();
    // will lex the input file line by line and make tokens 
    void run_lexer(std::string instruction);
    void make_tokens(std::string instruction);

    void print_tokenVector();
    void print_symbolTable();
};


class FactorNode
{
public:
    Token token;
    FactorNode() {}
};

class TermNode
{
public:
    FactorNode *left_factor;
    FactorNode *right_factor;
    /*
    used for nesting, either a term will be a factor node
    or a binary node indicating two number operation
    */
    TermNode *left_binary_term;
    Token op;

    TermNode()
    {
        left_factor = nullptr;
        right_factor = nullptr;
        op = Token();
    };
};

class ExpressionNode
{
public:
    TermNode *left_term;
    TermNode *right_term;
    ExpressionNode *left_binary_expression;
    Token op;

    ExpressionNode()
    {
        left_term = nullptr;
        right_term = nullptr;
        op = Token();
    };
};

class Parser
{
public:
    int current_token_index;
    Token current_token;
    std::vector<Token> *tokenVector;
    std::unordered_map<std::string, Token> *symbolTable;

    bool *whitespace_if;
    bool *whitespace_else;
    bool *whitespace_done;

    Parser();
    // will get the next token from token list 
    void advance_current_token_index();
    // will check if the current token is a number and return a number node 
    FactorNode *factor();
    TermNode *term();
    ExpressionNode *expression();
    ExpressionNode *arithmetic_expression();
    void calculate_expression(ExpressionNode *node, int tabs, bool has_a_right_initial_tree);
    void give_terms(TermNode *node, int tabs);
    void run_parser(std::string current_line);
};


int main(int argc, char **argv)
{
    std::unordered_map<std::string, Token> symbolTable;
    std::string input_file_name = argv[1];

    std::ifstream input(input_file_name);

    bool whitespace_if = true;
    bool whitespace_else = true;
    bool whitespace_done = true;

    for (std::string line; getline(input, line);)
    {
        // skip all empty and commented out lines 
        if (line == "" || line[0] == '#')
        {
            continue;
        }
        
        if (whitespace_if == false && whitespace_done == true)
        {
            if (line[0] == ' ')
                continue;
        }

        else if (whitespace_else == false && whitespace_done == true)
        {
            if (line[0] == ' ')
                continue;
        }

        if (line[line.length() - 1] == '\0')
        {
            line.pop_back();
        }
        
        std::vector<Token> tokenVector;
        Lexer lexer;
        lexer.tokenVector = &tokenVector;
        lexer.run_lexer(line);

        Parser parser;
        parser.tokenVector = &tokenVector;
        parser.symbolTable = &symbolTable;
        parser.whitespace_if = &whitespace_if;
        parser.whitespace_else = &whitespace_else;
        parser.whitespace_done = &whitespace_done;
        parser.current_token_index = -1;

        parser.advance_current_token_index();
        parser.run_parser(line);
        whitespace_if = true;
    }
    input.close();

    return 0;
};


Token::Token()
{
    type = "nothing";
};

Lexer::Lexer(){};

void Lexer::run_lexer(std::string line)
{
    // make tokens from the given line 
    make_tokens(line);
};

void Lexer::make_tokens(std::string instruction)
{
    for (int i = 0; i < instruction.length(); i++)
    {

        if (std::isdigit(instruction[i]))
        {
            std::string subStr = "";
            // form an integer 
            while (true)
            {
                subStr += instruction[i];

                //if next character isnt a number then break 
                if (!std::isdigit(instruction[i + 1]))
                    break;

                i++;
            }
            Token token;
            token.type = INT;
            token.value = subStr;
            tokenVector->push_back(token);
        }

        // check if the current char is a quotation  
        else if (instruction[i] == '\"')
        {
            std::string subStr = "";
            i++;
            while (instruction[i] != '\"')
            {
                subStr += instruction[i];
                i++;
            }
            Token token;
            token.type = STRING;
            token.value = subStr;
            tokenVector->push_back(token);
        }

        // check if its  identifier or keyword 
        else if (std::isalpha(instruction[i]))
        {
            std::string subStr = "";

            // keep looping until next space is found 
            while (true)
            {
                subStr += instruction[i];

                // break if the next character is not part of  the subStr 
                if (instruction[i + 1] == '=' ||instruction[i + 1] == '\0' ||instruction[i + 1] == '(' ||instruction[i + 1] == ')' ||instruction[i + 1] == '[' ||instruction[i + 1] == ',' ||instruction[i + 1] == ']' ||instruction[i + 1] == '-' ||instruction[i + 1] == '+' ||instruction[i + 1] == '/' ||instruction[i + 1] == '*' ||instruction[i + 1] == '\n' ||instruction[i + 1] == '%' ||instruction[i + 1] == ':' ||instruction[i + 1] == ' ' ||instruction[i + 1] == ';' ||instruction[i + 1] == '\t' ||instruction[i + 1] == '>' |instruction[i + 1] == '!' ||instruction[i + 1] == '<' ||isspace(instruction[i + 1]) || i + 1 > instruction.length())
                    break;
                i++;
            };

            if (subStr == IF)
            {
                Token token;
                token.type = IF;
                token.value = subStr;
                tokenVector->push_back(token);
            }
            else if (subStr == ELIF)
            {
                Token token;
                token.type = ELIF;
                token.value = subStr;
                tokenVector->push_back(token);
            }
            else if (subStr == ELSE)
            {
                Token token;
                token.type = ELSE;
                token.value = subStr;
                tokenVector->push_back(token);
            }
            else if (subStr == PRINT)
            {
                Token token;
                token.type = PRINT;
                token.value = subStr;
                tokenVector->push_back(token);
            }
            
            else
            {
                Token token;
                token.type = IDENTIFIER;
                token.value = subStr;
                tokenVector->push_back(token);
            }
        }

        // check if current char is = or == sign 
        else if (instruction[i] == '=')
        {
            if (instruction[i + 1] == '=')
            {
                // == 
                i++;
                Token token;
                token.type = EQUAL_EQUAL;
                token.value = "==";
                tokenVector->push_back(token);
            }
            else
            {
                // = 
                Token token;
                token.type = ASSIGNMENT;
                token.value = "=";
                tokenVector->push_back(token);
            }
        }

        // check if current char is < or <= 
        else if (instruction[i] == '<')
        {

            if (instruction[i + 1] == '=')
            {
                // <= 
                i++; //advance token an extra time
                Token token;
                token.type = LESS_THAN_EQUAL;
                token.value = "<=";
                tokenVector->push_back(token);
            }
            else
            {
                // < 
                Token token;
                token.type = LESS_THAN;
                token.value = "<";
                tokenVector->push_back(token);
            }
        }

        // check if current char is > or >= 
        else if (instruction[i] == '>')
        {
            if (instruction[i + 1] == '=')
            {
                // >= 
                i++; //advance token an extra time
                Token token;
                token.type = GREATER_THAN_EQUALS;
                token.value = ">=";
                tokenVector->push_back(token);
            }
            else
            {
                // > 
                Token token;
                token.type = GREATER_THAN;
                token.value = ">";
                tokenVector->push_back(token);
            }
        }

        else if (instruction[i] == '+')
        {
            Token token;
            token.type = ADD;
            token.value = "+";
            tokenVector->push_back(token);
        }

        else if (instruction[i] == '-')
        {
            Token token;
            token.type = MINUS;
            token.value = "-";
            tokenVector->push_back(token);
        }

        else if (instruction[i] == '/')
        {
            Token token;
            token.type = DIVIDE;
            token.value = "/";
            tokenVector->push_back(token);
        }

        else if (instruction[i] == '*')
        {
            Token token;
            token.type = MULTIPLY;
            token.value = "*";
            tokenVector->push_back(token);

        }
        else if (instruction[i] == '[')
        {
            Token token;
            token.type = L_BRACK;
            token.value = "[";
            tokenVector->push_back(token);
        }
        else if (instruction[i] == ']')
        {
            Token token;
            token.type = R_BRACK;
            token.value = "]";
            tokenVector->push_back(token);
        }
        else if (instruction[i] == '(')
        {
            Token token;
            token.type = L_PARENTH;
            token.value = "(";
            tokenVector->push_back(token);
        }
        else if (instruction[i] == ')')
        {
            Token token;
            token.type = R_PARENTH;
            token.value = ")";
            tokenVector->push_back(token);
        }
        else if (instruction[i] == ':')
        {
            Token token;
            token.type = COLON;
            token.value = ":";
            tokenVector->push_back(token);
        }
        
        else if (instruction[i] == ',')
        {
            Token token;
            token.type = COMMA;
            token.value = ",";
            tokenVector->push_back(token);
        }

    }
};

void Lexer::print_tokenVector()
{
    for (auto it = tokenVector->begin(); it != tokenVector->end(); it++)
    {
        std::cout << it->type << "-->" << it->value;
    }
};

Parser::Parser(){};

void Parser::advance_current_token_index()
{
    current_token_index++;
    if (current_token_index < tokenVector->size())
        //get the current token 
        current_token = (*tokenVector)[current_token_index];
}

FactorNode *Parser::factor()
{
    FactorNode *factor_node = new FactorNode();
    factor_node->token = Token();

    // if its a variable, get its value 
    if (current_token.type == IDENTIFIER)
    {
        auto variable_data = symbolTable->find(current_token.value);
        if (variable_data == symbolTable->end())
        {
            // variable doesnt exist, undefined error 
            std::cout << "error, undefined variable \n";
        }

        // if variable is a list and we are trying to access its elements 
        if (variable_data->second.type == LIST && (*tokenVector)[current_token_index + 1].type == L_BRACK)
        {
            //left bracket
            advance_current_token_index();
            //std::cout << variable_data->first << "  " << variable_data->second.type << variable_data->second.value << "\n";
            std::string list_value = variable_data->second.value;

            //start index
            advance_current_token_index();

            // get the start index of where to copy 
            int start_index = stoi(current_token.value);

            //get the entire list remove the commas
            std::vector<std::string> list_to_access;
            for(int i = 0; i < list_value.length(); i++)
            {
                std::string element = "";

                while (list_value[i] != ',' && i < list_value.length())
                {
                    element += list_value[i];
                    i++;
                }
                list_to_access.push_back(element);
            }
            //stop index in list we are accessing 
            int stop_index;

            // if just retrieving element or copying multi elements 
            bool one_element_access = false;
            //either colon or right bracket
            advance_current_token_index();
            // if only one index to get element from 
            if (current_token.type == R_BRACK)
            {
                one_element_access = true;
                stop_index = -1;
            }
            
            //get that one value from the list
            if (one_element_access)
            {
                auto value_in_list = list_to_access[start_index];
                std::string value_to_save_in_token = value_in_list;

                // store an integer token 
                if (std::isdigit(value_in_list[0]))
                {

                    Token t;
                    t.type = INT;
                    t.value = value_to_save_in_token;
                    factor_node->token = t;
                }
                else
                {
                    //store a string token, not needed for purpose of assingment
                    //but this allows support for list of strings, extra credit?
                    Token t;
                    t.type = STRING;
                    t.value = value_to_save_in_token;
                    factor_node->token = t;
                }
            }

            //copy rest of the list
            if (!one_element_access)
            {
                std::string copied_list = "";
                for (int i = start_index; i < stop_index; i++)
                {
                    copied_list += list_to_access[i];
                    copied_list += ", ";
                }
                copied_list.pop_back(); //remove last comma
                Token t;
                t.type = LIST;
                t.value = copied_list;
                factor_node->token = t;
            }
        }
        else
        {
            // its just an identifier not list access 
            Token t;
            t.type = variable_data->second.type;
            t.value = variable_data->second.value;
            factor_node->token = t;
        }

        advance_current_token_index();
    }

    //if the current token is an integer or decimal make a number node for AST 
    if (current_token.type == INT )
    {
        factor_node->token = current_token;
        advance_current_token_index();
    }

    // string type 
    if (current_token.type == STRING)
    {
        factor_node->token = current_token;
        advance_current_token_index();
    }
    
    // LIST TYPE 
    if (current_token.type == L_BRACK)
    {
        advance_current_token_index();

        // store all list elements  
        std::string list_elements = "";
        while (current_token.type != R_BRACK)
        {
            if (current_token_index == tokenVector->size())
            {
                // no right bracket == syntax error 
                std::cout << "error, expected right bracket\n";
            }
            list_elements += current_token.value;
            advance_current_token_index();
        }
        Token t;
        t.type = LIST;
        t.value = list_elements;
        factor_node->token = t;
    }

    return factor_node;
}

TermNode *Parser::term()
{

    // root term node will be returned to expression  
    TermNode *term_node = new TermNode();

    //will  be a tree itself, holding value of nested factors 
    term_node->left_binary_term = new TermNode();

    // get left factor 
    term_node->left_factor = factor();
    term_node->left_binary_term->left_factor = term_node->left_factor;

    // if the token is multiply or divide, not needed for assignment, 
    //hopefully we get some extra credit?
    while (current_token.type == MULTIPLY || current_token.type == DIVIDE)
    {

        term_node->op = current_token;

        // advance to next token 
        advance_current_token_index();

        // get right factor 
        term_node->right_factor = factor();

       TermNode* node = new TermNode();
       node->left_binary_term = term_node->left_binary_term;
       node->left_factor = term_node->left_binary_term->left_factor;
       node->right_factor = term_node->right_factor;
       node->op = term_node->op;

       term_node->left_binary_term = node;
    }

    // drop first node since it duplicates 
    term_node->left_binary_term = term_node->left_binary_term->left_binary_term;

    return term_node;
}

ExpressionNode *Parser::expression()
{
    // root node
    ExpressionNode *expression_node = new ExpressionNode();

    // will hold nested expressions that are trees 
    expression_node->left_binary_expression = new ExpressionNode();

    // get left term
    expression_node->left_term = term();
    expression_node->left_binary_expression->left_term = expression_node->left_term;

    // if the token is plus or minus 
    while (current_token.type == ADD || current_token.type == MINUS)
    {

        expression_node->op = current_token;

        // advance to next token 
        advance_current_token_index();

        // get right term 
        expression_node->right_term = term();
       
        ExpressionNode* node = new ExpressionNode();
        node->left_binary_expression = expression_node->left_binary_expression;
        node->left_term = expression_node->left_binary_expression->left_term;
        node->right_term = expression_node->right_term;
        node->op = expression_node->op;

        expression_node->left_binary_expression = node;
    }
    // drop first node since it duplicates 
    expression_node->left_binary_expression = expression_node->left_binary_expression->left_binary_expression;
    return expression_node;
}

void Parser::calculate_expression(ExpressionNode *node, int tabs, bool has_a_right_initial_tree)
{

    // to store results from terms
    std::string right_result_token;
    std::string left_result_token;
    std::string right_result;
    std::string left_result;

    // if it has a right tree recurse all the way down to second to last term 
    if (has_a_right_initial_tree && node->right_term == nullptr)
    {
        return;
    }

    // if it has no right tree, loop all the way recursively to the bottom 
    else if (!has_a_right_initial_tree && node == nullptr)
    {
        return;
    }
    else
    {
        // traverse all expressions till you get to last expression of terms 
        calculate_expression(node->left_binary_expression,tabs + 1,has_a_right_initial_tree);

        // traverse left side of the expression node 
        if (node->left_term)
        {

            // traverse the left term tree 
            give_terms(node->left_term, tabs + 3);

            // get the final left result value from the top node 
            left_result = node->left_term->left_factor->token.value;

            left_result_token = node->left_term->left_factor->token.type;
        }

        // traverse right side of the expression node 
        if (node->right_term)
        {
            // traverse the right term tree 
            give_terms(node->right_term, tabs + 3);

            right_result = node->right_term->left_factor->token.value;
            right_result_token = node->right_term->left_factor->token.type;
        }

        bool is_list_concat = true;
        if (current_token.type == R_BRACK)
        {
            auto left_bracket = (*tokenVector)[current_token_index - 3].type;
            auto single_number = (*tokenVector)[current_token_index - 2].type;

            // doing additon of values not list concatenation 
            if (left_bracket == L_BRACK && single_number == INT)
            {
                is_list_concat = false;
            }
            Token t;
            t.type = LIST;
            t.value = "";
            current_token = t;
        }
        if (current_token.type == IDENTIFIER)
        {

            auto variable_data = symbolTable->find(current_token.value);
            if (variable_data == symbolTable->end())
            {   
                //if variable not present in map
                std::cout << "error, unidentified variable \n";
            }

            //set the factor value to the variable tokens value 
            Token t;
            t.type = variable_data->second.type;
            t.value = variable_data->second.value;
            current_token = t;

        }

        // if we only have a left term 
        if (node->left_term && !node->right_term)
        {

            /* make the left term a new node and store the updated value in it
                will be read by previous node above it  */
            delete node->left_term;
            node->left_term = new TermNode();
            node->left_term->left_factor = new FactorNode();

            if (current_token.type == STRING || current_token.type == LIST)
            {
                Token t;
                t.type = left_result_token;
                t.value = left_result;
                node->left_term->left_factor->token = t;
            }
            else
            {
                // integers 
                Token t;
                t.type = left_result_token;
                t.value = std::to_string(stoi(left_result));
                node->left_term->left_factor->token = t;

            }

        }

        // if we only have a right term 
        if (node->right_term && !node->left_term)
        {

            /* make the left term a new node and store the updated value in it
                will be read by previous node above it  */
            delete node->left_term;
            node->left_term = new TermNode();
            node->left_term->left_factor = new FactorNode();

            if (current_token.type == STRING ||
                current_token.type == LIST)
            {
                Token t;
                t.type = left_result_token;
                t.value = right_result;
                node->left_term->left_factor->token = t;
            }
            else
            {
                Token t;
                t.type = right_result_token;
                t.value = std::to_string(stoi(right_result));
                node->left_term->left_factor->token = t;
            }

        }

        // if we have both a left and right term need to do binary operation 
        if (node->left_term && node->right_term)
        {
            if (node->op.type == ADD)
            {

                /* make the left term a new node and store the updated value in it
                will be read by previous node above it  */
                delete node->left_term;
                node->left_term = new TermNode();
                node->left_term->left_factor = new FactorNode();

                // list element access 
                if (current_token.type == LIST && !is_list_concat)
                {
                    if (!std::isdigit(right_result[0]))
                    {
                        // string addition 
                        Token t;
                        t.type = left_result_token;
                        t.value = left_result + right_result;
                        node->left_term->left_factor->token = t;
                    }
                    else
                    {   
                        // integer addition 
                        Token t;
                        t.type = left_result_token;
                        t.value = std::to_string(stoi(left_result) + stoi(right_result));
                        node->left_term->left_factor->token = t;
                    }
                }
                else if (current_token.type == LIST && is_list_concat)
                {
                    // list concatenation 
                    Token t;
                    t.type = left_result_token;
                    t.value = left_result + "," + right_result;
                    node->left_term->left_factor->token = t;
                }
                else if (current_token.type == STRING)
                {
                    // string concatenation 
                    Token t;
                    t.type = left_result_token;
                    t.value = left_result + right_result;
                    node->left_term->left_factor->token = t;
                }
                else
                {
                    // integers addition
                    Token t;
                    t.type = left_result_token;
                    t.value = std::to_string(stoi(left_result) + stoi(right_result));
                    node->left_term->left_factor->token = t;
                }

            }

            if (node->op.type == MINUS)
            {
                /* make the left term a new node and store new value here
                will be used by parent node */
                delete node->left_term;
                node->left_term = new TermNode();
                node->left_term->left_factor = new FactorNode();

                Token t;
                t.type = left_result_token;
                t.value = std::to_string(stoi(left_result) - stoi(right_result));
                node->left_term->left_factor->token = t;
            }
        }
    }
}

void Parser::give_terms(TermNode *node, int tabs)
{
    /* issue is that sometimes the left->binary even tho its empty
    its not null its 0xf , but the last node right_factor is always null
    */
    if (node->right_factor == nullptr)
    {

        return;
    }
    else
    {
        //traverse all the way down to the bottom node 
        give_terms(node->left_binary_term, tabs + 1);

        // perform mathematical operation on node 
        if (node->left_factor && node->right_factor)
        {
            // calculate result of both operands 
            if (node->left_factor->token.type == INT &&
                node->right_factor->token.type == INT)
            {

                if (node->op.type == MULTIPLY)
                {
                    auto result =
                        std::stoi(node->left_factor->token.value) *
                        std::stoi(node->right_factor->token.value);

                    /* store result in left factor of current node
                       next node will  read from this token and use it
                    */
                    Token t;
                    t.type = node->left_factor->token.type;
                    t.value = std::to_string(result);
                    node->left_factor->token = t;
                }

                if (node->op.type == DIVIDE)
                {
                    auto result =
                        std::stoi(node->left_factor->token.value) /
                        std::stoi(node->right_factor->token.value);

                    /* store result in left factor of current node
                       next node will  read from this token and use it
                    */
                   Token t;
                   t.type = node->left_factor->token.type;
                   t.value = std::to_string(result);
                   node->left_factor->token = t;
                }
            }
        }
    }
}

void Parser::run_parser(std::string current_line)
{

    //print the value to screen 
    if (current_token.type == PRINT)
    {
        //get left paren 
        advance_current_token_index();

        // get the current value to print 
        advance_current_token_index();

       // print variable name
        if ((*tokenVector)[current_token_index + 1].type ==R_PARENTH && current_token.type == IDENTIFIER)
        {
            // printing a variable 
            auto variable_data = symbolTable->find(current_token.value);
            if (variable_data == symbolTable->end())
            {
                std::cout << "error, undefined variable \n";
            }

            //printing a list 
            if (variable_data->second.type == LIST)
            {   
                std::cout << "[";
                for(int i = 0; i < variable_data->second.value.size(); i++)
                {
                    if(variable_data->second.value[i-1] == ',')
                    {
                        std::cout << " ";
                    }
                    std::cout << variable_data->second.value[i];
                }
                std::cout << "]";
            }
            else
            {
                //std::cout << variable_data->first << std::endl;
                //std::cout << variable_data->second.type << std::endl;
                std::cout << variable_data->second.value << std::endl;
            }
        }
         // print string literal
        else if ((*tokenVector)[current_token_index + 1].type == R_PARENTH && current_token.type == STRING)
        {
            std::cout << current_token.value << std::endl;
        }
    }


    // if its an identifier, get the arithmetic expression value  
    else if (current_token.type == IDENTIFIER)
    {
        // get variable name 
        auto variable_name = current_token.value;
        advance_current_token_index();

        // get equals sign 
        advance_current_token_index();        
        
        // look for new expression 
        auto variable_value = expression();
        // sometimes its null sometimes isnt, this makes it certain 
        if (variable_value->right_term == nullptr)
        {
            variable_value->left_binary_expression = nullptr;
        }

        calculate_expression(variable_value, 1, (variable_value->right_term != nullptr));

        //if value exists in map, then delete it 
        auto it = symbolTable->find(variable_name);
        if (it != symbolTable->end())
        {
            symbolTable->erase(variable_name);
        }

        //insert in variable map 
        Token t;
        t.type = variable_value->left_term->left_factor->token.type;
        t.value = variable_value->left_term->left_factor->token.value;

        std::pair<std::string, Token> name_and_token;
        name_and_token.first = variable_name;
        name_and_token.second = t;
        //std::cout << name_and_token.first << "  " << name_and_token.second.type << "  " <<  name_and_token.second.value << std::endl;
        symbolTable->insert(name_and_token);
    }

    // if statement 
    else if (current_token.type == IF)
    {

        std::vector<Token> *left_list = new std::vector<Token>();
        std::vector<Token> *right_list = new std::vector<Token>();

        int idx_comp_op = 0; //will be index of comparison operator

        //get index of comparison operator 
        for (auto it = tokenVector->begin();
             it != tokenVector->end();
             ++it)
        {
            idx_comp_op++;
            if (it->type == NOT_EQUAL ||it->type == EQUAL_EQUAL ||it->type == LESS_THAN ||it->type == LESS_THAN_EQUAL ||it->type == GREATER_THAN ||it->type == GREATER_THAN_EQUALS)
            {
                break;
            }
        }

        // copy over the expressions left and right of comparison operator 
        idx_comp_op -= 1;
        for (auto it = tokenVector->begin() + 1;
             it != tokenVector->begin() + idx_comp_op; ++it)
        {
            left_list->push_back(*it);
        }
        auto comparison_token = (*tokenVector)[idx_comp_op];
        for (auto it = tokenVector->begin() + idx_comp_op + 1;
             it != tokenVector->end() - 1; ++it)
        {
            right_list->push_back(*it);
        }

        // evaluate left half of the tokens list 
        tokenVector = left_list;
        current_token_index = -1;
        advance_current_token_index();
        auto left_comp_expr = expression();

        calculate_expression(left_comp_expr,1,(left_comp_expr->right_term != nullptr));

        // evaluate right half of the tokens list 
        tokenVector = right_list;
        current_token_index = -1;
        advance_current_token_index();
        auto right_comp_expr = expression();

        calculate_expression(right_comp_expr,1,(right_comp_expr->right_term != nullptr));
        // calculate result 
        auto left_value = left_comp_expr->left_term->left_factor->token.value;
        auto left_type =left_comp_expr->left_term->left_factor->token.type;
        auto right_value =right_comp_expr->left_term->left_factor->token.value;
        auto right_type =right_comp_expr->left_term->left_factor->token.value;

        // make comparison check 
        bool compare = false;
        if (comparison_token.type == EQUAL_EQUAL)
        {
            if (left_type == INT)
                compare = stoi(left_value) == stoi(right_value);
            else
                compare = left_value == right_value;
        }
        else if (comparison_token.type == LESS_THAN)
        {
            if (left_type == INT)
                compare = stoi(left_value) < stoi(right_value);
            else
                compare = left_value < right_value;
        }
        else if (comparison_token.type == LESS_THAN_EQUAL)
        {
            if (left_type == INT)
                compare = stoi(left_value) <= stoi(right_value);
            else
                compare = left_value <= right_value;
        }
        else if (comparison_token.type == GREATER_THAN)
        {
            if (left_type == INT)
                compare = stoi(left_value) > stoi(right_value);
            else
                compare = left_value > right_value;
        }
        else if (comparison_token.type == GREATER_THAN_EQUALS)
        {
            if (left_type == INT)
                compare = stoi(left_value) >= stoi(right_value);
            else
                compare = left_value >= right_value;
        }

        if (compare)
        {
            *whitespace_else = false;
            *whitespace_done = false;
        }
        else
        {
            *whitespace_else = true;
            *whitespace_done = true;
        }
    }

    // else statement 
    else if (current_token.type == ELSE)
    {
        // if a previous if statement passed, no need to check else block 
        if (*whitespace_else == false)
        {
            // if token is else statement and we did if block mark it complete 
            *whitespace_done = true;
            return;
        }
        else
        {
            *whitespace_done = false;
        }
    }
}
