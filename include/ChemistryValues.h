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

#ifndef ChemistryValues_H
#define ChemistryValues_H

namespace ChemistryValues
{
    static const long double gasConstant_R = 8.31446261815324;// J⋅K^−1⋅mol^−1
    static const long double faradayConstant = 96.485; // C/mol

    static const long double Na_Valence = 1;
    static const long double K_Valence = 1;
    static const long double Cl_Valence = -1;

    static const long double lnToLogConversionFactor = 2.303;

    static const long double KELVIN = 273.15;
}
#endif
