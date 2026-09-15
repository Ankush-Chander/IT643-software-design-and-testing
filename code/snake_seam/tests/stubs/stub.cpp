#include "raylib.h"
namespace stub {
    double time = 0; double timeStep = 0; std::vector<int> randomValues; std::map<int,bool> keys; int framesUntilClose = 0;
    int drawCalls=0, textureDraws=0, textDraws=0, soundPlays=0, rectDraws=0, textureLoads=0, textureUnloads=0;
    void reset() { time=0; timeStep=0; randomValues.clear(); keys.clear(); framesUntilClose=0;
        drawCalls=textureDraws=textDraws=soundPlays=rectDraws=textureLoads=textureUnloads=0; }
}
