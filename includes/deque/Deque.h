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
    int capacity = 0;
    std::vector<std::vector<T>> chunks;

    size_t begin_chunk = 0;
    size_t end_chunk = 0;
    int begin_index = 0;
    int end_index = 1;

    class iterator {
    private:
        size_t chunk = 0;
        size_t index = 0;
        std::vector<std::vector<T>> &link_on_chunks;
    public:

        iterator(std::vector<std::vector<T>> &link)
                : link_on_chunks(link) {

        }

        T &operator*() {
            return link_on_chunks[chunk][index];
        }

        iterator &operator++() {
//            if (index == chunk_size - 1) {
//                index = 0;
//                if (begin_chunk != chunks.size() - 1) {
//                    begin_chunk++;
//                    if (begin_chunk > end_chunk) {
//                        end_chunk = begin_chunk;
//                    }
//                } else {
//                    begin_index = chunk_size - 2; // ???
//                    end_index = chunk_size - 1; // ???
//                }
//            } else {
//                if (!(end_chunk == begin_chunk and begin_index == chunk_size - 2
//                      and (end_index == chunk_size - 1))) {
//                    begin_index++;
//                }
//            }
//            if (sz) {
//                sz--;
//            }
        }

        iterator &operator--() {

        }

        iterator &operator++(int);

        iterator &operator--(int);

        T *operator->();

        bool operator<(const iterator &it);

        bool operator<=(const iterator &it);

        bool operator>(const iterator &it);

        bool operator>=(const iterator &it);

        bool operator!=(const iterator &it);

        bool operator==(const iterator &it);

        iterator &operator+(int n);

        iterator &operator-(const iterator &it);
    };

//    iterator begin_;
//    iterator end_;

    iterator begin() {
        return begin;
    }

    iterator end() {
        return end;
    }

};

template<typename T>
size_t Deque<T>::size() const {
    return sz;
}

template<typename T>
Deque<T>::Deque(int capacity) :
        capacity(capacity),
        begin_chunk(((capacity / chunk_size) + 1) / 2),
        end_chunk(begin_chunk){
    chunks.reserve((capacity / chunk_size) + 1);
    for (size_t i = 0; i <= (capacity / chunk_size); i++) {
        chunks.push_back(std::vector<T>(chunk_size));
    }
    begin_index = chunk_size / 2;
    end_index = begin_index + 1;
}

template<typename T>
void Deque<T>::push_back(T item) {
    chunks[end_chunk][end_index] = item;
    end_index++;
    if (end_index == chunk_size) {
        end_index = 0;
        if (end_chunk == chunks.size() - 1) {
            chunks.push_back(std::vector<T>(chunk_size));
        } else {

        }
        end_chunk++;
    }
    sz++;
}

template<typename T>
void Deque<T>::pop_back() {
    if (end_index == 0) {
        end_index = chunks.size() - 1;
        if (end_chunk != 0) {
            end_chunk--;
        } else {
            end_index = 0;
            begin_index = 1;
        }
    } else {
        if (!(end_chunk == begin_chunk and begin_index == chunk_size - 2
              and (end_index == chunk_size - 1))) {
            end_index--;
        }
    }
    if (sz) {
        sz--;
    }
}

template<typename T>
void Deque<T>::push_front(T item) {
    chunks[begin_chunk][begin_index] = item;
    begin_index--;
    if (begin_index == -1) {
        begin_index = chunk_size - 1;
        if (begin_chunk == 0) {
            chunks.insert(chunks.begin(), std::vector<T>(chunk_size));
        } else {
            begin_chunk--;
        }
    }
    sz++;
}

template<typename T>
void Deque<T>::pop_front() {
    if (begin_index == chunk_size - 1) {
        begin_index = 0;
        if (begin_chunk != chunks.size() - 1) {
            begin_chunk++;
            if (begin_chunk > end_chunk) {
                end_chunk = begin_chunk;
            }
        } else {
            begin_index = chunk_size - 2; // ???
            end_index = chunk_size - 1; // ???
        }
    } else {
        if (!(end_chunk == begin_chunk and begin_index == chunk_size - 2
              and (end_index == chunk_size - 1))) {
            begin_index++;
        }
    }
    if (sz) {
        sz--;
    }
}

template<typename T>
T &Deque<T>::operator[](size_t index) {
    return chunks[begin_chunk + (begin_index + index + 1) / chunk_size][(begin_index + index + 1) % chunk_size];
}

template<typename T>
Deque<T>::Deque() {
    chunks.reserve(1);
    chunks.push_back(std::vector<T>(chunk_size));
    begin_index = chunk_size / 2;
    end_index = begin_index + 1;
}

template<typename T>
T &Deque<T>::at(size_t index) {
    if (index > sz - 1) {
        throw std::out_of_range("bad index");
    } else {
        return (*this)[index];
    }

}

template<typename T>
Deque<T>::Deque(const Deque &deq)
        :
        Deque(deq.capacity) {
    *this = deq;
}

template<typename T>
const T &Deque<T>::operator[](size_t index) const {
    return chunks[begin_chunk + (begin_index + index + 1) / chunk_size][(begin_index + index + 1) % chunk_size];
}

template<typename T>
Deque<T> &Deque<T>::operator=(const Deque &deq) {
    sz = deq.sz;
    begin_index = deq.begin_index;
    begin_chunk = deq.begin_chunk;
    end_index = deq.end_index;
    end_chunk = deq.end_chunk;
    chunks = deq.chunks;
    return *this;
}

template<typename T>
Deque<T>::Deque(int size, T item)
        :
        Deque(size) {
    for (size_t i = 0; i < size; i++) {
        this->push_back(item);
    }
}
