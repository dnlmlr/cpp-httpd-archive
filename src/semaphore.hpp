/**
 * @author Daniel Mueller
 * 
 * @copyright Copyright 2021 Daniel Mueller. All rights reserved.
 */

#ifndef _CPPSEMAPHORE_HPP
#define _CPPSEMAPHORE_HPP

extern "C" {
#include <semaphore.h>
}

/**
 * @brief The Semaphore class provides a simple CPP wrapper for the sem_* 
 * functions from the C "semaphore.h" library. This only uses the thread-shared
 * semaphore, not process shared.
 */
class Semaphore
{
private:
    /**
     * @brief Pointer to the underlying C semaphore.
     */
    sem_t * _semaphore;

public:

    /**
     * @brief Initialize the Semaphore and set the start value.
     * 
     * @param initialValue The Semaphore counter value to start with. 
     *  0 by default.
     * 
     * @see sem_init
     */
    Semaphore(int initialValue = 0);

    /**
     * @brief The semaphore must not be copied, so the copy constructors is deleted.
     * 
     * @param other The reference to copy from.
     */
    Semaphore(const Semaphore & other) = delete;

    /**
     * @brief The semaphore must not be copied, so the copy assignment is deleted.
     * 
     * @param other The reference to copy from.
     */
    Semaphore & operator=(const Semaphore & other) = delete;

    /**
     * @brief Create the instance by taking over an existing Semaphore.
     * 
     * @warning The moved-from instance is no longer valid to use after the move.
     * 
     * @param other The reference to take over.
     */
    Semaphore(Semaphore && other);

    /**
     * @brief Take over an existing Semaphore. The internal samaphore is destroyed 
     * and replace by the internal semaphore of the other instance. 
     * 
     * @warning The moved-from instance is no longer valid to use after the move.
     * 
     * @param other The reference to take over.
     */
    Semaphore & operator=(Semaphore && other);
    
    /**
     * @brief Destructor destroys the underlying semaphore and frees the memory.
     * 
     * @see sem_destroy
     */
    ~Semaphore();

    /**
     * @brief Incremet the Semaphore.
     * 
     * Increments the underlying semaphore using sem_post. If another thread is
     * waiting, it will be woken up.
     * 
     * @see sem_post
     */
    void post();

    /**
     * @brief Decrement the Semaphore or wait if 0.
     * 
     * Decrements the underlying semaphore using sem_wait. If the semaphore has 
     * the value 0, the function blocks until it can decrement (post was 
     * called).
     * 
     * @see sem_wait
     */
    void wait();

    /**
     * @brief Same as wait() but doesn't block if the value is 0.
     * 
     * Tries to decrement the underlying semaphore using 
     * sem_trywait. If the value is 0, it instantly returns false.
     * 
     * @return True if the semaphore was decremented, false otherwise.
     * 
     * @see sem_trywait
     */
    bool tryWait();

    /**
     * @brief Same as wait() but only blocks for a specified ammount of time.
     * 
     * Tries to decrement the underlying semaphore using sem_timedwait. If the 
     * value is 0, it will block until it can decrement or the specified time 
     * in ms has passed. 
     * 
     * @param ms The time in milliseconds that will be waited if the current 
     *  value is 0, before returning false.
     * 
     * @return True if the value could be decremented, false if the ms time has
     *  passed without the possibility to decrement. 
     * 
     * @see sem_timedwait
     */
    bool timedWait(long ms);

    /**
     * 
     * @brief Get the current Semaphore value.
     * 
     * Get the current value from the underlying semaphore using sem_getvalue.
     * 
     * @return The current semaphore value. (Due to the nature of semaphores 
     *  and threads, the real value might have already changed when parsing this 
     *  result)
     * 
     * @see sem_getvalue
     */
    int getValue();

};

#endif // _SEMAPHORE_HPP
