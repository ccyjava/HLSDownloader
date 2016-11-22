#ifndef __Multiple_Thread_Queue__
#define __Multiple_Thread_Queue__

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
 
template <typename T>
class Queue
{
 public:
 
  T pop(); 
  void pop(T& item); 
  void push(const T& item);
  void push(T&& item);
  bool empty();
 
 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cond_;
};


#endif /* defined(__Multiple_Thread_Queue__) */