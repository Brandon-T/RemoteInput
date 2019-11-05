#ifndef COMMANDTHREAD_HXX_INCLUDED
#define COMMANDTHREAD_HXX_INCLUDED

#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <stdexcept>
#include <vector>
#include "../Echo/SharedEvent.hxx"

class ResponsiveThread final
{
private:
    std::mutex mutex;
    std::thread thread;
    std::atomic_bool stop;
    std::function<void()> onStop;

public:
    ResponsiveThread(std::function<void(ResponsiveThread*)> &&task, std::function<void()> &&onStop);
    ~ResponsiveThread();
    ResponsiveThread(const ResponsiveThread&) = delete;
    ResponsiveThread& operator = (const ResponsiveThread&) = delete;

    void terminate();
    bool stopped();
};

#endif // COMMANDTHREAD_HXX_INCLUDED
