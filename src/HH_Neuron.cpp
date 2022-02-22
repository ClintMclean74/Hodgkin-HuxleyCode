#include <math.h>
#include <string.h>
#include "HH_Neuron.h"
#include "GL/glew.h"
#include "GraphicsUtilities.h"
#include "HH_NeuralNetwork.h"
#include "HH_NeuralNetworks.h"
#include "MathUtilities.h"
#include "Simulation.h"

HH_Neuron::HH_Neuron()
{
    //nonHH_Threshold = 0.000245;
    nonHH_Threshold = 0.0;

    neuralNetwork = NULL;

    //simHH = false;

    //signalling = 0;

    membrane_surface = 4000.0;  // [um^2] surface area of the membrane
    membrane_capacitance_density = 1.0;   // [uF/cm^2] membrane capacitance density
    membrane_capacitance = membrane_capacitance_density * membrane_surface * 1e-8;   // [uF] membrane capacitance

    Na_Concentration_out = 145;
    Na_Concentration_in = 15;

    K_Concentration_out = 5;
    K_Concentration_in = 150;

    Cl_Concentration_out = 110;
    Cl_Concentration_in = 10;

    Tc = 37;
    Tk = ChemistryValues::KELVIN + Tc;

    //Equilibrum Voltages
    //ENa = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::Na_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((double) Na_Concentration_out/Na_Concentration_in);

    //out/in ratios from http://www.scielo.org.za/scielo.php?script=sci_arttext&pid=S0038-23532008000400012
    /*ENa = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::Na_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((double) 7.210);
    EK = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::K_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((double) 0.036);
    EL = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::K_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((double) 0.0367);
    //--> EL = -88; //out/in ratio = 0.0367 from http://www.scielo.org.za/scielo.php?script=sci_arttext&pid=S0038-23532008000400012

    ENa = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::Na_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((double) Na_Concentration_out/Na_Concentration_in);
    EK = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::K_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((double) K_Concentration_out/K_Concentration_in);
    EL = -70;
    */

    //Maximum Conductances
    GNa_Max = 300.0;
    GK_Max = 150.0;
    GL_Max = 0.033;

    //Rest Conductances
    GNa_Rest = 0.05;
    GK_Rest = 1;
    GCl_Rest = 0.45;

    //Maximum Conductances for area
    maxSodiumConductanceForArea = GNa_Max * membrane_surface * 1e-8;// Na conductance [mS]
    maxPotasiumConductanceForArea = GK_Max * membrane_surface * 1e-8;  // K conductance [mS]
    maxLeakConductanceForArea = GL_Max * membrane_surface * 1e-8;  // leak conductance [mS]

    referenceVoltage = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::Na_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((Na_Concentration_out * GNa_Rest + K_Concentration_out * GK_Rest + Cl_Concentration_in * GCl_Rest)/(Na_Concentration_in * GNa_Rest + K_Concentration_in * GK_Rest + Cl_Concentration_out * GCl_Rest));
    //referenceVoltage = -70.0;
    ////membraneVoltage = referenceVoltage;
    //membraneVoltage = -70;
    membraneVoltage = -80.875; //stable membrane voltage after 60 ms

    membraneVoltageIndex = 0;

    minVoltage = 999999;
    maxVoltage = -999999;

    spike = false;
    spikeCount = 0;


    ENa = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::Na_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((double) Na_Concentration_out/Na_Concentration_in);
    EK = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::K_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((double) K_Concentration_out/K_Concentration_in);


    GNa_Rest = 0.05;
    GK_Rest = 1;
    GCl_Rest = 0.45;


    /*GNa_Rest = 0;
    GK_Rest = 0;
    GCl_Rest = 1;
    */


    /*Tk = 310;

    double VRest = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::Na_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((Na_Concentration_out * GNa_Rest + K_Concentration_out * GK_Rest + Cl_Concentration_in * GCl_Rest)/(Na_Concentration_in * GNa_Rest + K_Concentration_in * GK_Rest + Cl_Concentration_out * GCl_Rest));

    Tk = 350;

    double VRest2 = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::Na_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((Na_Concentration_out * GNa_Rest + K_Concentration_out * GK_Rest + Cl_Concentration_in * GCl_Rest)/(Na_Concentration_in * GNa_Rest + K_Concentration_in * GK_Rest + Cl_Concentration_out * GCl_Rest));
*/



    //Cl_OIRatio = 110/10;

    setConductance = 0;

    setStimulus = 0;

    receivingStimulus = false;

    printCount = 0;

    ResetHH();
}

void HH_Neuron::Load(FILE* file)
{
    char textBuffer[255];
    memset(textBuffer, 0, 255);

    fgets(textBuffer, 255, file);

    bias = atof(textBuffer);

    bias = 0;

    connections.Load(file);
}

void HH_Neuron::Save(FILE* file)
{
    char textBuffer[255];

    sprintf(textBuffer, "%f\n", bias);

    uint32_t length = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), length, file);

    connections.Save(file);
}

void HH_Neuron::CopyWeights(HH_Neuron* srcNeuron, int32_t replaceColumn, int32_t newColumn)
{
    bias = srcNeuron->bias;

    connections.CopyWeights(&srcNeuron->connections, replaceColumn, newColumn);
}

void HH_Neuron::Mutate(double rate)
{
    //double sigmoidGradientRate = rate/10;

    ////double biasMutationRate = rate/10;
    double biasMutationRate = 0.1;

    ////sigmoidGradient += (double) rand()/ RAND_MAX * rate - rate/2;

    bias += (double) rand()/ RAND_MAX * biasMutationRate - biasMutationRate/2;

    bias = MathUtilities::Limit(bias, 1);

    bias = 0;

    connections.Mutate(rate);
}

/*void HH_Neuron::MutateScale(double rate)
{
    double value = (double) rand()/ RAND_MAX * rate - rate/2;

    weightsScale += value;

    //connections.MutateScale(rate);
}
*/

void HH_Neuron::SetPos(Vector pos)
{
    this->pos = pos;
}

void HH_Neuron::SetTemperature(double temperature)
{
    this->Tc = temperature;
    this->Tk = ChemistryValues::KELVIN + this->Tc;
}

void HH_Neuron::SetNoise(double noiseLevel)
{
    this->noiseLevel = noiseLevel;
}

void HH_Neuron::SetPos(double x, double y, double z)
{
    this->pos.x = x;
    this->pos.y = y;
    this->pos.z = z;
}

//ref Spike_neural_models_part_I_The_Hodgkin-Huxley_mode for equations
double HH_Neuron::IncM()
    {
        double voltageDifference = membraneVoltage - referenceVoltage;

        double aM = 0.1 * (voltageDifference - 25.0) / (1.0 - exp(-(voltageDifference - 25.0) / 10.0));
        double bM = 4.0 * exp(-voltageDifference / 18.0);

        double derivativeM = aM * (1.0 - m)- bM * m;

        m = m + derivativeM * Simulation::deltaTime;

        if (m > 1)
            m = 1;
        else
            if (m < 0)
                m = 0;

        return m;
    }

    double HH_Neuron::IncH()
    {
        double voltageDifference = membraneVoltage - referenceVoltage;

        double aH = 0.07 * exp(-voltageDifference / 20.0);
        double bH = 1.0 / (1.0 + exp(-(voltageDifference - 30.0) / 10.0));

        double derivativeH = aH * (1.0 - h) - bH * h;

        h = h + derivativeH * Simulation::deltaTime;

        if (h > 1)
            h = 1;
        else
            if (h < 0)
                h = 0;

        return h;
    }

    double HH_Neuron::IncN()
    {
        double voltageDifference = membraneVoltage - referenceVoltage;

        double aN = 0.01 * (voltageDifference - 10.0) / (1.0 - exp(-(voltageDifference - 10.0) / 10.0));
        double bN = 0.125 * exp(-voltageDifference / 80.0);

        double derivativeN = aN * (1.0 - n) - bN * n;

        n = n + derivativeN * Simulation::deltaTime;

        if (n > 1)
            n = 1;
        else
            if (n < 0)
                n = 0;

        return n;
    }

    double HH_Neuron::IncMembraneVoltage()
    {
        naCurrent = maxSodiumConductanceForArea * m*m*m * h * (ENa - membraneVoltage);
        kCurrent = maxPotasiumConductanceForArea * n*n*n*n * (EK - membraneVoltage);
        lCurrent = maxLeakConductanceForArea * (EL - membraneVoltage);

        double derivativeVoltage = (naCurrent + kCurrent + lCurrent + synStimulus) / membrane_capacitance;

        return membraneVoltage + derivativeVoltage * Simulation::deltaTime;
    }

    void HH_Neuron::SetSynStimulus(double stimulus)
    {
        this->synStimulus = stimulus;
    }

    void HH_Neuron::SetReceivingStimulus(bool value)
    {
        receivingStimulus = value;
    }

    void HH_Neuron::AdjustAllSynapseConductance(double value)
    {
        connections.AdjustAllSynapseConductance(value);
    }

    void HH_Neuron::Reinforce(double value, bool usedConnections)
    {
        connections.Reinforce(value, usedConnections);
    }

    void HH_Neuron::Process(double noise)
    {
        if (HH_NeuralNetworks::simHH)
        {
        //receivingStimulus = true;

        if (!Simulation::loadingData)
        {
        if (Tc == 130)
            int grc = 1;

        referenceVoltage = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::Na_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((Na_Concentration_out * GNa_Rest + K_Concentration_out * GK_Rest + Cl_Concentration_in * GCl_Rest)/(Na_Concentration_in * GNa_Rest + K_Concentration_in * GK_Rest + Cl_Concentration_out * GCl_Rest));

        //Calculate equilibrium voltages for temperature
        ENa = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::Na_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((double) Na_Concentration_out/Na_Concentration_in);
        EK = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::K_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((double) K_Concentration_out/K_Concentration_in);
        EL = -70;


        double synapticSignallingConductance = 0;

        if (connections.count > 0)
        {
            synapticSignallingConductance = connections.GetCurrentSignallingConductance(((HH_NeuralNetwork*)neuralNetwork)->neurons->neurons, maxSodiumConductanceForArea);

            if (bias == -0.876268 && synapticSignallingConductance > 0.008)
                int grc = 1;

            ////original synapticSignallingConductance += (bias * ((HH_NeuralNetwork*)neuralNetwork)->weightsScale);

            synapticSignallingConductance = 1 / (1 + exp(-synapticSignallingConductance*1.3));
            synapticSignallingConductance = synapticSignallingConductance * 2 - 1;

            ////synapticSignallingConductance *= 0.001;
            synapticSignallingConductance *= ((HH_NeuralNetwork*)neuralNetwork)->weightsScale;


            //synapticSignallingConductance = 1 / (1 + exp(sigmoidGradient * synapticSignallingConductance)); //binary sigmoid -0.3

            //synapticSignallingConductance += bias;
            //synapticSignallingConductance *= ((HH_NeuralNetwork*)neuralNetwork)->weightsScale;

        }
        //else
            synapticSignallingConductance += setConductance * maxSodiumConductanceForArea;
            //synapticSignallingConductance += 0.1;


        ////double value = (double) rand()/RAND_MAX * 0.00001;

        ////double value = (double) rand()/RAND_MAX * noiseLevel;
        ////synapticSignallingConductance += value;

        ////double value = (double) rand()/RAND_MAX * noiseLevel;
        ////synapticSignallingConductance += HH_NeuralNetworks::NEURAL_NOISE;
        synapticSignallingConductance += noise;



        //if (receivingStimulus)
            //synStimulus = (maxSodiumConductanceForArea/150) * (ENa - membraneVoltage);
            //synStimulus = (maxSodiumConductanceForArea/3200) * (ENa - membraneVoltage);

            /*////original if (synapticSignallingConductance > maxSynapticSignallingConductance)
                maxSynapticSignallingConductance = synapticSignallingConductance;

            if (synapticSignallingConductance < minSynapticSignallingConductance)
                minSynapticSignallingConductance = synapticSignallingConductance;

            if (synapticSignallingConductance > 1)
                int grc = 1;

            if (synapticSignallingConductance > 0.9)
                int grc = 1;
                */

                synStimulus = synapticSignallingConductance * (ENa - membraneVoltage);


        synStimulus += setStimulus;

        if (synStimulus > 0)
            synStimulus = MathUtilities::Round(synStimulus, Simulation::floatingPointRounding);

        if (!Simulation::loadingData)
            membraneVoltage = IncMembraneVoltage();

        membraneVoltage = MathUtilities::Round(membraneVoltage, Simulation::floatingPointRounding);

        /*///original if (isnan(membraneVoltage))
            membraneVoltage = referenceVoltage;
            */


        double x, result;

        if (membraneVoltage < -HH_Neuron::MINMAX_VOLTAGE)
        {
            x = membraneVoltage + HH_Neuron::MINMAX_VOLTAGE;

            x *= -1;

            result = 1 / (1 + exp(-x*0.0233));

            result = (result - 0.5) * 2;

            result = -HH_Neuron::MINMAX_VOLTAGE - result * 75;

            membraneVoltage = result;

            //std::cout << result << "\n";
        }
        else
        if (membraneVoltage > HH_Neuron::MINMAX_VOLTAGE)
        {
            x = membraneVoltage - HH_Neuron::MINMAX_VOLTAGE;

            result = 1 / (1 + exp(-x*0.0233));

            result = (result - 0.5) * 2;

            result = HH_Neuron::MINMAX_VOLTAGE + result * 75;

            membraneVoltage = result;

            //std::cout << result << "\n";
        }
        //else
            //result = membraneVoltage;




        /*x = membraneVoltage;

        result = 1 / (1 + exp(-x*0.0333));

        result = (result - 0.5) * 2;

        membraneVoltage = result * 70;

        membraneVoltage = 150;

        x = membraneVoltage;

        result = 1 / (1 + exp(-x*0.0333));

        result = (result - 0.5) * 2;

        membraneVoltage = result;



        membraneVoltage = 100;

        x = membraneVoltage;

        result = 1 / (1 + exp(-x*0.0333));

        result = (result - 0.5) * 2;

        membraneVoltage = result;


        membraneVoltage = 50;

        x = membraneVoltage;

        result = 1 / (1 + exp(-x*0.0333));

        result = (result - 0.5) * 2;

        membraneVoltage = result;



        membraneVoltage = 25;

        x = membraneVoltage;

        result = 1 / (1 + exp(-x*0.0333));

        result = (result - 0.5) * 2;

        membraneVoltage = result;


        membraneVoltage = 12.5;

        x = membraneVoltage;

        result = 1 / (1 + exp(-x*0.0333));

        result = (result - 0.5) * 2;

        membraneVoltage = result;


        membraneVoltage = 6.25;

        x = membraneVoltage;

        result = 1 / (1 + exp(-x*0.0333));

        result = (result - 0.5) * 2;

        membraneVoltage = result;


        membraneVoltage = 3.125;

        x = membraneVoltage;

        result = 1 / (1 + exp(-x*0.0333));

        result = (result - 0.5) * 2;

        membraneVoltage = result;
        */

        /*if (membraneVoltage < -HH_Neuron::MINMAX_VOLTAGE)
        {
            x = membraneVoltage + HH_Neuron::MINMAX_VOLTAGE;

            x *= -1;

            result = 1 / (1 + exp(-x*0.0333));

            result = (result - 0.5) * 2;

            result = -HH_Neuron::MINMAX_VOLTAGE - result * 50;

            membraneVoltage = result;

            //std::cout << result << "\n";
        }
        else
        if (membraneVoltage > HH_Neuron::MINMAX_VOLTAGE)
        {
            x = membraneVoltage - HH_Neuron::MINMAX_VOLTAGE;

            result = 1 / (1 + exp(-x*0.0333));

            result = (result - 0.5) * 2;

            result = HH_Neuron::MINMAX_VOLTAGE + result * 50;

            membraneVoltage = result;

            //std::cout << result << "\n";
        }*/

        /*if (membraneVoltage < -HH_Neuron::MINMAX_VOLTAGE || membraneVoltage > HH_Neuron::MINMAX_VOLTAGE)
        {
            membraneVoltage = 1 / (1 + exp(sigmoidGradient * membraneVoltage)); //binary sigmoid -0.3

            membraneVoltage = (membraneVoltage * 600 - 300);
        }*/


        /*double x = HH_Neuron::MINMAX_VOLTAGE, result, gradient = -0.3, membraneVoltage2;

        for (gradient = -0.00001; gradient>-0.1;gradient-=0.001)
        {
        //for (x = HH_Neuron::MINMAX_VOLTAGE; x<=1000; x++)
        for (membraneVoltage2 = HH_Neuron::MINMAX_VOLTAGE; membraneVoltage2<=300; membraneVoltage2++)
        {
            x = membraneVoltage2 - HH_Neuron::MINMAX_VOLTAGE;

            result = 1 / (1 + exp(-x*0.0333)); //binary sigmoid -0.3

            result = (result - 0.5) * 2;

            result = HH_Neuron::MINMAX_VOLTAGE + result * 50;

            std::cout << result << "\n";
        }
        */

        /*for (x = 0; x<=300; x++)
        {
            result = exp(x); //binary sigmoid -0.3

            std::cout << result << "\n";
        }*/
        //}



        /*if (membraneVoltage > 200)
            membraneVoltage = 200;
        else
            if (membraneVoltage < -200)
                membraneVoltage = -200;

        if (membraneVoltage <= -1000 || membraneVoltage >= 1000)
            ResetHH();
            */


        }
        if (!Simulation::loadingData)
        {
        m = IncM();
        h = IncH();
        n = IncN();

        m = MathUtilities::Round(m, Simulation::floatingPointRounding);
        h = MathUtilities::Round(h, Simulation::floatingPointRounding);
        n = MathUtilities::Round(n, Simulation::floatingPointRounding);
        }


        if (m > 1 || h > 1 || n > 1 || m < 0 || h < 0 || n < 0)
            int grc = 1;

        //std::cout << "V: " << membraneVoltage << "\n";


        if (spikeCount % 10 == 0)
        {
            minVoltage = 999999;
            maxVoltage = -999999;
        }

        /*if (printCount % 100 == 0)
        {
            minVoltage = 999999;
            maxVoltage = -999999;

            std::cout << "V: " << membraneVoltage << "\n";
        }*/

        printCount++;

        if (membraneVoltage > maxVoltage)
            maxVoltage = membraneVoltage;

        if (membraneVoltage < minVoltage)
            minVoltage = membraneVoltage;

        if (membraneVoltage < 0)
        {
            spike = false;
        }
        else
        if (!spike && membraneVoltage > HH_Neuron::MEMBRANE_VOLTAGE_THRESHOLD)
        {
            spikeCount++;

            spike = true;

            ////connections.SetNeuralConnectionActivityTime(((HH_NeuralNetwork*)neuralNetwork)->neurons->neurons);
        }

        }
        else
        {
            double synapticSignallingConductance;

            if (connections.count > 0)
            {
                synapticSignallingConductance = connections.GetCurrentSignallingConductance(((HH_NeuralNetwork*)neuralNetwork)->neurons->neurons, 1.0);
                synapticSignallingConductance += bias;

                synapticSignallingConductance = 1 / (1 + exp(sigmoidGradient * synapticSignallingConductance)); //binary sigmoid -0.3


                /*double rndValue = (double) rand() / RAND_MAX;

                if (rndValue < synapticSignallingConductance && synapticSignallingConductance > 0.5)
                    */
                //if (synapticSignallingConductance > nonHH_Threshold)*/
                if (synapticSignallingConductance > 0.5)
                    membraneVoltage = 60;
                else
                    membraneVoltage = -70;
            }
            else
                if (setConductance >= 0.01)
                    membraneVoltage = 60;
                else
                    membraneVoltage = -70;


            if (membraneVoltage < 0)
            {
                spike = false;
            }
            else
                if (!spike && membraneVoltage > HH_Neuron::MEMBRANE_VOLTAGE_THRESHOLD)
                {
                    spikeCount++;

                    spike = true;

                    //signalling = 700;
                }
        }

        if (Graph::SCROLLING)
        {
            memcpy(membraneVoltageValues, &membraneVoltageValues[1], (Graph::MAX_GRAPH_INDEX-1) * sizeof(double));
            membraneVoltageValues[Graph::MAX_GRAPH_INDEX-1] = membraneVoltage;

            if (!Simulation::loadingData)
            {
            memcpy(mValues, &mValues[1], (Graph::MAX_GRAPH_INDEX-1) * sizeof(double));
            mValues[Graph::MAX_GRAPH_INDEX-1] = m;

            memcpy(nValues, &nValues[1], (Graph::MAX_GRAPH_INDEX-1) * sizeof(double));
            nValues[Graph::MAX_GRAPH_INDEX-1] = n;

            memcpy(hValues, &hValues[1], (Graph::MAX_GRAPH_INDEX-1) * sizeof(double));
            hValues[Graph::MAX_GRAPH_INDEX-1] = h;

            memcpy(graph_NaCurrent, &graph_NaCurrent[1], (Graph::MAX_GRAPH_INDEX-1) * sizeof(double));
            graph_NaCurrent[Graph::MAX_GRAPH_INDEX-1] = naCurrent;

            memcpy(graph_KCurrent, &graph_KCurrent[1], (Graph::MAX_GRAPH_INDEX-1) * sizeof(double));
            graph_KCurrent[Graph::MAX_GRAPH_INDEX-1] = kCurrent;

            memcpy(graph_LCurrent, &graph_LCurrent[1], (Graph::MAX_GRAPH_INDEX-1) * sizeof(double));
            graph_LCurrent[Graph::MAX_GRAPH_INDEX-1] = lCurrent;
            }
        }
        else
        {
            membraneVoltageValues[membraneVoltageIndex++] = membraneVoltage;

            if (membraneVoltageIndex >= Graph::MAX_GRAPH_INDEX)
            {
                membraneVoltageIndex = 0;
                //ResetHH();
            }
        }

        ////AddMembraneVoltageToGraphValues();


        //sprintf(text, "V(Cur): %f, V(Min): %f, V(Max): %f, Count: %d", membraneVoltage, minVoltage, maxVoltage, spikeCount);

        //sprintf(text, "V(Cur): %f, Stimulus: %f, Count: %d", membraneVoltage, synStimulus, spikeCount);
        ////sprintf(text, "V(Cur): %.1f, Stimulus: %.4f, Temp: %.0f", membraneVoltage, synStimulus, Tk);
        //sprintf(text, "V(Cur): %.1f, Temp: %.0f", membraneVoltage, Tk);
        //sprintf(text, "Temperature: %.0f degrees Celsius, Stimulus: %.6f", Tc, synStimulus);
    }

    void HH_Neuron::AddMembraneVoltageToGraphValues()
    {
        ////Graph::SCROLLING = false;
        if (Graph::SCROLLING)
        {
            memcpy(membraneVoltageValues, &membraneVoltageValues[1], (Graph::MAX_GRAPH_INDEX-1) * sizeof(double));
            membraneVoltageValues[Graph::MAX_GRAPH_INDEX-1] = membraneVoltage;
            ////membraneVoltageValues[Graph::MAX_GRAPH_INDEX-1] = 76;
        }
        else
        {
            membraneVoltageValues[membraneVoltageIndex++] = membraneVoltage;

            if (membraneVoltageIndex >= Graph::MAX_GRAPH_INDEX)
            {
                membraneVoltageIndex = 0;
                //ResetHH();
            }
        }
    }

    void HH_Neuron::DetermineElectromagneticSignallingErrors(double refVoltage)
    {
        if ((membraneVoltage > 0 && refVoltage <= 0) || (membraneVoltage <= 0 && refVoltage > 0))
        {
            if (errorsToRefNeuron.currentErrorDurationStartTime == 0)
                errorsToRefNeuron.currentErrorDurationStartTime = Simulation::currentSimTime;
        }
        else
            if (errorsToRefNeuron.currentErrorDurationStartTime != 0)
            {
                errorsToRefNeuron.totalErrorDuration += Simulation::currentSimTime - errorsToRefNeuron.currentErrorDurationStartTime;
                errorsToRefNeuron.currentErrorDurationStartTime = 0;
            }
    }

    /*void HH_Neuron::SaveVoltage(char* fileName)
    {

    }*/

    double HH_Neuron::GetStrongestABSWeight()
    {
        return connections.GetStrongestABSWeight();
    }

    Vector HH_Neuron::GetMinMaxWeight()
    {
        return connections.GetMinMaxWeight();
    }

    //void HH_Neuron::Draw(Vector minMaxWeight)
    void HH_Neuron::Draw(double maxABSWeight)
    {
        connections.Draw(this, ((HH_NeuralNetwork*)neuralNetwork)->neurons->neurons, maxABSWeight);

        glPushMatrix();
        glTranslatef(pos.x, pos.y, pos.z);

        glColor3f(1, 1, 1);
        //GraphicsUtilities::DrawTextStr(text, 0.0, Graph::HEIGHT, 0.0, 0.5);
        //GraphicsUtilities::DrawTextStr(text, 0.0, 0, 0.0, 0.3);
        //GraphicsUtilities::DrawTextStr(text, 0.0, Graph::HEIGHT/10, 0.0, 0.3);

        glLineWidth(4);

        glColor3f(1, 1, 1);
		glBegin(GL_LINE_STRIP);
		glVertex2f(0, 0);
		glVertex2f(Graph::MAX_GRAPH_INDEX, 0);
		glEnd();

        glLineWidth(2);

        //glColor3f(1, 1, 0);
        //glColor3f(0, 1, 0);

        glPushMatrix();
        glScalef(1, 2.5, 1);

        Color* graphColor = (((HH_NeuralNetwork*)neuralNetwork)->neurons->graphColor);

        glColor3f(graphColor->r, graphColor->g, graphColor->b);
        glBegin(GL_LINE_STRIP);
		for(uint32_t i = 0; i < Graph::MAX_GRAPH_INDEX; i++)
        {
			////glVertex2f((double) i, membraneVoltageValues[i] * 2.5);
			glVertex2f((double) i, membraneVoltageValues[i]);
		}
		glEnd();

		glColor3f(1, 1, 1);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < Graph::MAX_GRAPH_INDEX; i++)
        {
            glVertex2f(i, referenceVoltage);
            ////glVertex2f(Graph::MAX_GRAPH_INDEX, referenceVoltage+25);
        }
		glEnd();



		glPopMatrix();


		////sprintf(text, "%d", spikeCount);
		////GraphicsUtilities::DrawTextStr(text, Graph::MAX_GRAPH_INDEX + Graph::MAX_GRAPH_INDEX/10, 0, 0.0, 0.5);

		sprintf(text, "%d, Membrane Voltage: %f", spikeCount, membraneVoltage);
		GraphicsUtilities::DrawTextStr(text, Graph::MAX_GRAPH_INDEX + Graph::MAX_GRAPH_INDEX/10, 0, 0.0, 0.5);



		/*////
		glPushMatrix();
        glScalef(1, 100, 1);

        glColor3f(0, 1, 0);
		glBegin(GL_LINE_STRIP);
		for(uint32_t i = 0; i < Graph::MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, mValues[i]);
		}
		glEnd();


		glColor3f(0, 0, 1);
		glBegin(GL_LINE_STRIP);
		for(uint32_t i = 0; i < Graph::MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, nValues[i]);
		}
		glEnd();

        glColor3f(1, 0, 1);
		glBegin(GL_LINE_STRIP);
		for(uint32_t i = 0; i < Graph::MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, hValues[i]);
		}
		glEnd();


		glPopMatrix();




       glScalef(1, 700, 1);

        glColor3f(1, 0, 0);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < Graph::MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_NaCurrent[i]);
		}
		glEnd();

        glColor3f(1, 0.75, 0.25);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < Graph::MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_KCurrent[i]);
		}
		glEnd();

        glColor3f(1, 1, 0);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < Graph::MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_LCurrent[i]);
		}
		glEnd();


        glScalef(1, 10, 1);

		glColor3f(0, 1, 0);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < Graph::MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_NaCurrent[i] + graph_KCurrent[i] + graph_LCurrent[i]);
		}
		glEnd();
		*/


		glPopMatrix();
    }

    void HH_Neuron::ResetHH()
    {
        /*
        //membraneVoltage = -70;
        membraneVoltage = -80.875;
        //membraneVoltage = -66.325517523053534;
        ////membraneVoltage = referenceVoltage;

        m = 0;
        h = 1;
        n = 0;
        */

        ////equilibrum values for 37 C
        membraneVoltage = -81.357281222347424;
        m = 0.0080097454953444343;
        h = 0.9312798535106821;
        n = 0.12883604530856743;
    }

    void HH_Neuron::ResetSpikeCounts()
    {
        spikeCount =0;
    }

    double HH_Neuron::MEMBRANE_VOLTAGE_THRESHOLD = 0.0;

    double HH_Neuron::MINMAX_VOLTAGE = 125.0;
