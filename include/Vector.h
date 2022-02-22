#ifndef VECTOR_H
#define VECTOR_H

class Vector
{
    public:
    double x, y, z;

    Vector();
    Vector(Vector* pos);
    Vector(double x, double y, double z);
    bool operator==(Vector vector);
    void operator=(Vector vector);
    Vector operator+(Vector vector);
    Vector operator-(Vector vector);
    Vector operator*(double scale);
    double Length();
    void Set(double x, double y, double z);
    void Rotate(double angle);
    double Dot(Vector vector);
    double CalculateAngle(Vector vector);

};

#endif // VECTOR_H
