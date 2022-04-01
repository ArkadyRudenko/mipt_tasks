#include <vector>
#include <iostream>

using namespace std;

template<typename T>
class Deque {
public:
    Deque();
    explicit Deque(int capacity);
    Deque(const Deque &deq);
    Deque(int size, T item);
    ~Deque();

    size_t size() const;
    T &at(size_t index);

    void push_back(T item);
    void push_front(T item);
    void pop_back();
    void pop_front();

    Deque &operator=(const Deque &deq);
    T &operator[](size_t index);
    const T &operator[](size_t index) const;

private:
    static const size_t chunk_size = 1000; // 21 across the line
    size_t sz = 0;
    size_t size_main_arr = 0;
    int capacity = 0;
    T **chunks = nullptr;

    size_t offset = 0;

    void relocation_begin();
    void relocation_end();

private:
    template<class U>
    class deque_iterator {
    private:
        Deque<U> &parent;
        int chunk = 0;
        size_t index = 0;
    public:
        int getChunk() const {
            return chunk;
        }

        size_t getIndex() const {
            return index;
        }
        deque_iterator(Deque<U> &parent,
                       int chunk,
                       size_t index)
                : parent(parent),
                  chunk(chunk),
                  index(index) {}

        deque_iterator(const deque_iterator &it)
                : parent(it.parent),
                  chunk(it.chunk),
                  index(it.index)
                   {}

        deque_iterator<U> &operator=(const deque_iterator &it) {
            parent = it.parent;
            chunk = it.chunk;
            index = it.index;
            return *this;
        }

        U &operator*() {
            return parent.chunks[parent.offset + chunk][index];
        }

        deque_iterator &operator++() {
            if (index == chunk_size - 1) {
                index = 0;
                chunk++;
            } else {
                index++;
            }
            return *this;
        }

        deque_iterator &operator--() {
            if (index == 0) {
                index = chunk_size - 1;
                chunk--;
            } else {
                index--;
            }
            return *this;
        }

        deque_iterator &operator++(int) {
            ++*this;
            return *this;
        }

        deque_iterator &operator--(int) {
            --*this;
            return *this;
        }

        U *operator->() {
            return &parent.chunks[parent.offset + chunk][index];
        }

        bool operator<(const deque_iterator &it) {
            if (chunk != it.chunk) {
                return chunk < it.chunk;
            } else {
                return index < it.index;
            }
        }

        bool operator<=(const deque_iterator &it) {
            if (chunk != it.chunk) {
                return chunk <= it.chunk;
            } else {
                return index <= it.index;
            }
        }

        bool operator>(const deque_iterator &it) {
            if (chunk != it.chunk) {
                return chunk > it.chunk;
            } else {
                return index > it.index;
            }
        }

        bool operator>=(const deque_iterator &it) {
            if (chunk != it.chunk) {
                return chunk >= it.chunk;
            } else {
                return index >= it.index;
            }
        }

        bool operator!=(const deque_iterator &it) {
            if (chunk != it.chunk) {
                return true;
            } else {
                return index != it.index;
            }
        }

        bool operator==(const deque_iterator &it) {
            return chunk == it.chunk and index == it.index;
        }

        deque_iterator operator+(int n) {
            deque_iterator copy(parent,
                                chunk + (index + n) / chunk_size,
                                (index + n) % chunk_size);
            return copy;
        }

        deque_iterator operator-(const deque_iterator &it) {
            deque_iterator res(this->parent,
                               chunk - it.chunk,
                               index - it.index);
            return res;
        }
    };

public:
    using iterator = deque_iterator<T>;
    using const_iterator = deque_iterator<const T>;
private:
    iterator begin_;
    iterator end_;
public:
    deque_iterator<T> begin() const {
        return begin_;
    }

    deque_iterator<T> end() const {
        return end_;
    }

    const_iterator cbegin() const {
        return const_iterator(begin_);
    }

    const_iterator cend() const {
        return const_iterator(end_);
    }

    deque_iterator<T> insert(deque_iterator<T> pos, T item) {
        if (end_.getChunk() == size_main_arr and !end_.getIndex()) {
            relocation_end();
        }
        auto it = end_++;
        while (it != pos) {
            auto copy = it;
            *it = *(--copy);
            it--;
        }
        *pos = item;
        sz++;
        return pos;
    }

    deque_iterator<T> erase(deque_iterator<T> pos) {
        if (pos == end_) { return pos; }
        auto copy = pos;
        while (pos != begin_) {
            auto tmp = pos;
            *pos = *(--tmp);
            pos--;
        }
        begin_++;
        sz--;
        return ++copy;
    }
};

#include "Deque.cpp"