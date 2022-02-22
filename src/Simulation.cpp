#include "Simulation.h"
#include "Threading.h"
#include "HH_NeuralNetwork.h"

Simulation::Simulation()
{
}

Simulation::~Simulation()
{
}

double Simulation::currentSimTime = 0;
double Simulation::deltaTime = 0.01;
////double Simulation::generationDuration = 3000;
////double Simulation::generationDuration = 701;
////double Simulation::generationDuration = 212 * 3;
////double Simulation::generationDuration = 212;
double Simulation::generationDurationEvolvingTraining = 212;
double Simulation::generationDurationLayerTesting = Simulation::generationDurationEvolvingTraining;
////double Simulation::generationDurationTesting = HH_NeuralNetwork::stimIntervalTimeFromStimStart * 10 + 1;
double Simulation::generationDurationFoodTesting = Simulation::generationDurationEvolvingTraining * 60;
////double Simulation::generationDurationTesting = Simulation::generationDurationEvolvingTraining;
double Simulation::generationDuration = Simulation::generationDurationEvolvingTraining;
uint32_t Simulation::generations = 0;

double Simulation::stimulusDuration = 10;
double Simulation::stimulusInterval = 10;
double Simulation::lag = 0;
double Simulation::stimulus = true;
bool Simulation::threading = true;
////HANDLE Simulation::startNextGenerationSemaphore = NULL;
bool Simulation::trainingOrEvolution = true;
bool Simulation::testingLayerErrors = false;
bool Simulation::loadingData = false;
bool Simulation::savingData = !Simulation::loadingData;
////bool Simulation::savingData = true;
bool Simulation::nnVisible = false;
bool Simulation::paused = false;
uint8_t Simulation::floatingPointRounding = 4;
uint32_t Simulation::simStartRealTime;
