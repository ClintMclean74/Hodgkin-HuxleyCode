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

#ifndef NN_Connections_H
#define NN_Connections_H TRUE

#include "pstdint.h"
#include "MembraneVoltageGraph.h"
#include "NN_Connection.h"
#include "../include/Vector.h"


class NN_Connections
{
    public:
        static const uint32_t Max_Connections = 100;
        static double MutationRate;
        static double MUTATION_CONNECTION_PERCENTAGE;
        static double MAX_GRAPHICS_CONNECTION_THICKNESS_NEAR;
        static double MAX_GRAPHICS_CONNECTION_THICKNESS_FAR;
        static double MAX_GRAPHICS_CONNECTION_THICKNESS;
        double scale;
        uint32_t count;

        NN_Connection_Ptr* connections;
        NN_Connections();
        void Load(FILE *file);
        void Save(FILE* file);
        void CopyWeights(NN_Connections* srcConnections, int32_t replaceColumn = -1, int32_t newColumn = -1);
        void Mutate(double rate);
        void Add(uint32_t column, uint32_t row, long double synapseConductance);
        void SetSynapseConductance(long double value);
        long double GetExcitorySynapseConductance(void* neurons);
        long double GetInhibitorySynapseConductance(void* neurons);
        long double GetCurrentSignalingConductance(void* neurons, double maxSynapseConductanceForArea);
        double GetStrongestABSWeight();
        Vector GetMinMaxWeight();
        void Draw(void* parent, void* neurons, double maxABSWeight);
        ~NN_Connections();
};

#endif //NN_Connections_H
