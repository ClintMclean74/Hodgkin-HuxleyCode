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

#ifndef HH_Neuron_H
#define HH_Neuron_H TRUE

#include <iostream>
#include <cmath>

#include "ChemistryValues.h"
#include "MembraneVoltageGraph.h"
#include "NN_Connections.h"

class ErrorDuration
{
    public:
        double totalErrorDuration = 0;
        double currentErrorDurationStartTime = 0;

        double totalNeuronsInErrorCount = 0;
        double totalPatternErrorCount = 0;

        double currentSignalingDurationStartTime = 0;
        double totalSignalingDuration = 0;
};

class HH_Neuron
{
    double nonHH_Threshold;

    char text[255];

    double minVoltage;
    double maxVoltage;

    long double q10AdjustedRateScale = 1.0;

    FILE* graphVoltagesFile;

    public:
        static double MEMBRANE_VOLTAGE_THRESHOLD;
        static double MINMAX_VOLTAGE;

        bool spike = false;

        ErrorDuration errorsToRefNeuron;

        uint32_t spikeCount;
        uint32_t printCount;

        void* neuralNetwork;

        ////uint32_t signalling;

        NN_Connections connections;
        double bias = 0;

        double rndThresholdValue = 0.5;

        long double Na_Concentration_out;
        long double Na_Concentration_in;

        long double K_Concentration_out;
        long double K_Concentration_in;

        long double Cl_Concentration_out;
        long double Cl_Concentration_in;

        long double membrane_surface; // [um^2] surface area of the membrane
        long double membrane_capacitance_density; // [uF/cm^2] membrane capacitance density
        long double membrane_capacitance; // [uF] membrane capacitance

        //Equilibrum Voltages
        long double ENa;
        long double EK;
        long double EL;

        //Maximum Conductances
        long double GNa_Max; //Na conductance [mS]
        long double GK_Max; //K conductance [mS]
        long double GL_Max; //Leak conductance [mS]
        long double GCl_Max; //Chloride conductance [mS]

        long double GNa_Max_Synapse;
        long double GCl_Max_Synapse;
        long double GK_Max_Synapse;

        long double maxSynStimulus;
        long double minSynStimulus;

        //Rest Conductances
        long double PNa_Rest;
        long double PK_Rest;
        long double PCl_Rest;

        long double GNa_Rest;
        long double GK_Rest;
        long double GCl_Rest;

        //Maximum Conductances for area
        long double maxSodiumConductanceForArea;
        long double maxPotasiumConductanceForArea;
        long double maxLeakConductanceForArea;

        long double membraneVoltage;
        long double referenceVoltage;
        /*////
        double thresholdVoltage;
        double HH_GatingVariablesReferenceVoltage;
        */

        //const static uint32_t MembraneVoltageGraph::MAX_GRAPH_INDEX = 1000;
        uint32_t membraneVoltageIndex;
        long double membraneVoltageValues[MembraneVoltageGraph::MAX_GRAPH_INDEX];

        long double m;
        long double h;
        long double n;

        double mValues[MembraneVoltageGraph::MAX_GRAPH_INDEX];
        double nValues[MembraneVoltageGraph::MAX_GRAPH_INDEX];
        double hValues[MembraneVoltageGraph::MAX_GRAPH_INDEX];

        long double naCurrent;
        long double kCurrent;
        long double lCurrent;

        long double graph_NaCurrent[MembraneVoltageGraph::MAX_GRAPH_INDEX];
        long double graph_KCurrent[MembraneVoltageGraph::MAX_GRAPH_INDEX];
        long double graph_LCurrent[MembraneVoltageGraph::MAX_GRAPH_INDEX];

        long double Tc; //Temperature Celcius
        long double Tk; //Temperature Kelvin

        long double targetTk; // Target Temperature Kelvin that Tk will adjust to
        long double maxTk = 0;

        double noiseLevel = 0;

        bool receivingStimulus;
        long double synStimulus;
        double setStimulus;

        Vector pos;

        double sigmoidGradient = -4;

        double maxSynapticSignalingConductance;
        double minSynapticSignalingConductance;

        bool drawEquilibrumVoltages = false;
        bool drawCurrents = false;
        bool drawGatingVariables = false;
        bool drawVoltage = true;

        bool voltageClamped = false;
        double clampVoltage;
        double receivingPreSynapticStimulus = false;

        //functions
        long double IncM();
        long double IncH();
        long double IncN();
        long double AdjustMembraneVoltage();
        long double AdjustTemperature();

    //public:
        HH_Neuron();
        void Load(FILE* file);
        void Save(FILE* file);
        void saveMembraneVoltagesTraceGraph(FILE* file);
        void CopyWeights(HH_Neuron* srcNeuron, int32_t replaceColumn = -1, int32_t newColumn = -1);
        void Mutate(double rate);
        void SetPos(Vector pos);
        void SetPos(double x, double y, double z);
        void SetMembraneVoltage(long double voltage, bool clamp);
        void SetTargetTemperature(long double temperature);
        void SetNoise(double noise);
        void SetSynStimulus(double synStimulus);
        void SetReceivingStimulus(bool value);
        void Process(double noise);
        double GetStrongestABSWeight();
        Vector GetMinMaxWeight();
        void Draw(double maxABSWeight, long double* comparisonVoltageValues = NULL);
        void ResetHH();
        void ResetSpikeCounts();
};

typedef HH_Neuron* HH_Neuron_ptr;

#endif //HH_Neuron_H
