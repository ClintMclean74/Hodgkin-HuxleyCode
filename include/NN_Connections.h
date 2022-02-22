#ifndef NN_Connections_H
#define NN_Connections_H TRUE

#include "pstdint.h"
#include "Graph.h"
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
        //void MutateScale(double rate);
        void Add(uint32_t column, uint32_t row, double synapseConductance);
        void AdjustAllSynapseConductance(double value);
        double GetCurrentSignallingConductance(void* neurons, double maxSynapseConductanceForArea);
        void SetNeuralConnectionActivityTime(void* neurons);
        void Reinforce(double value, bool usedConnections);
        double GetStrongestABSWeight();
        Vector GetMinMaxWeight();
        void Draw(void* parent, void* neurons, double maxABSWeight);
};


/*class NN_Connections
{
    public:

        HH_Neurons *neurons;
};*/

#endif //NN_Connections_H
