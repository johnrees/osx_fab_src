#ifndef TRANSLATOR_NODES
#define TRANSLATOR_NODES

#include "node.hpp"
#include "node_macro.hpp"

// This header file defines nodes that take in inputs of
// one type and return outputs of another type.
TRANSLATOR(BoolToNum, UnaryNode);
TRANSLATOR(NumToBool, UnaryNode);
TRANSLATOR(BoolToColor, UnaryNode);
TRANSLATOR(NumToColor, UnaryNode);

#endif