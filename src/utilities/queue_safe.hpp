#ifndef QUEUE_SAFE
#define QUEUE_SAFE

#include <condition_variable>
#include <mutex>
#include <queue>

namespace util 
{
    template <typename T>
    class queue_safe 
    {
        public:
        // Pushes an element onto the queue
        void push(T const& val);

        // Pops and returns the front element of the queue
        T pop();

        private:
        std::queue<T> m_queue;                // Underlying queue to store elements
        std::condition_variable m_condition;  // Condition variable for synchronization
        std::mutex m_mutex;                   // Mutex for exclusive access to the queue
    };


    template <typename T>
    inline void queue_safe<T>::push(T const& val) 
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_queue.push(val);
      m_condition.notify_one();  // Notify one waiting thread that data is available
    }

    template <typename T>
    inline T util::queue_safe<T>::pop()
    {
      std::unique_lock<std::mutex> uLock(m_mutex);
      m_condition.wait(uLock,
              [&] { return !m_queue.empty(); });  // Wait until the queue is not empty
      T front = m_queue.front();
      m_queue.pop();
      return front;
    }

        }  // namespace util
#endif