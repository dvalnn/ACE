#ifndef _HOURGLASS_H_
#define _HOURGLASS_H_

#include <elapsedMillis.h>

typedef unsigned long ulong;

/**
 * @brief Hourglass-like timing class
 *
 */
class Hourglass {
   public:
    Hourglass(ulong numSteps, ulong timeStepMS);

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
    void setNumSteps(ulong newNumSteps);

    ulong getCurrentStep();

    ulong getTotalTime();
    ulong getTimeRemaining();

    // Operator overloads
    operator std::string() const;
    Hourglass& operator=(const Hourglass& other);
    // Hourglass& operator=(const ulong& val);
    // Hourglass& operator+=(const ulong& val);
    // Hourglass& operator-=(const ulong& val);

   private:
    ulong _numSteps;
    ulong _timeStep;
    ulong _totalTime;
    ulong _timeRemaining;
    elapsedMillis _elapsedMillisTimer;
    bool _isPaused;
};

#endif