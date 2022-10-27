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

#ifndef AGENTS_H
#define AGENTS_H

#include "pstdint.h"
#include "Agent.h"

class Agents
{
    public:
        Agent_ptr *agents;
        static uint32_t count;

        static uint32_t maxSize;

        Agents();
        Agents(uint32_t maxSize);
        double GetTotalFitness();
        double GetAvgFitness();
        void Reset();
        void RegenerateNNConnections();
        void ResetTraces();
        void Add(Agent *agent);
        void SetTargetTemperature(long double temperature, int32_t agentIndex = -1);
        void ActivateNeuronsFromFoodAngle();
        void ProcessIfEating();
        uint32_t Process(uint32_t agentIndex = maxSize);
        void Draw();
        void saveMembraneVoltagesTraceGraph();
        virtual ~Agents();
};

#endif // AGENTS_H
