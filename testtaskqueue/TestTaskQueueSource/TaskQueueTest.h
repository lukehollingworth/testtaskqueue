
//
//  TaskQueueTest.h
//  testtaskqueue
//
//  Created by Luke Hollingworth on 04/04/2019.
//  Copyright © 2019 midas. All rights reserved.
//

#ifndef __TASKQUEUETEST_
#define __TASKQUEUETEST_

#include <thread>
#include <vector>

#include "TaskQueue.h"

struct Job {
    Job(std::string message, unsigned int thread_num, unsigned int job_num) {
        this->message = message;
        this->thread_num = thread_num;
        this->job_num = job_num;
    }
    std::string message;
    unsigned int thread_num;
    unsigned int job_num;
};

class TaskQueueTest {
public:
    TaskQueueTest() = default;
    
    ~TaskQueueTest();
    
    // Deleted copy constructor
    TaskQueueTest(const TaskQueueTest&) = delete;

    // Deleted move constructor
    TaskQueueTest(TaskQueueTest&&) = delete;

    // Deleted assignment operator
    TaskQueueTest& operator=(const TaskQueueTest&) = delete;

    // Deleted move operator
    TaskQueueTest& operator=(TaskQueueTest&&) = default;


    bool runTest(const bool use_pop_try,
                 const unsigned int queue_length,
                 const unsigned int num_producers,
                 const unsigned int num_jobs_per_producer,
                 const std::chrono::milliseconds sleep_time_producer,
                 const std::chrono::milliseconds sleep_time_consumer);
    
private:
    void terminateThreads();
    
    unsigned int consume(const bool use_pop_try, 
        const std::chrono::milliseconds sleep_time_between_pops,
        const unsigned int num_jobs,
        std::shared_ptr< TaskQueue<Job> > queue);

    void startProducerThreads(const unsigned int num_producers,
                              const unsigned int num_jobs,
                              const std::chrono::milliseconds sleep_time_ms,
                              std::shared_ptr< TaskQueue<Job> > queue);

    static void producerThread(const unsigned int thread_num,
                              const unsigned int num_jobs,
                              const std::chrono::milliseconds sleep_time,
                              std::shared_ptr< TaskQueue<Job> >
                              task_queue);

    static int getSkippedJobCount();

    static void addSkippedJob(std::shared_ptr<Job> job);

    static unsigned int skipped_job_count_;   //!< Static so can only use one instance at a time, quick hack
    static std::mutex skipped_job_mtx_;
    static std::vector<std::shared_ptr<Job> > skipped_jobs_;

    std::unique_ptr< TaskQueue<Job> > queue_;
    bool test_running_ = false;
    bool finish_test_ = false;
    std::vector< std::shared_ptr<std::thread> > producers_;
};

#endif
