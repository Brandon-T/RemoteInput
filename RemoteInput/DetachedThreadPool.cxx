#include "DetachedThreadPool.hxx"

DetachedThreadPool::DetachedThreadPool() : DetachedThreadPool(std::thread::hardware_concurrency())
{
}

DetachedThreadPool::DetachedThreadPool(std::size_t max_threads) : mutex(), condition(), tasks(), threads(), stop(false), max_threads(max_threads)
{
    this->create_threads();
}

DetachedThreadPool::~DetachedThreadPool()
{
	this->terminate();
}

void DetachedThreadPool::create_threads()
{
    if (this->threads.size() != this->max_threads)
    {
        this->threads.reserve(this->max_threads);

        for (std::size_t i = 0; i < this->max_threads; ++i)
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

            this->threads[i].detach();
        }
    }
}

void DetachedThreadPool::terminate()
{
	if (!this->stop)
	{
		std::unique_lock<std::mutex> lock(this->mutex);
		this->stop = true;
		lock.unlock();
		//this->condition.notify_all();  //Causes a crash on Windows.. Unless using VS2015+
		std::vector<std::thread>().swap(this->threads);
	}
}

void DetachedThreadPool::add_task(std::function<void()> &&task)
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
        throw std::runtime_error("DetachedThreadPool is currently shutting down. Cannot enqueue more tasks.");
    }
}
