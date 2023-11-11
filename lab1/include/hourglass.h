#ifndef _HOURGLASS_H_
#define _HOURGLASS_H_

#include <elapsedMillis.h>
#include <inttypes.h>

/**
 * @brief Hourglass-like timing class
 *
 */
class Hourglass {
   public:
    Hourglass(uint32_t numSteps, uint32_t timeStepMS);

    void reset();
    void pause();
    void resume();
    void update();
    bool isPaused();
    bool isFinished();
    void addTime(uint32_t timeToAdd);

    // Getters and setters
    uint32_t getTimeStep();
    void setTimeStep(uint32_t newTimeStep);

    int getNumSteps();
    void setNumSteps(int newNumSteps);

    int getCurrentStep();

    uint32_t getTotalTime();
    uint32_t getTimeRemaining();

    // Operator overloads
    operator std::string() const;
    Hourglass& operator=(const Hourglass& other);

   private:
    bool paused;
    int timeStep;
    int numSteps;
    uint32_t totalTime;
    uint32_t timeRemaining;
    elapsedMillis elapsedMillisTimer;
};

#endif