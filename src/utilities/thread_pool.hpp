#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <vector>
#include <thread>
#include <functional>
#include <future>
#include <algorithm>
#include <atomic>
#include <iostream>
#include "queue_safe.hpp"

namespace util 
{
    constexpr unsigned int DEFAULT_NUM_OF_THREADS = 2;

    class thread_pool 
    {
    public:
        explicit thread_pool(unsigned int requested_threads = DEFAULT_NUM_OF_THREADS);

        ~thread_pool();

        unsigned int size() const;

        template <class F, class... Args>
        auto ExecuteTask(F &&f, Args &&...args)
            -> std::future<std::invoke_result_t<F, Args...>>;

    private:
        // Workers
        std::vector<std::thread> m_workers;

        // Task queue
        queue_safe<std::function<void()>> m_tasks;

        // Synchronization
        std::atomic<bool> m_stop;
    };


    // Constructor
    inline thread_pool::thread_pool(unsigned int requested_threads) : m_stop(false) 
    {
        unsigned int max_threads = std::thread::hardware_concurrency();

        if (max_threads == 0) {
            max_threads = DEFAULT_NUM_OF_THREADS;
        }

        if (requested_threads <= 0) {
            requested_threads = DEFAULT_NUM_OF_THREADS;
        }

        const unsigned int pool_size = std::min(requested_threads, max_threads);

        for (unsigned int i = 0; i < pool_size; ++i) {
            m_workers.emplace_back([this, &i]() {
                while (true) {
                    if (m_stop) break;
                    auto task = m_tasks.pop();
                    if (m_stop) break;
                    
                    try {
                        task();
                    } catch (const std::exception &e) {
                        std::cerr << "[ThreadPool] Task threw exception: " 
                                  << e.what() << "\n";
                    } catch (...) {
                        std::cerr << "[ThreadPool] Task threw unknown exception\n";
                    }
                }
            });
        }
    }


    // Destructor
    inline thread_pool::~thread_pool() 
    {
        m_stop = true;

        // Push dummy tasks to unblock workers waiting on pop()
        for (size_t i = 0; i < m_workers.size(); ++i) {
            m_tasks.push([] {}); 
        }

        for (auto &worker : m_workers) {
            if (worker.joinable())
                worker.join();
        }
    }

    inline unsigned int thread_pool::size() const
    {
        return m_workers.size();
    }

    // ExecuteTask: wraps arbitrary callable + args into packaged_task
    template <class F, class... Args>
    inline auto thread_pool::ExecuteTask(F &&f, Args &&...args)
        -> std::future<std::invoke_result_t<F, Args...>> 
    {
        using return_type = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();

        if (m_stop) {
            // Return invalid future instead of throwing
            std::cerr << "[ThreadPool] Ignoring ExecuteTask after m_stop\n";
            return std::future<return_type>();
        }

        m_tasks.push([task]() { 
            try {
                (*task)();
            } catch (const std::exception &e) {
                std::cerr << "[ThreadPool] ExecuteTask exception: " 
                          << e.what() << "\n";
            } catch (...) {
                std::cerr << "[ThreadPool] ExecuteTask unknown exception\n";
            }
        });

        return res;
    }

}  // namespace util

#endif
