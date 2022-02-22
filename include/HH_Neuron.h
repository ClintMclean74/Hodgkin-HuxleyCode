#ifndef HH_Neuron_H
#define HH_Neuron_H TRUE

#include <iostream>
#include <cmath>

#include "ChemistryValues.h"
#include "Graph.h"
#include "NN_Connections.h"

class ErrorDuration
{
    public:
        double totalErrorDuration = 0;
        double currentErrorDurationStartTime = 0;

        double totalNeuronsInErrorCount = 0;
        double totalPatternErrorCount = 0;

        double currentSignallingDurationStartTime = 0;
        double totalSignallingDuration = 0;
};

class HH_Neuron
{
    double nonHH_Threshold;

    char text[255];

    double minVoltage;
    double maxVoltage;

    bool spike;

    public:
        static double MEMBRANE_VOLTAGE_THRESHOLD;
        static double MINMAX_VOLTAGE;

        ErrorDuration errorsToRefNeuron;

        uint32_t spikeCount;
        uint32_t printCount;

        void* neuralNetwork;

        ////uint32_t signalling;

        NN_Connections connections;
        double bias = 0;

        double rndThresholdValue = 0.5;

        double Na_Concentration_out;
        double Na_Concentration_in;

        double K_Concentration_out;
        double K_Concentration_in;

        double Cl_Concentration_out;
        double Cl_Concentration_in;

        double membrane_surface; // [um^2] surface area of the membrane
        double membrane_capacitance_density; // [uF/cm^2] membrane capacitance density
        double membrane_capacitance; // [uF] membrane capacitance

        //Equilibrum Voltages
        double ENa;
        double EK;
        double EL;

        //Maximum Conductances
        double GNa_Max; //Na conductance [mS]
        double GK_Max; //K conductance [mS]
        double GL_Max; //Leak conductance [mS]

        //Rest Conductances
        double PNa_Rest;
        double PK_Rest;
        double PCl_Rest;

        double GNa_Rest;
        double GK_Rest;
        double GCl_Rest;

        //Maximum Conductances for area
        double maxSodiumConductanceForArea;
        double maxPotasiumConductanceForArea;
        double maxLeakConductanceForArea;

        double membraneVoltage;
        double referenceVoltage;

        //const static uint32_t Graph::MAX_GRAPH_INDEX = 1000;
        uint32_t membraneVoltageIndex;
        double membraneVoltageValues[Graph::MAX_GRAPH_INDEX];

        double m;
        double h;
        double n;

        double mValues[Graph::MAX_GRAPH_INDEX];
        double nValues[Graph::MAX_GRAPH_INDEX];
        double hValues[Graph::MAX_GRAPH_INDEX];

        double naCurrent;
        double kCurrent;
        double lCurrent;

        double graph_NaCurrent[Graph::MAX_GRAPH_INDEX];
        double graph_KCurrent[Graph::MAX_GRAPH_INDEX];
        double graph_LCurrent[Graph::MAX_GRAPH_INDEX];

        double Tc; //Temperature Celcius
        double Tk; //;Temperature Kelvin

        double noiseLevel = 0;

        bool receivingStimulus;
        double synStimulus;
        double setConductance;
        double setStimulus;

        Vector pos;

        double sigmoidGradient = -4;

        double maxSynapticSignallingConductance;
        double minSynapticSignallingConductance;

        //functions
        double IncM();
        double IncH();
        double IncN();
        double IncMembraneVoltage();

    //public:
        HH_Neuron();
        void Load(FILE* file);
        void Save(FILE* file);
        void CopyWeights(HH_Neuron* srcNeuron, int32_t replaceColumn = -1, int32_t newColumn = -1);
        void Mutate(double rate);
        //void MutateScale(double rate);
        void SetPos(Vector pos);
        void SetPos(double x, double y, double z);
        void SetTemperature(double temperature);
        void SetNoise(double noise);
        void SetSynStimulus(double synStimulus);
        void SetReceivingStimulus(bool value);
        void AdjustAllSynapseConductance(double value);
        void Reinforce(double value, bool usedConnections);
        void Process(double noise);
        void AddMembraneVoltageToGraphValues();
        ////void SaveVoltage(char* fileName);
        void DetermineElectromagneticSignallingErrors(double refVoltage);
        double GetStrongestABSWeight();
        Vector GetMinMaxWeight();
        void Draw(double maxABSWeight);
        void ResetHH();
        void ResetSpikeCounts();
};

typedef HH_Neuron* HH_Neuron_ptr;

#endif //HH_Neuron_H
