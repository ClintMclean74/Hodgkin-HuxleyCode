#include <math.h>
#include "Vector.h"

Vector::Vector()
{
    x = 0;
    y = 0;
    z = 0;
}

Vector::Vector(Vector* pos)
{
    this->x = pos->x;
    this->y = pos->y;
    this->z = pos->z;
}

Vector::Vector(double x, double y, double z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

void Vector::operator=(Vector vector)
{
    x = vector.x;
    y = vector.y;
    z = vector.z;
}

bool Vector::operator==(Vector vector)
{
    if (x == vector.x && y == vector.y && z == vector.z)
        return true;
    else
        return false;
}

Vector Vector::operator+(Vector vector)
{
    Vector newVector(this);

    newVector.x += vector.x;
    newVector.y += vector.y;
    newVector.z += vector.z;

    return newVector;
}

Vector Vector::operator-(Vector vector)
{
    ////Vector* newVector = new Vector(this);
    Vector newVector(this);

    newVector.x -= vector.x;
    newVector.y -= vector.y;
    newVector.z -= vector.z;

    return newVector;
}

Vector Vector::operator*(double scale)
{
    Vector newVector(this);

    newVector.x *= scale;
    newVector.y *= scale;
    newVector.z *= scale;

    return newVector;
}

double Vector::Length()
{
    return sqrt(x*x + y*y + z*z);
}

void Vector::Set(double x, double y, double z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

void Vector::Rotate(double angle)
{
    double newX = x*cos(angle) - y*sin(angle);
    y = x*sin(angle) + y*cos(angle);

    x = newX;
}

double Vector::Dot(Vector vector)
{
    double dot = x*vector.x + y*vector.y + z*vector.z;

    return dot;
}

double Vector::CalculateAngle(Vector vector)
{
    /*double dot = this->Dot(vector) / (Length() * vector.Length());

    return acos(dot);
    */

    double dot = x*vector.x + y*vector.y;
    double det = x*vector.y - y*vector.x; //determinant
    double angle = atan2(det, dot); //atan2(y, x) or atan2(sin, cos)

    return -angle;
}
