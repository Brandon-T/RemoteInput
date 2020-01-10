#ifndef DETACHEDTHREADPOOL_HXX_INCLUDED
#define DETACHEDTHREADPOOL_HXX_INCLUDED

#include <thread>
#include <future>
#include <mutex>
#include <atomic>
#include <queue>
#include <functional>
#include <condition_variable>
#include <stdexcept>
#include <vector>

class DetachedThreadPool final
{
private:
    std::mutex mutex;
    std::condition_variable condition;
    std::queue<std::function<void()>> tasks;
    std::vector<std::thread> threads;
    std::atomic_bool stop;
    std::size_t max_threads;
    void create_threads();

public:
    DetachedThreadPool();
	DetachedThreadPool(std::size_t max_threads);
    ~DetachedThreadPool();
    DetachedThreadPool(const DetachedThreadPool&) = delete;
    DetachedThreadPool& operator = (const DetachedThreadPool&) = delete;

	void terminate();

    void add_task(std::function<void()> &&task);

    template<typename Task, typename... Args>
    auto enqueue(Task &&task, Args&&... args) -> std::future<std::invoke_result_t<Task, Args...>>;
};

template<typename Task, typename... Args>
auto DetachedThreadPool::enqueue(Task &&task, Args&&... args) -> std::future<std::invoke_result_t<Task, Args...>>
{
    auto packaged_task = std::make_shared<std::packaged_task<std::invoke_result_t<Task, Args...>(Args&&...)>>(
        std::bind(std::forward<Task>(task), std::forward<Args>(args)...)
    );

    std::future<std::invoke_result_t<Task, Args...>> result = packaged_task->get_future();
    std::unique_lock<std::mutex> lock(this->mutex);
    if (this->stop)
    {
        throw std::runtime_error("DetachedThreadPool is currently shutting down. Cannot enqueue more tasks.");
    }

    this->tasks.emplace([packaged_task] {
        (*packaged_task)();
    });

    condition.notify_one();
    return result;
}

#endif // DETACHEDTHREADPOOL_HXX_INCLUDED
