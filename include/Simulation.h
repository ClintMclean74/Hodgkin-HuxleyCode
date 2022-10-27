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

#ifndef SIMULATION_H
#define SIMULATION_H

#include <random>
#include <chrono>

#include "pstdint.h"
#include "HH_NeuralNetworks.h"
#include "ResultsAndGraphs.h"

class Simulation
{
    private:

    public:
        static Agents* agents;
        static HH_NeuralNetworks* neuralNetworks;
        static ResultsAndGraphs* resultsAndGraphs;

        static uint32_t simStartRealTime;
        static double generationTime;
        static long double deltaTime;
        static double totalSimTime;
        static uint32_t deltas;
        static uint32_t graphDataPointIntervalDeltas;
        static uint32_t randomSeed;
        static std::mt19937_64 randomGenerator;

        static double synapticStimulusIntervalFromStimStart;
        static double generationDurationEvolvingTraining;
        static double generationDurationLayerTesting;
        static double generationDurationFoodTesting;
        static double generationDuration;
        static uint32_t generations;

        static char saveRootFolder[255];
        static char saveFittestNNFolder[255];

        static bool saveAgentAllMembraneVoltagesTrace;
        static char saveAgentAllMembraneVoltagesFolder[255];

        static bool loadAgentAllMembraneVoltagesTrace;
        static char loadAgentAllMembraneVoltagesFolder[255];

        static uint32_t saveMembraneVoltagesTraceGraphTime;
        static char saveMembraneVoltagesTraceGraphFolder[255];

        static char saveResultGraphsFolder[255];

        static char agentSaveLogFolder[255];

        const static uint32_t MAX_STIMULUS_ARRAY_COUNT = 1000;
        static long double rfTemperaturePulseDurations[MAX_STIMULUS_ARRAY_COUNT];
        static long double rfTemperaturePulseIntervals[MAX_STIMULUS_ARRAY_COUNT];
        static uint32_t nextStimulusIndex;
        static uint32_t rfTemperaturePulseSequenceCount;
        static uint32_t maxRFTemperaturePulseBeforeReset;
        static uint32_t rfTemperaturePulseDurationsArrayCount;
        static uint32_t rfTemperaturePulseIntervalsArrayCount;

        static double lag;
        static bool stimulus;
        static bool resetTraining;
        static bool training;
        static bool testingLayerErrors;
        static bool loadingData;
        static bool savingData;
        static uint8_t roundingDecimalPoints;
        static uint32_t trainingGeneration;
        static long double referenceTemperature;
        static long double HH_SQUID_REFERENCE_TEMPERATURE;
        static long double HH_SQUID_REFERENCE_TEMPERATURE_Q10_RATE;

        const static uint32_t MAX_NEURAL_NETWORKS = 1000;
        static long double rfInducedTemperaturesFromPulsesForAgents[MAX_NEURAL_NETWORKS];
        static uint32_t rfInducedTemperaturesFromPulsesForAgentsCount;

        static long double MAX_NEURAL_NOISE;

        static uint32_t agentsWithoutNoise[MAX_NEURAL_NETWORKS];
        static uint32_t agentsWithoutNoiseCount;

        static bool rfInducedIncTemperatureInstant;
        static bool rfInducedDecTemperatureInstant;

        static long double gatingThresholdVoltage;
        static long double HH_GatingVariablesReferenceVoltage;
        static bool nnAvgsTesting;
        static long double stimulusValue;
        static long double stimulusValueInc;
        static long double stimDuration;
        static uint32_t maxTestingGenerations;
        static uint32_t maxRFTemperaturePulseSequenceTrials;
        static bool averagingPulseSequencesResults;
        static bool debugging;
        static double resetHHDuration;
        static double preSynapticaActivityVoltageThreshold;
        static long double TempIncRatePerSecond;
        static long double TempIncRatePerDelta;
        static long double TempDecRatePerSecond;
        static long double TempDecRatePerDelta;

        static bool findStimulusValue;
        static bool stimulusFound;
        static long double minStimulusValue;
        static long double maxStimulusValue;

        static bool findWeightValue;
        static bool weightFound;
        static long double weightValue;
        static long double minWeightValue;
        static long double maxWeightValue;
        static long double weightsRange;

        static const uint32_t MAX_STIMULUS_ARRAY_LENGTH = 1000;
        static char* stimulusArray[MAX_STIMULUS_ARRAY_LENGTH];
        static uint32_t stimulusArrayLength;

        static bool setWeightValue;

        static char foodLeftStimulus[10];
        static char foodForwardStimulus[10];
        static char foodRightStimulus[10];

        static double currentRFStimulusDuration;
        static double currentRFStimulusInterval;

        static double startGenerationTime;
        static double startRFStimulusTime;

        static bool processing;

        Simulation();
        static void LoadSettings(char* fileName);
        static void CreateFolders();
        static void Initialize(char* settingsFileName);
        static void SetCurrentRFStimulusDurationAndInterval();
        virtual ~Simulation();
};

#endif // SIMULATION_H
