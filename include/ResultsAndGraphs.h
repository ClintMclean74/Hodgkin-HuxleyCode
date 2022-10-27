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

#ifndef RESULTSANDGRAPHS_H
#define RESULTSANDGRAPHS_H

#include "Graph.h"

class ResultsAndGraphs
{
    private:
        typedef double* double_ptr;

        double_ptr* electromagneticErrorsForLayerTotal;
        double_ptr* electromagneticErrorPercentageForLayerTotal;
        double_ptr* currentErrorForLayerPercentage;
        uint32_t electromagneticErrorForLayerAdditionCount = 0;

        double_ptr* totalVoltageDifferencesForLayer;
        double_ptr* totalIncreasedVoltageDifferencesForLayer;
        double_ptr* totalDecreasedVoltageDifferencesForLayer;
        double* currentIncreasedVoltageDifference;
        double* totalIncreasedVoltageDifference;
        double* totalDecreasedVoltageDifference;
        double* totalVoltageDifference;

    public:
        uint32_t voltageDifferencesSamples = 0;
        Graph* errorLayerPercentageGraph;
        Graph* voltageErrorGraph;
        Graph* increasedVoltageErrorGraph;
        Graph* decreasedVoltageErrorGraph;
        Graph* accruedErrorPercentageGraph;
        Graph* rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph;
        Graph* rfTemperatureCurrentSpikeCountGraph;
        Graph* normalTemperatureCurrentSpikeCountGraph;
        Graph* rfTargetTemperatureGraph;
        Graph* rfCurrentTemperatureGraph;

        ResultsAndGraphs();
        void InitializeGraphs();
        void SaveGraphs();

        void Draw();
        void ResetTotalVoltageDifferencesForLayerAndTotalErrorPercentageForLayerResultingFromRFTemperatureIncreases();
        void DetermineElectromagneticInducedTotalVoltageDifferences();
        void GraphCurrentExcitoryRFSpikeCount();
        void GraphCurrentRFTemperature();
        void GraphAvgRFMembraneVoltageDifferences();
        double DetermineError(char* result1, char* result2, uint32_t length);
        void DetermineLayerPercentageDifferences();
        void GraphAvgLayerPercentageDifferences();
};

#endif // RESULTSANDGRAPHS_H
