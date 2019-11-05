#include "CommandThread.hxx"

ResponsiveThread::ResponsiveThread(std::function<void(ResponsiveThread*)> &&task, std::function<void()> &&onStop) : mutex(), thread(), stop(false), onStop(onStop)
{
    this->thread = std::thread([this, task] {
        task(this);
    });

    this->thread.detach();
}

ResponsiveThread::~ResponsiveThread()
{
    if (!this->stop)
    {
        this->terminate();
    }
}

void ResponsiveThread::terminate()
{
    this->stop = true;
    onStop();
}

bool ResponsiveThread::stopped()
{
    return this->stop;
}
