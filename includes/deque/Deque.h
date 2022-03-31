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
    static const size_t chunk_size = 1000; // 2 across the line
    size_t sz = 0;
    size_t size_main_arr = 0;
    int capacity = 0;
    T **chunks = nullptr;

    void relocation_begin();

    void relocation_end();

private:
    template<class U>
    class deque_iterator {
    private:
        size_t chunk = 0;
        size_t index = 0;
        U **chunks = nullptr;
    public:
        size_t getChunk() const {
            return chunk;
        }

        void setChunks(U **chunks) {
            deque_iterator::chunks = chunks;
        }

        void setChunk(size_t chunk) {
            deque_iterator::chunk = chunk;
        }

        size_t getIndex() const {
            return index;
        }

        void setIndex(size_t index) {
            deque_iterator::index = index;
        }

        deque_iterator(U **chunks,
                       size_t chunk,
                       size_t index)
                : chunks(chunks),
                  chunk(chunk),
                  index(index) {}

        deque_iterator(const deque_iterator &it)
                : chunks(it.chunks),
                  index(it.index),
                  chunk(it.chunk) {}

        U &operator*() {
            return chunks[chunk][index];
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
            return &chunks[chunk][index];
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
            deque_iterator copy(chunks,
                                chunk + (index + n) / chunk_size,
                                (index + n) % chunk_size);
            return copy;
        }

        deque_iterator operator-(const deque_iterator &it) {
            deque_iterator res(this->chunks,
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
