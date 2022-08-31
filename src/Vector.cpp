/*
 * Code System for the book "Solving Havana Syndrome and Biological Effects of RF
 * Using the Hodgkin-Huxley Neuron Model"
 * Copyright (C) 2022 by Clint Mclean <clint@mcleanresearchinstitute.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    double dot = x*vector.x + y*vector.y;
    double det = x*vector.y - y*vector.x;
    double angle = atan2(det, dot);

    return -angle;
}
