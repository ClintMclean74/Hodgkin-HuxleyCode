#include <math.h>

#include "GL/glut.h"
#include "GraphicsUtilities.h"


namespace GraphicsUtilities
{
    void DrawTextStr(char *string, float x, float y, float z, float scale, float angle)
    {
        glPushMatrix();

        char *c;
        glMatrixMode(GL_MODELVIEW);

        glTranslatef(x, y, z);
        glScalef(scale, scale, scale);
        glRotatef(angle, 0, 0, 1);

        for (c = string; *c != '\0'; c++)
        {
            glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
        }

        glPopMatrix();
    }

    void DrawCircle(float cx, float cy, float r, int num_segments, bool filled)
    {
        if (filled)
            glBegin(GL_TRIANGLE_FAN);
        else
            glBegin(GL_LINE_LOOP);

        for (int ii = 0; ii < num_segments; ii++)
        {
            float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle
            float x = r * cos(theta);//calculate the x component
            float y = r * sin(theta);//calculate the y component
            glVertex2f(x + cx, y + cy);//output vertex
        }
        glEnd();
    }
}
