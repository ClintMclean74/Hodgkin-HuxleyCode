#ifndef COLOR_H
#define COLOR_H

class Color
{
    public:
    double r, g, b;

    Color();
    Color(double r, double g, double b);
    void Set(double r, double g, double b);
    void Set(Color color);
    void operator=(Color color);
};

#endif // COLOR_H
