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

#ifdef _DEBUG
 #define _ITERATOR_DEBUG_LEVEL 2
#endif
#include <string>
#include <math.h>
#include "pstdint.h"
#include "MathUtilities.h"

namespace MathUtilities
{
	long double Round(long double value, uint8_t decimalPoints)
	{
	    if (decimalPoints >= 100)
            return value;

        long double result;

		if (decimalPoints>0)
		{
			long double shiftValue = pow(10.0, (int) decimalPoints);

			result = ((int32_t)((value * shiftValue)+0.5)) / shiftValue;

			////result += 0.00000000000001; //fixes some floating point errors, i.e, 18.0 / 100.0

			return result;
		}
		else
        {
            result = ((int32_t)(value + 0.5));

			result += 0.0000000000000000001; //fixes some floating point errors, i.e, 18.0 / 100.0

			return result;
        }
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
