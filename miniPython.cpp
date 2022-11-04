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
#define T_DIVIDE "DIVIDE"

#define T_COLON "COLON"
#define T_COMMA "COMMA"
#define T_LEFT_PAREN "LEFT_PAREN"
#define T_RIGHT_PAREN "RIGHT_PAREN"
#define T_LEFT_BRACKET "LEFT_BRACKET"
#define T_RIGHT_BRACKET "RIGHT_BRACKET"

#define T_IDENTIFIER "IDENTIFIER"

#define T_KEYWORD_IF "if"
#define T_KEYWORD_ELIF "elif"
#define T_KEYWORD_ELSE "else"
#define T_KEYWORD_PRINT "print"


#define T_KEYWORD_AND "AND"
#define T_KEYWORD_NOT "NOT"
#define T_KEYWORD_OR "OR"

#define T_NOT_EQUAL "NOT_EQUAL"
#define T_EQUAL_EQUAL "EQUAL_EQUAL"
#define T_LESS_THAN "LESS_THAN"
#define T_LESS_THAN_EQUAL "LESS_THAN_OR_EQUAL"
#define T_GREATER_THAN "GREATER_THAN"
#define T_GREATER_THAN_EQUALS "GREATER_THAN_OR_EQUAL"


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

};






class Lexer
{
public:
    std::vector<Token>* tokenVector;
    std::unordered_map<std::string, Token>* symbol_table;

//public:
     /* constructor */
    Lexer(std::vector<Token> *tk_list,
          std::unordered_map<std::string, Token> *sym_tab);

    /* will lex the input file and make tokens */
    void run_lexer(std::string line);

    /* will make a token given a string line */
    void make_tokens(std::string line_to_lex);

    /* will print tokens list */
    void print_tokenVector();

    /* will return a reference to the current tokenVector */
    std::vector<Token> get_tokenVector();

    /* will return a reference to the symbol_table */
    std::unordered_map<std::string, Token> *get_symbol_table();

    void print_symbol_table();
};


//struct FactorNode
class FactorNode
{
public:
    Token token;
    FactorNode(Token t) { token = t; }
    FactorNode() {}
};

//struct TermNode
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

//struct ExpressionNode
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
    ExpressionNode(TermNode *lt,
                   TermNode *rt,
                   Token opx)
    {
        left_term = lt;
        right_term = rt;
        op = opx;
    }

    /* for double operator */
    ExpressionNode(
        ExpressionNode *lbe,
        TermNode *lt,
        TermNode *rt,
        Token opx)
    {
        left_binary_expression = lbe;
        left_term = lt;
        right_term = rt;
        op = opx;
    }

    ExpressionNode()
    {
        // left_binary_expression = new ExpressionNode();
        left_term = nullptr;
        right_term = nullptr;
        op = Token();
    };
};


// TODO:There may be a need to come here and add more classes for the AST
class Parser
{
public:
    int current_token_index;
    Token current_token;
    std::vector<Token> *tokenVector;
    std::unordered_map<std::string, Token> *symbol_table;

    bool *should_do_if_statement;
    bool *should_do_else_statement;
    bool *is_if_done;


    Parser();
    Parser(std::vector<Token> *lexer_tokenVector,
           std::unordered_map<std::string, Token> *lexer_symbol_table,
           bool *should_do_if_s,
           bool *should_do_else_s, bool *should_do_elif_s);

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

    bool should_do_if_statement = true;
    bool should_do_else_statement = true;
    bool is_if_done = true;

    for (std::string line; getline(input, line);)
    {
        /* skip all empty and commented out lines */
        if (line == "" || line[0] == '#')
        {
            continue;
        }
        // TODO:ignore any parts of lines that follow a comment symbol

        if (line[line.length() - 1] == '\0')
        {
            line.pop_back();
        }
        
        if (should_do_if_statement == false && is_if_done == true)
        {
            if (line[0] == ' ')
                continue;
        }

        else if (should_do_else_statement == false &&
                 is_if_done == true)
        {
            if (line[0] == ' ')
                continue;
        }

        std::vector<Token> tokenVector;

        Lexer lexer = Lexer(
            &tokenVector,
            &symbol_table);

        lexer.run_lexer(line);

        // TODO:investigate this part here and see if it is needed
        Parser parser = Parser(&tokenVector,
                               &symbol_table,
                               &should_do_if_statement,
                               &should_do_else_statement,
                               &is_if_done

        );
        parser.run_parser(line);


        should_do_if_statement = true;

       
    }
    input.close();

    return 0;
};



/* constructors */
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
    for (int i = 0; i < line_to_lex.length(); i++)
    {

        if (std::isdigit(line_to_lex[i]))
        {
            std::string str_formed = "";
            /* form an integer */
            while (true)
            {
                str_formed += line_to_lex[i];

                /* if next character isnt a number then break */
                if (!std::isdigit(line_to_lex[i + 1]))
                    break;

                i++;
            }
            Token token;
            token.type = INT;
            token.value = str_formed;
            tokenVector->push_back(token);
        }
        // TODO: THIS BUILDS the STRING literals
        /* check if the current char is a quotation  */
        else if (line_to_lex[i] == '\"')
        {
            std::string str_formed = "";
            i++;
            while (line_to_lex[i] != '\"')
            {
                str_formed += line_to_lex[i];
                i++;
            }
            Token token;
            token.type = STRING;
            token.value = str_formed;
            tokenVector->push_back(token);
        }

        /* check if its  identifier or keyword */

        else if (std::isalpha(line_to_lex[i]))
        {
            std::string str_formed = "";

            /* keep looping until next space is found */
            while (true)
            {
                str_formed += line_to_lex[i];

                /* break if the next character is not part of  the str_formed */
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

            
            if (str_formed == T_KEYWORD_IF)
            {
                Token token;
                token.type = T_KEYWORD_IF;
                token.value = str_formed;
                tokenVector->push_back(token);
            }
            else if (str_formed == T_KEYWORD_ELIF)
            {
                Token token;
                token.type = T_KEYWORD_ELIF;
                token.value = str_formed;
                tokenVector->push_back(token);
            }
            else if (str_formed == T_KEYWORD_ELSE)
            {
                Token token;
                token.type = T_KEYWORD_ELSE;
                token.value = str_formed;
                tokenVector->push_back(token);
            }
            else if (str_formed == T_KEYWORD_PRINT)
            {
                Token token;
                token.type = T_KEYWORD_PRINT;
                token.value = str_formed;
                tokenVector->push_back(token);
            }
            else if (str_formed == T_KEYWORD_AND)
            {
                Token token;
                token.type = T_KEYWORD_AND;
                token.value = str_formed;
                tokenVector->push_back(token);
            }
            else if (str_formed == T_KEYWORD_OR)
            {
                Token token;
                token.type = T_KEYWORD_OR;
                token.value = str_formed;
                tokenVector->push_back(token);
            }
            else if (str_formed == T_KEYWORD_NOT)
            {
                Token token;
                token.type = T_KEYWORD_NOT;
                token.value = str_formed;
                tokenVector->push_back(token);
            }
            else
            {
                Token token;
                token.type = T_IDENTIFIER;
                token.value = str_formed;
                tokenVector->push_back(token);
            }
        }

       // TODO: //this should work on the = or ==
        // check if current char is = or == sign 
        else if (line_to_lex[i] == '=')
        {
            if (line_to_lex[i + 1] == '=')
            {
                /* == */
                i++;
                Token token;
                token.type = T_EQUAL_EQUAL;
                token.value = "==";
                tokenVector->push_back(token);
            }
            else
            {
                /* = */
                Token token;
                token.type = ASSIGNMENT;
                token.value = "=";
                tokenVector->push_back(token);
            }
        }

        /* check if current char is != */
        else if (line_to_lex[i] == '!')
        {
            /* if != */
            if (line_to_lex[i + 1] == '=')
            {
                i++; //advance token an extra time
                Token token;
                token.type = T_NOT_EQUAL;
                token.value = "!=";
                tokenVector->push_back(token);
            }
            else
            {
                std::cout << "syntax error\n";
            }
        }

        /* check if current char is < or <= */
        else if (line_to_lex[i] == '<')
        {

            if (line_to_lex[i + 1] == '=')
            {
                /* <= */
                i++; //advance token an extra time
                Token token;
                token.type = T_LESS_THAN_EQUAL;
                token.value = "<=";
                tokenVector->push_back(token);
            }
            else
            {
                // < 
                Token token;
                token.type = T_LESS_THAN;
                token.value = "<";
                tokenVector->push_back(token);
            }
        }

        /* check if current char is > or >= */
        else if (line_to_lex[i] == '>')
        {
            if (line_to_lex[i + 1] == '=')
            {
                /* >= */
                i++; //advance token an extra time
                Token token;
                token.type = T_GREATER_THAN_EQUALS;
                token.value = ">=";
                tokenVector->push_back(token);
            }
            else
            {
                /* > */
                Token token;
                token.type = T_GREATER_THAN;
                token.value = ">";
                tokenVector->push_back(token);
            }
        }

        /* check if current char is plus sign */
        else if (line_to_lex[i] == '+')
        {
            Token token;
            token.type = ADD;
            token.value = "+";
            tokenVector->push_back(token);
        }

        /* check if current char is minus sign */
        else if (line_to_lex[i] == '-')
        {
            Token token;
            token.type = MINUS;
            token.value = "-";
            tokenVector->push_back(token);
        }

        /* check if current char is divide sign */
        else if (line_to_lex[i] == '/')
        {
            Token token;
            token.type = T_DIVIDE;
            token.value = "/";
            tokenVector->push_back(token);
        }

        /* check if current char is mult sign */
        else if (line_to_lex[i] == '*')
        {
            Token token;
            token.type = MULTIPLY;
            token.value = "*";
            tokenVector->push_back(token);

        }
        else if (line_to_lex[i] == '[')
        {
            Token token;
            token.type = T_LEFT_BRACKET;
            token.value = "[";
            tokenVector->push_back(token);
        }
        else if (line_to_lex[i] == ']')
        {
            Token token;
            token.type = T_RIGHT_BRACKET;
            token.value = "]";
            tokenVector->push_back(token);
        }
        else if (line_to_lex[i] == '(')
        {
            Token token;
            token.type = T_LEFT_PAREN;
            token.value = "(";
            tokenVector->push_back(token);
        }
        else if (line_to_lex[i] == ')')
        {
            Token token;
            token.type = T_RIGHT_PAREN;
            token.value = ")";
            tokenVector->push_back(token);
        }
        else if (line_to_lex[i] == ':')
        {
            Token token;
            token.type = T_COLON;
            token.value = ":";
            tokenVector->push_back(token);
        }
        
        else if (line_to_lex[i] == ',')
        {
            Token token;
            token.type = T_COMMA;
            token.value = ",";
            tokenVector->push_back(token);
        }

    }
};

void Lexer::print_tokenVector()
{

    for (auto it = tokenVector->begin(); it != tokenVector->end(); it++)
    {
        it->print_token();
    }
};









Parser::Parser(){};

Parser::Parser(std::vector<Token> *lexer_tokenVector,
               std::unordered_map<std::string, Token> *lexer_symbol_table,
               bool *is_condition_t,
               bool *should_do_else_s,
               bool *should_do_elif_s)

{
    current_token_index = -1;
    tokenVector = lexer_tokenVector;
    symbol_table = lexer_symbol_table;

    should_do_if_statement = is_condition_t;
    should_do_else_statement = should_do_else_s;
    is_if_done = should_do_elif_s;

    /* get a token */
    advance_current_token_index();
}

/**
 * 
 * 
 * 
 * 
 */

void Parser::advance_current_token_index()
{
    current_token_index++;
    if (current_token_index < tokenVector->size())
        /* get the current token */
        current_token = (*tokenVector)[current_token_index];
}

/**
 * 
 * 
 * 
 * 
 */

FactorNode *Parser::factor()
{
    FactorNode *factor_node = new FactorNode();
    factor_node->token = Token();

    /* if its a variable, get its value */
    if (current_token.type == T_IDENTIFIER)
    {
        auto variable_data = symbol_table->find(current_token.value);
        if (variable_data == symbol_table->end())
        {
            /* variable doesnt exist, undefined error */
            std::cout << "error\n"
                      << std::endl;
        }

        // TODO: HERE WE DO BASIC LIST ACCESS OPERATIONS
        /* if variable is a list and we are trying to access its elements */
        if (variable_data->second.type == LIST &&
            (*tokenVector)[current_token_index + 1].type ==
                T_LEFT_BRACKET)
        {
            //left bracket
            advance_current_token_index();

            std::string list_value = variable_data->second.value;

            // TODO:This is the starting index of the list
            //start index
            advance_current_token_index();

            /* get the start index of where to copy */
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

            /* stop index in list we are accessing */
            int stop_index;

            /* if just retrieving element or copying multi elements */
            bool one_element_access = false;
            //either colon or right bracket
            advance_current_token_index();

            /* if only one index to get element from */
            if (current_token.type == T_RIGHT_BRACKET)
            {
                one_element_access = true;
                stop_index = -1;
            }
            /* if slicing into list */
            // TODO: This is the slicing part of the Parser
            else if (current_token.type == T_COLON)
            {

                /* check if token after colon is an integer */
                advance_current_token_index();

                /* get the number after the colon as the stop index  */
                if (current_token.type == INT)
                {
                    //dont wanna copy last element but everything up to it
                    stop_index = stoi(current_token.value);
                }
                else
                {
                    /* if token after the colon is a bracket
                        we are copying entire remainder of list */

                    /* throw syntax error no right bracket */
                    if (current_token.type != T_RIGHT_BRACKET)
                    {
                        std::cout << "error\n";
                    }
                    //get everything from the remainder of the list
                    stop_index = list_to_access.size();
                }
            }
            // TODO: This does single index values from lists
            //get that one value from the list
            if (one_element_access)
            {
                /* invalid list out of bound */
                if (start_index > (int)list_to_access.size() - 1)
                {
                    Token t;
                    t.type = STRING;
                    t.value = "ERROR";
                    factor_node->token = t;
                }
                else
                {
                    auto value_in_list = list_to_access[start_index];

                    std::string value_to_save_in_token = value_in_list;

                    /* store an integer token */
                    if (std::isdigit(value_in_list[0]))
                    {

                        Token t;
                        t.type = INT;
                        t.value = value_to_save_in_token;
                        factor_node->token = t;
                    }
                    else
                    {
                        /* store a string token */
                        Token t;
                        t.type = STRING;
                        t.value = value_to_save_in_token;
                        factor_node->token = t;
                        //factor_node->token = Token(STRING, value_to_save_in_token);
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
            /* its just an identifier not list access */
            Token t;
            t.type = variable_data->second.type;
            t.value = variable_data->second.value;
            factor_node->token = t;
        }

        advance_current_token_index();
    }

    /* if the current token is an integer or decimal make a number node for AST */
    if (current_token.type == INT )
    {
        factor_node->token = current_token;
        advance_current_token_index();
    }

    /* string type */
    if (current_token.type == STRING)
    {
        factor_node->token = current_token;
        advance_current_token_index();
    }

    /* if the current token is a LEFT_PAREN or RIGHT_PARENT */
    if (current_token.type == T_LEFT_PAREN)
    {
        advance_current_token_index();

        /* solve inner expression and save result value */
        ExpressionNode *expr = expression();
        interpret_expression_in_order(expr, 1, (expr->right_term != nullptr));

        /* if we get right paren, inner expression is done */
        if (current_token.type == T_RIGHT_PAREN)
        {
            /* advance past the inner expression right paren */
            advance_current_token_index();
            /* return solved inner expression token  */
            factor_node->token = expr->left_term->left_factor->token;
        }
        else
        {
            /* throw syntax error if we dont got a right paren */
            std::cout << "error\n";
        }
    }

    /* LIST TYPE */
    if (current_token.type == T_LEFT_BRACKET)
    {
        advance_current_token_index();

        /* store all list elements  */
        std::string list_elements = "";
        while (current_token.type !=
               T_RIGHT_BRACKET)
        {
            if (current_token_index == tokenVector->size())
            {
                /* no right bracket == syntax error */
                std::cout << "error\n";
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
    while (current_token.type == MULTIPLY ||
           current_token.type == T_DIVIDE)
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
/**
 * 
 * 
 * 
 * 
 */

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
        current_token.type == ADD ||
        current_token.type == MINUS)
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
                node->left_term->left_factor->token.value;

            left_result_token =
                node->left_term->left_factor->token.type;
        }

        /* traverse right side of the expression node */
        if (node->right_term)
        {
            /* traverse the right term tree */
            interpret_term_in_order(node->right_term, tabs + 3);

            right_result = node->right_term->left_factor->token.value;
            right_result_token = node->right_term->left_factor->token.type;
        }

        /* calculate the final result and store it in left node of the current node
           next expression before it will read from it recursively
        */

        
        /* remove last token in list when the advancing token doesnt remove it */

        bool is_list_concat = true;
        if (current_token.type == T_RIGHT_BRACKET)
        {
            auto left_bracket = (*tokenVector)[current_token_index - 3].type;
            auto single_number = (*tokenVector)[current_token_index - 2].type;

            /* doing additon of values not list concatenation */
            if (left_bracket == T_LEFT_BRACKET && single_number == INT)
            {
                is_list_concat = false;
            }
            Token t;
            t.type = LIST;
            t.value = "";
            current_token = t;
        }
        if (current_token.type == T_IDENTIFIER)
        {

            auto variable_data = symbol_table->find(current_token.value);
            if (variable_data == symbol_table->end())
            {
                std::cout << "error\n";
            }

            /* set the factor value to the variable tokens value */
            Token t;
            t.type = variable_data->second.type;
            t.value = variable_data->second.value;
            current_token = t;
        }

        /* if we only have a left term */
        if (node->left_term && !node->right_term)
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
                t.value = left_result;
                node->left_term->left_factor->token = t;
            }
            else
            {
                /* integers */
                Token t;
                t.type = left_result_token;
                t.value = std::to_string(stoi(left_result));
                node->left_term->left_factor->token = t;
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

        /* if we have both a left and right term need to do binary operation */
        if (node->left_term && node->right_term)
        {
            if (node->op.type == ADD)
            {

                /* make the left term a new node and store the updated value in it
                will be read by previous node above it  */
                delete node->left_term;
                node->left_term = new TermNode();
                node->left_term->left_factor = new FactorNode();

                /* list element access */
                if (current_token.type == LIST &&
                    !is_list_concat)
                {
                    if (!std::isdigit(right_result[0]))
                    {
                        /* string addition */
                        Token t;
                        t.type = left_result_token;
                        t.value = left_result + right_result;
                        node->left_term->left_factor->token = t;
                    }
                    else
                    {   //////////////////////////////////////////////////////////////////////
                        /* integer addition */
                        Token t;
                        t.type = left_result_token;
                        t.value = std::to_string(stoi(left_result) + stoi(right_result));
                        node->left_term->left_factor->token = t;
                        //std::cout << t.value;
                    }
                }
                else if (current_token.type == LIST &&
                         is_list_concat)
                {
                    /* list concatenation */
                    Token t;
                    t.type = left_result_token;
                    t.value = left_result + "," + right_result;
                    node->left_term->left_factor->token = t;
                }
                else if (current_token.type == STRING)
                {
                    /* string concatenation */
                    Token t;
                    t.type = left_result_token;
                    t.value = left_result + right_result;
                    node->left_term->left_factor->token = t;
                }
                else
                {
                    /* integers addition */
                    Token t;
                    t.type = left_result_token;
                    t.value = std::to_string(stoi(left_result) + stoi(right_result));
                    node->left_term->left_factor->token = t;

                }

            }

            if (node->op.type == MINUS)
            {

                /* make the left term a new node and store the updated value in it
                will be read by previous node above it  */
                delete node->left_term;
                node->left_term = new TermNode();
                node->left_term->left_factor = new FactorNode();

                if (current_token.type == STRING ||
                    current_token.type == LIST)
                {
                    // std::cout << "!!!!!! L - R  TOTAL = "
                    //           << left_result - right_result << std::endl
                    //           << std::endl;
                }
                else
                {
                    Token t;
                    t.type = left_result_token;
                    t.value = std::to_string(stoi(left_result) - stoi(right_result));
                    node->left_term->left_factor->token = t;

                    //node->left_term->left_factor->token =
                    //    Token(left_result_token,
                    //          std::to_string(stoi(left_result) - stoi(right_result)));
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
                    node->left_factor->token =
                        Token(node->left_factor->token.type, std::to_string(result));
                }

                if (node->op.type == T_DIVIDE)
                {
                    auto result =
                        std::stoi(node->left_factor->token.value) /
                        std::stoi(node->right_factor->token.value);

                    /* store result in left factor of current node
                       next node will  read from this token and use it
                    */
                    node->left_factor->token =
                        Token(node->left_factor->token.type,
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
    if (current_token.type == T_KEYWORD_PRINT)
    {
        /* get left paren */
        advance_current_token_index();

        /* get the current value to print */
        advance_current_token_index();

       /* print variable name  */
        if ((*tokenVector)[current_token_index + 1].type ==
                T_RIGHT_PAREN &&
            current_token.type == T_IDENTIFIER)
        {
            /* printing a variable */
            auto variable_data = symbol_table->find(current_token.value);
            if (variable_data == symbol_table->end())
            {
                std::cout << "Variable not defined" << std::endl;
                std::cout << "error\n";
            }

            /* check if printing a list */
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
                std::cout << variable_data->second.value << std::endl;
            }
        }
         /*  print string */
        else if ((*tokenVector)[current_token_index + 1].type ==
                     T_RIGHT_PAREN &&
                 current_token.type == STRING)
        {
            std::ofstream myfile;
            myfile.open("tc1.txt", std::ios_base::app);
            myfile << current_token.value << std::endl;
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
            if (token_to_print.type ==
                LIST)
            {
                std::cout << "["
                       << token_to_print.value
                       << "]"
                       << std::endl;
            }
            else
            {
                std::cout
                    << token_to_print.value
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

    /* if its an identifier, get the arithmetic expression value  */
    else if (current_token.type == T_IDENTIFIER)
    {

        /* get variable name */
        auto variable_name = current_token.value;
        advance_current_token_index();

        /* get equals sign */
        advance_current_token_index();

        
        /* check if its a function call */
        if (current_token.type ==
                     T_IDENTIFIER &&
                 (*tokenVector)[current_token_index + 1].type == T_LEFT_PAREN)
        {

            /* get function code */
            auto variable_data = symbol_table->find(current_token.value);
            if (variable_data == symbol_table->end())
            {
                std::cout << "error" << std::endl;
            }

            std::string function_definition = variable_data->second.value;
            std::string function_body = variable_data->second.value;
            std::string function_params = variable_data->second.value;

            /* get params */
            auto start_pos_to_erase = function_params.find(":");
            if (start_pos_to_erase == std::string::npos)
            {
                std::cout << "error\n";
            }
            function_params.erase(start_pos_to_erase, function_params.length());

            /* get definition */
            auto end_pos_to_erase = function_definition.find(":") + 1;
            if (end_pos_to_erase == std::string::npos)
            {
                std::cout << "error\n";
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
            while (current_token.type !=
                   T_RIGHT_PAREN)
            {
                if (current_token.value != ",")
                {
                    args.push_back(current_token.value);
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

                function_definition = up_to_var +
                                      args[i] +
                                      var_to_end;

                advance_current_token_index();
            }

            /* if its a lambda if then solve the if  */
            auto is_if = function_definition.find("if");
            if (is_if != std::string::npos)
            {
                auto is_else = function_definition.find("else");

                if (is_else == std::string::npos)
                {
                    std::cout << "error\n";
                }

                auto else_value = function_definition;
                auto if_value = function_definition;

                /* get proper return values for both if and else  */
                if_value.erase(is_if, function_definition.length());
                else_value.erase(0, is_else + 4); // + 4 == size of else

                /* get the condition between if and else */
                auto condition = function_definition;
                condition.erase(0, is_if + 2); // + 2 == size of if
                auto condition_else = condition.find("else");
                condition.erase(condition_else, condition.length());

                /* get the comparison operator and operands */

                /* lex condition get the tokens */
                this->tokenVector = new std::vector<Token>();
                Lexer lexer = Lexer(
                    this->tokenVector,
                    symbol_table);
                lexer.run_lexer(condition);
                current_token_index = -1;
                advance_current_token_index();

                // std::getchar();

                /* solve left and right result, then interpret results */
                std::vector<Token> *left_list = new std::vector<Token>();
                std::vector<Token> *right_list = new std::vector<Token>();

                int idx_comp_op = 0; //will be index of comparison operator

                /* get index of comparison operator */
                for (auto it = tokenVector->begin();
                     it != tokenVector->end();
                     ++it)
                {
                    idx_comp_op++;
                    if (it->type == T_NOT_EQUAL ||
                        it->type == T_EQUAL_EQUAL ||
                        it->type == T_LESS_THAN ||
                        it->type == T_LESS_THAN_EQUAL ||
                        it->type == T_GREATER_THAN ||
                        it->type == T_GREATER_THAN_EQUALS)
                    {
                        break;
                    }
                }

                /* copy over the expressions left and right of comparison operator */
                idx_comp_op -= 1;
                for (auto it = tokenVector->begin();
                     it != tokenVector->begin() + idx_comp_op; ++it)
                {
                    left_list->push_back(*it);
                }
                auto comparison_token = (*tokenVector)[idx_comp_op];
                for (auto it = tokenVector->begin() + idx_comp_op + 1;
                     it != tokenVector->end(); ++it)
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

                /* calculate results */
                auto left_value =
                    left_comp_expr->left_term->left_factor->token.value;
                auto left_type =
                    left_comp_expr->left_term->left_factor->token.type;

                auto right_value =
                    right_comp_expr->left_term->left_factor->token.value;
                auto right_type =
                    right_comp_expr->left_term->left_factor->token.value;

                /* make comparison check */
                bool condition_result = false;
                if (comparison_token.type == T_NOT_EQUAL)
                {
                    if (left_type == INT)
                        condition_result = stoi(left_value) != stoi(right_value);
                    else
                        condition_result = left_value != right_value;
                }
                else if (comparison_token.type == T_EQUAL_EQUAL)
                {
                    if (left_type == INT)
                        condition_result = stoi(left_value) == stoi(right_value);
                    else
                        condition_result = left_value == right_value;
                }
                else if (comparison_token.type == T_LESS_THAN)
                {
                    if (left_type == INT)
                        condition_result = stoi(left_value) < stoi(right_value);
                    else
                        condition_result = left_value < right_value;
                }
                else if (comparison_token.type == T_LESS_THAN_EQUAL)
                {
                    if (left_type == INT)
                        condition_result = stoi(left_value) <= stoi(right_value);
                    else
                        condition_result = left_value <= right_value;
                }
                else if (comparison_token.type == T_GREATER_THAN)
                {
                    if (left_type == INT)
                        condition_result = stoi(left_value) > stoi(right_value);
                    else
                        condition_result = left_value > right_value;
                }
                else if (comparison_token.type == T_GREATER_THAN_EQUALS)
                {
                    if (left_type == INT)
                        condition_result = stoi(left_value) >= stoi(right_value);
                    else
                        condition_result = left_value >= right_value;
                }

                /* store the result of the condition in func def to be parsed */
                if (condition_result)
                    function_definition = if_value;
                else
                    function_definition = else_value;

                
            }

            /* lex and parse new line */
            this->tokenVector = new std::vector<Token>();
            std::string line = variable_name + "=" + function_definition;
            Lexer lexer = Lexer(
                this->tokenVector,
                symbol_table);
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
                        variable_value->left_term->left_factor->token.type,
                        variable_value->left_term->left_factor->token.value)));
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
    else if (current_token.type == T_KEYWORD_IF)
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
            if (it->type == T_NOT_EQUAL ||
                it->type == T_EQUAL_EQUAL ||
                it->type == T_LESS_THAN ||
                it->type == T_LESS_THAN_EQUAL ||
                it->type == T_GREATER_THAN ||
                it->type == T_GREATER_THAN_EQUALS)
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
            left_comp_expr->left_term->left_factor->token.value;
        auto left_type =
            left_comp_expr->left_term->left_factor->token.type;

        auto right_value =
            right_comp_expr->left_term->left_factor->token.value;
        auto right_type =
            right_comp_expr->left_term->left_factor->token.value;


        /* make comparison check */
        bool compare = false;
        if (comparison_token.type == T_NOT_EQUAL)
        {
            if (left_type == INT)
                compare = stoi(left_value) != stoi(right_value);
            else
                compare = left_value != right_value;
        }
        else if (comparison_token.type == T_EQUAL_EQUAL)
        {
            if (left_type == INT)
                compare = stoi(left_value) == stoi(right_value);
            else
                compare = left_value == right_value;
        }
        else if (comparison_token.type == T_LESS_THAN)
        {
            if (left_type == INT)
                compare = stoi(left_value) < stoi(right_value);
            else
                compare = left_value < right_value;
        }
        else if (comparison_token.type == T_LESS_THAN_EQUAL)
        {
            if (left_type == INT)
                compare = stoi(left_value) <= stoi(right_value);
            else
                compare = left_value <= right_value;
        }
        else if (comparison_token.type == T_GREATER_THAN)
        {
            if (left_type == INT)
                compare = stoi(left_value) > stoi(right_value);
            else
                compare = left_value > right_value;
        }
        else if (comparison_token.type == T_GREATER_THAN_EQUALS)
        {
            if (left_type == INT)
                compare = stoi(left_value) >= stoi(right_value);
            else
                compare = left_value >= right_value;
        }


        if (compare)
        {
            *should_do_else_statement = false;
            *is_if_done = false;
        }
        else
        {
            *should_do_else_statement = true;
            *is_if_done = true;
        }
    }

    /* else statement */
    else if (current_token.type == T_KEYWORD_ELSE)
    {
        /* if a previous if statement passed, no need to check else block */
        if (*should_do_else_statement == false)
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
