
//
//  TaskQueueTest.cpp
//  testtaskqueue
//
//  Created by Luke Hollingworth on 04/04/2019.
//  Copyright © 2019 midas. All rights reserved.
//

#include <algorithm>
#include <atomic>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <thread>
#include "TaskQueueTest.h"

unsigned int TaskQueueTest::skipped_job_count = 0;
std::mutex TaskQueueTest::skipped_job_count_mtx;

TaskQueueTest::~TaskQueueTest() {
    terminateThreads();
}

void TaskQueueTest::terminateThreads() {
    if(test_running_) {
        for (auto writer : producers_) {
            writer->join();
        }
        test_running_ = false;
    }
}

bool TaskQueueTest::runTest(const bool use_pop_try,
             const unsigned int queue_length,
             const unsigned int num_producers,
             const unsigned int num_jobs_per_producer,
             const std::chrono::milliseconds sleep_time_producer,
             const std::chrono::milliseconds sleep_time_consumer) {
    bool success = false;
    if(!test_running_) {
        test_running_ = true;
        skipped_job_count = 0;
        std::shared_ptr< TaskQueue<Job> > queue = std::make_shared< TaskQueue<Job> >(queue_length);
        std::vector< std::shared_ptr< std::vector< std::shared_ptr<Job> > > > lost_jobs_rec;
        startProducerThreads(num_producers, num_jobs_per_producer, sleep_time_producer, queue, lost_jobs_rec);
        unsigned int consumed_job_count = consume(use_pop_try, sleep_time_consumer, num_jobs_per_producer * num_producers, queue);
        int lost_job_count = 0;
        for(auto jobs : lost_jobs_rec) {
            lost_job_count += jobs->size();
        }
        if(lost_job_count > 0) {
            std::cout << lost_job_count << " jobs were lost because the queue was full, they are;\n";
            for(auto jobs : lost_jobs_rec) {
                for(auto job : *jobs.get()) {
                    std::cout << "Skipped job: " << job->message << "\n";
                }
            }
            std::cout << std::endl;
        }
        unsigned int num_jobs_exp = num_producers * num_jobs_per_producer;
        unsigned int num_jobs_done = lost_job_count + consumed_job_count;
        std::cout << "Jobs consumed " << consumed_job_count << std::endl;
        std::cout << "Jobs skipped " << lost_job_count << std::endl;
        std::cout << "Total jobs processed and skipped " << num_jobs_done << std::endl;
        std::cout << "Expected jobs processed and skipped " << num_jobs_exp << std::endl;
        if (num_jobs_exp == num_jobs_done) {
            std::cout << "TEST_PASSED" << std::endl << std::endl;
        } else {
            std::cout << "TEST_FAILED" << std::endl << std::endl;
        }
        finish_test_ = true;
        success = true;
    }
    return success;
}

unsigned int TaskQueueTest::consume(const bool use_pop_try, const std::chrono::milliseconds sleep_time_between_pops, const int num_jobs, std::shared_ptr< TaskQueue<Job> > queue) {
    unsigned int consumeCount = 0;
    while(1) {
        std::shared_ptr<Job> job;
        bool success = false;
        if(use_pop_try) {
            std::tie(success, job) = queue->pop_try();
            if (success) {
                std::cout << "Popped job: " << job->message << std::endl;
                std::cout << "No job ready - poll again in " << sleep_time_between_pops.count() << " ms" << std::endl;
                if(sleep_time_between_pops.count()) {
                    std::this_thread::sleep_for(sleep_time_between_pops);
                }
            }
        } else {
            job = queue->pop();
            std::cout << "Popped job: " << job->message << std::endl;
            std::this_thread::sleep_for(sleep_time_between_pops);
            if(sleep_time_between_pops.count()) {
                std::this_thread::sleep_for(sleep_time_between_pops);
            }
            success = true;
        }
        if(success) {
            int skipped = getSkippedJobs();
            if(++consumeCount + skipped >= num_jobs) {
                std::cout << "TEST FINISHED" << std::endl;
                break;
            }
        }
    }
    return consumeCount;
}

void TaskQueueTest::startProducerThreads(
                          const unsigned int num_producers,
                          const unsigned int num_jobs,
                          const std::chrono::milliseconds sleep_time_ms,
                          std::shared_ptr< TaskQueue<Job> > queue,
                          std::vector< std::shared_ptr< std::vector< std::shared_ptr<Job> > > > &lost_jobs_rec) {
    
    lost_jobs_rec.resize(num_producers);
    for(auto &lost_job_vec : lost_jobs_rec) {
        lost_job_vec = std::make_shared<std::vector<std::shared_ptr<Job> > >();
    }
    std::vector< std::shared_ptr<std::thread> > producers;
    for (unsigned int i = 0; i < num_producers; i++) {
        std::shared_ptr< std::vector< std::shared_ptr<Job> > > rec = lost_jobs_rec.at(i);
        std::shared_ptr<std::thread> producer =
        std::make_shared<std::thread>(producerThread, i, num_jobs, sleep_time_ms, queue, rec);
        producers_.push_back(producer);
    }
}

void TaskQueueTest::producerThread(const int thread_num, const int num_jobs, const std::chrono::milliseconds sleep_time, std::shared_ptr< TaskQueue<Job> > task_queue,
                           std::shared_ptr< std::vector< std::shared_ptr<Job> > > lost_jobs_rec) {
    
    for(unsigned int i = 0; i < num_jobs; i++) {
        if (sleep_time != std::chrono::milliseconds(0)) {
            std::this_thread::sleep_for(sleep_time);
        }
        std::stringstream ss;
        ss << "Thread:" << thread_num << " job:" << i;
        std::shared_ptr<Job> job = std::make_shared<Job>(ss.str(), thread_num, i);
        job->message = ss.str();
        if(!task_queue->push(job)) {
            lost_jobs_rec->push_back(job);
            incrementSkippedJobs();
        }
    }
}

void TaskQueueTest::incrementSkippedJobs() {
    std::unique_lock<std::mutex> lock(skipped_job_count_mtx);
    skipped_job_count++;
}

int TaskQueueTest::getSkippedJobs() {
    std::unique_lock<std::mutex> lock(skipped_job_count_mtx);
    int ret_val = skipped_job_count;
    return ret_val;
}
