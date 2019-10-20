#include "ThreadPool.hxx"

ThreadPool::ThreadPool() : mutex(), condition(), tasks(), threads(), stop(false)
{
    this->threads.reserve(std::thread::hardware_concurrency());

    for (std::size_t i = 0; i < std::thread::hardware_concurrency(); ++i)
    {
        this->threads.emplace_back([this] {
            while(true)
            {
                std::unique_lock<std::mutex> lock(this->mutex);
                condition.wait(lock, [this] {
                    return !this->tasks.empty() || this->stop;
                });

                if (this->stop) //&& this->tasks.empty()
                {
                    lock.unlock();
                    break;
                }

                auto task = std::move(this->tasks.front());
                this->tasks.pop();

                lock.unlock();
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    std::unique_lock<std::mutex> lock(this->mutex);
    this->stop = true;
    lock.unlock();
    this->condition.notify_all();

    for (auto&& thread : this->threads)
    {
        thread.join();
    }

    std::vector<std::thread>().swap(this->threads);
}

void ThreadPool::enqueue(std::function<void()> &&task)
{
    if (!this->stop)
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        this->tasks.emplace(task);
        lock.unlock();
        this->condition.notify_one();
    }
    else
    {
        throw std::runtime_error("ThreadPool is currently shutting down. Cannot enqueue more tasks.");
    }
}
