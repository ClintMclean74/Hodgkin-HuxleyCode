#pragma once

#ifndef GRAPHICSUTILITIES_H_INCLUDED
#define GRAPHICSUTILITIES_H_INCLUDED

namespace GraphicsUtilities
{
    void DrawTextStr(char *string, float x, float y, float z, float scale = 1.0, float angle = 0.0);
    void DrawCircle(float cx, float cy, float r, int num_segments, bool filled = false);
};

#endif //GRAPHICSUTILITIES_H_INCLUDED
