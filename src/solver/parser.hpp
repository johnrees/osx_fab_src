#ifndef PARSE_H
#define PARSE_H

#include <string>
#include <list>

#include "opcodes.hpp"
#include "math_tree.hpp"
#include "fabvars.hpp"

// This class parses a .math file, converting it into a MathTree.
class Parser
{
public:
    Parser();
    
    // Tokens are divided into a set of types
    enum token_type {
        TOKEN_NUM,
        TOKEN_FUNC,
        TOKEN_OP,
        TOKEN_LPARENS,
        TOKEN_ARGSEP,
        TOKEN_RPARENS,
        
        TOKEN_MAP_START,
            TOKEN_MAP_DEF, TOKEN_MAP_SEPARATOR,
        TOKEN_MAP_END,
        
        TOKEN_EMPTY,
        TOKEN_ERROR
    };
    
    // Input and output types for parse tokens.
    enum io_type {
        IO_NUM,
        IO_BOOL,
        IO_COLOR,
        IO_NONE
    };
    
    // Small structure to hold a single parse token
    typedef struct parse_token
    {
        /* parse_token()
         *
         *  Token constructor.  Creates an empty token.
         */
        parse_token();
        
        
        /* parse_token(token_type type)
         *
         *  Token constructor.  Creates an null token of
         *  the given type.
         */
        parse_token(token_type type);
        
        /* parse_token(Node* n)
         *
         *  Token constructor.  Creates an token based on
         *  a Node object, using helper functions to look up
         *  the node's parameters (e.g. number of arguments)
         */
        parse_token(Node* n);
    
        // Every token points to a Node
        Node* n;
        
        // How many arguments will this token want to pull
        // off the stack?
        int num_args;
        
        // This token's precedence (lower is more important)
        int precedence;
        
        // The type of this token (function, number, operation, etc)
        token_type ttype;
        
        // Token's input and output data types
        io_type input;
        io_type output;
        
        // 'l' or 'r' for left or right associative
        char associativity;
        
    } parse_token;


    /* MathTree* parse(std::string input, solver_mode& mode)
     *
     *  Takes in a string of math and solver mode, and returns an
     *  appropriate MathTree.
     */
    MathTree* parse(std::string input, solver_mode& mode);
    
private:
    /* parse_token next_token()
     *
     *  Extracts and returns the next token from the input string.
     */
    parse_token next_token();
    
    
    /* void operator_to_stack(parse_token& o1)
     *
     *  Puts an operator node on the top of the operator stack.
     *  This may require popping other nodes off the operator stack
     *  depending on their precedence.
     */
    void operator_to_stack(parse_token& o1);
    
    
    /* void operator_to_output(parse_token& oper)
     *
     *  Puts an operator node on the top of the output stack.
     *  This may involve pulling arguments from the output stack
     *  and assigning them as the operator's children.
     */    
    void operator_to_output(parse_token& oper);


    /* void cache_node(Node*& node)
     *
     *  Stores a node in the cache.  If COMBINE_NODES is enabled, then
     *  this function uniquifies nodes in the cache.
     */
    void cache_node(Node*& node);
    
    
    /* void remove_ignored()
     *
     *  Traverses the node cache, deleting any items with no references.
     *  This is needed if the SIMPLIFY_TREE is turned on, because nodes
     *  are optimized out of the tree after they are cached.
     */
    void remove_ignored();
    
    
    /* MathTree* cache_to_tree()
     *
     *  Copies the set of cached nodes into a MathTree and
     *  returns the tree.
     */
    MathTree* cache_to_tree(Node* root);


    /* bool simplify(parse_token& oper,
                  parse_token& lh_arg,
                  parse_token& rh_arg)
     *
     *  Simplifies an expression by replacing an operation with one of its
     *  operands if appropriate (e.g. X + 0 -> X).
     *
     *  Returns true if the operation was simplified, false otherwise.
     */
    bool simplify(parse_token& oper,
                  parse_token& lh_arg,
                  parse_token& rh_arg);
                  
                  
    /* void convert_colors(parse_token& oper,
                           parse_token& lh_arg,
                           parse_token& rh_arg);
     *
     *  Replaces logical operations with bitwise operations where
     *  appropriate.  This occurs in a few situations:
     *      boolean * number -> boolean & number, which will be
     *          wrapped into color(boolean) & color(number) by
     *          wrap_argument
     *      color && color -> color & color
     *      color || color -> color | color
     *      !color -> ~color
     */
    void convert_colors(parse_token& oper,
                        parse_token& lh_arg,
                        parse_token& rh_arg);
                        
                        
    /* void wrap_argument(parse_token& arg, io_type desired)
     *
     *  Coerces the output type of the provided parse token into the
     *  desired output types, using translator nodes if needed.
     */
    void wrap_argument(parse_token& arg, io_type desired);
    
    // The math string in question
    const char* math_string;
    
    // Our current position in the math string (used in tokenizer)
    const char* start;

    // State variable to keep track of whether the next subtraction is
    // unary (e.g. 4 * -1 vs. 4 - 1)
    bool unary_subtraction;
    
    // Stack of output tokens
    std::list<parse_token> output;
    
    // Stack of operator tokens
    std::list<parse_token> operators;
    
    // Cached nodes
    std::vector<std::vector<std::list<Node*> > > node_cache;
    
    // Stacks for X, Y, and Z (to add support from "map" operation)
    std::list<Node*> currentX;
    std::list<Node*> currentY;
    std::list<Node*> currentZ;
    
    // Store where we are in a map operation.
    char map_state;
    
    // New nodes (for partway through a map operation)
    Node* newX;
    Node* newY;
    Node* newZ;
};

// Print operator for token type enum
std::ostream& operator<<(std::ostream& o, const Parser::token_type& t);

// Print operator for io type enum
std::ostream& operator<<(std::ostream& o, const Parser::io_type& t);


/* int get_precedence(opcode op)
 *
 *  Returns the precedence of an operation.  Lower values are equivalent
 *  to a higher precedence.
 */
int get_precedence(opcode op);


/* Parser::token_type get_token_type(opcode op)
 *
 *  From an opcode, return a token type for the parser's use.
 */
Parser::token_type get_token_type(opcode op);


/* node_type get_node_type(opcode op)
 *
 *  From an opcode, return a node type.
 */
node_type get_node_type(opcode op);


/* Parser::io_type get_input(opcode op)
   Parser::io_type get_output(opcode op)
 *
 *  Find a node's preferred input and output formats.
 */
Parser::io_type get_input(opcode op);
Parser::io_type get_output(opcode op);


/* int get_argcount(opcode op)
 *
 *  Returns the number of arguments required by a node.
 */
int get_argcount(opcode op);


/* char get_associativity(opcode op)
 *
 *  Returns 'l' or 'r' for left or right associative operators.
 */
char get_associativity(opcode op);


#endif