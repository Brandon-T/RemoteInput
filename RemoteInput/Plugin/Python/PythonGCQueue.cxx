#include "PythonGCQueue.hxx"

PyGCQueue::PyGCQueue(EIOS* eios, size_t max_size) : eios(eios), max_size(max_size) {}

PyGCQueue::PyGCQueue(EIOS* eios) : eios(eios), max_size(PyGCQueue::DEFAULT_MAX_SIZE) {}

PyGCQueue::~PyGCQueue()
{
    flush();
}

void PyGCQueue::add(jobject object)
{
    if (object != nullptr)
    {
        objects.insert(object);
        if (objects.size() >= max_size)
        {
            flush();
        }
    }
}

void PyGCQueue::flush()
{
    if (objects.size() > 0) {
        std::vector<jobject> results;
        results.reserve(objects.size());
        for (auto& it : objects)
        {
            results.push_back(std::move(objects.extract(it).value()));
        }
        Reflect_Release_Objects(eios.get(), &results[0], results.size());
        objects.clear();
    }
}