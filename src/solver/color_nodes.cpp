#include "color_nodes.hpp"

using namespace std;
////////////////////////////////////////////////////////////////////////////////
ColorAnd::ColorAnd()
    : BinaryNode(COLOR_AND)
{
    // Nothing to do here.
}

void ColorAnd::eval(const float X, const float Y, const float Z)
{
    result_color = left->result_color & right->result_color;
}

void ColorAnd::eval(const FabInterval& X,
                    const FabInterval& Y,
                    const FabInterval& Z)
{
    // Black ANDed with anything remains black.
    if (left->result_color == 0 || right->result_color == 0)
        result_color = 0;
    else if (left->result_color == -1 || right->result_color == -1)
        result_color = -1;
    else
        result_color = left->result_color & right->result_color;
    marked = (result_color != -1);
}

////////////////////////////////////////////////////////////////////////////////
ColorOr::ColorOr()
    : BinaryNode(COLOR_OR)
{
    // Nothing to do here.
}

void ColorOr::eval(const float X, const float Y, const float Z)
{
    result_color = left->result_color | right->result_color;
}

void ColorOr::eval(const FabInterval& X,
                    const FabInterval& Y,
                    const FabInterval& Z)
{
    // Pure white OR-ed with anything remains white.
    const int WHITE = 255 + (255 << 8) + (255 << 16);
    if (left->result_color == WHITE || right->result_color == WHITE)
        result_color = WHITE;
    else if (left->result_color == -1 || right->result_color == -1)
        result_color = -1;
    else
        result_color = left->result_color | right->result_color;
    marked = (result_color != -1);
}

////////////////////////////////////////////////////////////////////////////////
ColorNot::ColorNot()
    : UnaryNode(COLOR_NOT)
{
    // Nothing to do here.
}

void ColorNot::eval(const float X, const float Y, const float Z)
{
    result_color = ~child->result_color;
}

void ColorNot::eval(const FabInterval& X,
                    const FabInterval& Y,
                    const FabInterval& Z)
{
    if (child->result_color == -1)
        result_color = -1;
    else
        result_color = ~child->result_color;
    marked = (result_color != -1);
}