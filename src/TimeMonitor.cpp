#include "TimeMonitor.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// This class manages time. Calculates current frames per second in a given interval and returns delta time.
TimeMonitor::TimeMonitor()
{
    LastFrameTime = 0;
    LastFPSUpdateTime = 0;
    FramesPerSecond = 0;
    TimeBetweenUpdates = 1;
    counter = 0;
}

void TimeMonitor::Monitor()
{
    float currentTime = glfwGetTime();
    DeltaTime = currentTime - LastFrameTime;

    if (currentTime - LastFPSUpdateTime >= TimeBetweenUpdates)
    {
        FramesPerSecond = (float)(counter) / (currentTime - LastFPSUpdateTime);
        LastFPSUpdateTime = currentTime;
        counter = 0;
    }
    counter++;
    LastFrameTime = currentTime;
}

int TimeMonitor::GetFPS()
{
    return FramesPerSecond;
}

float TimeMonitor::getDeltaTime()
{
    return DeltaTime;
}