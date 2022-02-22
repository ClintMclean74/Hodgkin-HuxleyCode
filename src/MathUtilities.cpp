#ifdef _DEBUG
 #define _ITERATOR_DEBUG_LEVEL 2
#endif
#include <string>
#include <math.h>
#include "pstdint.h"
#include "MathUtilities.h"

namespace MathUtilities
{
	double Round(double value, uint8_t decimalPoints)
	{
		if (decimalPoints>0)
		{
			double shiftValue = pow(10.0, (int) decimalPoints);

			////return ((int32_t)(value * shiftValue + 0.5)) / shiftValue;
			return ((int32_t)(value * shiftValue)) / shiftValue;
		}
		else
			return ((int32_t)(value + 0.5));
	}

	double Limit(double value, double range)
	{
	    if (value > range)
            value = range;
        else
            if (value < -range)
                value = -range;

        return value;
	}

	double Distance(Vector pos1, Vector pos2)
	{
	    Vector pos3 = pos1 - pos2;

	    return pos3.Length();
	}
}
