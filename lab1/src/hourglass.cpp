#include "hourglass.h"

/**
 * @brief Construct a new hourglass::hourglass object
 *
 * @param numSteps number of time steps - defaults to 5
 * @param timeStepMS time in millisseconds per step - defaults to 2000
 */
Hourglass::Hourglass(uint32_t numSteps, uint32_t timeStepMS) {
    if (numSteps < 1 || timeStepMS < 1) {
        numSteps = 5;
        timeStepMS = 2000;
    }

    this->numSteps = numSteps;
    this->timeStep = timeStepMS;

    this->paused = false;
    this->totalTime = this->timeStep * this->numSteps;
    this->timeRemaining = this->totalTime;
    this->elapsedMillisTimer = this->totalTime - this->timeRemaining;
}

void Hourglass::pause() { this->paused = true; }

void Hourglass::resume() { this->paused = false; }

void Hourglass::reset() {
    this->elapsedMillisTimer = 0;
    this->timeRemaining = this->totalTime;
}

void Hourglass::update() {
    if (!this->paused) {
        this->timeRemaining = this->totalTime < this->elapsedMillisTimer
                                  ? 0
                                  : this->totalTime - this->elapsedMillisTimer;
    } else {
        this->elapsedMillisTimer = this->totalTime - this->timeRemaining;
    }
}

bool Hourglass::isPaused() { return this->paused; }

bool Hourglass::isFinished() { return this->timeRemaining == 0; }

void Hourglass::addTime(uint32_t timeToAdd) {
    if(timeToAdd >= this->elapsedMillisTimer){
        timeToAdd = this->elapsedMillisTimer;
    }
    this->elapsedMillisTimer -= timeToAdd; 
}

// ------------------- Getters and setters -------------------

uint32_t Hourglass::getTimeRemaining() { return this->timeRemaining; }

uint32_t Hourglass::getTotalTime() { return this->totalTime; }

void Hourglass::setTimeStep(int newTimeStep) {
    if (newTimeStep < 1) {
        newTimeStep = 1;
    }

    this->timeStep = newTimeStep;
    this->totalTime = this->timeStep * this->numSteps;
    this->timeRemaining = this->totalTime - this->elapsedMillisTimer;
}

uint32_t Hourglass::getTimeStep() { return this->timeStep; }

void Hourglass::setNumSteps(uint32_t newNumSteps) {
    if (newNumSteps < 1) {
        newNumSteps = 1;
    }

    this->numSteps = newNumSteps;
    this->totalTime = this->timeStep * this->numSteps;
    this->timeRemaining = this->totalTime - this->elapsedMillisTimer;
}

uint32_t Hourglass::getNumSteps() { return this->numSteps; }

/**
 * @brief return the current time step, [1 ... maxStep]
 *
 * @return uint32this->t
 */
uint32_t Hourglass::getCurrentStep() {
    return this->timeRemaining ? (this->timeRemaining / this->timeStep + 1) : 0;
}

// ------------------- Operator overloads -------------------

Hourglass::operator std::string() const {
    std::string retString =
        "Time Remaining: " + std::to_string(this->timeRemaining) + "ms\n" +
        "\t " + std::to_string(this->timeRemaining / 1000) + "s\n" +
        "Total Time: " + std::to_string(this->totalTime) + "\n" +
        "Time Step: " + std::to_string(this->timeStep) + "\n" +
        "Current Step: " +
        std::to_string((int)std::ceil(this->timeRemaining / this->timeStep)) +
        " / " + std::to_string(this->numSteps) + "\n";

    return retString;
}

Hourglass& Hourglass::operator=(const Hourglass& other) {
    this->numSteps = other.numSteps;
    this->timeStep = other.timeStep;
    this->totalTime = other.totalTime;
    this->timeRemaining = other.timeRemaining;
    this->elapsedMillisTimer = other.elapsedMillisTimer;
    this->paused = other.paused;

    return *this;
}

// Hourglass& Hourglass::operator=(const uint32this->t& val) {
//     this->timeRemaining = val;
//     this->timeRemaining = this->timeRemaining > this->totalTime ?
//     this->totalTime : this->timeRemaining; this->timeRemaining =
//     this->timeRemaining < 0 ? 0 : this->timeRemaining; return *this;
// }

// Hourglass& Hourglass::operator+=(const uint32this->t& val) {
//     this->timeRemaining += val;
//     this->timeRemaining = this->timeRemaining > this->totalTime ?
//     this->totalTime : this->timeRemaining; return *this;
// }

// Hourglass& Hourglass::operator-=(const uint32this->t& val) {
//     this->timeRemaining -= val;
//     this->timeRemaining = this->timeRemaining < 0 ? 0 : this->timeRemaining;
//     return *this;
// }