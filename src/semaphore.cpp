/**
 * @author Daniel Mueller
 * 
 * @copyright Copyright 2021 Daniel Mueller. All rights reserved.
 */

#include "semaphore.hpp"

#include <stdexcept>
#include <ctime>


Semaphore::Semaphore(int initialValue)
{
    _semaphore = new sem_t;
    sem_init(_semaphore, 0, initialValue);
}

Semaphore::Semaphore(Semaphore && other)
    : _semaphore{other._semaphore}
{
    other._semaphore = nullptr;
}

Semaphore & Semaphore::operator=(Semaphore && other)
{
    if (&other != this)
    {
        sem_destroy(_semaphore);
        delete _semaphore;

        _semaphore = other._semaphore;
        other._semaphore = nullptr;
    }
    return *this;
}

Semaphore::~Semaphore()
{
    sem_destroy(_semaphore);
    delete _semaphore;
}

void Semaphore::post()
{
    if (_semaphore == nullptr) 
        throw std::runtime_error("Semaphore used after move");

    sem_post(_semaphore);
}

void Semaphore::wait()
{
    if (_semaphore == nullptr) 
        throw std::runtime_error("Semaphore used after move");

    sem_wait(_semaphore);
}

bool Semaphore::tryWait()
{
    if (_semaphore == nullptr) 
        throw std::runtime_error("Semaphore used after move");

    return (sem_trywait(_semaphore) == 0);
}

bool Semaphore::timedWait(long ms)
{
    if (_semaphore == nullptr) 
        throw std::runtime_error("Semaphore used after move");
    
    // Split the milliseconds into full seconds and leftover nanoseconds
    int secs = ms / 1000;
    int nsecs = (ms % 1000) * 1000000L;

    timespec ts;

    // sem_timedwait need the absolute timestamp when to stop waiting, so get 
    // the current timestamp and add the wait time.
    clock_gettime(CLOCK_REALTIME, &ts);

    ts.tv_sec += secs;
    ts.tv_nsec += nsecs;

    return (sem_timedwait(_semaphore, &ts) == 0);
}

int Semaphore::getValue()
{
    if (_semaphore == nullptr) 
        throw std::runtime_error("Semaphore used after move");
    
    int val;
    sem_getvalue(_semaphore, &val);
    return val;
}
