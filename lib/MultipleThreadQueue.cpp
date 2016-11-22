#include "MultipleThreadQueue.h"


#include <hls.h>
#include <memory>

 
template <typename T>
T Queue<T>::pop(){
  std::unique_lock<std::mutex> mlock(mutex_);
  
  cond_.wait(mlock,[this]()->bool{return !(queue_.empty());});

  auto item = queue_.front();
  queue_.pop();
  return item;
}


template <typename T>
void Queue<T>::pop(T& item){
  std::unique_lock<std::mutex> mlock(mutex_);
  cond_.wait(mlock,[this]()->bool{return !(queue_.empty());});

  item = queue_.front();
  queue_.pop();
}

template <typename T>
void Queue<T>::push(const T& item){
  std::unique_lock<std::mutex> mlock(mutex_);
  queue_.push(item);
  cond_.notify_one();
}

template <typename T>
void Queue<T>::push(T&& item){
  std::unique_lock<std::mutex> mlock(mutex_);
  queue_.push(std::move(item));
  cond_.notify_one();
}

template <typename T>
bool Queue<T>::empty(){
  std::unique_lock<std::mutex> mlock(mutex_);
  return queue_.empty();
}

template class Queue<std::shared_ptr<int> >;
template class Queue<std::shared_ptr<hls_media_segment> >;
template class Queue<hls_media_segment *  >;
