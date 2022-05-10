#include "Simulation.h"

int main(int argc, char *argv[])
{
    srand(time(NULL));
    Simulation *simulation = new Simulation(1600, 900, "Shoal of fish // Maciej Chotkowski");

    while (simulation->isRunning())
    {
        simulation->Update();
    }

    delete simulation;
    return 0;
}