#ifndef REMOTEINPUT_PYTHONGCQUEUE_HXX
#define REMOTEINPUT_PYTHONGCQUEUE_HXX

#include <unordered_set>

class PyGCQueue
{
public:
    PyGCQueue(EIOS* eios, size_t max_size);
    PyGCQueue(EIOS* eios);
    PyGCQueue(PyGCQueue&&);
    PyGCQueue(const PyGCQueue&) = delete;
    ~PyGCQueue();

    void add(jobject object);
    void flush();

private:
    std::shared_ptr<EIOS> eios;
    size_t max_size;
    std::unordered_set<jobject> objects;

    static const std::size_t DEFAULT_MAX_SIZE = 50;
};

#endif //REMOTEINPUT_PYTHONGCQUEUE_HXX