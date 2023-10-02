#ifndef _HOURGLASS_H_
#define _HOURGLASS_H_

#include "elapsedMillis.h"

typedef unsigned long ulong;

/**
 * @brief hourglass-like timing class
 *
 */
class hourglass {
   public:
    hourglass(ulong numSteps, ulong timeStepMS);

    void reset();
    void pause();
    void resume();
    void update();
    bool isPaused();
    bool isFinished();
    void addTime(int timeToAdd);

    // Getters and setters
    void setTimeStep(int newTimeStep);
    ulong getTimeStep();

    ulong getNumSteps();
    ulong setNumSteps(ulong newNumSteps);
    
    ulong getCurrentStep();

    ulong getTotalTime();
    ulong getTimeRemaining();

    // Operator overloads
    operator std::string() const;
    hourglass& operator=(const hourglass& other);
    hourglass& operator=(const ulong& val);
    hourglass& operator+=(const ulong& val);
    hourglass& operator-=(const ulong& val);

   private:
    ulong _numSteps;
    ulong _timeStep;
    ulong _totalTime;
    ulong _timeRemaining;
    elapsedMillis _elapsedMillisTimer;
    bool _isPaused;
};

#endif