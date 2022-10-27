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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Food.h"
#include "GL/glew.h"
#include "MathUtilities.h"

Food::Food()
{
    color.Set(0, 1, 0);

    agentIndex = 0;
}

bool Food::WithinRadius(Vector pos, double withinRadius)
{
    if (MathUtilities::Distance(this->pos, pos)<(radius + withinRadius))
        return true;
}

void Food::Draw()
{
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);

    glColor3f(color.r, color.g, color.b);

    if (!eaten)
    {
        glBegin(GL_TRIANGLES);
            glVertex3f(0, radius/2, 0);
            glVertex3f(radius, 0, 0);
            glVertex3f(0, -radius/2, 0);

            glVertex3f(0, -radius/2, 0);
            glVertex3f(-radius, 0, 0);
            glVertex3f(0, radius/2, 0);

            glVertex3f(-radius/2, 0, 0);
            glVertex3f(radius/2, 0, 0);
            glVertex3f(0, radius, 0);

            glVertex3f(radius/2, 0, 0);
            glVertex3f(-radius/2, 0, 0);
            glVertex3f(0, -radius, 0);
        glEnd();
    }
    else
    {
        glBegin(GL_LINE_STRIP);
            glVertex3f(0, radius/2, 0);
            glVertex3f(radius, 0, 0);
            glVertex3f(0, -radius/2, 0);
        glEnd();

        glBegin(GL_LINE_STRIP);
            glVertex3f(0, -radius/2, 0);
            glVertex3f(-radius, 0, 0);
            glVertex3f(0, radius/2, 0);
        glEnd();

        glBegin(GL_LINE_STRIP);
            glVertex3f(-radius/2, 0, 0);
            glVertex3f(0, radius, 0);
            glVertex3f(radius/2, 0, 0);
        glEnd();

        glBegin(GL_LINE_STRIP);
            glVertex3f(radius/2, 0, 0);
            glVertex3f(0, -radius, 0);
            glVertex3f(-radius/2, 0, 0);
        glEnd();
    }

	glPopMatrix();
}

Food::~Food()
{
}

double Food::radius = 100;

Color Food::FoodDefaultColor(0, 1, 0);
