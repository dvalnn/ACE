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
    void setTimeStep(int newTimeStep);
    uint32_t getTimeStep();

    uint32_t getNumSteps();
    void setNumSteps(uint32_t newNumSteps);

    uint32_t getCurrentStep();

    uint32_t getTotalTime();
    uint32_t getTimeRemaining();

    // Operator overloads
    operator std::string() const;
    Hourglass& operator=(const Hourglass& other);
    // Hourglass& operator=(const uint32_t& val);
    // Hourglass& operator+=(const uint32_t& val);
    // Hourglass& operator-=(const uint32_t& val);

   private:
    uint32_t numSteps;
    uint32_t timeStep;
    uint32_t totalTime;
    uint32_t timeRemaining;
    elapsedMillis elapsedMillisTimer;
    bool paused;
};

#endif