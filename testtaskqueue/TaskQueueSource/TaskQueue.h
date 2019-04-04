
//
//  MainWindow.h
//  testtaskqueue
//
//  Created by Luke Hollingworth on 03/04/2019.
//  Copyright © 2019 midas. All rights reserved.
//

#ifndef __TASK_QUEUE__
#define __TASK_QUEUE__

#include <iostream>
#include <mutex>
#include <vector>


constexpr unsigned int default_queue_size = 1000; //!< Default queue size

/** 
 * @brief A FIFO task queue can be used to pass work items from multiple producer
 * threads to a single consumer thread. The queue is of a fixed size specified during
 * construction.
 */
template<class T>
class TaskQueue {

public:
  /**
   * @brief Constructor
   *
   * @param queue_size The maximum number of tasks to queue
   */
  TaskQueue(const size_t queue_size = default_queue_size);

  /**
   * @brief Push a task onto the queue.
   * Can be called by any number of producer threads.
   *
   * @param task A shared pointer of the task to queue.
   * @return True if successful, false if not. Fails if the queue is full.
   */
  bool push(const std::shared_ptr<T> task);
  
  /**
   * @brief Pop a task from the queue.
   * Should only be called by a single consumer thread and not in conjunction with pop_try.
   *
   * @return A shared pointer of the popped task.
   */
  std::shared_ptr<T> pop();
  
  /**
   * @brief Try to pop a task from the queue.
   * Should only be called by a single consumer thread and not in conjunction with pop.
   *
   * @return A tuple of success flag; true if successful, false if not,
   * and a shared pointer of the popped task
   */
  std::tuple<bool, std::shared_ptr<T> > pop_try();
  
private:
  /**
   * @brief Implementation for to pop a task from the queue.
   * Not thread safe.
   * 
   * @return A shared pointer of the popped task
   */
  std::shared_ptr<T> popImp(); 

  std::mutex queue_mutex_;  //!< Mutex to lock write access to the queue
  std::condition_variable cv_;  //!< Condition variable used to sleep the consumer thread
  typename std::vector< std::shared_ptr<T> > queue_;    //!< The task queue
  typename std::vector< std::shared_ptr<T> >::iterator write_it_;   //!< Iterator for writing to the queue
  typename std::vector< std::shared_ptr<T> >::iterator read_it_;    //!< Iterator for reading from the queue
  size_t queue_cnt_ = 0;  //!< Number of tasks in the queue

};

template<class T>
TaskQueue<T>::TaskQueue(const size_t queue_size) {
  queue_.resize(queue_size, nullptr);
  write_it_ = queue_.begin();
  read_it_ = queue_.begin();
}

template<class T>
bool TaskQueue<T>::push(const std::shared_ptr<T> job) {
  bool success = false;
  std::unique_lock<std::mutex> lk(queue_mutex_);
  const size_t queue_size = queue_.size();
  if (queue_cnt_ < queue_size) {
    *write_it_ = job;
    ++write_it_;
    if (write_it_ == queue_.end()) {
      write_it_ = queue_.begin();
    }
    queue_cnt_++;
    cv_.notify_one();
    success = true;
  }
  return success;
}


template<class T>
std::shared_ptr<T> TaskQueue<T>::pop() {
  if (queue_cnt_ == 0) {
    // sleep until at least one item is ready to be processed
    std::unique_lock<std::mutex> lck(queue_mutex_);
    auto check_func = [this] {return this->queue_cnt_ != 0; };
    cv_.wait(lck, check_func);
  }
  return popImp();
}

template<class T>
std::tuple<bool, std::shared_ptr<T> > TaskQueue<T>::pop_try() {

  std::shared_ptr<T> ret_val = nullptr;
  bool success = false;
  if (queue_cnt_ > 0) {
    ret_val = popImp();
    success = true;
  }
  return std::make_tuple(success, ret_val);
}

template<class T>
std::shared_ptr<T> TaskQueue<T>::popImp() {
  std::shared_ptr<T> ret_val = *read_it_++;
  if (read_it_ == queue_.end()) {
    read_it_ = queue_.begin();
  }
  queue_cnt_--;
  return ret_val;
}

#endif
