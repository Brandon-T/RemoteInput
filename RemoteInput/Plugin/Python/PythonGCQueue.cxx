#include "PyGCQueue.hxx"

PyGCQueue::PyGCQueue(EIOS* eios, size_t max_size) : eios(eios), max_size(max_size) {};

PyGCQueue::PyGCQueue(EIOS* eios) : eios(eios), max_size(PyGCQueue::DEFAULT_MAX_SIZE) {};

PyGCQueue::PyGCQueue(PyGCQueue&& other): eios(other.eios), max_size(other.max_size), objects(std::move(objects))
{
    other.eios = nullptr;
    other.flush();
}

PyGCQueue& PyGCQueue::operator = (PyGCQueue&& other)
{
    eios = other.eios;
    max_size = other.max_size;
    objects = std::move(other.objects);

    other.eios = nullptr;
    other.flush();
    return *this;
}

PyGCQueue::~PyGCQueue()
{
    flush();
}

PyGCQueue::add(jobject object)
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

PyGCQueue::flush()
{
    if (objects.size() > 0) {
        std::vector<jobject> results;
        results.reserve(objects.size());
        for (auto& it : objects)
        {
            results.push_back(std::move(objects.extract(it).value()));
        }
        Reflect_Release_Objects(eios, &result[0], result.size());
        objects.clear();
    }
}