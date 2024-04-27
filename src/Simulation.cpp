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

#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "Simulation.h"
#include "Environment.h"
#include "HH_NeuralNetwork.h"
#include "GraphicsAndUI.h"
#include "Compatibility.h"

std::string trim(const std::string& str, const std::string& whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

Simulation::Simulation()
{
}

void Simulation::LoadSettings(char* fileName)
{
        FILE* settings = fopen(fileName, "r");

    //FILE* settings = fopen("settings/anode_break_agents_nns_layer_testing.txt", "r");

    if (settings == NULL) {
        printf("Failed to open file: %s\n", fileName);
        return;
    }

    char textBuffer[10000];
    char *ptr;
    while (fgets(textBuffer, sizeof(textBuffer), settings))
    {
        ptr = strtok(textBuffer, ":");

        std::string strTxt = ptr;
        strTxt = trim(strTxt);

        ptr = (char *) strTxt.c_str();

        if (strstr(ptr, "//") == ptr)
            continue;

        if (strstr(ptr, "training"))
        {
            ptr = strtok(NULL, "\n");

            if (strstr(ptr, "true"))
                Simulation::training = true;
            else
            {
                Simulation::training = false;
                Simulation::resetTraining = false;
            }
        }
        else
            if (strstr(ptr, "testingLayerErrors"))
            {
                ptr = strtok(NULL, "\n");

                if (strstr(ptr, "true"))
                    Simulation::testingLayerErrors = true;
                else
                    Simulation::testingLayerErrors = false;
            }
            else
            if (strstr(ptr, "nnAvgsTesting"))
            {
                ptr = strtok(NULL, "\n");

                if (strstr(ptr, "true"))
                    Simulation::nnAvgsTesting = true;
                else
                    Simulation::nnAvgsTesting = false;
            }
            else
            if (strstr(ptr, "MAX_NEURAL_NOISE"))
            {
                ptr = strtok(NULL, "\n");

                Simulation::MAX_NEURAL_NOISE = atof(ptr);
            }
            else
            if (strstr(ptr, "agentsWithoutNoise"))
            {
                ptr = strtok(NULL, ",");

                uint32_t i = 0;
                while (ptr != NULL)
                {
                    Simulation::agentsWithoutNoise[i++] = atoi(ptr);

                    ptr = strtok(NULL, ",");
                }

                Simulation::agentsWithoutNoiseCount = i;
            }
            else
            if (strstr(ptr, "referenceTemperature"))
            {
                ptr = strtok(NULL, "\n");

                Simulation::referenceTemperature = atof(ptr);
            }
            else
            if (strstr(ptr, "rfTemperaturePulseDurations"))
            {
                ptr = strtok(NULL, ",");

                uint32_t i = 0;
                while (ptr != NULL)
                {
                    Simulation::rfTemperaturePulseDurations[i++] = atof(ptr);

                    ptr = strtok(NULL, ",");
                }

                Simulation::rfTemperaturePulseDurationsArrayCount = i;
            }
            else
            if (strstr(ptr, "rfTemperaturePulseIntervals"))
            {
                ptr = strtok(NULL, ",");

                uint32_t i = 0;
                while (ptr != NULL)
                {
                    Simulation::rfTemperaturePulseIntervals[i++] = atoi(ptr);

                    ptr = strtok(NULL, ",");
                }

                Simulation::rfTemperaturePulseIntervalsArrayCount = i;
            }
            else
            if (strstr(ptr, "rfInducedTemperaturesFromPulsesForAgents"))
            {
                ptr = strtok(NULL, ",");

                uint32_t i = 0;
                while (ptr != NULL)
                {
                    Simulation::rfInducedTemperaturesFromPulsesForAgents[i++] = atof(ptr);

                    ptr = strtok(NULL, ",");
                }

                Simulation::rfInducedTemperaturesFromPulsesForAgentsCount = i;
            }
            else
            if (strstr(ptr, "rfInducedIncTemperatureInstant"))
            {
                ptr = strtok(NULL, "\n");

                if (strstr(ptr, "true"))
                    Simulation::rfInducedIncTemperatureInstant = true;
                else
                    Simulation::rfInducedIncTemperatureInstant = false;
            }
            else
            if (strstr(ptr, "rfInducedDecTemperatureInstant"))
            {
                ptr = strtok(NULL, "\n");

                if (strstr(ptr, "true"))
                    Simulation::rfInducedDecTemperatureInstant = true;
                else
                    Simulation::rfInducedDecTemperatureInstant = false;
            }
            else
            if (strstr(ptr, "maxRFTemperaturePulseBeforeReset"))
            {
                ptr = strtok(NULL, "\n");

                Simulation::maxRFTemperaturePulseBeforeReset = atoi(ptr);
            }
            else
            if (strstr(ptr, "averagingPulseSequencesResults"))
            {
                ptr = strtok(NULL, "\n");

                if (strstr(ptr, "true"))
                {
                    Simulation::averagingPulseSequencesResults = true;
                    Graph::MAX_INDEX=60000;
                }
                else
                {
                    Simulation::averagingPulseSequencesResults = false;
                    Graph::MAX_INDEX=1000;
                }
            }
            else
            if (strstr(ptr, "synapticStimulusIntervalFromStimStart"))
            {
                ptr = strtok(NULL, "\n");

                Simulation::synapticStimulusIntervalFromStimStart = atoi(ptr);
            }
            else
            if (strstr(ptr, "layer1Length"))
            {
                ptr = strtok(NULL, "\n");

                HH_NeuralNetwork::layer1Length = atoi(ptr);
            }
            else
            if (strstr(ptr, "hiddenLayerLength"))
            {
                ptr = strtok(NULL, "\n");

                HH_NeuralNetwork::hiddenLayerLength = atoi(ptr);
            }
            else
            if (strstr(ptr, "resultLayerLength"))
            {
                ptr = strtok(NULL, "\n");

                HH_NeuralNetwork::resultLayerLength = atoi(ptr);
            }
            else
            if (strstr(ptr, "LAYERS"))
            {
                ptr = strtok(NULL, "\n");

                HH_NeuralNetwork::LAYERS = atoi(ptr);
            }
            else
            if (strstr(ptr, "saveRootFolder"))
            {
                ptr = strtok(NULL, "\n");

                strcpy(Simulation::saveRootFolder, ptr);
                Simulation::saveRootFolder[strlen(Simulation::saveRootFolder)]=0;
            }
            else
            if (strstr(ptr, "saveFittestNNFolder"))
            {
                ptr = strtok(NULL, "\n");

                strcpy(Simulation::saveFittestNNFolder, ptr);
                Simulation::saveFittestNNFolder[strlen(Simulation::saveFittestNNFolder)]=0;
            }
            else
            if (strstr(ptr, "saveAgentAllMembraneVoltagesTrace"))
            {
                ptr = strtok(NULL, "\n");

                if (strstr(ptr, "true"))
                    Simulation::saveAgentAllMembraneVoltagesTrace = true;
                else
                    Simulation::saveAgentAllMembraneVoltagesTrace = false;
            }
            else
            if (strstr(ptr, "saveAgentAllMembraneVoltagesFolder"))
            {
                ptr = strtok(NULL, "\n");

                strcpy(Simulation::saveAgentAllMembraneVoltagesFolder, ptr);
                Simulation::saveAgentAllMembraneVoltagesFolder[strlen(Simulation::saveAgentAllMembraneVoltagesFolder)]=0;
            }
            else
            if (strstr(ptr, "loadAgentAllMembraneVoltagesFolder"))
            {
                ptr = strtok(NULL, "\n");

                strcpy(Simulation::loadAgentAllMembraneVoltagesFolder, ptr);
                Simulation::loadAgentAllMembraneVoltagesFolder[strlen(Simulation::loadAgentAllMembraneVoltagesFolder)]=0;
            }
            else
            if (strstr(ptr, "saveMembraneVoltagesTraceGraphFolder"))
            {
                ptr = strtok(NULL, "\n");

                strcpy(Simulation::saveMembraneVoltagesTraceGraphFolder, ptr);
                Simulation::saveMembraneVoltagesTraceGraphFolder[strlen(Simulation::saveMembraneVoltagesTraceGraphFolder)]=0;
            }
            else
            if (strstr(ptr, "saveMembraneVoltagesTraceGraphTime"))
            {
                ptr = strtok(NULL, "\n");

                Simulation::saveMembraneVoltagesTraceGraphTime = atoi(ptr);
            }
            else
            if (strstr(ptr, "saveResultGraphsFolder"))
            {
                ptr = strtok(NULL, "\n");

                strcpy(Simulation::saveResultGraphsFolder, ptr);
                Simulation::saveResultGraphsFolder[strlen(Simulation::saveResultGraphsFolder)]=0;
            }
            else
            if (strstr(ptr, "agentSaveLogFolder"))
            {
                ptr = strtok(NULL, "\n");

                strcpy(Simulation::agentSaveLogFolder, ptr);
                Simulation::agentSaveLogFolder[strlen(Simulation::agentSaveLogFolder)]=0;
            }
            else
            if (strstr(ptr, "stimulusValue"))
            {
                ptr = strtok(NULL, "\n");

                Simulation::stimulusValue = atof(ptr);
            }
            else
            if (strstr(ptr, "foodRightStimulus"))
            {
                ptr = strtok(NULL, "\n");

                strcpy(Simulation::foodRightStimulus, ptr);
            }
            else
            if (strstr(ptr, "foodForwardStimulus"))
            {
                ptr = strtok(NULL, "\n");

                strcpy(Simulation::foodForwardStimulus, ptr);
            }
            else
            if (strstr(ptr, "foodLeftStimulus"))
            {
                ptr = strtok(NULL, "\n");

                strcpy(Simulation::foodLeftStimulus, ptr);
            }
            else
            if (strstr(ptr, "resetHHDuration"))
            {
                ptr = strtok(NULL, "\n");

                Simulation::resetHHDuration = atoi(ptr);
            }
            else
            if (strstr(ptr, "stimDuration"))
            {
                ptr = strtok(NULL, "\n");

                Simulation::stimDuration = atoi(ptr);
            }
            else
            if (strstr(ptr, "setWeightValue"))
            {
                ptr = strtok(NULL, "\n");

                if (strstr(ptr, "true"))
                    Simulation::setWeightValue = true;
                else
                    Simulation::setWeightValue = false;
            }
            else
            if (strstr(ptr, "weightValue"))
            {
                ptr = strtok(NULL, "\n");

                Simulation::weightValue = atof(ptr);
            }
            else
            if (strstr(ptr, "viewPos"))
            {
                ptr = strtok(NULL, ",");
                GraphicsAndUI::pos.x = atof(ptr);

                ptr = strtok(NULL, ",");
                GraphicsAndUI::pos.y = atof(ptr);

                ptr = strtok(NULL, "\n");
                GraphicsAndUI::pos.z = atof(ptr);

                GraphicsAndUI::originalViewPos = GraphicsAndUI::pos;
            }
            else
            if (strstr(ptr, "fov"))
            {
                ptr = strtok(NULL, "\n");
                GraphicsAndUI::fov = atof(ptr);

                GraphicsAndUI::originalViewFov = GraphicsAndUI::fov;
            }
            else
            if (strstr(ptr, "lag"))
            {
                ptr = strtok(NULL, "\n");
                Simulation::lag = atof(ptr);
            }
            else
            if (strstr(ptr, "nnVisible"))
            {
                ptr = strtok(NULL, "\n");

                if (strstr(ptr, "true"))
                    GraphicsAndUI::nnVisible = true;
                else
                    GraphicsAndUI::nnVisible= false;
            }
            else
            if (strstr(ptr, "connectionsVisible"))
            {
                ptr = strtok(NULL, "\n");

                if (strstr(ptr, "true"))
                    GraphicsAndUI::connectionsVisible = true;
                else
                    GraphicsAndUI::connectionsVisible = false;
            }
            else
            if (strstr(ptr, "randomSeed"))
            {
                ptr = strtok(NULL, "\n");

                Simulation::randomSeed = atoi(ptr);
            }
    }
    fclose(settings);

    if (strlen(Simulation::foodRightStimulus)==0)
        strcpy(Simulation::foodRightStimulus, "001");

    if (strlen(Simulation::foodForwardStimulus)==0)
        strcpy(Simulation::foodForwardStimulus, "010");

    if (strlen(Simulation::foodLeftStimulus)==0)
        strcpy(Simulation::foodLeftStimulus, "100");

    Simulation::stimulusArray[0] = new char[255];
    Simulation::stimulusArray[1] = new char[255];
    Simulation::stimulusArray[2] = new char[255];

    strcpy(Simulation::stimulusArray[0], Simulation::foodRightStimulus);
    strcpy(Simulation::stimulusArray[1], Simulation::foodForwardStimulus);
    strcpy(Simulation::stimulusArray[2], Simulation::foodLeftStimulus);

    Simulation::stimulusArrayLength = 3;

    Simulation::generationDurationEvolvingTraining = (Simulation::synapticStimulusIntervalFromStimStart + Simulation::resetHHDuration) * 3 + 2;
    Simulation::generationDurationLayerTesting = Simulation::generationDurationEvolvingTraining;
    Simulation::generationDurationFoodTesting = Simulation::generationDurationEvolvingTraining * 60;
    Simulation::generationDuration = Simulation::generationDurationEvolvingTraining;
}

bool CreatePath(std::string wsPath)
{
    int pos, pos2;
    bool result = true;
    DWORD attr;

    pos = wsPath.find_last_of("\\");
    pos2 = wsPath.find_last_of('//');

    pos = std::max(pos, pos2);

    if (pos + 1 == wsPath.length())  // last character is '/\'
        wsPath.resize(pos);

    struct stat st;
    if (stat(wsPath.c_str(), &st) == -1) //doesn’t exist
    {        
                pos = wsPath.find_last_of('\\');

                pos2 = wsPath.find_last_of('//');

                pos = std::max(pos, pos2);

                if (pos > 0)
                {
                    // Create parent dirs:
                    std::string wsPath2 = wsPath.substr(0, pos);

                    result = CreatePath(wsPath2);
                }
                
                //create folder
                #ifdef _WIN32
                    result = result && (_mkdir(wsPath.c_str()) == 0);
                #elif __linux__                
                    result = result && (mkdir(wsPath.c_str(), 0777) == 0);
                #endif
    }    
    else if ((st.st_mode & S_IFMT) != S_IFDIR)
    {  // object already exists, but is not a dir

        //SetLastError(ERROR_FILE_EXISTS);

        result = false;
    }

    return result;
}

void Simulation::CreateFolders()
{
    char folderFileName[255];

    CreatePath(Simulation::saveRootFolder);

    sprintf(folderFileName, "%s/%s", Simulation::saveRootFolder, Simulation::saveFittestNNFolder);
    CreatePath(folderFileName);

    sprintf(folderFileName, "%s/%s", Simulation::saveRootFolder, Simulation::agentSaveLogFolder);
    CreatePath(folderFileName);

    sprintf(folderFileName, "%s/%s", Simulation::saveRootFolder, Simulation::saveResultGraphsFolder);
    CreatePath(folderFileName);

    sprintf(folderFileName, "%s/%s", Simulation::saveRootFolder, Simulation::loadAgentAllMembraneVoltagesFolder);
    CreatePath(folderFileName);

    sprintf(folderFileName, "%s/%s", Simulation::saveRootFolder, Simulation::saveAgentAllMembraneVoltagesFolder);
    CreatePath(folderFileName);

    sprintf(folderFileName, "%s/%s", Simulation::saveRootFolder, Simulation::saveMembraneVoltagesTraceGraphFolder);
    CreatePath(folderFileName);
}


void Simulation::Initialize(char* settingsFileName)
{
    Simulation::LoadSettings(settingsFileName);
    Simulation::CreateFolders();

    Simulation::simStartRealTime = Compatibility::GetTickCount();
    if (Simulation::randomSeed == 0)
        Simulation::randomSeed = Simulation::simStartRealTime;

    Simulation::randomGenerator.seed(Simulation::randomSeed); //seed the random number generator
    srand(Simulation::randomSeed);

    Simulation::agents = new Agents();

    Simulation::neuralNetworks = new HH_NeuralNetworks(Simulation::agents);

    Simulation::neuralNetworks->InitializeNNs("");

    Environment::foodForAgents_ptr = new FoodForAgents();
    Environment::foodForAgents_ptr->GenerateAndSaveFoodPosForAgents();

    HH_NeuralNetwork::LAYERS = Simulation::neuralNetworks->neuralNetworks[0]->layers;

    Simulation::resultsAndGraphs = new ResultsAndGraphs();

    Simulation::resultsAndGraphs->InitializeGraphs();
}

void Simulation::SetCurrentRFStimulusDurationAndInterval()
{
    if (!Simulation::averagingPulseSequencesResults)
        nextStimulusIndex = (double) rand() / RAND_MAX * Simulation::rfTemperaturePulseDurationsArrayCount;

    Simulation::currentRFStimulusDuration = Simulation::rfTemperaturePulseDurations[nextStimulusIndex];
    Simulation::currentRFStimulusInterval = Simulation::rfTemperaturePulseIntervals[nextStimulusIndex];

    Simulation::nextStimulusIndex++;

    if (Simulation::nextStimulusIndex >= Simulation::rfTemperaturePulseDurationsArrayCount)
        Simulation::nextStimulusIndex = 0;
}

Simulation::~Simulation()
{
}

double Simulation::generationTime = 0; //ms
long double Simulation::deltaTime = 0.01; //ms
double Simulation::totalSimTime = 0; //ms
uint32_t Simulation::deltas = 0;
uint32_t Simulation::graphDataPointIntervalDeltas = 1 /*ms*/ / Simulation::deltaTime;

uint32_t Simulation::randomSeed = 0;

//durations
double Simulation::generationDurationEvolvingTraining = (Simulation::synapticStimulusIntervalFromStimStart + Simulation::resetHHDuration) * 3 + 2;
double Simulation::generationDurationLayerTesting = Simulation::generationDurationEvolvingTraining;
double Simulation::generationDurationFoodTesting = Simulation::generationDurationEvolvingTraining * 60;
double Simulation::generationDuration = Simulation::generationDurationEvolvingTraining;

uint32_t Simulation::generations = 0;

//folders for saving data
char Simulation::saveFittestNNFolder[] = "agentsNN/";

bool Simulation::saveAgentAllMembraneVoltagesTrace = false;
char Simulation::saveAgentAllMembraneVoltagesFolder[] = "agent_all_membrane_voltages/";

bool Simulation::loadAgentAllMembraneVoltagesTrace = false;
char Simulation::loadAgentAllMembraneVoltagesFolder[] = "agent_all_membrane_voltages/";

uint32_t Simulation::saveMembraneVoltagesTraceGraphTime = UINT32_MAX;
char Simulation::saveMembraneVoltagesTraceGraphFolder[] = "trace_membrane_voltages/";

char Simulation::saveResultGraphsFolder[] = "resultGraphs";
char Simulation::agentSaveLogFolder[] = "agent_logs";

uint32_t Simulation::simStartRealTime;
uint32_t Simulation::trainingGeneration = 0;
long double Simulation::HH_SQUID_REFERENCE_TEMPERATURE = 6.3;

//Hodgkin-Huxley Q10 adjustment rate for temperatures, stated their paper "A quantitative description of membrane current and its application to conduction and excitation in nerve"
long double Simulation::HH_SQUID_REFERENCE_TEMPERATURE_Q10_RATE = 3;

long double Simulation::referenceTemperature = Simulation::HH_SQUID_REFERENCE_TEMPERATURE;

long double Simulation::gatingThresholdVoltage = -55;
long double Simulation::HH_GatingVariablesReferenceVoltage = Simulation::gatingThresholdVoltage - 10;

Agents* Simulation::agents;
HH_NeuralNetworks* Simulation::neuralNetworks;
ResultsAndGraphs* Simulation::resultsAndGraphs;

uint32_t Simulation::maxTestingGenerations = 30; //for statistically significant averages
uint32_t Simulation::maxRFTemperaturePulseSequenceTrials = Simulation::maxTestingGenerations;

bool Simulation::averagingPulseSequencesResults = false;

bool Simulation::loadingData = false;
bool Simulation::debugging = false;

double Simulation::preSynapticaActivityVoltageThreshold = 20; //threshold for signaling to post synaptic neuron


//An increasing temperature rate of 2 C/s is chosen, since 1 C/s to 10 C/s causes the microwave auditory effect that results in thermoelastic pressure waves and a heating rates of 0.001 C over 500 μs are also considered realistic exposures from transmissions that could result in neuropathological effects (Dagro et al, 2021)(Foster and Glaser, 2007)
long double Simulation::TempIncRatePerSecond = 2; //2 degrees a second
long double Simulation::TempIncRatePerDelta = Simulation::TempIncRatePerSecond / (1/Simulation::deltaTime * 1000); //rate per delta = inc rate per second / (deltas in a second = ((deltas in a ms) * (ms per second (1000))))

//The temperature decreases at a rate of 0.001 C per millisecond, since temperature increases of ~10^-3 C should reach equilibrium in less than one millisecond:
//"...the minuscule temperature increase of << 10 -3 K would reach equilibrium in less than one millisecond..." (Foster and Glaser 2007)
long double Simulation::TempDecRatePerSecond = 1; //for 6.301 C to 6.3 C over 1 ms
long double Simulation::TempDecRatePerDelta = Simulation::TempDecRatePerSecond / (1/Simulation::deltaTime * 1000); //rate per delta = dec rate per second / (deltas in a second = ((deltas in a ms) * (ms per second (1000))))

long double Simulation::stimulusValueInc = 0.01;

bool Simulation::findStimulusValue = false;
bool Simulation::stimulusFound = false;

long double Simulation::minStimulusValue = Simulation::stimulusValue / 2;
long double Simulation::maxStimulusValue = Simulation::stimulusValue * 2;

bool Simulation::setWeightValue = false;
bool Simulation::findWeightValue = false;
bool Simulation::weightFound = false;
long double Simulation::weightValue = 0.00000000000000000000001;
long double Simulation::minWeightValue = 0;
long double Simulation::maxWeightValue = Simulation::weightValue;
long double Simulation::weightsRange = 0.01;

bool Simulation::stimulus = false;

double Simulation::resetHHDuration = 3; //amount of sim time after reset before stimulus so that the values can adjust to equilibrium values

double Simulation::synapticStimulusIntervalFromStimStart = 70; //the interval from when the stimulus occurred to when the neuron reset occurs and the next stimulus

long double Simulation::stimulusValue = 10;
long double Simulation::stimDuration = 1;

bool Simulation::nnAvgsTesting = false;

bool Simulation::resetTraining = true;
bool Simulation::training = false;
bool Simulation::testingLayerErrors = false;

long double Simulation::rfTemperaturePulseDurations[] = {10, 7, 5, 3, 1};
long double Simulation::rfTemperaturePulseIntervals[] = {10, 7, 5, 3, 1};

uint32_t Simulation::nextStimulusIndex = 0;

uint32_t Simulation::rfTemperaturePulseSequenceCount = 0;

uint32_t Simulation::maxRFTemperaturePulseBeforeReset = 3;

uint32_t Simulation::rfTemperaturePulseDurationsArrayCount = 5;
uint32_t Simulation::rfTemperaturePulseIntervalsArrayCount = 5;

long double Simulation::rfInducedTemperaturesFromPulsesForAgents[] = {16.3};
uint32_t Simulation::rfInducedTemperaturesFromPulsesForAgentsCount = 1;
bool Simulation::rfInducedIncTemperatureInstant = false;
bool Simulation::rfInducedDecTemperatureInstant = false;

long double Simulation::MAX_NEURAL_NOISE = 0;

uint32_t Simulation::agentsWithoutNoise[] = {};
uint32_t Simulation::agentsWithoutNoiseCount = 0;

char* Simulation::stimulusArray[];
uint32_t Simulation::stimulusArrayLength = 0;

char Simulation::foodLeftStimulus[];
char Simulation::foodForwardStimulus[];
char Simulation::foodRightStimulus[];

char Simulation::saveRootFolder[] = "Results";

double Simulation::currentRFStimulusDuration;
double Simulation::currentRFStimulusInterval;

double Simulation::startGenerationTime = 0;
double Simulation::startRFStimulusTime = 0;

bool Simulation::processing = true;

std::mt19937_64 Simulation::randomGenerator;
