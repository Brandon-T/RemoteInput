#include "DetachedThreadPool.hxx"
#include <windows.h>

DetachedThreadPool::DetachedThreadPool() : DetachedThreadPool(std::thread::hardware_concurrency())
{
}

DetachedThreadPool::DetachedThreadPool(std::size_t max_threads) : mutex(std::make_shared<std::mutex>()), condition(std::make_shared<std::condition_variable>()), tasks(std::make_shared<std::queue<std::function<void()>>>()), threads(), stop(std::make_shared<std::atomic_bool>(false)), max_threads(max_threads)
{
    this->create_threads();
}

DetachedThreadPool::~DetachedThreadPool()
{
	this->terminate();
}

void DetachedThreadPool::create_threads()
{
    if (threads.size() != max_threads)
    {
        threads.reserve(max_threads);

        for (std::size_t i = 0; i < max_threads; ++i)
        {
            threads.emplace_back([mutex = this->mutex, condition = this->condition, tasks = this->tasks, stop = this->stop] {
                while(true)
                {
                    std::unique_lock<std::mutex> lock(*mutex);
                    condition->wait(lock, [&tasks, &stop] {
                        return !tasks->empty() || *stop;
                    });

                    if (*stop) //&& tasks->empty()
                    {
                        lock.unlock();
                        break;
                    }

                    auto task = std::move(tasks->front());
                    tasks->pop();

                    lock.unlock();
                    task();
                }
            });

            this->threads[i].detach();
        }
    }
}

void DetachedThreadPool::terminate()
{
	if (!*stop)
	{
		std::unique_lock<std::mutex> lock(*mutex);
		*stop = true;
		lock.unlock();
		condition->notify_all();
		std::vector<std::thread>().swap(this->threads);
	}
}

void DetachedThreadPool::add_task(std::function<void()> &&task)
{
    if (!this->stop)
    {
        std::unique_lock<std::mutex> lock(*mutex);
        tasks->emplace(task);
        lock.unlock();
        condition->notify_one();
    }
    else
    {
        throw std::runtime_error("DetachedThreadPool is currently shutting down. Cannot enqueue more tasks.");
    }
}
