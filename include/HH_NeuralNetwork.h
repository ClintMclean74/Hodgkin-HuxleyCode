#ifndef HH_NeuralNetwork_H
#define HH_NeuralNetwork_H TRUE

#include "HH_Neurons.h"

#include "TrainingQA.h"
#include "TrainingSet.h"

#include "Agent.h"

struct ElectromagneticErrorForTemperature
{
    uint32_t error;
    double temperature;
};

class HH_NeuralNetwork

{
    public:
        static uint32_t* ROWS;
        static const uint32_t MAX_LAYERS = 100;
        static const uint32_t MAX_ROW_COUNT = 100;
        ////static const uint32_t LAYERS = 100;
        ////static const uint32_t LAYERS = 31;
        ////static const uint32_t LAYERS = 10;
        ////static const uint32_t LAYERS = 13; //13 layers works with recurrent an 0.0001 of a degree increase
        ////static const uint32_t LAYERS = 3;
        ////static const uint32_t LAYERS = 30;
        static uint32_t LAYERS;

        uint32_t* rowCountsForColumns;
        uint32_t layers = LAYERS;

        static double Layer1ActiveSynapticConductance;
        static double Layer1ActiveSynapticConductanceHH;

        static double WIDTH;
        static double HEIGHT;
        static double X_GAP;
        static double Y_GAP;

        static uint32_t layer1Length;
        static uint32_t hiddenLayerLength;
        static uint32_t resultLayerLength;

        static double rewardDecayRate;
        static double weightActivityWeakenRate;
        static double weightRegenerationRate;
        static double intervalTime;
        static double stimIntervalTimeFromStimStart;
        static double stimDuration;
        static uint32_t MAX_LAYER_COLUMN_WIDTH;

        Vector pos;

        HH_Neurons* neurons;

        double weightsMutationRate;
        double weightsMutationRateMutationRate;

        double weightsScale;

        double rotationMutationRate;
        double velocityMutationRate;

        double temperature;
        double rfTemperature;

        ElectromagneticErrorForTemperature electromagneticErrorForTemperature[1000];

        uint16_t electromagneticErrorForTemperatureCount = 0;

        bool recurrent = false;

        Agent* agent;

        double signal;

        HH_NeuralNetwork(uint32_t layers, uint32_t* rowCountsForColumns, Vector* pos = NULL);
        ~HH_NeuralNetwork();
        static HH_NeuralNetwork* Load(char* fileName);
        void AddLayersFromNN(HH_NeuralNetwork* network);
        void JoinLayer(uint32_t startIndex, uint32_t endIndex);
        void Save(char* fileName);
        void Copy(HH_NeuralNetwork* neuralNetwork);
        void Mutate(uint32_t startLayerIndex = HH_NeuralNetwork::MAX_LAYERS, uint32_t endLayerIndex = HH_NeuralNetwork::MAX_LAYERS);
        //void SetTemperature(double temperature);
        //void SetTemperature(double temperature, uint32_t column);
        void SetTemperature(double temperature, int32_t column = -1, int32_t row = -1);
        void SetNoise(double noise, int32_t column = -1);
        void SetSynStimulus(double stimulus);
        void SetSynStimulus(double stimulus, uint32_t column, uint32_t row);
        void SetQ(char* q);
        void SetQA(TrainingQA* currentQA);
        void Reinforce(double value, bool usedConnections = true);
        void Process();
        ////void SaveVoltages(FILE* voltagesFile);
        void Draw();
        void ResetHH();
        void ResetSpikeCounts();
};

typedef HH_NeuralNetwork* HH_NeuralNetwork_ptr;

#endif // HH_NeuralNetwork_H
