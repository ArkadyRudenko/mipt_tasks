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

public:
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

        deque_iterator &operator-(const deque_iterator &it) {
            deque_iterator res(this->chunks,
                               chunk - it.chunk,
                               index - it.index);
            return res;
        }
    };

private:
    using iterator = deque_iterator<T>;
    using const_iterator = deque_iterator<const T>;
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

template<typename T>
size_t Deque<T>::size() const {
    return sz;
}

template<typename T>
Deque<T>::Deque(int capacity) :
        capacity(capacity),
        begin_(chunks, ((capacity / chunk_size) + 1) / 2, chunk_size / 2),
        end_(begin_),
        size_main_arr((capacity / chunk_size) + 1) {
    chunks = new T *[size_main_arr];
    for (size_t i = 0; i < size_main_arr; i++) {
        try {
            chunks[i] = new T[chunk_size];
        } catch (...) { cout << "\nFAIL\n;"; }
    }
    begin_.setChunks(chunks);
    end_.setChunks(chunks);
}


template<typename T>
Deque<T>::Deque(const Deque &deq) :
        end_(deq.end_), begin_(deq.begin_) {
    *this = deq;
}

template<typename T>
Deque<T> &Deque<T>::operator=(const Deque &deq) {
    if(chunks) {
        for (size_t i = 0; i < size_main_arr; i++) {
            delete[] chunks[i];
        }
        delete[] chunks;
    }
    size_main_arr = deq.size_main_arr;
    capacity = deq.capacity;
    sz = deq.sz;
    chunks = new T*[size_main_arr];
    for (size_t i = 0; i < size_main_arr; i++) {
        chunks[i] = new T[chunk_size];
    }
    end_ = deq.end_;  begin_ = deq.begin_;
    end_.setChunks(chunks);
    begin_.setChunks(chunks);
    auto tmp = begin_;
    for (auto tmp_begin = deq.begin_; tmp_begin != deq.end_;) {
        *tmp = *tmp_begin;
        tmp_begin++; tmp++;
    }
    return *this;
}

template<typename T>
void Deque<T>::push_back(T item) {
    if (begin_ == end_) {
        *begin_ = item;
    } else if (end_.getChunk() == size_main_arr and !end_.getIndex()) {
        relocation_end();
        *end_ = item;
    } else {
        *end_ = item;
    }
    end_++;
    sz++;
}

template<typename T>
void Deque<T>::pop_back() {
    if (end_ != begin_) {
        --end_;
        --sz;
    }
}

template<typename T>
void Deque<T>::push_front(T item) {
    if (begin_ == end_) {
        *begin_ = item;
        end_++;
    } else if (!begin_.getChunk() and !begin_.getIndex()) {
        relocation_begin();
        begin_.setIndex(chunk_size - 1);
        *begin_ = item;
        end_.setChunk(end_.getChunk() + 1);
    } else {
        *(--begin_) = item;
    }
    sz++;
}

template<typename T>
void Deque<T>::pop_front() {
    if (end_ != begin_) {
        ++begin_;
        --sz;
    }
}

template<typename T>
T &Deque<T>::operator[](size_t index) {
    return *(begin_ + index);
}

template<typename T>
Deque<T>::Deque() :
        begin_(chunks, 0, chunk_size / 2),
        end_(begin_),
        size_main_arr(1) {
    chunks = new T *[size_main_arr];
    chunks[0] = new T[chunk_size];
    begin_.setChunks(chunks);
    end_.setChunks(chunks);
}

template<typename T>
T &Deque<T>::at(size_t index) {
    if (index > sz - 1) {
        throw std::out_of_range("bad index");
    } else {
        return *(begin_ + index);
    }
}

template<typename T>
const T &Deque<T>::operator[](size_t index) const {
    return *(begin_ + index);
}

template<typename T>
Deque<T>::Deque(int size, T item)
        : Deque(size) {
    for (size_t i = 0; i < size; i++) {
        this->push_back(item);
    }
}

template<typename T>
void Deque<T>::relocation_begin() {
    size_main_arr++;
    T **new_arr = new T *[size_main_arr];
    new_arr[0] = new T[chunk_size];
    for (size_t i = 0; i < size_main_arr - 1; i++) {
        new_arr[i + 1] = chunks[i];
    }
    delete[] chunks;
    chunks = new_arr;
    begin_.setChunks(chunks);
    end_.setChunks(chunks);
}

template<typename T>
Deque<T>::~Deque() {
    for (size_t i = 0; i < size_main_arr; i++) {
        delete[] chunks[i];
    }
    delete[] chunks;
}

template<typename T>
void Deque<T>::relocation_end() {
    T **new_arr = new T *[size_main_arr + 1];
    for (size_t i = 0; i < size_main_arr; i++) {
        new_arr[i] = chunks[i];
    }
    new_arr[size_main_arr] = new T[chunk_size];
    delete[] chunks;
    chunks = new_arr;
    size_main_arr++;
    begin_.setChunks(chunks);
    end_.setChunks(chunks);
}