#include <algorithm>
#include <cstdlib>
#include <list>

#include "logic_nodes.hpp"
#include "numeric_nodes.hpp"
#include "translator_nodes.hpp"
#include "color_nodes.hpp"

#include "parser.hpp"
#include "opcodes.hpp"
#include "switches.hpp"

using namespace std;

Parser::parse_token::parse_token()
        : n(NULL), num_args(0), precedence(100),
          ttype(TOKEN_EMPTY), input(IO_NONE), output(IO_NONE)
    {
        // Nothing to do here.
    }
        
Parser::parse_token::parse_token(token_type type)
        : n(NULL), num_args(0), precedence(100),
          ttype(type), input(IO_NONE), output(IO_NONE)
    {
        // Nothing to do here.
    }
        
Parser::parse_token::parse_token(Node* n)
        : n(n),
          num_args(get_argcount(n->op())),
          precedence(get_precedence(n->op())),
          ttype(get_token_type(n->op())),
          input(get_input(n->op())),
          output(get_output(n->op())),
          associativity(get_associativity(n->op()))
    {
        // Nothing to do here.
    }


ostream& operator<<(ostream& o, const Parser::token_type& t)
{
    switch (t) {
        case Parser::TOKEN_NUM:
            o << "TOKEN_NUM"; return o;
        case Parser::TOKEN_FUNC:
            o << "TOKEN_FUNC"; return o;
        case Parser::TOKEN_OP:
            o << "TOKEN_OP"; return o;
        case Parser::TOKEN_LPARENS:
            o << "TOKEN_LPARENS"; return o;
        case Parser::TOKEN_ARGSEP:
            o << "TOKEN_ARGSEP"; return o;
        case Parser::TOKEN_RPARENS:
            o << "TOKEN_RPARENS"; return o;
        case Parser::TOKEN_ERROR:
            o << "TOKEN_ERROR"; return o;
        case Parser::TOKEN_EMPTY:
        default:
            o << "TOKEN_EMPTY"; return o;
    }
}

ostream& operator<<(ostream& o, const Parser::io_type& t)
{
    switch (t) {
        case Parser::IO_NUM:
            o << "IO_NUM"; return o;
        case Parser::IO_BOOL:
            o << "IO_BOOL"; return o;
        case Parser::IO_COLOR:
            o << "IO_COLOR"; return o;
        case Parser::IO_NONE:
        default:
            o << "IO_NONE"; return o;
    }
}


Parser::Parser()
    : math_string(NULL), start(NULL), unary_subtraction(true), map_state(0),
      newX(NULL), newY(NULL), newZ(NULL)
{
    // Stores plain old X, Y, and Z as the current
    // X, Y, and Z coordinates.
    currentX.push_front(new VarX());
    cache_node(currentX.front());
    
    currentY.push_front(new VarY());
    cache_node(currentY.front());
        
    currentZ.push_front(new VarZ());
    cache_node(currentZ.front());
}

void Parser::wrap_argument(parse_token& arg, io_type desired)
{
    if (!arg.n || arg.output == desired)
        return;
        
    // Transform numerical nodes into logical nodes.
    if (desired == IO_BOOL && arg.output == IO_NUM) {
        arg.n = new NumToBool(arg.n);
        arg.output = IO_BOOL;
        
    // Transform booleans into numbers
    } else if (desired == IO_NUM && arg.output == IO_BOOL) {
        arg.n = new BoolToNum(arg.n);
        arg.output = IO_NUM;

    // Transform numbers into colors
    } else if (desired == IO_COLOR && arg.output == IO_NUM) {
        arg.n = new NumToColor(arg.n);
        arg.output = IO_COLOR;    

    // Transform booleans into colors
    } else if (desired == IO_COLOR && arg.output == IO_BOOL) {
        arg.n = new BoolToColor(arg.n);
        arg.output = IO_COLOR;
    
    // If we can't wrap the operator, then return false.
    } else {
        cerr << "Error: node " << *(arg.n) << " has output of type "
             << arg.output << ", which cannot be converted to "
             << desired << endl;
        exit(1);
    }
    
    cache_node(arg.n);
}


void print_list(list<Parser::parse_token> L, ostream& o)
{
    list<Parser::parse_token>::iterator it;
    int limit = 10;
    for (it = L.begin(); it != L.end(); ++it) {
        if (it->ttype == Parser::TOKEN_LPARENS)
            o << "\t(\n";
        else if (it->ttype == Parser::TOKEN_RPARENS)
            o << "\t)\n";
        else if (it->ttype == Parser::TOKEN_ARGSEP)
            o << "\t,\n";
        else if (it->n)
            o << "\t" << *(it->n) << "\n";
        if (--limit == 0)
            return;
    }
}


void print_parse(list<Parser::parse_token> output,
                 list<Parser::parse_token> operators,
                 ostream& o)
{
    for(int i = 0; i < 80; ++i)
        o << '-';
    o << endl;
    o << "Output stack:\n";
    print_list(output, o);
    o << "Operators stack:\n";
    print_list(operators, o);
    o << endl;
}

void print_parse(list<Parser::parse_token> output,
                 list<Parser::parse_token> operators)
{
    print_parse(output, operators, cout);
}


// Stores a node in the cache, uniquifying if COMBINE_NODES is enabled.
void Parser::cache_node(Node*& node)
{
    // If this is a dummy node of some kind (e.g. parenthesis,
    // argument separator), then don't do anything.
    if (!node)
        return;
        
    opcode op = node->op();
    unsigned weight = node->get_weight();
    
    // If we don't have a cache line for this weight, then expand the cache,
    // filling new lines with a vector of node lists.
    if (weight+1 > node_cache.size())
        node_cache.resize(weight+1, vector<list<Node*> >(LAST_OP));
    
#if COMBINE_NODES
    list<Node*>::iterator it;
    Node* match = NULL;

    // Look for matches
    for (it = node_cache[weight][op].begin();
         it != node_cache[weight][op].end();
         ++it) {
        if (**it == *(node)) {
            match = *it;
            break;
        }
    }

    // If we found a match, delete the old node and replace it with
    // the matching node.
    if (match) {
        if (node != match) {
            delete node;
            node = match;
        }
    } else {
        node_cache[weight][op].push_front(node);
    }
#else
    node_cache[weight][op].push_front(node);
#endif
}


// Go through the node cache and delete nodes with no references
void Parser::remove_ignored()
{
    list<Node*>::iterator it;
    bool keep_going;
    
    do {
        keep_going = false;

        // Iterate through each list in the node cache
        for (unsigned weight = 0; weight < node_cache.size(); ++weight) {
            for (int op = 0; op < LAST_OP; ++op) {
                it = node_cache[weight][op].begin();
                
                // Iterate through each node in this cache
                while (it != node_cache[weight][op].end()) {
                
                    // If the node has no one watching it, then
                    // deactivate and delete it
                    if ((**it).ignored()) {
                        (**it).deactivate();
                        delete *it;
                        it = node_cache[weight][op].erase(it);
                        keep_going = true;
                    } else {
                        ++it;
                    }
                }
            } // end of loop through operators
        } // end of loop through weights

    } while (keep_going);
}
    

// Copies the entire cache to a MathTree
MathTree* Parser::cache_to_tree(Node* root)
{
    MathTree* tree = new MathTree();
    list<Node*>::iterator it;
    for (unsigned weight = 0; weight < node_cache.size(); ++weight)
        for (int op = 0; op < LAST_OP; ++op)
            for (it = node_cache[weight][op].begin();
                 it != node_cache[weight][op].end(); ++it)
                tree->add(*it);
    tree->pack();
    tree->set_root(root);
    cout << "Done." << endl;
    return tree;
}


void Parser::convert_colors(parse_token& oper,
                            parse_token& lh_arg,
                            parse_token& rh_arg)
{
    // If we find a boolean multiplied by a numerical value, then
    // we treat it as a color creation.
    if (oper.n->op() == OP_MULT &&
         ((rh_arg.output == IO_NUM && lh_arg.output == IO_BOOL) ||
          (lh_arg.output == IO_NUM && rh_arg.output == IO_BOOL)) )
    {
        delete oper.n;
        oper.n = new ColorAnd();
        oper.input = IO_COLOR;
        oper.output = IO_COLOR;
    }
    // Logical OR on colors -> bitwise or
    if (oper.n->op() == OP_OR && (rh_arg.output == IO_COLOR ||
                                          lh_arg.output == IO_COLOR))
    {
        delete oper.n;
        oper.n = new ColorOr();
        oper.input = IO_COLOR;
        oper.output = IO_COLOR;
    }
    // Logical AND on colors -> bitwise and
    if (oper.n->op() == OP_AND && (rh_arg.output == IO_COLOR ||
                                          lh_arg.output == IO_COLOR))
    {
        delete oper.n;
        oper.n =  new ColorAnd();
        oper.input = IO_COLOR;
        oper.output = IO_COLOR;
    }
    // Logical not on colors -> bitwise not
    if (oper.n->op() == OP_NOT && rh_arg.output == IO_COLOR)
    {
        delete oper.n;
        oper.n =  new ColorNot();
        oper.input = IO_COLOR;
        oper.output = IO_COLOR;
    }
}


bool Parser::simplify(parse_token& oper,
                      parse_token& lh_arg,
                      parse_token& rh_arg)
{
    // We're only going to simplify numeric nodes to keep life easy.
    if (oper.input != IO_NUM)
        return false;
    
    opcode op = oper.n->op();
    
    if (op == OP_PLUS) {
        // 0 + X => X
        if (lh_arg.n->marked && lh_arg.n->result_float == 0) {
            delete oper.n;
            oper.n = rh_arg.n;
            return true;
        }
        // X + 0 => X
        if (rh_arg.n->marked && rh_arg.n->result_float == 0) {
            delete oper.n;
            oper.n = lh_arg.n;
            return true;
        }
    } else if (op == OP_MINUS) {
        // X - 0 => X
        if (rh_arg.n->marked && rh_arg.n->result_float == 0) {
            delete oper.n;
            oper.n = lh_arg.n;
            return true;
        }
    } else if (op == OP_MULT) {
        // X * 0 => 0
        if (rh_arg.n->marked && rh_arg.n->result_float == 0) {
            delete oper.n;
            oper.n = rh_arg.n;
            return true;
        }
        // 0 * X => 0
        if (lh_arg.n->marked && lh_arg.n->result_float == 0) {
            delete oper.n;
            oper.n = lh_arg.n;
            return true;
        }
        // X * 1 => X
        if (rh_arg.n->marked && rh_arg.n->result_float == 1) {
            delete oper.n;
            oper.n = lh_arg.n;
            return true;
        }
        // 1 * X => X
        if (lh_arg.n->marked && lh_arg.n->result_float == 1) {
            delete oper.n;
            oper.n = rh_arg.n;
            return true;
        }
    }

    return false;
}


// operator_to_output
//
//      Find out how many arguments the operation wants to take from
//      the output stack
//      Take them off the stack and make them the operator's children
//      Put the new operation on the output stack.
void Parser::operator_to_output(parse_token& oper)
{
    if (oper.ttype == TOKEN_LPARENS || oper.ttype == TOKEN_MAP_START)
        return;
    
    // If this node's children have already been populated, then
    // we can skip the song and dance about pulling them from the
    // stack.
    if (oper.n && oper.n->null_children() == false) {
        cache_node(oper.n);
        output.push_front(oper);
        return;
    }
    
    parse_token rh_arg;
    parse_token lh_arg;
    
    // Handle the first argument
    if (oper.num_args >= 1) {
        if (output.empty()) {
            cerr << "Parse failed:\n\t'" << *(oper.n)
                 << "' failed to acquire its first operand." << endl;
            exit(1);
        }
        rh_arg = output.front();
        output.pop_front();
    }
    
    // Handle the second argument
    if (oper.num_args >= 2) {
        if (output.empty()) {
            cerr << "Parse failed:\n\t'" << *(oper.n)
                 << "' failed to acquire its second operand." << endl;
            exit(1);
        }
        lh_arg = output.front();
        output.pop_front();
    }   
    
    // Set of special cases to handle colors correctly.
    convert_colors(oper, lh_arg, rh_arg);

    // If the argument outputs are of a different type then
    // the operator's inputs, wrap them in a lightweight
    // wrapper classes to do the conversion.
    wrap_argument(lh_arg, oper.input);
    wrap_argument(rh_arg, oper.input);

    // Simplify common arithmetic expressions (e.g. X + 0 => X)
    bool simplified = false;
#if SIMPLIFY_TREE
    simplified = simplify(oper, lh_arg, rh_arg);
#endif

    // If this node has been simplified, then we don't need to worry
    // about assigning children or caching (since the node is already
    // cached).
    if (!simplified) {
        if (oper.num_args == 1)
            static_cast<UnaryNode*>(oper.n)->set_child(rh_arg.n);
        else if (oper.num_args == 2)
            static_cast<BinaryNode*>(oper.n)->set_children(lh_arg.n,
                                                           rh_arg.n);
        cache_node(oper.n);
    }
    
    output.push_front(oper);
}


// operator_to_stack
//
//      Add an operator to the operator stack, while checking if precedence
//      requires removing other operators from the top of the stack.
void Parser::operator_to_stack(parse_token& o1)
{   
    while (!operators.empty())
    {
        parse_token o2 = operators.front();
        if ((o1.associativity == 'l' && o1.precedence >= o2.precedence) ||
            (o1.associativity == 'r' && o1.precedence > o2.precedence))
        {
            operators.pop_front();
            operator_to_output(o2);
        } else
            break;
    }
    operators.push_front(o1);
}


bool str_match(const char* s1, const char* s2)
{
    do
        if (!*s1 || *s1++ != *s2++)
            return false;
    while (*s2);
    
    return true;
}

Parser::parse_token Parser::next_token()
{
    bool next_unary_subtraction = true;
    
    parse_token result;
    
    if (*start == '(') {
        start += 1;
        result = TOKEN_LPARENS;
    }
    else if (str_match(start, "{")) {
        start += 1;
        result = TOKEN_MAP_START;
    }
    else if (str_match(start, "X:")) {
        start +=2;
        result = TOKEN_MAP_DEF;
        result.n = currentX.front();
    }
    else if (str_match(start, "Y:")) {
        start +=2;
        result = TOKEN_MAP_DEF;
        result.n = currentY.front();
    }
    else if (str_match(start, "Z:")) {
        start +=2;
        result = TOKEN_MAP_DEF;
        result.n = currentZ.front();
    }
    else if (str_match(start, ";")) {
        start += 1;
        result = TOKEN_MAP_SEPARATOR;
    }
    else if (str_match(start, "}")) {
        start += 1;
        result = TOKEN_MAP_END;
    }
    else if (*start == ')') {
        start += 1;
        next_unary_subtraction = false;
        result = TOKEN_RPARENS;
    } else if (*start == 'X' || *start == 'x') {
        start += 1;
        next_unary_subtraction = false;
        result = currentX.front();
        result.ttype = TOKEN_NUM;
    } else if (*start == 'Y' || *start == 'y') {
        start += 1;
        next_unary_subtraction = false;
        result = currentY.front();
        result.ttype = TOKEN_NUM;
    } else if (*start == 'Z' || *start == 'z') {
        start += 1;
        next_unary_subtraction = false;
        result = currentZ.front();
        result.ttype = TOKEN_NUM;
    }
    else if (*start == '-') {
        start += 1;
        if (unary_subtraction) {
            result = new NumericNeg();
        } else {
            result = new NumericMinus();
        }
    }
    else if (*start == ',') {
        start += 1;
        result = TOKEN_ARGSEP;
    }
    else if (*start == '*') {
        start += 1;
        result = new NumericMult();
    }
    else if (*start == '+') {
        start += 1;
        result = new NumericPlus();
    }
    else if (*start == '/') {
        start += 1;
        result = new NumericDiv();
    }
    else if (str_match(start, "atan2")) {
        start += 5;
        result = new NumericATan2();
    }
    else if (str_match(start, "pow")) {
        start += 3;
        result = new NumericPow();
    }
    else if (str_match(start, "min")) {
        start += 3;
        result = new NumericMin();
    }
    else if (str_match(start, "max")) {
        start += 3;
        result = new NumericMax();
    }
    else if (str_match(start, "exp")) {
        start += 3;
        result = new NumericExp();
    }
    else if (str_match(start, "sgn")) {
        start += 3;
        result = new NumericSgn();
    }
    else if (str_match(start, "abs")) {
        start += 3;
        result = new NumericAbs();
    }
    else if (str_match(start, "cos")) {
        start += 3;
        result = new NumericCos();
    }
    else if (str_match(start, "sin")) {
        start += 3;
        result = new NumericSin();
    }
    else if (str_match(start, "acos")) {
        start += 4;
        result = new NumericACos();
    }
    else if (str_match(start, "asin")) {
        start += 4;
        result = new NumericASin();
    }
    else if (str_match(start, "atan")) {
        start += 4;
        result = new NumericATan();
    }
    else if (str_match(start, "sqrt")) {
        start += 4;
        result = new NumericSqrt();
    }
    else if (str_match(start, "<=")) {
        start += 2;
        result = new TransitionLeq();
    }
    else if (str_match(start, "<")) {
        start += 1;
        result = new TransitionLt();
    }
    else if (str_match(start, ">=")) {
        start += 2;
        result = new TransitionGeq();
    }
    else if (str_match(start, ">")) {
        start += 1;
        result = new TransitionGt();
    }
    else if (str_match(start, "!=")) {
        start += 2;
        result = new LogicNeq();
    }
    else if (*start == '!' || *start == '~') {
        start += 1;
        result = new LogicNot();
    }
    else if (str_match(start, "&&"))
    {
        start += 2;
        result = new LogicAnd();
    }
    else if (*start == '&')
    {
        start += 1;
        result = new LogicAnd();
    }
    else if (str_match(start, "||"))
    {
        start += 2;
        result = new LogicOr();
    }
    else if (*start == '|')
    {
        start += 1;
        result = new LogicOr();
    }
    else if (str_match(start, "pi"))
    {
        start += 2;
        result = new NumericConst(3.14159265358979323846);
    }
    else if ((*start >= '0' && *start <= '9') || *start == '.')
    {
        
        // Accumulated value
        float v = 0;
        
        // Divided value once we're after the decimal place
        double divider = 0;
        do {
            if (*start == '.')
                divider = 10;
            else if (divider) {
                v += float(*start - '0') / divider;
                divider *= 10;
            } else {
                v = v * 10 + (*start - '0');
            }
        
            start++;
        } while ((*start >= '0' && *start <= '9') || *start == '.');
        
        // Check for scientific notation.
        if (*start == 'e') {
            int e = 0;
            bool neg_exp = false;
            start++;
            if (*start == '-') {
                neg_exp = true;
                start++;
            } else if (*start == '+') {
                neg_exp = false;
                start++;
            }
            do {
                e = e * 10 + (*start++ - '0');
            } while (*start >= '0' && *start <= '9');
            // Negate the exponent if needed.
            e = e * (neg_exp ? -1 : 1);
            v = v * pow(10.0, e);
        }
        
        next_unary_subtraction = false;
        result = new NumericConst(v);
    }
    else if (*start != ' ')
    {
        cerr << "Warning:  Unknown token at '";
        if (string(start).size() > 10)
            cerr << string(start, start + 10) << "...'" << endl;
        else
            cerr << string(start) << "'" << endl;
        start++;
        return parse_token();
    }
    
    // Zoom along until we're staring at something useful.
    while (*start == ' ')
        start++;
    
    unary_subtraction = next_unary_subtraction;

    return result;
}


MathTree* Parser::parse(string input, solver_mode& mode)
{
    parse_token current;

    cout << "Parsing... ";
    cout.flush();

    // Convert into a c-style string    
    math_string = input.c_str();
    start = math_string;
    
    unary_subtraction = true;
    
    while (*start)
    {     
        current = next_token();
            
        // If there was a parse failure, then return.
        if (current.ttype == TOKEN_ERROR)
            return NULL;
            
        // If this token was empty, then continue.
        if (current.ttype == TOKEN_EMPTY)
            continue;
            
        if (current.ttype == TOKEN_NUM)
            operator_to_output(current);
            
        else if (current.ttype == TOKEN_FUNC)
            operators.push_front(current);
            
        else if (current.ttype == TOKEN_MAP_START) {
        
            if (map_state) {
                cerr << "Parse error: badly nested map statement."
                     << endl;
                exit(1);
            }
            
            // If they are using curly braces without any map commands, then
            // push trivial maps to the stacks.
            if (*start && *(start+1) == ':') {
                map_state = '?';
            } else {
                currentX.push_front(currentX.front());
                currentY.push_front(currentY.front());
                currentZ.push_front(currentZ.front());
            }
            
            operators.push_front(current);
        }
        
        else if (current.ttype == TOKEN_MAP_DEF) {
            // When we get a colon, it better be within in the context of
            // a map operation (and it should follow X,Y, or Z)
            
            if (current.n == currentX.front()) {
                if (newX || map_state != '?') {
                    cerr << "Parse error: Repeated X mapping." << endl;
                    exit(1);
                }
                map_state = 'X';
            } else if (current.n == currentY.front()) {
                if (newY || map_state != '?') {
                    cerr << "Parse error: Repeated Y mapping." << endl;
                    exit(1);
                }
                map_state = 'Y';
            } else if (current.n == currentZ.front()) {
                if (newZ || map_state != '?') {
                    cerr << "Parse error: Repeated Z mapping." << endl;
                    exit(1);
                }
                map_state = 'Z';
            }
        }

        
        else if (current.ttype == TOKEN_ARGSEP)
        {   
            if (operators.empty()) {
                cerr << "Parse error: Misplaced argument separator."
                     << endl;
                exit(1);
            }
            while (operators.front().ttype != TOKEN_LPARENS)
            {
                operator_to_output(operators.front());
                operators.pop_front();
                if (operators.empty()) {
                    cerr << "Parse error: Misplaced argument separator."
                         << endl;
                    exit(1);
                }
            }
        }
        
        
        else if (current.ttype == TOKEN_MAP_SEPARATOR) {
        
            if (!map_state) {
                cerr << "Parse error: Misplaced map argument separator."
                     << endl;
                exit(1);
            }
        
            if (operators.empty()) {
                cerr << "Parse error: Misplaced map argument separator."
                     << endl;
                exit(1);
            }
            while (operators.front().ttype != TOKEN_MAP_START)
            {
                operator_to_output(operators.front());
                operators.pop_front();
                if (operators.empty()) {
                    cerr << "Parse error: Misplaced map argument separator."
                         << endl;
                    exit(1);
                }
            }
        
            // If we're in the middle of a map operation, save the
            // new mapped value for X, Y, or Z.
            if (map_state == 'X') {
                newX = output.front().n;
                output.pop_front();
            } else if (map_state == 'Y') {
                newY = output.front().n;
                output.pop_front();
            } else if (map_state == 'Z') {
                newZ = output.front().n;
                output.pop_front();
            }
            
            // Check to see if another map operation is coming down the line.
            if (*start && *(start+1) == ':') {
                map_state = '?';
            } 
            
            // If not, then apply the saved maps for X, Y, and Z.
            else {
                if (newX)
                    currentX.push_front(newX);
                else
                    currentX.push_front(currentX.front());
                    
                if (newY)
                    currentY.push_front(newY);
                else
                    currentY.push_front(currentY.front());
                
                if (newZ)
                    currentZ.push_front(newZ);
                else
                    currentZ.push_front(currentZ.front());
                
                newX = NULL;
                newY = NULL;
                newZ = NULL;
                
                map_state = 0;
            }
        }
        
        
        
        else if (current.ttype == TOKEN_OP)
            operator_to_stack(current);
            
        else if (current.ttype == TOKEN_LPARENS)
            operators.push_front(current);
            
        else if (current.ttype == TOKEN_RPARENS)
        {
            if (operators.empty()) {
                cerr << "Parse error: mismatched parentheses." << endl;
                exit(1);
            }        
            while (operators.front().ttype != TOKEN_LPARENS)
            {
                operator_to_output(operators.front());
                operators.pop_front();
                if (operators.empty()) {
                    cerr << "Parse error: mismatched parentheses." << endl;
                    exit(1);
                }
            }
            operators.pop_front();
            
            // If there's a function on top of the stack, then it belongs
            // with the set of parentheses.
            if (!operators.empty() && operators.front().ttype == TOKEN_FUNC) {
                operator_to_output(operators.front());
                operators.pop_front();
            }
        }
        
        else if (current.ttype == TOKEN_MAP_END)
        {
            if (operators.empty()) {
                cerr << "Parse error: mismatched curly braces." << endl;
                exit(1);
            }        
            while (operators.front().ttype != TOKEN_MAP_START)
            {
                operator_to_output(operators.front());
                operators.pop_front();
                if (operators.empty()) {
                    cerr << "Parse error: mismatched curly braces." << endl;
                    exit(1);
                }
            }
            operators.pop_front();
            
            currentX.pop_front();
            currentY.pop_front();
            currentZ.pop_front();
        }
    }
    
    // Finish the parse by popping operators off the stack.
    while (!operators.empty())
    {
        operator_to_output(operators.front());
        operators.pop_front();
    }

    if (!output.size())
    {
        cerr << "Parse failed:\n\tEmpty math tree." << endl;
        return NULL;
    } else if (output.size() > 1) {
        cerr << "Parse failed:\n\tInvalid math string." << endl;
        return NULL;
    }

    // Make sure that the root's output type is correct; otherwise
    // return NULL.
    if (mode == SOLVE_BOOL)
        wrap_argument(output.front(), IO_BOOL);
    else if (mode == SOLVE_RGB)
        wrap_argument(output.front(), IO_COLOR);
    else if (mode == SOLVE_REAL) {
        wrap_argument(output.front(), IO_NUM);
    }
    
    // Add a watcher to the top node of the tree (so that it doesn't get
    // auto-deleted)
    Node* root = output.front().n;
    root->add_ref();

    // Delete any ignored nodes (that were optimized out of the tree)
    remove_ignored();
    
    // Convert the cache into the tree.
    return cache_to_tree(root);
}

////////////////////////////////////////////////////////////////////////////////
// Functions below this point are used to extract parsing information from node
// opcodes.
////////////////////////////////////////////////////////////////////////////////

int get_precedence(opcode op)
{
    switch (op)
    {
        case OP_NOT:
        case OP_NEGATIVE:
        case COLOR_NOT:
            return 1;
        case OP_MULT:
        case OP_DIV:
            return 2;
        case OP_PLUS:
        case OP_MINUS:
            return 3;
        case OP_LT:
        case OP_GT:
        case OP_LEQ:
        case OP_GEQ:
        case OP_NEQ:
            return 4;
        case OP_AND:
        case COLOR_AND:
            return 5;
        case OP_OR:
        case COLOR_OR:
            return 6;
        default:
            return 100;
    }
}

////////////////////////////////////////////////////////////////////////////////

// get_node_type
//
//      From an opcode, infer the type of the node.
//      (e.g. cosine implies a numerical node)
node_type get_node_type(opcode op)
{
    switch (op)
    {
        case OP_ABS:
        case OP_COS:
        case OP_SIN:
        case OP_ACOS:
        case OP_ASIN:
        case OP_ATAN:
        case OP_SQRT:
        case OP_NEGATIVE:
        case OP_EXP:
        case OP_SGN:
        case OP_PLUS:
        case OP_MINUS:
        case OP_MULT:
        case OP_DIV:
        case OP_ATAN2:
        case OP_POW:
        case OP_MIN:
        case OP_MAX:
        case VAR_X:
        case VAR_Y:
        case VAR_Z:
        case NUM_CONST:
            return NODE_NUM;
        case OP_AND:
        case OP_OR:
        case OP_NOT:
        case OP_NEQ:
            return NODE_LOGIC;
        case OP_LT:
        case OP_LEQ:
        case OP_GT:
        case OP_GEQ:
            return NODE_TRANS;
        case COLOR_AND:
        case COLOR_OR:
        case COLOR_NOT:
            return NODE_COLOR;
        default:
            cerr << "Error: unknown operator " << op << " in get_node_type." << endl;
            exit(1);
        }
}

////////////////////////////////////////////////////////////////////////////////

// get_token_type
//
//      From an opcode, infer the type of the token.
//      (e.g. cosine implies a function node)
Parser::token_type get_token_type(opcode op)
{
    switch (op)
    {
        case OP_ABS:
        case OP_COS:
        case OP_SIN:
        case OP_ACOS:
        case OP_ASIN:
        case OP_ATAN:
        case OP_SQRT:
        case OP_ATAN2:
        case OP_POW:
        case OP_MIN:
        case OP_MAX:
        case OP_EXP:
        case OP_SGN:
        case NUM2BOOL:
        case BOOL2NUM:
        case NUM2COLOR:
        case BOOL2COLOR:
            return Parser::TOKEN_FUNC;
        case OP_NEGATIVE:
        case OP_PLUS:
        case OP_MINUS:
        case OP_MULT:
        case OP_DIV:
        case OP_AND:
        case OP_OR:
        case OP_NOT:
        case OP_LT:
        case OP_LEQ:
        case OP_GT:
        case OP_GEQ:
        case OP_NEQ:
        case COLOR_OR:
        case COLOR_AND:
        case COLOR_NOT:
            return Parser::TOKEN_OP;
        case VAR_X:
        case VAR_Y:
        case VAR_Z:
        case NUM_CONST:
            return Parser::TOKEN_NUM;
        default:
            cerr << "Error: unknown operator " << op << " in get_token_type." << endl;
            exit(1);
        }
}

////////////////////////////////////////////////////////////////////////////////

Parser::io_type get_output(opcode op)
{
    // Special case for translator nodes
    switch (op) {
        case BOOL2NUM:
            return Parser::IO_NUM;
        case NUM2BOOL:
            return Parser::IO_BOOL;
        case BOOL2COLOR:
        case NUM2COLOR:
            return Parser::IO_COLOR;
        default:
            ; // Continue to the rest of the function
    }

    node_type ntype = get_node_type(op);
    switch (ntype)
    {
        case NODE_NUM:
            return Parser::IO_NUM;
        case NODE_TRANS:
        case NODE_LOGIC:
            return Parser::IO_BOOL;
        case NODE_COLOR:
            return Parser::IO_COLOR;
        default:
            return Parser::IO_NONE;
    }
}

////////////////////////////////////////////////////////////////////////////////

Parser::io_type get_input(opcode op)
{
    // Special case for translator nodes
    switch (op) {
        case BOOL2NUM:
        case BOOL2COLOR:
            return Parser::IO_BOOL;
        case NUM2BOOL:
        case NUM2COLOR:
            return Parser::IO_NUM;
        default:
            ; // Continue to the rest of the function
    }
    
    node_type ntype = get_node_type(op);
    switch (ntype)
    {
        case NODE_NUM:
        case NODE_TRANS:
            return Parser::IO_NUM;
        case NODE_LOGIC:
            return Parser::IO_BOOL;
        case NODE_COLOR:
            return Parser::IO_COLOR;
        default:
            return Parser::IO_NONE;
    }
}

////////////////////////////////////////////////////////////////////////////////

// get_argcount
//
//      From an opcode, infer the number of arguments that a node will want
//      to take (e.g. cosine wants one, atan2 wants 2)
int get_argcount(opcode op)
{
    switch (op)
    {
        case NUM_CONST:
        case VAR_X:
        case VAR_Y:
        case VAR_Z:
            return 0;
        case OP_NEGATIVE:
        case OP_EXP:
        case OP_SGN:
        case OP_ABS:
        case OP_COS: 
        case OP_SIN: 
        case OP_ACOS:
        case OP_ASIN:
        case OP_ATAN: 
        case OP_SQRT:
        case OP_NOT:
        case COLOR_NOT:
            return 1;
        default:
            return 2;
        }
}

////////////////////////////////////////////////////////////////////////////////

// get_associativity
//
//      From an opcode, return the associativity of the operation.
char get_associativity(opcode op)
{
    switch (op)
    {
        case OP_NEGATIVE:
        case OP_NOT:
        case COLOR_NOT:
            return 'r';
        default:
            return 'l';
    }
}