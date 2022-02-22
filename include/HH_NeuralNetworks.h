#ifndef HH_NeuralNetworks_H
#define HH_NeuralNetworks_H TRUE

#include "HH_NeuralNetwork.h"
#include "Agents.h"

class HH_NeuralNetworks
{
    private:
        char graphicsText[255];

        uint32_t fittestIndexes[2];

        FILE* voltagesFile;

    public:


        /*////
        static const uint32_t NETWORKS_ROWS = 6;
        static const uint32_t NETWORKS_COLUMNS = 5;
        */



        ////static const uint32_t NETWORKS_ROWS = 7;
        ////static const uint32_t NETWORKS_COLUMNS = 1;



        /*////
        static const uint32_t NETWORKS_ROWS = 2;
        static const uint32_t NETWORKS_COLUMNS = 1;
        */

        static const uint32_t NETWORKS_ROWS = 1;
        static const uint32_t NETWORKS_COLUMNS = 4;




        static bool simHH;
        static bool electromagneticRF_HH;
        static double MAX_NEURAL_NOISE;
        static double NEURAL_NOISE_DELTA;
        static double* NEURAL_NOISE;

        double averageError = 0;

        uint32_t neuralNetworksCount = 0;

        bool visible;

        HH_NeuralNetwork_ptr neuralNetworks[NETWORKS_ROWS * NETWORKS_COLUMNS];

        Vector pos;

        Agents *agents;

        HH_NeuralNetworks(Agents *agents);
        ~HH_NeuralNetworks();
        void InitializeNNs(char* fileName = "");
        void Add(HH_NeuralNetwork *neuralNetwork);
        uint32_t* GetFittestNNIndexes();
        void SetResultsBuffers(ResultsBuffer_ptr* srcBuffers);
        void CopyWeights(uint32_t* indexes);
        void Mutate(uint32_t startLayerIndex = HH_NeuralNetwork::MAX_LAYERS, uint32_t endLayerIndex = HH_NeuralNetwork::MAX_LAYERS);
        void ResetTotalErrorAndResultsBuffer();
        void SetTemperature(double temperature, int32_t index = -1, int32_t layerIndex = -1, int32_t neuronIndex = -1);
        void SetNoise(double noise, uint32_t index, int32_t layerIndex = -1);
        /*void SetQA(TrainingQA* currentQA, uint32_t index);
        uint32_t GetTotalSpikeCounts();
        uint32_t GetSignalingCount();
        void DetermineError(uint32_t index, bool print);
        void DetermineErrorsForPreviousQA(uint32_t printIndex = 999999999);
        void DetermineElectromagneticSignallingErrors(double Simulation::currentSimTime);
        void DetermineElectromagneticSpikeCountErrors();
        void DetermineElectromagneticErrors();
        void SetLayer1Stimulus(char* layer1Stimulus, uint32_t index);
        */
        void Process();
        void SaveVoltages();
        double GetAverageError();
        void Draw();
        void ResetHH();
        void ResetSpikeCounts();
};

#endif // HH_NeuralNetworks_H
