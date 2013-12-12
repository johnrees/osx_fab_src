#ifndef OPCODES_H
#define OPCODES_H

#include <iostream>
#include <string>

// opcode defines which operator a node is performing.
enum opcode {  
    // Logical operations
    OP_AND = 0, OP_OR, OP_NEQ, OP_NOT,
    
    // Transition operations
    OP_LT, OP_LEQ, OP_GT, OP_GEQ,
    
    // Numerical unary operations
    OP_ABS, OP_COS, OP_SIN, OP_ACOS, OP_ASIN, OP_ATAN,
    OP_SQRT, OP_NEGATIVE, OP_EXP, OP_SGN,
    
    // Numeric binary operations
    OP_PLUS, OP_MINUS, OP_MULT, OP_DIV, OP_ATAN2, OP_POW,
    OP_MIN, OP_MAX,
    
    // Variables and constants
    NUM_CONST, VAR_X, VAR_Y, VAR_Z,
    
    // Color operations
    COLOR_AND, COLOR_OR, COLOR_NOT,
    
    // Translator nodes
    BOOL2NUM, NUM2BOOL, BOOL2COLOR, NUM2COLOR,
    
    NOP_UNKNOWN,
    LAST_OP
};
std::ostream& operator<<(std::ostream& o, const opcode& i);

// Overarching node types
enum node_type {
    NODE_NUM,
    NODE_TRANS,
    NODE_LOGIC,
    NODE_COLOR,
    NODE_UNKNOWN
};



/* std::string dot_color(opcode op)
   std::string dot_arrow(opcode op)
   std::string dot_shape(opcode op)
   std::string dot_label(opcode op)
 *
 *  Returns various format strings for graphviz/dot file output.
 */
std::string dot_color(opcode op);
std::string dot_arrow(opcode op);
std::string dot_shape(opcode op);
std::string dot_label(opcode op);

#endif