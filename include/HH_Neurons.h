#ifndef HH_Neurons_H
#define HH_Neurons_H TRUE

#include "HH_Neuron.h"
#include "TrainingQA.h"
#include "ResultsBuffer.h"
#include "Graph.h"
#include "Color.h"

typedef uint8_t* uint8_t_ptr;
typedef uint32_t* uint32_t_ptr;

class HH_Neurons
{
    void* neuralNetwork;
    char graphicsText[255];
    char currentQ[255];
    char resultStr[255];

    FILE* voltagesFile;

    public:
        uint32_t layers;
        uint32_t* rowCountsForColumns;
        uint32_t count;

        HH_Neuron_ptr* neurons;
        Color* graphColor;

        Vector pos;
        uint32_t columns;

        uint32_t signalingCounts[6];
        uint32_t_ptr* layerSignals;

        ResultsBuffer_ptr* resultsBuffers;

        char mostRecentStimulus[255];

        HH_Neurons(uint32_t layers, uint32_t* rowCountsForColumns, void* neuralNetwork);
        ~HH_Neurons();
        void Load(FILE* file);
        void AddLayersFromNN(void* network);
        void AddNNHiddenLayer(void* network);
        void JoinLayer(uint32_t startIndex, uint32_t endIndex);
        void Save(FILE* file);
        void Copy(HH_Neurons* srcNeurons);
        void Mutate(double rate, uint32_t startLayerIndex, uint32_t endLayerIndex);
        void SetTemperature(double temperature);
        void SetTemperature(double temperature, int32_t column);
        void SetTemperature(double temperature, int32_t column, int32_t row);
        void SetNoise(double noise, int32_t column);
        void SetSynStimulus(double stimulus);
        void SetSynStimulus(double stimulus, int32_t column, int32_t row);
        void SetQ(char* q);
        void SetQA(TrainingQA* currentQA);
        void Reinforce(double value, bool usedConnections);
        uint32_t GetTotalSpikeCounts();
        uint32_t GetSpikeCountsForLayer(uint32_t layerIndex);
        uint32_t* GetSpikeCountsArrayForLayer(uint32_t layerIndex, uint32_t* spikeCountsArray);
        void GetSpikeCountArraysForLayers(uint32_t **spikeCountsArrays);
        uint32_t GetSignalingCount();
        void SetRandomThreshold();
        void DetermineError(bool print);
        double* GetResultLayerVoltages(double* voltages);
        void DetermineElectromagneticSignallingErrors(HH_Neurons *srcNeurons);
        uint32_t* GetSpikeCounts(uint32_t* spikeCounts);
        void DetermineElectromagneticSpikeCountErrors(uint32_t *spikeCounts);
        void DetermineElectromagneticErrors(uint32_t *result);
        void SetLayer1Stimulus(char* layer1Stimulus);
        void Process();
        void LoadVoltagesForlayer(FILE* file);
        void LoadVoltageData(FILE* file);
        uint32_t LoadVoltagesForLayerBinary(char* buffer, uint32_t offset);
        uint32_t LoadVoltageDataBinary(char* buffer, uint32_t offset);
        void SaveVoltagesForLayer(FILE* file, uint32_t layerIndex);
        void SaveVoltagesForLayerBinary(std::ofstream* file, uint32_t layerIndex);
        void SaveVoltages(FILE* file);
        void SaveVoltagesBinary(std::ofstream* file);
        void AddResultsBuffers();
        char* GetStringFromResult(uint32_t index);
        Action GetResultAction();
        uint32_t GetExistingQCount(char* q, uint32_t layerIndex);
        uint32_t GetExistingACount(HH_Neuron* result, uint32_t layerIndex);
        Vector GetNewGraphXY(uint32_t column, uint32_t row);
        void AdjustSynapseConductanceForColumn(uint32_t column, double value);
        uint32_t_ptr* GetLayersSignaling(bool print);
        uint32_t_ptr* GetLayersSignalingHH(bool print);
        void GetSignalCountForEachThirdOfResultLayer(uint32_t *signalCountsForEachThirdOfResultLayer);
        double GetStrongestABSWeight();
        Vector GetMinMaxWeight();
        void Draw();
        void ResetHH();
        void ResetSpikeCounts();
        uint32_t GetTotalError();
        void SetResultsBuffers(ResultsBuffer_ptr* srcBuffers);
        void CopyResultsBuffers(ResultsBuffer_ptr* srcBuffers);
        void ClearResultsBuffers();
};

#endif // HH_Neurons_H
