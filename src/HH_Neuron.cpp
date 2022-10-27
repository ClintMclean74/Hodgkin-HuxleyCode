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

#include <math.h>
#include <string.h>
#include "HH_Neuron.h"
#include "GL/glew.h"
#include "GraphicsAndUI.h"
#include "HH_NeuralNetwork.h"
#include "HH_NeuralNetworks.h"
#include "MathUtilities.h"
#include "Simulation.h"

HH_Neuron::HH_Neuron()
{
    memset(membraneVoltageValues, 0, MembraneVoltageGraph::MAX_GRAPH_INDEX * sizeof(long double));
    nonHH_Threshold = 0.0;

    neuralNetwork = NULL;
    membrane_capacitance = 1;

    //HH squid concentrations
    Na_Concentration_out = 440;
    Na_Concentration_in = 50;

    K_Concentration_out = 20;
    K_Concentration_in = 400;

    Cl_Concentration_out = 560;
    Cl_Concentration_in = 40;

    SetTargetTemperature(Simulation::referenceTemperature);

    Tk = targetTk;


    //HH squid Maximum Conductances (millisiemens per cm2)
    GNa_Max = 120.0;
    GK_Max = 36.0;
    GL_Max = 0.3;
    GCl_Max = 120*15;

    //These values for the synapse conductance produce biologically plausible balances of activations and deactivations
    GNa_Max_Synapse = 8;
    GCl_Max_Synapse = 230;

    maxSodiumConductanceForArea = GNa_Max;// Na conductance [mS]
    maxPotasiumConductanceForArea = GK_Max;  // K conductance [mS]
    maxLeakConductanceForArea = GL_Max;  // leak conductance [mS]

    membraneVoltageIndex = 0;

    minVoltage = 999999;
    maxVoltage = -999999;

    minSynStimulus = 999999;
    maxSynStimulus = -999999;

    spike = false;
    spikeCount = 0;

    ENa = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::Na_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((long double) (Na_Concentration_out/Na_Concentration_in));
    EK = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::K_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((long double) (K_Concentration_out/K_Concentration_in));
    EL = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::Cl_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((long double) (Cl_Concentration_out/Cl_Concentration_in));

    GNa_Rest = 0.05;
    GK_Rest = 1;
    GCl_Rest = 0.45;

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

void HH_Neuron::saveMembraneVoltagesTraceGraph(FILE* file)
{
    char textBuffer[255];
    uint32_t length;
    uint32_t inc = MembraneVoltageGraph::MAX_GRAPH_INDEX / 300;

    for(uint32_t i = 0; i < MembraneVoltageGraph::MAX_GRAPH_INDEX; i+=inc)
    {
        sprintf(textBuffer, ",%.12Lf", membraneVoltageValues[i]);
        length = strlen(textBuffer);
        fwrite(textBuffer, sizeof(char), length, file);
    }
    fflush(file);
}

void HH_Neuron::CopyWeights(HH_Neuron* srcNeuron, int32_t replaceColumn, int32_t newColumn)
{
    bias = srcNeuron->bias;

    connections.CopyWeights(&srcNeuron->connections, replaceColumn, newColumn);
}

void HH_Neuron::Mutate(double rate)
{
    double biasMutationRate = 0.1;

    bias += (double) rand()/ RAND_MAX * biasMutationRate - biasMutationRate/2;

    bias = MathUtilities::Limit(bias, 1);

    bias = 0;

    connections.Mutate(rate);
}

void HH_Neuron::SetPos(Vector pos)
{
    this->pos = pos;
}

void HH_Neuron::SetMembraneVoltage(long double voltage, bool clamp)
{
    clampVoltage = voltage;
    voltageClamped = clamp;
}

void HH_Neuron::SetTargetTemperature(long double temperature)
{
    Tc = temperature;

    targetTk = ChemistryValues::KELVIN + this->Tc;
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

long double HH_Neuron::IncM()
{
    long double voltageDifference = Simulation::HH_GatingVariablesReferenceVoltage - membraneVoltage;

    long double aM;

    if (voltageDifference == -25) //equation produces divide by zero error when voltageDifference = -25, so adjust slightly
        voltageDifference = -25.0000000000001;

    double expDouble = exp(voltageDifference);
    long double expLongDouble = exp(voltageDifference);

    long double exponent = (voltageDifference + 25.0) / 10.0;
    long double divisor = (exp(exponent) - 1.0);

    aM = (0.1 * (voltageDifference + 25.0)) / divisor;

    exponent = voltageDifference / 18.0;
    long double bM = 4.0 * exp(exponent);

    aM *= q10AdjustedRateScale; //adjust for temperature difference from HH squid temperature (6.3 C)
    bM *= q10AdjustedRateScale;

    long double derivativeM = aM * (1.0 - m) - bM * m;

    m = m + derivativeM * Simulation::deltaTime;

    if (m > 1)
        m = 1;
    else
        if (m < 0)
            m = 0;

    return m;
}

long double HH_Neuron::IncH()
{
    long double voltageDifference = Simulation::HH_GatingVariablesReferenceVoltage - membraneVoltage;

    if (voltageDifference == -30) //equation produces divide by zero error when voltageDifference = -30, so adjust slightly
        voltageDifference = -30.0000000000001;

    long double exponent = (voltageDifference + 30) / 10;
    long double divisor = (exp(exponent) + 1.0);

    exponent = voltageDifference / 20.0;
    long double aH = 0.07 * exp(exponent);
    long double bH = 1.0 / divisor;

    aH *= q10AdjustedRateScale; //adjust for temperature difference from HH squid temperature (6.3 C)
    bH *= q10AdjustedRateScale;

    long double derivativeH = aH * (1.0 - h) - bH * h;

    h = h + derivativeH * Simulation::deltaTime;

    if (h > 1)
        h = 1;
    else
        if (h < 0)
            h = 0;

    return h;
}

long double HH_Neuron::IncN()
{
    long double voltageDifference = Simulation::HH_GatingVariablesReferenceVoltage - membraneVoltage;

    long double aN;

    if (voltageDifference == -10) //equation produces divide by zero error when voltageDifference = -10, so adjust slightly
        voltageDifference = -10.0000000000001;

    long double exponent = (voltageDifference + 10.0) / 10.0;
    long double divisor = exp(exponent) - 1;

    aN = 0.01 * (voltageDifference + 10.0) / divisor;

    exponent = -voltageDifference / 80.0;
    long double bN = 0.125 * exp(exponent);

    aN *= q10AdjustedRateScale; //adjust for temperature difference from HH squid temperature (6.3 C)
    bN *= q10AdjustedRateScale;

    long double derivativeN = aN * (1.0 - n) - bN * n;

    n = n + derivativeN * Simulation::deltaTime;

    if (n > 1)
        n = 1;
    else
        if (n < 0)
            n = 0;

    return n;
}

long double HH_Neuron::AdjustMembraneVoltage()
{
    naCurrent = maxSodiumConductanceForArea * m* (membraneVoltage - ENa) *m*m * h;
    kCurrent = maxPotasiumConductanceForArea * n* (membraneVoltage - EK) *n*n*n;
    lCurrent = maxLeakConductanceForArea * (membraneVoltage - EL);

    long double derivativeVoltage = (synStimulus - (naCurrent + kCurrent + lCurrent)) / membrane_capacitance;

    return membraneVoltage + derivativeVoltage * Simulation::deltaTime;
}

long double HH_Neuron::AdjustTemperature()
{
    if (Tk < targetTk)
    {
        if (Simulation::rfInducedIncTemperatureInstant)
            Tk = targetTk;
        else
        {
            Tk += Simulation::TempIncRatePerDelta;

            if (Tk > targetTk)
                Tk = targetTk;
        }
    }
    else
        if (Tk > targetTk)
        {
            if (Simulation::rfInducedDecTemperatureInstant)
                Tk = targetTk;
            else
            {
                Tk -= Simulation::TempDecRatePerDelta;

                if (Tk < targetTk)
                    Tk = targetTk;
            }
        }

    if (Tk > maxTk)
        maxTk = Tk;

    long double exponent = ((Tk - ChemistryValues::KELVIN) - Simulation::HH_SQUID_REFERENCE_TEMPERATURE)/10;
    q10AdjustedRateScale = pow(Simulation::HH_SQUID_REFERENCE_TEMPERATURE_Q10_RATE, exponent); //calculate the Q10 adjustment for the specified temperature

    return Tk;
}

void HH_Neuron::SetSynStimulus(double stimulus)
{
    this->synStimulus = stimulus;
}

void HH_Neuron::SetReceivingStimulus(bool value)
{
    receivingStimulus = value;
}

void HH_Neuron::Process(double noise)
{
    if (!Simulation::loadingData)
    {
        //Calculate equilibrium voltages for temperature
        ENa = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::Na_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((long double) (Na_Concentration_out/Na_Concentration_in));
        EK = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::K_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((long double) (K_Concentration_out/K_Concentration_in));
        EL = (ChemistryValues::gasConstant_R * Tk) / (ChemistryValues::Cl_Valence * ChemistryValues::faradayConstant) * ChemistryValues::lnToLogConversionFactor * log10((long double) (Cl_Concentration_out/Cl_Concentration_in));

        long double totalExcitorySynapseConductance = 0;
        long double totalInhibitorySynapseConductance = 0;
        long double excitorySynapseCurrent;
        long double inhibitorySynapseCurrent;

        synStimulus = 0;

        if (membraneVoltage >= 0)
            receivingPreSynapticStimulus = false;
        else
            if (membraneVoltage < 0)
                receivingPreSynapticStimulus = true;

        if (connections.count > 0 && receivingPreSynapticStimulus) //get received stimulus (excitory and inhibitory)
        {
            totalExcitorySynapseConductance = connections.GetExcitorySynapseConductance(((HH_NeuralNetwork*)neuralNetwork)->neurons->neurons);
            excitorySynapseCurrent = -totalExcitorySynapseConductance * GNa_Max_Synapse * (membraneVoltage - ENa);

            totalInhibitorySynapseConductance = connections.GetInhibitorySynapseConductance(((HH_NeuralNetwork*)neuralNetwork)->neurons->neurons);
            inhibitorySynapseCurrent = totalInhibitorySynapseConductance * GCl_Max_Synapse * (membraneVoltage - EL);//EK closer to resting membrane voltage so driving voltage far less than ENa, synapticSignalingConductance then would have to be greater negative

            ////Note inverted (-totalExcitorySynapseConductance ) because (membraneVoltage - ENa) should be negative = positive charge flowing into cell. This increases mV so stimulus should be +
            //// if (membraneVoltage - EL) = + = negative charge flowing out of cell. This decreases mV so stimulus should be - and totalInhibitorySynapseConductance < 0 so works
            //// also, although negative VGC current = a + charge flowing into the cell
            //// and a positive VGC current = + charge flowing out of cell
            //// a positive stimulus though = flowing into cell


            synStimulus = excitorySynapseCurrent + inhibitorySynapseCurrent;

            if (synStimulus > maxSynStimulus)
                maxSynStimulus = synStimulus;

            if (synStimulus < minSynStimulus)
            minSynStimulus = synStimulus;
        }

        synStimulus += noise;

        synStimulus += setStimulus;

        if (!Simulation::loadingData)
        {
            membraneVoltage = AdjustMembraneVoltage();
        }

        double x, result;

        /*////if (membraneVoltage < -HH_Neuron::MINMAX_VOLTAGE)
        {
            x = membraneVoltage + HH_Neuron::MINMAX_VOLTAGE;
            x *= -1;

            result = 1 / (1 + exp(-x*0.0233));
            result = (result - 0.5) * 2;
            result = -HH_Neuron::MINMAX_VOLTAGE - result * 75;

            membraneVoltage = result;
        }
        else
            if (membraneVoltage > HH_Neuron::MINMAX_VOLTAGE)
            {
                x = membraneVoltage - HH_Neuron::MINMAX_VOLTAGE;

                result = 1 / (1 + exp(-x*0.0233));
                result = (result - 0.5) * 2;
                result = HH_Neuron::MINMAX_VOLTAGE + result * 75;

                membraneVoltage = result;
            }*/
    }

    if (!Simulation::loadingData)
    {
        m = IncM();
        h = IncH();
        n = IncN();

        Tk = AdjustTemperature();
    }

    if (spikeCount % 10 == 0)
    {
        minVoltage = 999999;
        maxVoltage = -999999;
    }

    printCount++;

    if (membraneVoltage > maxVoltage)
        maxVoltage = membraneVoltage;

    if (membraneVoltage < minVoltage)
        minVoltage = membraneVoltage;

    if (!spike && membraneVoltage > Simulation::preSynapticaActivityVoltageThreshold)
    {
        spikeCount++;

        spike = true;
    }
    else
        if (membraneVoltage <= Simulation::preSynapticaActivityVoltageThreshold - 10 && spike)
        {
            spike = false;
        }

    if (MembraneVoltageGraph::SCROLLING)
    {
        memcpy(membraneVoltageValues, &membraneVoltageValues[1], (MembraneVoltageGraph::MAX_GRAPH_INDEX-1) * sizeof(long double));
        membraneVoltageValues[MembraneVoltageGraph::MAX_GRAPH_INDEX-1] = membraneVoltage;

        if (!Simulation::loadingData)
        {
            memcpy(mValues, &mValues[1], (MembraneVoltageGraph::MAX_GRAPH_INDEX-1) * sizeof(long double));
            mValues[MembraneVoltageGraph::MAX_GRAPH_INDEX-1] = m;

            memcpy(nValues, &nValues[1], (MembraneVoltageGraph::MAX_GRAPH_INDEX-1) * sizeof(long double));
            nValues[MembraneVoltageGraph::MAX_GRAPH_INDEX-1] = n;

            memcpy(hValues, &hValues[1], (MembraneVoltageGraph::MAX_GRAPH_INDEX-1) * sizeof(long double));
            hValues[MembraneVoltageGraph::MAX_GRAPH_INDEX-1] = h;

            memcpy(graph_NaCurrent, &graph_NaCurrent[1], (MembraneVoltageGraph::MAX_GRAPH_INDEX-1) * sizeof(long double));
            graph_NaCurrent[MembraneVoltageGraph::MAX_GRAPH_INDEX-1] = naCurrent;

            memcpy(graph_KCurrent, &graph_KCurrent[1], (MembraneVoltageGraph::MAX_GRAPH_INDEX-1) * sizeof(long double));
            graph_KCurrent[MembraneVoltageGraph::MAX_GRAPH_INDEX-1] = kCurrent;

            memcpy(graph_LCurrent, &graph_LCurrent[1], (MembraneVoltageGraph::MAX_GRAPH_INDEX-1) * sizeof(long double));
            graph_LCurrent[MembraneVoltageGraph::MAX_GRAPH_INDEX-1] = lCurrent;
        }
    }
    else
    {
        membraneVoltageValues[membraneVoltageIndex++] = membraneVoltage;

        if (membraneVoltageIndex >= MembraneVoltageGraph::MAX_GRAPH_INDEX)
        {
            membraneVoltageIndex = 0;
        }
    }
}

double HH_Neuron::GetStrongestABSWeight()
{
    return connections.GetStrongestABSWeight();
}

void HH_Neuron::Draw(double maxABSWeight, long double* comparisonVoltageValues)
{
    if (GraphicsAndUI::connectionsVisible)
        connections.Draw(this, ((HH_NeuralNetwork*)neuralNetwork)->neurons->neurons, maxABSWeight);

    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);

    glColor3f(1, 1, 1);
    glLineWidth(4);

    glColor3f(1, 1, 1);
	glBegin(GL_LINE_STRIP);
	glVertex2f(0, 0);
	glVertex2f(MembraneVoltageGraph::MAX_GRAPH_INDEX, 0);
	glEnd();

    glLineWidth(2);

    glPushMatrix();
    glScalef(1, 2.5, 1);

    Color* graphColor = (((HH_NeuralNetwork*)neuralNetwork)->neurons->graphColor);

    if (drawVoltage)
    {
        double membraneVoltageDouble;

        glColor3f(graphColor->r, graphColor->g, graphColor->b);

        glBegin(GL_LINE_STRIP);
        for(uint32_t i = 0; i < MembraneVoltageGraph::MAX_GRAPH_INDEX; i++)
        {
            glVertex2f((double) i, membraneVoltageValues[i]);
        }
        glEnd();
    }

    if (comparisonVoltageValues)
    {
        glColor3f(1, 1, 0);
        glBegin(GL_LINE_STRIP);
        for(uint32_t i = 0; i < MembraneVoltageGraph::MAX_GRAPH_INDEX; i++)
        {
            glVertex2f((double) i, (membraneVoltageValues[i] - comparisonVoltageValues[i]) *100000);
        }
        glEnd();
    }


    if (drawEquilibrumVoltages)
    {
        if (Tc == Simulation::referenceTemperature)
        {
            glColor3f(1, 0, 0);
            glBegin(GL_LINE_STRIP);
            glVertex2f(0, ENa);
            glVertex2f(MembraneVoltageGraph::MAX_GRAPH_INDEX, ENa);
            glEnd();

            glColor3f(1, 0.75, 0.25);
            glBegin(GL_LINE_STRIP);
            glVertex2f(0, EK);
            glVertex2f(MembraneVoltageGraph::MAX_GRAPH_INDEX, EK);
            glEnd();

            glColor3f(1, 1, 0);
            glBegin(GL_LINE_STRIP);
            glVertex2f(0, EL);
            glVertex2f(MembraneVoltageGraph::MAX_GRAPH_INDEX, EL);
            glEnd();
        }
        else
        {
            glColor3f(1, 0, 0);
            glBegin(GL_LINES);
            for(uint32_t i = 0; i < MembraneVoltageGraph::MAX_GRAPH_INDEX; i++)
            {
                glVertex2f(i, ENa);
            }
            glEnd();

            glColor3f(1, 0.75, 0.25);
            glBegin(GL_LINES);
            for(uint32_t i = 0; i < MembraneVoltageGraph::MAX_GRAPH_INDEX; i++)
            {
                glVertex2f(i, EK);
            }
            glEnd();

            glColor3f(1, 1, 0);
            glBegin(GL_LINES);
            for(uint32_t i = 0; i < MembraneVoltageGraph::MAX_GRAPH_INDEX; i++)
            {
                glVertex2f(i, EL);
            }
            glEnd();
        }
    }

	glPopMatrix();

	glColor3f(graphColor->r, graphColor->g, graphColor->b);

	sprintf(text, "%d, Membrane Voltage: %.12Lf", spikeCount, membraneVoltage);
	GraphicsAndUI::DrawTextStr(text, MembraneVoltageGraph::MAX_GRAPH_INDEX + MembraneVoltageGraph::MAX_GRAPH_INDEX/10, 0, 0.0, 0.5);

	sprintf(text, "synStimulus: %.12Lf", synStimulus);
	GraphicsAndUI::DrawTextStr(text, MembraneVoltageGraph::MAX_GRAPH_INDEX + MembraneVoltageGraph::MAX_GRAPH_INDEX/10, -100, 0.0, 0.5);

	if (drawGatingVariables)
    {
        glPushMatrix();
        glScalef(1, 100, 1);

        glColor3f(0, 1, 0);
        glBegin(GL_LINE_STRIP);
        for(uint32_t i = 0; i < MembraneVoltageGraph::MAX_GRAPH_INDEX; i++)
        {
            glVertex2f(i, mValues[i]);
        }
        glEnd();

        glColor3f(0, 0, 1);
        glBegin(GL_LINE_STRIP);
        for(uint32_t i = 0; i < MembraneVoltageGraph::MAX_GRAPH_INDEX; i++)
        {
            glVertex2f(i, nValues[i]);
        }
        glEnd();

        glColor3f(1, 0, 1);
        glBegin(GL_LINE_STRIP);
        for(uint32_t i = 0; i < MembraneVoltageGraph::MAX_GRAPH_INDEX; i++)
        {
            glVertex2f(i, hValues[i]);
        }
        glEnd();

        glPopMatrix();
    }

    if (drawCurrents)
    {
        glScalef(1, 0.1, 1);

        glColor3f(1, 0, 0);
        glBegin(GL_LINES);
        for(uint32_t i = 0; i < MembraneVoltageGraph::MAX_GRAPH_INDEX; i++)
        {
            glVertex2f(i, graph_NaCurrent[i]);
        }
        glEnd();

        glColor3f(1, 0.75, 0.25);
        glBegin(GL_LINES);
        for(uint32_t i = 0; i < MembraneVoltageGraph::MAX_GRAPH_INDEX; i++)
        {
            glVertex2f(i, graph_KCurrent[i]);
        }
        glEnd();

        glColor3f(1, 1, 0);
        glBegin(GL_LINES);
        for(uint32_t i = 0; i < MembraneVoltageGraph::MAX_GRAPH_INDEX; i++)
        {
            glVertex2f(i, graph_LCurrent[i]);
        }
        glEnd();


        glScalef(1, 10, 1);
        glColor3f(0, 1, 0);
        glBegin(GL_LINES);
        for(uint32_t i = 0; i < MembraneVoltageGraph::MAX_GRAPH_INDEX; i++)
        {
            glVertex2f(i, graph_NaCurrent[i] + graph_KCurrent[i] + graph_LCurrent[i]);
        }
        glEnd();
    }

	glPopMatrix();
}

void HH_Neuron::ResetHH()
{
    //these values were determined to be stable equilibrium values, running the simulation without stimulus
    membraneVoltage = -66.548431611267787;
    m = 0.044029298256692072;
    h = 0.64902711752519804;
    n = 0.30234115639914516;

    SetTargetTemperature(Simulation::referenceTemperature);
    Tk = targetTk;
}

void HH_Neuron::ResetSpikeCounts()
{
    spikeCount =0;
}

double HH_Neuron::MINMAX_VOLTAGE = 125.0;
