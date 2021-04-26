#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>

#include "semaphore.hpp"

/**
 * @brief A collection of worker threads that can work on tasks in a task queue.
 * This allows for using threads to parallelize tasks while only having a fixed 
 * number of reusable threads. 
 * 
 * This highly reduces overhead that would come with creating new threads and can 
 * also prevent the "too many threads" problem. There is no point in having more 
 * actively working threads than cpu cores since the cpu can't parallelize the 
 * work further.
 */
class Threadpool
{
private:

    /**
     * @brief Indicated if a shutdown was initiated and the threadpool can no longer
     * accept new tasks.
     */
    bool shutdownInitiated = false;

    /**
     * @brief The number of concurrent workers in the threadpool
     */
    int numberOfWorkers;

    /**
     * @brief The list of thread handles for the concurrent workers
     */
    std::vector<std::thread> workerThreads;

    /**
     * @brief A task queue that contains functions that will be executed by the 
     * workers threads in the same order as they were added (fifo).
     * 
     * NOTE: The access is not synchronized by default and the mtxTaskQueue mutex
     * must be used to access this variable.
     */
    std::queue <
        std::function<void ()>
    > taskQueue;

    /**
     * @brief Mutex to synchronize access to the taskQueue variable.
     */
    std::mutex mtxTaskQueue;

    /**
     * @brief This semaphore is used to notify the workers that there are tasks available.
     * The value represents te number of open tasks in the taskQueue. Workers will
     * therefore wait if there are no tasks and continue if tasks get added to the 
     * taskQueue. 
     * 
     * The semaphore has is posted when tasks are added to the taskQueue.
     */
    Semaphore semWaitForTask;

    /**
     * @brief The maximum number of open tasks in the taskQueue. If this number 
     * is reached, the addTask() method will block.
     */
    int maxQueueBacklog;

    /**
     * @brief This semaphore is used to limit the number of open tasks on the taskQueue.
     * 
     * It is initialized with the max number of open tasks (maxQueueBacklog) and 
     * the addTask function waits on the semaphore while the workers post it 
     * after removing a task from the taskQueue.
     */
    Semaphore semTaskQueueLimit;

    /**
     * @brief Internal worker function that waits for and then completes tasks from 
     * the taskQueue. This is the function that is executed by the worker threads.
     * 
     * @param workerId The id of the current worker
     */
    void workLoop(int workerId);
    
public:

    /**
     * @brief Automatically determine the number of threads to use in the threadpool.
     * This will use (number of logical cores / 2) as the number of threads.
     */
    static const int AUTO_NO_WORKERS = 0;

    /**
     * @brief Allow an unlimited queue size and never block when adding tasks.
     */
    static const int DISABLE_MAX_QUEUE_BACKLOG = 0;

    /**
     * @brief Create a new Threadpool with the specified number of worker threads 
     * and maximum queue backlog.
     * 
     * The number of worker threads should not be higher than the number of physical 
     * cores. In certain unlikely circumstances it might exist a small performance 
     * increase by setting it to the number of logical cores (cpu "threads" with 
     * hyperthreading), but this will not be true in almost all cases.
     * 
     * @param numberOfWorkers The number of worker threads to use for the threadpool.
     * 
     * @param maxQueueBacklog The number of open tasks in the task queue before the 
     * addTask method will block.
     */
    Threadpool(int numberOfWorkers = AUTO_NO_WORKERS, int maxQueueBacklog = DISABLE_MAX_QUEUE_BACKLOG);

    /**
     * @brief Add a task to the task queue to be processed by the threadpool.
     * If the number of open tasks in the threadpool is equal to the value of
     * maxQueueBacklog this function will block until tasks are removed from the 
     * queue by the worker threads.
     * 
     * @param task The task function to be executed by the threadpool.
     * 
     * @warning This function will block if the number of open tasks in the 
     * threadpool is equal to the value of maxQueueBacklog this function will 
     * block until tasks are removed from the queue by the worker threads.
     */
    void addTask(std::function<void ()> task);

    /**
     * @brief Block until all of the worker threads have ended.
     * Note that the worker threads will not end on their own unless shutdown 
     * is called.
     */
    void joinAll();

    /**
     * @brief Initiate a shutdown of the threadpool. The worker threads will continue 
     * to finish all open tasks and then terminate.
     */
    void shutdown();

};


#endif // _THREADPOOL_H