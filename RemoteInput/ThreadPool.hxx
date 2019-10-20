#ifndef THREADPOOL_HXX_INCLUDED
#define THREADPOOL_HXX_INCLUDED

#include <thread>
#include <future>
#include <mutex>
#include <atomic>
#include <queue>
#include <functional>
#include <condition_variable>
#include <stdexcept>
#include <vector>

class ThreadPool final
{
private:
    std::mutex mutex;
    std::condition_variable condition;
    std::queue<std::function<void()>> tasks;
    std::vector<std::thread> threads;
    std::atomic_bool stop;

public:
    ThreadPool();
    ~ThreadPool();
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator = (const ThreadPool&) = delete;

    void enqueue(std::function<void()> &&task);

    template<typename Task, typename... Args>
    auto enqueue(Task &&task, Args&&... args) -> std::future<typename std::result_of<Task(Args...)>::type>;
};

template<typename Task, typename... Args>
auto ThreadPool::enqueue(Task &&task, Args&&... args) -> std::future<typename std::result_of<Task(Args...)>::type>
{
    auto packaged_task = std::make_shared<std::packaged_task<typename std::result_of<Task(Args...)>::type>>(
        std::bind(std::forward<Task>(task), std::forward<Args>(args)...)
    );

    std::future<typename std::result_of<Task(Args...)>::type> result = packaged_task->get_future();
    std::unique_lock<std::mutex> lock(this->mutex);
    if (this->stop)
    {
        throw std::runtime_error("ThreadPool is currently shutting down. Cannot enqueue more tasks.");
    }

    this->tasks.emplace([packaged_task] {
        (*packaged_task)();
    });

    condition.notify_one();
    return result;
}

#endif // THREADPOOL_HXX_INCLUDED
