#pragma once

#include <iostream>

class TimeMonitor
{
private:
    float LastFPSUpdateTime;
    float LastFrameTime;
    int FramesPerSecond;
    int TimeBetweenUpdates;
    float DeltaTime;
    int counter;

public:
    TimeMonitor();
    void Monitor();
    int GetFPS();
    float getDeltaTime();
};