#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>



#define STRING "STRING"
#define INT "INT"
#define LIST "LIST"

#define ADD "PLUS"
#define MINUS "MINUS"
#define ASSIGNMENT "EQUAL"
#define MULTIPLY "MULTIPLY"
#define DIVIDE "DIVIDE"

#define COLON "COLON"
#define COMMA "COMMA"
#define SPACE "SPACE"
#define LEFT_PAR "LEFT_PAREN"
#define RIGHT_PAR "RIGHT_PAREN"
#define LEFT_BRACK "LEFT_BRACKET"
#define RIGHT_BRACK "RIGHT_BRACKET"

#define VARIABLE "VARIABLE"

#define IF "if"
#define ELIF "elif"
#define ELSE "else"
#define PRINT "print"


#define AND "AND"
#define NOT "NOT"
#define OR "OR"

#define NOT_EQUAL "NOT_EQUAL"
#define COMP_EQUAL "EQUAL_EQUAL"
#define LESS_THAN "LESS_THAN"
#define LESS_THAN_EQUAL "LESS_THAN_OR_EQUAL"
#define GREATER_THAN "GREATER_THAN"
#define GREATER_THAN_EQUALS "GREATER_THAN_OR_EQUAL"


using namespace std;
class Token
{
public:
    std::string type;
    std::string value;

    Token(std::string type, std::string value);
    Token(std::string type);
    Token();
    void print_token();
    std::string get_token_type();
    std::string get_token_value();
};






class Lexer
{
public:
    std::vector<Token>* tokenVector;
    std::unordered_map<std::string, Token>* symbol_table;


    Lexer(std::vector<Token> *tk_list,
          std::unordered_map<std::string, Token> *sym_tab);

    //reads input file, gives all tokens
    void run_lexer(std::string line);

    // helper, given a string, creates a token
    void make_tokens(std::string line_to_lex);

    /* will print tokens list */
    //void print_tokenVector();

    /* will return a reference to the current tokenVector */
    std::vector<Token> get_tokenVector();

    /* will return a reference to the symbol_table */
    std::unordered_map<std::string, Token> *get_symbol_table();

    void print_symbol_table();
};


class FactorNode
{
public:
    Token token;
    FactorNode(Token t) { token = t; }
    FactorNode() {}
};

class TermNode
{
public:
    /* if only a left factor */
    FactorNode *left_factor;

    /* if both a left and right factor */
    FactorNode *right_factor;

    /* 
    used for nesting, either a term will be a factor node
    or a binary node indicating two number operation
    */
    TermNode *left_binary_term;

    Token op;

    /* for single operator */
    TermNode(FactorNode *lf,
             FactorNode *rf,
             Token opx)
    {
        left_factor = lf;
        right_factor = rf;
        op = opx;
    }

    /* for double operator */
    TermNode(
        TermNode *lbt,
        FactorNode *lf,
        FactorNode *rf,
        Token opx)
    {
        left_binary_term = lbt;
        left_factor = lf;
        right_factor = rf;
        op = opx;
    }

    TermNode()
    {
        // left_binary_term = new TermNode();
        left_factor = nullptr;
        right_factor = nullptr;
        op = Token();
    };
};

class ExpressionNode
{
public:
    /* if only a left term */
    TermNode *left_term;

    /* 
    used for nesting, either a expression will be a term node
    or a binary node indicating multi term operation
    */

    /* if both a left and right term */
    TermNode *right_term;
    ExpressionNode *left_binary_expression;

    Token op;

    /* for single operator */
    ExpressionNode(TermNode *lt, TermNode *rt, Token opx)
    {
        left_term = lt;
        right_term = rt;
        op = opx;
    }

    /* for double operator */
    ExpressionNode(ExpressionNode *lbe, TermNode *lt, TermNode *rt, Token opx)
    {
        left_binary_expression = lbe;
        left_term = lt;
        right_term = rt;
        op = opx;
    }

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
    std::unordered_map<std::string, Token> *symbol_table;

    bool *take_branch;
    bool *take_branch_else;
    bool *is_if_done;

    /* will intialize the parsers token list to the 
       address of the lexer generatedtoken list */
    Parser();
    Parser(std::vector<Token> *lexer_tokenVector,
           std::unordered_map<std::string, Token> *lexer_symbol_table,
           bool *take_branch,
           bool *take_branch_else, bool *take_branch_elif);

    /* will get the current token from token list */
    Token get_current_token();

    /* will get the current token from token list */
    void advance_current_token_index();

    /* will check if the current token is a number and return a number node */
    FactorNode *factor();

    /*  */
    TermNode *term();

    /*  */
    ExpressionNode *expression();
    ExpressionNode *arithmetic_expression();

    void interpret_expression_in_order(ExpressionNode *node, int tabs,
                                       bool has_a_right_initial_tree);
    void interpret_term_in_order(TermNode *node, int tabs);

    void run_parser(std::string current_line);

};











int main(int argc, char **argv)
{

    std::unordered_map<std::string, Token> symbol_table;
    std::string input_file_name = argv[1];

    std::ifstream input(input_file_name);

    bool take_branch = true;
    bool take_branch_else = true;
    bool is_if_done = true;

    for (std::string line; getline(input, line);)
    {
        //skip comments
        if (line == "" || line[0] == '#')
        {
            continue;
        }
        //soon as you hit end character, start a new line
        if (line[line.length() - 1] == '\0')
        {
            line.pop_back();
        }
        
        if (take_branch == false && is_if_done == true)
        {
            if (line[0] == ' ')
                continue;
        }

        else if (take_branch_else == false &&
                 is_if_done == true)
        {
            if (line[0] == ' ')
                continue;
        }

        std::vector<Token> tokenVector;

        //Lexer lexer1;
        //lexer1.tokenVector = &tokenVector;

        Lexer lexer = Lexer(
            &tokenVector,
            &symbol_table);

        lexer.run_lexer(line);
        /*
        Parser parser1;
        parser1.tokenVector = &tokenVector;
        parser1.symbol_table = &symbol_table;
        parser1.take_branch = &take_branch;
        parser1.take_branch_else = &take_branch_else;
        parser1.is_if_done = &is_if_done;

        parser1.run_parser(line);
        */

        Parser parser = Parser(&tokenVector,&symbol_table,&take_branch,&take_branch_else,&is_if_done);
        parser.run_parser(line);


        take_branch = true;

       
    }
    input.close();

    return 0;
};



 //constructors 
Token::Token(std::string token_type, std::string token_variable_value)
{
    type = token_type;
    value = token_variable_value;
};


Token::Token(std::string token_type)
{
    type = token_type;
};
Token::Token()
{
    type = "N/A";
};

void Token::print_token()
{

    std::cout << "\t token -> "
              << "(" << this->type << ","
              << this->value << ")\n"
              << std::endl;
};

std::string Token::get_token_type()
{
    return type;
};
std::string Token::get_token_value()
{
    return value;
};





Lexer::Lexer(std::vector<Token> *tk_list,
             std::unordered_map<std::string, Token> *sym_tab)
{
    tokenVector = tk_list;
    symbol_table = sym_tab;
};

void Lexer::run_lexer(std::string line)
{
    // make tokens from the given line 
    make_tokens(line);
};

void Lexer::make_tokens(std::string line_to_lex)
{
    Token* token = new Token;
    for (int i = 0; i < line_to_lex.length(); i++)
    {

        if (std::isdigit(line_to_lex[i]))
        {
            std::string curr_string = "";
            /* form an integer */
            while (true)
            {
                curr_string += line_to_lex[i];

                /* if next character isnt a number then break */
                if (!std::isdigit(line_to_lex[i + 1]))
                    break;

                i++;
            }
            token->type = INT;
            token->value = curr_string;
            tokenVector->push_back(*token);
            //tokenVector->push_back(Token(INT, curr_string));
        }

        /* check if the current char is a quotation  */
        else if (line_to_lex[i] == '\"')
        {
            std::string curr_string = "";
            i++;
            while (line_to_lex[i] != '\"')
            {
                curr_string += line_to_lex[i];
                i++;
            }
            token->type = STRING;
            token->value = curr_string;
            tokenVector->push_back(*token);
            //tokenVector->push_back(Token(STRING, curr_string));
        }

        /* check if its  VARIABLE or keyword */

        else if (std::isalpha(line_to_lex[i]))
        {
            std::string curr_string = "";

            /* keep looping until next space is found */
            while (true)
            {
                curr_string += line_to_lex[i];

                /* break if the next character is not part of  the curr_string */
                if (
                    line_to_lex[i + 1] == '=' ||
                    line_to_lex[i + 1] == '\0' ||
                    line_to_lex[i + 1] == '(' ||
                    line_to_lex[i + 1] == ')' ||
                    line_to_lex[i + 1] == '[' ||
                    line_to_lex[i + 1] == ',' ||
                    line_to_lex[i + 1] == ']' ||
                    line_to_lex[i + 1] == '-' ||
                    line_to_lex[i + 1] == '+' ||
                    line_to_lex[i + 1] == '/' ||
                    line_to_lex[i + 1] == '*' ||
                    line_to_lex[i + 1] == '\n' ||
                    line_to_lex[i + 1] == '%' ||
                    line_to_lex[i + 1] == ':' ||
                    line_to_lex[i + 1] == ' ' ||
                    line_to_lex[i + 1] == ';' ||
                    line_to_lex[i + 1] == '\t' ||
                    line_to_lex[i + 1] == '>' ||
                    line_to_lex[i + 1] == '!' ||
                    line_to_lex[i + 1] == '<' ||
                    isspace(line_to_lex[i + 1]) ||
                    i + 1 > line_to_lex.length())
                    break;

                i++;
            };

            if (curr_string == IF)
            {   
                token->type = IF;
                token->value = curr_string;
                tokenVector->push_back(*token);
            }
            else if (curr_string == ELIF)
            {
                token->type = ELIF;
                token->value = curr_string;
                tokenVector->push_back(*token);
            }
            else if (curr_string == ELSE)
            {
                token->type = ELSE;
                token->value = curr_string;
                tokenVector->push_back(*token);
            }
            else if (curr_string == PRINT)
            {
                token->type = PRINT;
                token->value = curr_string;
                tokenVector->push_back(*token);
            }
            else if (curr_string == AND)
            {
                token->type = AND;
                token->value = curr_string;
                tokenVector->push_back(*token);
            }
            else if (curr_string == OR)
            {
                token->type = OR;
                token->value = curr_string;
                tokenVector->push_back(*token);
            }
            else if (curr_string == NOT)
            {
                token->type = NOT;
                token->value = curr_string;
                tokenVector->push_back(*token);
            }
            else
            {
                token->type = VARIABLE;
                token->value = curr_string;
                tokenVector->push_back(*token);
            }
        }

        /* check if current char is = or == sign */
        else if (line_to_lex[i] == '=')
        {
            if (line_to_lex[i + 1] == '=')
            {
                i++;
                token->type = COMP_EQUAL;
                token->value = "==";
                tokenVector->push_back(*token);
                //tokenVector->push_back(Token(COMP_EQUAL,"=="));
            }
            else
            {   
                token->type = ASSIGNMENT;
                token->value = '=';
                tokenVector->push_back(*token);
                //tokenVector->push_back(Token(ASSIGNMENT,std::string(1, '=')));
            }
        }

        /* check if current char is != */
        else if (line_to_lex[i] == '!')
        {
            /* if != */
            if (line_to_lex[i + 1] == '=')
            {
                i++; //advance token an extra time
                token->type = NOT_EQUAL;
                token->value = "!=";
                tokenVector->push_back(*token);
                //tokenVector->push_back(Token(NOT_EQUAL, "!="));
            }
        }

        /* check if current char is < or <= */
        else if (line_to_lex[i] == '<')
        {

            if (line_to_lex[i + 1] == '=')
            {
                /* <= */
                i++; //advance token an extra time
                token->type = LESS_THAN_EQUAL;
                token->value = "<=";
                tokenVector->push_back(*token);
                //tokenVector->push_back(Token(LESS_THAN_EQUAL,
                //                             "<="));
            }
            else
            {
                /* < */
                token->type = LESS_THAN;
                token->value = '<';
                tokenVector->push_back(*token);
                //tokenVector->push_back(Token(LESS_THAN,
                //                             std::string(1, '<')));
            }
        }

        /* check if current char is > or >= */
        else if (line_to_lex[i] == '>')
        {
            if (line_to_lex[i + 1] == '=')
            {
                /* >= */
                i++; //advance token an extra time
                token->type = GREATER_THAN_EQUALS;
                token->value = ">=";
                tokenVector->push_back(*token);

                //tokenVector->push_back(Token(GREATER_THAN_EQUALS,
                //                             ">="));
            }
            else
            {
                /* > */
                token->type = GREATER_THAN;
                token->value = ">";
                tokenVector->push_back(*token);

                //tokenVector->push_back(Token(GREATER_THAN,
                //                             std::string(1, '>')));
            }
        }

        /* check if current char is plus sign */
        else if (line_to_lex[i] == '+')
        {
            token->type = ADD;
            token->value = "+";
            //token->value = line_to_lex[i];
            tokenVector->push_back(*token);
        }

        /* check if current char is plus sign */
        else if (line_to_lex[i] == '-')
        {
            token->type = MINUS;
            token->value = "-";
            //token->value = line_to_lex[i];
            tokenVector->push_back(*token);
        }

        /* check if current char is divide sign */
        else if (line_to_lex[i] == '/')
        {
            token->type = DIVIDE;
            //token->value = line_to_lex[i];
            token->value = "/";
            tokenVector->push_back(*token);
        }

        /* check if current char is plus sign */
        else if (line_to_lex[i] == '*')
        {
            token->type = MULTIPLY;
            token->value = "*";
            //token->value = line_to_lex[i];
            tokenVector->push_back(*token);
        }
        else if (line_to_lex[i] == '[')
        {
            token->type = LEFT_BRACK;
            token->value = "[";
            //token->value = line_to_lex[i];
            tokenVector->push_back(*token);
        }
        else if (line_to_lex[i] == ']')
        {
            token->type = RIGHT_BRACK;
            //token->value = line_to_lex[i];
            token->value = "]";
            tokenVector->push_back(*token);
        }
        else if (line_to_lex[i] == '(')
        {
            token->type = LEFT_PAR;
            token->value = "(";
            //token->value = line_to_lex[i];
            tokenVector->push_back(*token);
        }
        else if (line_to_lex[i] == ')')
        {
            token->type = RIGHT_PAR;
            //token->value = line_to_lex[i];
            token->value = ")";
            tokenVector->push_back(*token);
        }
        else if (line_to_lex[i] == ':')
        {
            token->type = COLON;
            token->value = ":";
            tokenVector->push_back(*token);
        }
        
        else if (line_to_lex[i] == ',')
        {
            token->type = COMMA;
            token->type = ",";
            //token->value = line_to_lex[i];
            tokenVector->push_back(*token);
        }

        else
        {
        }
    }
};



Parser::Parser(){};

Parser::Parser(std::vector<Token> *lexer_tokenVector,
               std::unordered_map<std::string, Token> *lexer_symbol_table,
               bool *is_condition_t,
               bool *take_branch_else,
               bool *take_branch_elif)

{
    current_token_index = -1;
    tokenVector = lexer_tokenVector;
    symbol_table = lexer_symbol_table;

    take_branch = is_condition_t;
    take_branch_else = take_branch_else;
    is_if_done = take_branch_elif;
    advance_current_token_index();
}


void Parser::advance_current_token_index()
{
    current_token_index++;
    if (current_token_index < tokenVector->size())
        /* get the current token */
        current_token = (*tokenVector)[current_token_index];
}


FactorNode *Parser::factor()
{
    FactorNode *factor_node = new FactorNode();
    factor_node->token = Token();

    /* if its a variable, get its value */
    if (current_token.get_token_type() == VARIABLE)
    {
        auto variable_data = symbol_table->find(current_token.get_token_value());
        if (variable_data == symbol_table->end())
        {
            /* variable doesnt exist, undefined error */
            std::cout << "there was an error";
        }

        /* if variable is a list and we are trying to access its elements */
        if (variable_data->second.get_token_type() == LIST &&
            (*tokenVector)[current_token_index + 1].get_token_type() ==
                LEFT_BRACK)
        {
            //left bracket
            advance_current_token_index();

            std::string list_value = variable_data->second.get_token_value();

            //start index
            advance_current_token_index();

            /* get the start index of where to copy */
            int start_index = stoi(current_token.get_token_value());

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

            /* stop index in list we are accessing */
            int stop_index;

            /* if just retrieving element or copying multi elements */
            bool one_element_access = false;

            //either colon or right bracket
            advance_current_token_index();

            /* if only one index to get element from */
            if (current_token.get_token_type() == RIGHT_BRACK)
            {
                one_element_access = true;
                stop_index = -1;
            }
            /* if slicing into list */
            else if (current_token.get_token_type() == COLON)
            {

                /* check if token after colon is an integer */
                advance_current_token_index();

                /* get the number after the colon as the stop index  */
                if (current_token.get_token_type() == INT)
                {
                    //dont wanna copy last element but everything up to it
                    stop_index = stoi(current_token.get_token_value());
                }
                else
                {
                    /* if token after the colon is a bracket
                        we are copying entire remainder of list */

                    /* throw syntax error no right bracket */
                    if (current_token.get_token_type() != RIGHT_BRACK)
                    {
                        std::cout << "there was an error";
                    }
                    //get everything from the remainder of the list
                    stop_index = list_to_access.size();
                }
            }

            //get that one value from the list
            if (one_element_access)
            {
                /* invalid list out of bound */
                if (start_index > (int)list_to_access.size() - 1)
                {
                    factor_node->token = Token(STRING, "error");
                }
                else
                {
                    auto value_in_list = list_to_access[start_index];

                    std::string value_to_save_in_token = value_in_list;

                    /* store an integer token */
                    if (std::isdigit(value_in_list[0]))
                    {
                        factor_node->token = Token(INT, value_to_save_in_token);
                    }
                    else
                    {
                        /* store a string token */
                        factor_node->token = Token(STRING, value_to_save_in_token);
                    }
                }
            }

            //copy rest of the list
            if (!one_element_access)
            {
                std::string copied_list = "";
                for (int i = start_index; i < stop_index; i++)
                {
                    copied_list += list_to_access[i];
                    copied_list += ",";
                }
                copied_list.pop_back(); //remove last comma
                factor_node->token = Token(LIST, copied_list);
            }
        }
        else
        {
            /* its just an VARIABLE not list access */
            factor_node->token =
                Token(variable_data->second.get_token_type(),
                      variable_data->second.get_token_value());
        }

        advance_current_token_index();
    }

    /* if the current token is an integer or decimal make a number node for AST */
    if (current_token.get_token_type() == INT )
    {
        factor_node->token = current_token;
        advance_current_token_index();
    }

    /* string type */
    if (current_token.get_token_type() == STRING)
    {
        factor_node->token = current_token;
        advance_current_token_index();
    }

    /* if the current token is a LEFT_PAREN or RIGHT_PARENT */
    if (current_token.get_token_type() == LEFT_PAR)
    {
        advance_current_token_index();

        /* solve inner expression and save result value */
        ExpressionNode *expr = expression();
        interpret_expression_in_order(expr, 1, (expr->right_term != nullptr));

        /* if we get right paren, inner expression is done */
        if (current_token.get_token_type() == RIGHT_PAR)
        {
            /* advance past the inner expression right paren */
            advance_current_token_index();
            /* return solved inner expression token  */
            factor_node->token = expr->left_term->left_factor->token;
        }
        else
        {
            /* throw syntax error if we dont got a right paren */
            std::cout << "there was an error\n";        
        }
    }

    /* LIST TYPE */
    if (current_token.get_token_type() == LEFT_BRACK)
    {
        advance_current_token_index();

        /* store all list elements  */
        std::string list_elements = "";
        while (current_token.get_token_type() !=
               RIGHT_BRACK)
        {
            if (current_token_index == tokenVector->size())
            {
                /* no right bracket == syntax error */
                std::cout << "there was an error";
            }

            list_elements += current_token.get_token_value();
            advance_current_token_index();
        }

        factor_node->token = Token(LIST,
                                   list_elements);
    }

    return factor_node;
}

/**
 * 
 * 
 * 
 * 
 */

TermNode *Parser::term()
{

    /* root term node will be returned to expression  */
    TermNode *term_node = new TermNode();

    /* will  be a tree itself, holding value of nested factors */
    term_node->left_binary_term = new TermNode();

    /* get left factor */
    term_node->left_factor = factor();
    term_node->left_binary_term->left_factor = term_node->left_factor;

    /* if the token is multiply or divide */
    while (current_token.get_token_type() == MULTIPLY ||
           current_token.get_token_type() == DIVIDE)
    {

        term_node->op = current_token;

        /* advance to next token */
        advance_current_token_index();

        /* get right factor */
        term_node->right_factor = factor();

        /* re assign the left node of term node to be a binary factor node */
        term_node->left_binary_term = new TermNode(
            term_node->left_binary_term, //make this new node leftbinterm previous leftbinterm
            term_node->left_binary_term->left_factor,
            term_node->right_factor,
            term_node->op

        );
    }

    /* drop first node since it duplicates */
    term_node->left_binary_term = term_node->left_binary_term->left_binary_term;

    return term_node;
}


ExpressionNode *Parser::expression()
{
    /* root expression node aka head of the tree */
    ExpressionNode *expression_node = new ExpressionNode();

    /* will hold nested expressions that are trees  */
    expression_node->left_binary_expression = new ExpressionNode();

    /* get left term */
    expression_node->left_term = term();
    expression_node->left_binary_expression->left_term = expression_node->left_term;

    /* if the token is plus or minus */
    while (
        current_token.get_token_type() == ADD ||
        current_token.get_token_type() == MINUS)
    {

        expression_node->op = current_token;

        /* advance to next token */
        advance_current_token_index();

        /* get right term */
        expression_node->right_term = term();

        /* re assign the left node of expression node to be a binary term node */
        expression_node->left_binary_expression = new ExpressionNode(
            expression_node->left_binary_expression,
            expression_node->left_binary_expression->left_term,
            expression_node->right_term,
            expression_node->op

        );
    }

    /* drop first node since it duplicates */
    expression_node->left_binary_expression =
        expression_node->left_binary_expression->left_binary_expression;

    return expression_node;
}

/**
 * 
 * 
 * 
 * 
 */

void Parser::interpret_expression_in_order(ExpressionNode *node,
                                           int tabs,
                                           bool has_a_right_initial_tree)
{

    /* to store results from terms, will be used in previous recursive expressions */
    std::string right_result_token;
    std::string left_result_token;
    std::string right_result;
    std::string left_result;

    /* if it has a right tree recurse all the way down to second to last term */
    if (has_a_right_initial_tree && node->right_term == nullptr)
    {
        return;
    }

    /* if it has no right tree, loop all the way recursively to the bottom */
    else if (!has_a_right_initial_tree && node == nullptr)
    {
        return;
    }
    else
    {
        /* traverse all expressions till you get to last expression of terms */
        interpret_expression_in_order(node->left_binary_expression,
                                      tabs + 1,
                                      has_a_right_initial_tree);

        /* traverse left side of the expression node */
        if (node->left_term)
        {

            /* traverse the left term tree */
            interpret_term_in_order(node->left_term, tabs + 3);

            /* get the final left result value from the top node */
            left_result =
                node->left_term->left_factor->token.get_token_value();

            left_result_token =
                node->left_term->left_factor->token.get_token_type();
        }

        /* traverse right side of the expression node */
        if (node->right_term)
        {
            /* traverse the right term tree */
            interpret_term_in_order(node->right_term, tabs + 3);

            right_result = node->right_term->left_factor->token.get_token_value();
            right_result_token = node->right_term->left_factor->token.get_token_type();
        }

        /* remove last token in list when the advancing token doesnt remove it */

        bool is_list_concat = true;
        if (current_token.get_token_type() == RIGHT_BRACK)
        {
            auto left_bracket = (*tokenVector)[current_token_index - 3].get_token_type();
            auto single_number = (*tokenVector)[current_token_index - 2].get_token_type();

            /* doing additon of values not list concatenation */
            if (left_bracket == LEFT_BRACK && single_number == INT)
            {
                is_list_concat = false;
            }
            current_token = Token(LIST, "");
        }
        if (current_token.get_token_type() == VARIABLE)
        {

            auto variable_data = symbol_table->find(current_token.get_token_value());
            if (variable_data == symbol_table->end())
            {
                std::cout << "there waw an error";
            }

            /* set the factor value to the variable tokens value */
            current_token =
                Token(variable_data->second.get_token_type(),
                      variable_data->second.get_token_value());
        }

        /* if we only have a left term */
        if (node->left_term && !node->right_term)
        {

            /* make the left term a new node and store the updated value in it
                will be read by previous node above it  */
            delete node->left_term;
            node->left_term = new TermNode();
            node->left_term->left_factor = new FactorNode();

            if (current_token.get_token_type() == STRING ||
                current_token.get_token_type() == LIST)
            {
                node->left_term->left_factor->token =
                    Token(left_result_token, left_result);
            }
            else
            {
                /* integers */
                node->left_term->left_factor->token =
                    Token(left_result_token,
                          std::to_string(stoi(left_result)));
            }

        }

        /* if we only have a right term */
        if (node->right_term && !node->left_term)
        {

            /* make the left term a new node and store the updated value in it
                will be read by previous node above it  */
            delete node->left_term;
            node->left_term = new TermNode();
            node->left_term->left_factor = new FactorNode();

            if (current_token.get_token_type() == STRING ||
                current_token.get_token_type() == LIST)
            {
                node->left_term->left_factor->token =
                    Token(left_result_token, right_result);
            }
            else
            {
                node->left_term->left_factor->token =
                    Token(right_result_token,
                          std::to_string(stoi(right_result)));
            }

        }

        /* if we have both a left and right term need to do binary operation */
        if (node->left_term && node->right_term)
        {
            if (node->op.get_token_type() == ADD)
            {

                /* make the left term a new node and store the updated value in it
                will be read by previous node above it  */
                delete node->left_term;
                node->left_term = new TermNode();
                node->left_term->left_factor = new FactorNode();

                /* list element access */
                if (current_token.get_token_type() == LIST &&
                    !is_list_concat)
                {
                    if (!std::isdigit(right_result[0]))
                    {
                        /* string addition */
                        node->left_term->left_factor->token =
                            Token(left_result_token, left_result + right_result);
                    }
                    else
                    {
                        /* integer addition */
                        node->left_term->left_factor->token =
                            Token(left_result_token,
                                  std::to_string(stoi(left_result) +
                                                 stoi(right_result)));
                    }
                }
                else if (current_token.get_token_type() == LIST &&
                         is_list_concat)
                {
                    /* list concatenation */
                    node->left_term->left_factor->token =
                        Token(left_result_token,
                              left_result + "," + right_result);
                }
                else if (current_token.get_token_type() == STRING)
                {
                    /* string concatenation */
                    node->left_term->left_factor->token =
                        Token(left_result_token, left_result + right_result);
                }
                else
                {
                    /* integers addition */
                    node->left_term->left_factor->token =
                        Token(left_result_token,
                              std::to_string(stoi(left_result) +
                                             stoi(right_result)));
                }

            }

            if (node->op.get_token_type() == MINUS)
            {

                /* make the left term a new node and store the updated value in it
                will be read by previous node above it  */
                delete node->left_term;
                node->left_term = new TermNode();
                node->left_term->left_factor = new FactorNode();

                if (current_token.get_token_type() == STRING ||
                    current_token.get_token_type() == LIST)
                {
                    // std::cout << "!!!!!! L - R  TOTAL = "
                    //           << left_result - right_result << std::endl
                    //           << std::endl;
                }
                else
                {

                    node->left_term->left_factor->token =
                        Token(left_result_token,
                              std::to_string(stoi(left_result) - stoi(right_result)));
                }

            }
        }
    }
}

/**
 * 
 * 
 * 
 * 
 */

void Parser::interpret_term_in_order(TermNode *node, int tabs)
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
        /* traverse all the way down to the bottom node */
        interpret_term_in_order(node->left_binary_term, tabs + 1);

        /* perform mathematical operation on node */
        if (node->left_factor && node->right_factor)
        {
            /* calculate result of both operands */
            if (node->left_factor->token.get_token_type() == INT &&
                node->right_factor->token.get_token_type() == INT)
            {

                if (node->op.get_token_type() == MULTIPLY)
                {
                    auto result =
                        std::stoi(node->left_factor->token.get_token_value()) *
                        std::stoi(node->right_factor->token.get_token_value());

                    /* store result in left factor of current node
                       next node will  read from this token and use it
                    */
                    node->left_factor->token =
                        Token(node->left_factor->token.get_token_type(), std::to_string(result));
                }

                if (node->op.get_token_type() == DIVIDE)
                {
                    auto result =
                        std::stoi(node->left_factor->token.get_token_value()) /
                        std::stoi(node->right_factor->token.get_token_value());

                    /* store result in left factor of current node
                       next node will  read from this token and use it
                    */
                    node->left_factor->token =
                        Token(node->left_factor->token.get_token_type(),
                              std::to_string(result));
                }
            }
        }
    }
}

/**
 * 
 * 
 * 
 * 
 */

void Parser::run_parser(std::string current_line)
{

    /* print the value to screen */
    if (current_token.get_token_type() == PRINT)
    {
        /* get left paren */
        advance_current_token_index();

        /* get the current value to print */
        advance_current_token_index();

       /* print variable name  */
        if ((*tokenVector)[current_token_index + 1].get_token_type() ==
                RIGHT_PAR &&
            current_token.get_token_type() == VARIABLE)
        {
            /* printing a variable */
            auto variable_data = symbol_table->find(current_token.get_token_value());
            if (variable_data == symbol_table->end())
            {
                std::cout << "Variable not defined" << std::endl;
                std::cout << "error\n";
                exit(-1);
            }

            /* check if printing a list */
            if (variable_data->second.get_token_type() == LIST)
            {
                std::cout << "[" << variable_data->second.get_token_value()
                       << "]"
                       << std::endl;
            }
            else
            {
                std::cout << variable_data->second.get_token_value() << std::endl;
            }
        }
         /*  print string */
        else if ((*tokenVector)[current_token_index + 1].get_token_type() ==
                     RIGHT_PAR &&
                 current_token.get_token_type() == STRING)
        {
            std::ofstream myfile;
            myfile.open("tc1.txt", std::ios_base::app);
            myfile << current_token.get_token_value() << std::endl;
            myfile.close();
        }
        else
        {
            /* print an expression */
            auto expression_to_print = expression();

            /* sometimes its null sometimes isnt, this makes it certain */
            if (expression_to_print->right_term == nullptr)
            {
                expression_to_print->left_binary_expression = nullptr;
            }

            interpret_expression_in_order(expression_to_print, 1,
                                          (expression_to_print->right_term !=
                                           nullptr));

            auto token_to_print =
                expression_to_print->left_term->left_factor->token;

            /* check if printing a list */
            if (token_to_print.get_token_type() ==
                LIST)
            {
                std::cout << "["
                       << token_to_print.get_token_value()
                       << "]"
                       << std::endl;
            }
            else
            {
                std::cout
                    << token_to_print.get_token_value()
                    << std::endl;
            }
        }
    }

    /**
 * 
 * 
 * 
 * 
 */

    /* if its an VARIABLE, get the arithmetic expression value  */
    else if (current_token.get_token_type() == VARIABLE)
    {

        /* get variable name */
        auto variable_name = current_token.get_token_value();
        advance_current_token_index();

        /* get equals sign */
        advance_current_token_index();

        
        /* check if its a function call */
        if (current_token.get_token_type() ==
                     VARIABLE &&
                 (*tokenVector)[current_token_index + 1].get_token_type() == LEFT_PAR)
        {

            /* get function code */
            auto variable_data = symbol_table->find(current_token.get_token_value());
            if (variable_data == symbol_table->end())
            {
                std::cout << "error" << std::endl;
                exit(-1);
            }

            std::string function_definition = variable_data->second.get_token_value();
            std::string function_body = variable_data->second.get_token_value();
            std::string function_params = variable_data->second.get_token_value();

            /* get params */
            auto start_pos_to_erase = function_params.find(":");
            if (start_pos_to_erase == std::string::npos)
            {
                std::cout << "error\n";
                exit(-1);
            }
            function_params.erase(start_pos_to_erase, function_params.length());

            /* get definition */
            auto end_pos_to_erase = function_definition.find(":") + 1;
            if (end_pos_to_erase == std::string::npos)
            {
                std::cout << "there was an error \n";
            }
            function_definition.erase(0, end_pos_to_erase);

            std::vector<std::string> function_params_trimmed;
            for(int i = 0; i < function_params.length(); i++)
            {
                std::string param_name = "";
                while (i < function_params.length() && function_params[i] != ',')
                {
                    if (function_params[i] != ' ')
                    {
                        param_name += function_params[i];
                    }
                    i++;
                };
                function_params_trimmed.push_back(param_name);
            }

            std::string function_def_trimmed = "";
            for(int i = 0; i < function_definition.length(); i++)
            {
                if (function_definition[i] == ' ')
                    continue;
                else
                    function_def_trimmed += function_definition[i];
            }

            function_definition = function_def_trimmed;

            /* token after left paren */
            advance_current_token_index();
            advance_current_token_index();

            std::vector<std::string> args;
            /* get function args to pass in */
            while (current_token.get_token_type() !=
                   RIGHT_PAR)
            {
                if (current_token.get_token_value() != ",")
                {
                    args.push_back(current_token.get_token_value());
                }

                advance_current_token_index();
            };


            /* replace all variables with the proper values */
            for(int i = 0; i < function_params_trimmed.size();)
            {

                auto idx_of_var = function_definition.find(
                    function_params_trimmed[i]);

                if (idx_of_var == std::string::npos)
                    break;

                auto up_to_var = function_definition;
                auto var_to_end = function_definition;

                up_to_var.erase(idx_of_var, function_definition.length());
                var_to_end.erase(0, idx_of_var + function_params_trimmed[i].length());

                function_definition = up_to_var + args[i] + var_to_end;

                advance_current_token_index();
            }


            /* lex and parse new line */
            this->tokenVector = new std::vector<Token>();
            std::string line = variable_name + "=" + function_definition;
            Lexer lexer = Lexer(this->tokenVector,symbol_table);
            lexer.run_lexer(line);
            current_token_index = -1;
            advance_current_token_index();
            this->run_parser(line);
        }

        /* for a simple variable assigment expression   */
        else
        {
            /* look for new expression */
            auto variable_value = expression();

            /* sometimes its null sometimes isnt, this makes it certain */
            if (variable_value->right_term == nullptr)
            {
                variable_value->left_binary_expression = nullptr;
            }

            interpret_expression_in_order(variable_value, 1, (variable_value->right_term != nullptr));

            /* if value exists in map, then delete it */
            auto it = symbol_table->find(variable_name);
            if (it != symbol_table->end())
            {
                symbol_table->erase(variable_name);
            }

            /* insert in variable map */
            symbol_table->insert(
                std::pair<std::string, Token>(
                    variable_name,
                    Token(
                        variable_value->left_term->left_factor->token.get_token_type(),
                        variable_value->left_term->left_factor->token.get_token_value())));
        }
    }

    /**
 * 
 * 
 * 
 *
 * 
 * 
 * 
 * 
 * 
 * 
 */
    /* if statement */
    else if (current_token.get_token_type() == IF)
    {

        std::vector<Token> *left_list = new std::vector<Token>();
        std::vector<Token> *right_list = new std::vector<Token>();

        int idx_comp_op = 0; //will be index of comparison operator

        /* get index of comparison operator */
        for (auto it = tokenVector->begin();
             it != tokenVector->end();
             ++it)
        {
            idx_comp_op++;
            if (it->get_token_type() == NOT_EQUAL ||
                it->get_token_type() == COMP_EQUAL ||
                it->get_token_type() == LESS_THAN ||
                it->get_token_type() == LESS_THAN_EQUAL ||
                it->get_token_type() == GREATER_THAN ||
                it->get_token_type() == GREATER_THAN_EQUALS)
            {
                break;
            }
        }

        /* copy over the expressions left and right of comparison operator */
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


        /* evaluate left half of the tokens list */
        tokenVector = left_list;
        current_token_index = -1;
        advance_current_token_index();
        auto left_comp_expr = expression();

        /* sometimes its null sometimes isnt, this makes it certain */
        if (left_comp_expr->right_term == nullptr)
        {
            left_comp_expr->left_binary_expression = nullptr;
        }

        interpret_expression_in_order(left_comp_expr,
                                      1,
                                      (left_comp_expr->right_term != nullptr));

        /* evaluate right half of the tokens list */
        tokenVector = right_list;
        current_token_index = -1;
        advance_current_token_index();
        auto right_comp_expr = expression();

        /* sometimes its null sometimes isnt, this makes it certain */
        if (right_comp_expr->right_term == nullptr)
        {
            right_comp_expr->left_binary_expression = nullptr;
        }

        interpret_expression_in_order(right_comp_expr,
                                      1,
                                      (right_comp_expr->right_term != nullptr));

        /* calculate result */
        auto left_value =
            left_comp_expr->left_term->left_factor->token.get_token_value();
        auto left_type =
            left_comp_expr->left_term->left_factor->token.get_token_type();

        auto right_value =
            right_comp_expr->left_term->left_factor->token.get_token_value();
        auto right_type =
            right_comp_expr->left_term->left_factor->token.get_token_value();


        /* make comparison check */
        bool compare = false;
        if (comparison_token.get_token_type() == NOT_EQUAL)
        {
            if (left_type == INT)
                compare = stoi(left_value) != stoi(right_value);
            else
                compare = left_value != right_value;
        }
        else if (comparison_token.get_token_type() == COMP_EQUAL)
        {
            if (left_type == INT)
                compare = stoi(left_value) == stoi(right_value);
            else
                compare = left_value == right_value;
        }
        else if (comparison_token.get_token_type() == LESS_THAN)
        {
            if (left_type == INT)
                compare = stoi(left_value) < stoi(right_value);
            else
                compare = left_value < right_value;
        }
        else if (comparison_token.get_token_type() == LESS_THAN_EQUAL)
        {
            if (left_type == INT)
                compare = stoi(left_value) <= stoi(right_value);
            else
                compare = left_value <= right_value;
        }
        else if (comparison_token.get_token_type() == GREATER_THAN)
        {
            if (left_type == INT)
                compare = stoi(left_value) > stoi(right_value);
            else
                compare = left_value > right_value;
        }
        else if (comparison_token.get_token_type() == GREATER_THAN_EQUALS)
        {
            if (left_type == INT)
                compare = stoi(left_value) >= stoi(right_value);
            else
                compare = left_value >= right_value;
        }


        if (compare)
        {
            *take_branch_else = false;
            *is_if_done = false;
        }
        else
        {
            *take_branch_else = true;
            *is_if_done = true;
        }
    }

    /* else statement */
    else if (current_token.get_token_type() == ELSE)
    {
        /* if a previous if statement passed, no need to check else block */
        if (*take_branch_else == false)
        {
            /* if token is else statement and we did if block mark it complete */
            *is_if_done = true;
            return;
        }
        else
        {
            *is_if_done = false;
        }
    }

}