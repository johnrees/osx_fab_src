#include <limits>

#include "translator_nodes.hpp"

using namespace std;
using boost::logic::indeterminate;

BoolToNum::BoolToNum()
    : UnaryNode(BOOL2NUM)
{
    // Nothing to do here.
}


BoolToNum::BoolToNum(Node* c)
    : UnaryNode(BOOL2NUM)
{
    set_child(c);
}

void BoolToNum::eval(const float X, const float Y, const float Z)
{
    if (child->result_bool)
        result_float = 1;
    else
        result_float = 0;
}

void BoolToNum::eval(const FabInterval& X,
                     const FabInterval& Y,
                     const FabInterval& Z)
{
    if (child->result_bool)
        result_interval = 1;
    else if (!child->result_bool)
        result_interval = 0;
    else
        result_interval = FabInterval(0, 1);
}

////////////////////////////////////////////////////////////////////////////////

NumToBool::NumToBool()
    : UnaryNode(NUM2BOOL)
{
    // Nothing to do here.
}


NumToBool::NumToBool(Node* c)
    : UnaryNode(NUM2BOOL)
{
    set_child(c);
}

void NumToBool::eval(const float X, const float Y, const float Z)
{
    if (child->result_float)
        result_bool = true;
    else
        result_bool = false;
}

void NumToBool::eval(const FabInterval& X,
                     const FabInterval& Y,
                     const FabInterval& Z)
{
    if (child->result_interval.lower() == 0 &&
        child->result_interval.upper() == 0)
        result_bool = false;
    else if (child->result_interval.upper() < 0 ||
             child->result_interval.lower() > 0)
        result_bool = true;
    else
        result_bool = indeterminate;
}

///////////////////////////////////////////////////////////////////////////////

BoolToColor::BoolToColor()
    : UnaryNode(BOOL2COLOR)
{
    // Nothing to do here.
}

BoolToColor::BoolToColor(Node* c)
    : UnaryNode(BOOL2COLOR)
{
    set_child(c);
}

void BoolToColor::eval(const float X, const float Y, const float Z)
{
    if (child->result_bool)
        result_color = 255 + (255 << 8) + (255 << 16);
    else
        result_color = 0;
}

void BoolToColor::eval(const FabInterval& X,
                       const FabInterval& Y,
                       const FabInterval& Z)
{
    if (child->result_bool)
        result_color = 255 + (255 << 8) + (255 << 16);
    else if (!child->result_bool)
        result_color = 0;
    else
        result_color = -1;
}

///////////////////////////////////////////////////////////////////////////////

NumToColor::NumToColor()
    : UnaryNode(NUM2COLOR)
{
    // Nothing to do here.
}

NumToColor::NumToColor(Node* c)
    : UnaryNode(NUM2COLOR)
{
    set_child(c);
}

void NumToColor::eval(const float X, const float Y, const float Z)
{
    result_color = int(child->result_float);
}

void NumToColor::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    if (child->result_interval.lower() == child->result_interval.upper())
        result_color = int(child->result_interval.lower());
    else
        result_color = -1;
}