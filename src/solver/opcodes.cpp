#include <iostream>
#include <sstream>

#include "opcodes.hpp"
#include "parser.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

ostream& operator<<(ostream& o, const opcode& op)
{
    switch(op)
    {
        case OP_AND:
            o << " && "; return o;
        case OP_OR:
            o << " || "; return o;
        case OP_NOT:
            o << "!"; return o;
        case OP_LT:
            o << "<"; return o;
        case OP_LEQ:
            o << "<="; return o;
        case OP_GT:
            o << ">"; return o;
        case OP_GEQ:
            o << ">="; return o;
        case OP_NEQ:
            o << "!="; return o;
        case OP_ABS:
            o << "abs"; return o;
        case OP_COS:
            o << "cos"; return o;
        case OP_SIN:
            o << "sin"; return o;
        case OP_ACOS:
            o << "acos"; return o;
        case OP_ASIN: 
            o << "asin"; return o;
        case OP_ATAN:
            o << "atan"; return o;
        case OP_SQRT:
            o << "sqrt"; return o;
        case OP_NEGATIVE:
            o << "-"; return o;
        case OP_EXP:
            o << "exp"; return o;
        case OP_SGN:
            o << "sgn"; return o;
        case OP_PLUS:
            o << "+"; return o;
        case OP_MINUS: 
            o << "-"; return o;
        case OP_MULT:
            o << "*"; return o;
        case OP_DIV: 
            o << "/"; return o;
        case OP_ATAN2:
            o << "atan2"; return o;
        case OP_POW:
            o << "pow"; return o;
        case OP_MIN:
            o << "min"; return o;
        case OP_MAX:
            o << "max"; return o;
        case NUM_CONST:
            return o;
        case VAR_X:
            o << "X"; return o;
        case VAR_Y:
            o << "Y"; return o;
        case VAR_Z:
            o << "Z"; return o;
        case COLOR_AND:
            o << "&"; return o;
        case COLOR_OR:
            o << "|"; return o;
        case COLOR_NOT:
            o << "~"; return o;
        case BOOL2NUM:
            o << "bool2num"; return o;
        case NUM2BOOL:
            o << "num2bool"; return o;
        case BOOL2COLOR:
            o << "bool2color"; return o;
        case NUM2COLOR:
            o << "num2color"; return o;
        default:
            o << "???"; return o;
    }
    return o;
}


////////////////////////////////////////////////////////////////////////////////

string dot_color(opcode op)
{
    if (op == VAR_X || op == VAR_Y || op == VAR_Z)
        return "red";
    else if (op == NUM_CONST)
        return "orangered";
        
    switch (get_output(op)) {
        case Parser::IO_NUM:
            return "goldenrod";
        case Parser::IO_BOOL:
            return get_input(op) == Parser::IO_NUM ? "green" : "dodgerblue";
        case Parser::IO_COLOR:
            return "palevioletred2";
        default:
            return "black";
    }
}

string dot_arrow(opcode op)
{
    switch (get_input(op)) {
        case Parser::IO_NUM:
            return "darkgoldenrod";
        case Parser::IO_BOOL:
            return "dodgerblue4";
        case Parser::IO_COLOR:
            return "palevioletred4";
        default:
            return "grey"; 
    }
}

string dot_shape(opcode op)
{
    if (op == NUM_CONST)
        return "oval";
    else if (get_token_type(op) == Parser::TOKEN_FUNC)
        return "rectangle";
    else
        return "square";
}

string dot_label(opcode op)
{
    stringstream ss;
    
    switch (op) {
        case OP_NEGATIVE:
        case OP_MINUS:
            return "−";
        case OP_MULT:
            return "×";
        default:
            ss << op;
    }
    
    return ss.str();
}   

