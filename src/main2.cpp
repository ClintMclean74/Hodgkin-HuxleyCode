/*
//HH_Sim

6.3 C = 2 spikes
11.17 = 3 spikes

stimulus = 20 nA for 10 msec
*/


#include <iostream>
#include <cmath>
#include "GL/glew.h"
#include "GL/glut.h"

const static uint32_t MAX_GRAPH_INDEX = 100000;
const double minGraphHeight = -100, maxGraphHeight = 100;
//const double minGraphX = 25000, maxGraphX = 30000;
const double minGraphX = 0, maxGraphX = 100000;

double graphMembraneVoltageData[MAX_GRAPH_INDEX];
double graph_m[MAX_GRAPH_INDEX];
double graph_h[MAX_GRAPH_INDEX];
double graph_n[MAX_GRAPH_INDEX];
double graph_NaCurrent[MAX_GRAPH_INDEX];
double graph_KCurrent[MAX_GRAPH_INDEX];
double graph_LCurrent[MAX_GRAPH_INDEX];


uint32_t graphDataIndex = 0;

double stimulus = 0.0;
uint32_t threshholdCount = 0;

    double time_step = 0.01;
    double num_samples = 100000;

    double gasConstant_R = 8.31446261815324;// J⋅K^−1⋅mol^−1
    double faradayConstant = 96.485; // C/mol

    const uint16_t Na_Concentration_out = 145;
    const uint16_t Na_Concentration_in = 15;

    const uint16_t K_Concentration_out = 5;
    const uint16_t K_Concentration_in = 150;

    /*const uint16_t Na_Concentration_out = 500;
    const uint16_t Na_Concentration_in = 10;

    const uint16_t K_Concentration_out = 4;
    const uint16_t K_Concentration_in = 300;
    */

    double Na_Valence = 1;
    double K_Valence = 1;

    //double Tc = 6;
    //double Tc = 25;
    //double Tc = 37;
    double Tc = 45;
    //double Tc = 60;
    //double Tc = 120;

    double Tk = 273.15 + Tc;

    double lnToLogConversionFactor = 2.303;

    double initial_voltage = -70.0;
    double referenceVoltage = -70.0;

    double membrane_surface = 4000.0;  // [um^2] surface area of the membrane
    double membrane_capacitance_density = 1.0;   // [uF/cm^2] membrane capacitance density
    double membrane_capacitance = membrane_capacitance_density * membrane_surface * 1e-8;   // [uF] membrane capacitance

    /*double GNa = 120.0;
    double GK = 36.0;
    double GL = 0.3;
    double ENa = 125.0;
    double EK = -55.0;
    double EL = -25.0;
    */

    /*double GNa = 120.0;
    double GK = 36.0;
    double GL = 0.3;
    double ENa = 60.0;
    double EK = -88.0;
    double EL = -61.0;
    */

    /*double GNa = 120.0;
    double GK = 36.0;
    double GL = 0.3;
    */

    double GNa = 300.0;
    double GK = 150.0;
    double GL = 0.033;


    double ENa = (gasConstant_R * Tk) / (Na_Valence * faradayConstant) * lnToLogConversionFactor * log10((double) Na_Concentration_out/Na_Concentration_in);
    double EK = (gasConstant_R * Tk) / (K_Valence * faradayConstant) * lnToLogConversionFactor * log10((double) K_Concentration_out/K_Concentration_in);
    //double EL = 10.6;
    //double EL = -70;

    //double EK = -88.0;
    //double EL = -61.0;
    //double EL = -57.2;
    double EL = 0;

    double sodium_conductance = GNa * membrane_surface * 1e-8;// Na conductance [mS]
    double kalium_conductance = GK * membrane_surface * 1e-8;  // K conductance [mS]
    double leak_conductance = GL * membrane_surface * 1e-8;  // leak conductance [mS]

class Simulation
{
    public:
    double membraneVoltage;
    double m;
    double h;
    double n;

    double naCurrent;
    double kCurrent;
    double lCurrent;

    double incM(const double deltaTime)
    {
        double voltageDifference = membraneVoltage - referenceVoltage;

        double aM = 0.1 * (voltageDifference - 25.0) / (1.0 - exp(-(voltageDifference - 25.0) / 10.0));
        double bM = 4.0 * exp(-voltageDifference / 18.0);

        double derivativeM = aM * (1.0 - m)- bM * m;

        return (m + derivativeM * deltaTime);
    }

    double incH(const double deltaTime)
    {
        double voltageDifference = membraneVoltage - referenceVoltage;

        double aH = 0.07 * exp(-voltageDifference / 20.0);
        double bH = 1.0 / (1.0 + exp(-(voltageDifference - 30.0) / 10.0));

        double derivativeH = aH * (1.0 - h) - bH * h;

        return h + derivativeH * deltaTime;
    }

    double incN(const double deltaTime)
    {
        double voltageDifference = membraneVoltage - referenceVoltage;

        double aN = 0.01 * (voltageDifference - 10.0) / (1.0 - exp(-(voltageDifference - 10.0) / 10.0));
        double bN = 0.125 * exp(-voltageDifference / 80.0);

        double derivativeN = aN (1.0 - n) - bN * n;

        return n + derivativeN * deltaTime;
    }

    double step_membraneVoltage(const double time, const double stimulus)
    {
        naCurrent = sodium_conductance * m*m*m * h * (ENa - membraneVoltage);
        kCurrent = kalium_conductance * n*n*n*n * (EK - membraneVoltage);
        lCurrent = leak_conductance * (EL - membraneVoltage);

        double derivative_potential = (naCurrent + kCurrent + lCurrent + stimulus) / membrane_capacitance;

        return membraneVoltage + derivative_potential * time;
    }

    Simulation(const double initial_potential, const double initial_m, const double initial_h, const double initial_n)
    {
        membraneVoltage = initial_potential;
        m = initial_m;
        h = initial_h;
        n = initial_n;
    }

    void step(const double deltaTime, const double current_stimulus)
    {
        membraneVoltage = step_membraneVoltage(deltaTime, current_stimulus);
        m = step_m(deltaTime);
        h = step_h(deltaTime);
        n = step_n(deltaTime);

        //std::cout << "V: " << membraneVoltage << "\n";
    }

    double latest_potential() const
    {
        return membraneVoltage;
    }
};

void run_simulation()
{

    Simulation sim(initial_voltage, 0, 0, 0);

    double Tk37 = 273.15 + 37;

    double ENa37 = (gasConstant_R * Tk37) / (Na_Valence * faradayConstant) * lnToLogConversionFactor * log10((double) Na_Concentration_out/Na_Concentration_in);
    double naCurrent37 = sodium_conductance * (ENa37 - sim.membraneVoltage);


    double Tk45 = 273.15 + 45;

    double ENa45 = (gasConstant_R * Tk45) / (Na_Valence * faradayConstant) * lnToLogConversionFactor * log10((double) Na_Concentration_out/Na_Concentration_in);
    double naCurrent45 = sodium_conductance * (ENa45 - sim.membraneVoltage);





    //double stimulus = 0.0;
    //double stimulus = 0.0002965; //stimulus for 60 C
    //double stimulus = 0.0015;

    uint32_t count = 0;

    bool threshold = false;

    threshholdCount = 0;

    stimulus = 0.01;
    for (double j = 0; j < num_samples; j++)
    {
        //stimulus = 0.0;
        //stimulus = 0.0;
        //if (j >= 25000 && j <= 75000) stimulus += 0.00001;
        //if (j >= 25000 && j <= 75000) stimulus = 0.0005;
        //if (j >= 25000 && j <= 75000) stimulus = 0.01; //larger stimulus normal 25 C temperature doesn't produce prolonged (sub threshold?) spikes, higher temperatures does
        //if (j >= 25000 && j <= 75000) stimulus = 0.0017;
        //if (j >= minGraphX && j <= maxGraphX)
        //if (j >= minGraphX && j <= minGraphX + 4)
            //stimulus = 0.01;
            //stimulus = 0.01;


        sim.step(time_step, stimulus);

        //std::cout << "V: " << graphMembraneVoltageData[graphDataIndex]<< "\n";

        graphDataIndex = j;

        if (!threshold && graphMembraneVoltageData[graphDataIndex]>0)
        {
            threshold = true;
            //stimulus -= 0.00001;

            threshholdCount++;
        }
        else
            if (threshold && graphMembraneVoltageData[graphDataIndex]<0)
                threshold = false;

        if (graphMembraneVoltageData[graphDataIndex]<0)
            count++;
        else
            count=0;

        if (count>1000)
            int grc = 1;

        graphMembraneVoltageData[graphDataIndex] = sim.latest_potential();
        graph_m[graphDataIndex] = sim.m;
        graph_h[graphDataIndex] = sim.h;
        graph_n[graphDataIndex] = sim.n;

        graph_NaCurrent[graphDataIndex] = sim.naCurrent;
        graph_KCurrent[graphDataIndex] = sim.kCurrent;
        graph_LCurrent[graphDataIndex] = sim.lCurrent;

        if (graphDataIndex >= MAX_GRAPH_INDEX)
            graphDataIndex = 0;

        stimulus -= 0.0000001;
    }

    /*if (threshholdCount>50)
        stimulus += 0.00001;
    else
        int grc = 1;
        */

    //stimulus -= 1;
}

void display(void)
{
    run_simulation();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//gluPerspective(45, (GLdouble)glutGet(GLUT_WINDOW_HEIGHT) / glutGet(GLUT_WINDOW_WIDTH) * 1.3, 1.0, 12000.0);
		//glOrtho(0, 100000, minGraphHeight, maxGraphHeight, 1.0, 1000.0);

		//glOrtho(0, 100000, minGraphHeight, maxGraphHeight, 1.0, 1000.0);
		glOrtho(minGraphX-100, maxGraphX+100, minGraphHeight, maxGraphHeight, 1.0, 1000.0);

		glMatrixMode(GL_MODELVIEW);

		gluLookAt(0.0, 0.0, 0.0,
			0.0, 0.0, 0.0,
			0.0, 1.0, 0.);

		glLoadIdentity();

		glTranslatef(0,0, -10);


        glLineWidth(1);

        glColor3f(1, 1, 1);
		glBegin(GL_LINES);
		glVertex2f(0, 0);
		glVertex2f(100000, 0);
		glEnd();

        glLineWidth(4);

        glColor3f(0, 1, 0);
        glBegin(GL_LINES);
		for(uint32_t i = 0; i < MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graphMembraneVoltageData[i]);

			//std::cout << "V: " << graphMembraneVoltageData[graphDataIndex]<< "\n";
		}
		glEnd();


		glLineWidth(2);

        /*
        glPushMatrix();
        glScalef(1, 100, 1);

        glColor3f(0, 1, 0);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_m[i]);
		}
		glEnd();


		glColor3f(0, 0, 1);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_n[i]);
		}
		glEnd();

        glColor3f(1, 0, 1);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_h[i]);
		}
		glEnd();

		glPopMatrix();
*/

        glScalef(1, 700, 1);

        glColor3f(1, 0, 0);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_NaCurrent[i]);
		}
		glEnd();

        glColor3f(1, 0.75, 0.25);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_KCurrent[i]);
		}
		glEnd();

        glColor3f(1, 1, 0);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_LCurrent[i]);
		}
		glEnd();



/*		glColor3f(0, 1, 0);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_NaCurrent[i] + graph_KCurrent[i] + graph_LCurrent[i]);
		}
		glEnd();
*/






	glutSwapBuffers();

	glutPostRedisplay();
}


void InitGL()
{
    glutInitDisplayMode(GLUT_DEPTH |
		GLUT_DOUBLE |
		GLUT_RGBA |
		GLUT_MULTISAMPLE);

    //Now create the graphics window
    glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT));
    glutCreateWindow("SDR Reradiation Spectrum Analyzer");

	GLenum err = glewInit();

	glutDisplayFunc(display);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 0.0);

	glPointSize(5);

	glShadeModel(GL_SMOOTH);
}

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std::cerr << "requires number of benchmarking loops as command line argument #1";
        return 1;
    }

    glutInit(&argc, argv);
    InitGL();

    double num_loops = atoi(argv[1]);

    /*ENa = (gasConstant_R * Tk) / (Na_Valence * faradayConstant) * lnToLogConversionFactor;
    double logValue = (double) Na_Concentration_out/Na_Concentration_in;
    ENa *= log10(logValue);

    EK = (gasConstant_R * Tk) / (K_Valence * faradayConstant) * lnToLogConversionFactor;
    logValue = (double) K_Concentration_out/K_Concentration_in;
    EK *= log10(logValue);
    */


    /*ENa = 60.0;
    EK = -88.0;
    EL = -61.0;
    */


    glutMainLoop();

    getchar();
}
