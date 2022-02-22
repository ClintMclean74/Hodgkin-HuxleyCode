#include "Food.h"
#include "GL/glew.h"
#include "MathUtilities.h"

Food::Food()
{
    ////radius = Food::radius;

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
