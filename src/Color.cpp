#include "Color.h"

Color::Color()
{
    r = 0;
    g = 0;
    b = 0;
}

Color::Color(double r, double g, double b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

void Color::Set(double r, double g, double b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

void Color::Set(Color color)
{
    this->r = color.r;
    this->g = color.g;
    this->b = color.b;
}


void Color::operator=(Color color)
{
    r = color.r;
    g = color.g;
    b = color.b;
}
