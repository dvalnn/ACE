#include "hourglass.h"

/**
 * @brief Construct a new hourglass::hourglass object
 *
 * @param numSteps number of time steps - defaults to 5
 * @param timeStepMS time in millisseconds per step - defaults to 2000
 */
Hourglass::Hourglass(ulong numSteps, ulong timeStepMS) {
    if (numSteps < 1 || timeStepMS < 1) {
        numSteps = 5;
        timeStepMS = 2000;
    }

    _numSteps = numSteps;
    _timeStep = timeStepMS;

    _isPaused = false;
    _totalTime = _timeStep * _numSteps;
    _timeRemaining = _totalTime;
    _elapsedMillisTimer = _totalTime - _timeRemaining;
}

void Hourglass::pause() { _isPaused = true; }

void Hourglass::resume() { _isPaused = false; }

void Hourglass::reset() {
    _elapsedMillisTimer = 0;
    _timeRemaining = _totalTime;
}

void Hourglass::update() {
    if (!_isPaused) {
        _timeRemaining = _totalTime < _elapsedMillisTimer
                             ? 0
                             : _totalTime - _elapsedMillisTimer;
    } else {
        _elapsedMillisTimer = _timeRemaining;
    }
}

bool Hourglass::isPaused() { return _isPaused; }

bool Hourglass::isFinished() { return _timeRemaining == 0; }

void Hourglass::addTime(int timeToAdd) {
    if (timeToAdd < 0) {
        timeToAdd = 0;
    }

    ulong newRemainingTime = timeToAdd + _timeRemaining;
    _timeRemaining =
        newRemainingTime > _totalTime ? _totalTime : newRemainingTime;
}

// ------------------- Getters and setters -------------------

ulong Hourglass::getTimeRemaining() { return _timeRemaining; }

ulong Hourglass::getTotalTime() { return _totalTime; }

void Hourglass::setTimeStep(int newTimeStep) {
    if (newTimeStep < 1) {
        newTimeStep = 1;
    }

    _timeStep = newTimeStep;
    _totalTime = _timeStep * _numSteps;
    _timeRemaining = _totalTime - _elapsedMillisTimer;
}

ulong Hourglass::getTimeStep() { return _timeStep; }

ulong Hourglass::setNumSteps(ulong newNumSteps) {
    if (newNumSteps < 1) {
        newNumSteps = 1;
    }

    _numSteps = newNumSteps;
    _totalTime = _timeStep * _numSteps;
    _timeRemaining = _totalTime - _elapsedMillisTimer;
}

ulong Hourglass::getNumSteps() { return _numSteps; }

ulong Hourglass::getCurrentStep() { return _timeRemaining / _timeStep + 1; }

// ------------------- Operator overloads -------------------

Hourglass::operator std::string() const {
    std::string retString =
        "Time Remaining: " + std::to_string(_timeRemaining) + "ms\n" + "\t " +
        std::to_string(_timeRemaining / 1000) + "s\n" +
        "Total Time: " + std::to_string(_totalTime) + "\n" +
        "Time Step: " + std::to_string(_timeStep) + "\n" +
        "Current Step: " + std::to_string(_timeRemaining / _timeStep + 1) +
        " / " + std::to_string(_numSteps) + "\n";

    return retString;
}

Hourglass& Hourglass::operator=(const Hourglass& other) {
    _numSteps = other._numSteps;
    _timeStep = other._timeStep;
    _totalTime = other._totalTime;
    _timeRemaining = other._timeRemaining;
    _elapsedMillisTimer = other._elapsedMillisTimer;
    _isPaused = other._isPaused;

    return *this;
}

Hourglass& Hourglass::operator=(const ulong& val) {
    _timeRemaining = val;
    _timeRemaining = _timeRemaining > _totalTime ? _totalTime : _timeRemaining;
    _timeRemaining = _timeRemaining < 0 ? 0 : _timeRemaining;
    return *this;
}

Hourglass& Hourglass::operator+=(const ulong& val) {
    _timeRemaining += val;
    _timeRemaining = _timeRemaining > _totalTime ? _totalTime : _timeRemaining;
    return *this;
}

Hourglass& Hourglass::operator-=(const ulong& val) {
    _timeRemaining -= val;
    _timeRemaining = _timeRemaining < 0 ? 0 : _timeRemaining;
    return *this;
}