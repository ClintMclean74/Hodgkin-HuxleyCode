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

#ifndef GRAPHICSANDUI_H
#define GRAPHICSANDUI_H

#include "Vector.h"

class GraphicsAndUI
{
    public:
        static char graphicsText[10000];

        static Vector originalViewPos;
        static double originalViewFov;

        static Vector pos;
        static double fov;

        static double xRot;
        static double yRot;

        static double rotScale;
        static double translateScale;
        static double zoomScale;

        static int prevX;
        static int prevY;

        static int currentMouseButton;

        static bool setVoltage;

        static bool incNoise;
        static bool decNoise;
        static bool resetNoise;
        static bool prevIncNoise;

        static bool incStimulus;
        static bool decStimulus;
        static bool resetStimulus;

        static bool drawingGraphics;

        static bool decSpeed;
        static bool incSpeed;

        static bool nnVisible;
        static bool graphsVisible;
        static bool agentsVisible;
        static bool connectionsVisible;
        static bool paused;

        GraphicsAndUI();

        static void InitGL();
        static void DrawTextStr(char *string, float x, float y, float z, float scale = 1.0, float angle = 0.0);
        static void DrawCircle(float cx, float cy, float r, int num_segments, bool filled = false);
        static void MouseButtons(int button, int state, int x, int y);
        static void MouseMotion(int x, int y);
        static void PassiveMouseMotion(int x, int y);
        static void ProcessKey(unsigned char key, int x, int y);
        static void Resize();
        static void Render();
        static void SetNNsSameXYPosAndSetRedVoltages();

    protected:

    private:
};

#endif // GRAPHICSANDUI_H
