#include "threadpool.hpp"

Threadpool::Threadpool(int _numberOfWorkers, int _maxQueueBacklog)
    : numberOfWorkers{_numberOfWorkers}, maxQueueBacklog{_maxQueueBacklog}
{

    if (numberOfWorkers < 0)
    {
        throw std::runtime_error("Threadpool number of threads can't be less than 0");
    }

    if (maxQueueBacklog < 0)
    {
        throw std::runtime_error("Threadpool maxQueueBacklog can't be less than 0");
    }

    if (numberOfWorkers == AUTO_NO_WORKERS)
    {
        // Get the hardware concurrency (number of logical cpu cores) and create half
        // as many worker threads. If there is hyperthreading, the number of logical cores 
        // is double the amount of physical cores. So on a modern machine with hyperthreading
        // the number of worker threads will be equivalent to the number of "real" cpu cores
        numberOfWorkers = std::thread::hardware_concurrency() / 2;
    }

    if (maxQueueBacklog != DISABLE_MAX_QUEUE_BACKLOG)
    {
        // Change the initial semaphore value if the max queue backlog is not unlimited
        semTaskQueueLimit = Semaphore(maxQueueBacklog);
    }

    // Create the requested number of worker threads
    for (auto i = 0; i < numberOfWorkers; i++)
    {
        workerThreads.push_back(
            std::thread(&Threadpool::workLoop, this, i)
        );
    }
}

void Threadpool::workLoop(int workerId)
{
    // The task function that is received from the task queue
    std::function<void ()> taskFunction;

    while (true)
    {
        // Wait until a task is available in the task queue
        semWaitForTask.wait();

        // synchronize taskQueue
        {
            std::unique_lock<std::mutex> lock(mtxTaskQueue);

            // Graceful shutdown is initiated by posting the sem without new tasks.
            // This will cause remaining tasks to be processed and then shuts down
            // the threads.
            if (taskQueue.empty())
            {
                return;
            }

            // Get the next task
            taskFunction = taskQueue.front();
            // And pop it from the queue
            taskQueue.pop();

        }

        // If there is a queue backlog limit, notify to addTask that a task has
        // been removed from the queue and therefore one more slot is available
        if (maxQueueBacklog != DISABLE_MAX_QUEUE_BACKLOG)
        {
            semTaskQueueLimit.post();
        }

        // Execute the task blocking
        taskFunction();

    }

}

void Threadpool::addTask(std::function<void ()> task)
{
    if (shutdownInitiated)
    {
        throw std::runtime_error("Can't add task to threadpool after shutdown");
    }
    
    // If there is a queue backlog limit, wait until the queue is no longer full
    if (maxQueueBacklog != DISABLE_MAX_QUEUE_BACKLOG)
    {
        semTaskQueueLimit.wait();
    }

    // synchronize taskQueue
    {
        std::unique_lock<std::mutex> lock(mtxTaskQueue);
        taskQueue.push(task);
    }

    // Notify worker pool that a new task is available
    semWaitForTask.post();
}

void Threadpool::shutdown()
{
    shutdownInitiated = true;

    // Post once for every worker thread. This will cause all tasks to be processed
    // and after that the workers will terminate
    for (int i = 0; i < workerThreads.size(); i++)
        semWaitForTask.post();
}

void Threadpool::joinAll()
{
    for (auto &t : workerThreads)
        t.join();
}