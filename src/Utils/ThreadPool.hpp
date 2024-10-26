#pragma once

#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace Dynamo {
    /**
     * @brief A pool of threads to assign jobs that run concurrently.
     *
     */
    class ThreadPool {
        std::vector<std::thread> _threads;
        std::queue<std::function<void()>> _jobs;
        unsigned _job_count = 0;

        mutable std::mutex _mutex;
        std::condition_variable _conditional_start;
        std::condition_variable _conditional_finish;

        bool _terminate = false;
        bool _waiting = false;

        /**
         * @brief Main thread loop that waits for new jobs to execute.
         *
         */
        void thread_main() {
            while (!_terminate) {
                // Wait until a job becomes available
                std::unique_lock<std::mutex> lock(_mutex);
                _conditional_start.wait(lock, [this]() { return !_jobs.empty() || _terminate; });
                if (!_terminate) {
                    // Get the next job
                    std::function<void()> job = std::move(_jobs.front());
                    _jobs.pop();

                    // Run the job out of the lock context
                    lock.unlock();
                    job();
                    lock.lock();

                    // Update job count and signal the waiter
                    _job_count--;
                    if (_waiting) {
                        _conditional_finish.notify_one();
                    }
                }
            }
        }

      public:
        /**
         * @brief Construct a new ThreadPool object.
         *
         * @param pool_size Number of threads in the pool.
         */
        ThreadPool(unsigned pool_size) {
            _threads.resize(pool_size);
            for (unsigned i = 0; i < pool_size; i++) {
                _threads[i] = std::thread([this]() { thread_main(); });
            }
        }

        /**
         * @brief Construct a new ThreadPool object.
         *
         */
        ThreadPool() : ThreadPool(std::thread::hardware_concurrency()) {}

        /**
         * @brief Destroy the ThreadPool object.
         *
         */
        ~ThreadPool() {
            {
                std::scoped_lock<std::mutex> lock(_mutex);
                _terminate = true;
                _conditional_start.notify_all();
            }
            for (std::thread &thread : _threads) {
                thread.join();
            }
        }

        /**
         * @brief Submit a concurrent job, returning a future to its result.
         *
         * @tparam F Function type.
         * @tparam A Zero or more argument types.
         * @tparam R Return type.
         * @param callable Callable function.
         * @param args     Arguments to the function.
         * @return std::future<R>
         */
        template <typename F, typename... A, typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<A>...>>
        std::future<R> submit(F &&callable, A &&...args) {
            auto promise = std::make_shared<std::promise<R>>();
            std::function<void()> job = [promise, callable, args...]() {
                try {
                    if constexpr (std::is_void<R>::value) {
                        callable(args...);
                        promise->set_value();
                    } else {
                        promise->set_value(callable(args...));
                    }
                } catch (...) {
                    promise->set_exception(std::current_exception());
                }
            };

            {
                std::unique_lock<std::mutex> lock(_mutex);
                _jobs.push(job);
                _job_count++;
                _conditional_start.notify_one();
            }
            return promise->get_future();
        }

        /**
         * @brief Wait for all enqueued jobs to finish executing.
         *
         */
        void wait_all() {
            _waiting = true;
            std::unique_lock<std::mutex> lock(_mutex);
            _conditional_finish.wait(lock, [this]() { return !_job_count; });
            _waiting = false;
        }
    };
} // namespace Dynamo