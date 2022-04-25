
template<typename T>
Deque<T>::Deque() :
        begin_(*this, 0, KChunkSize / 2),
        end_(begin_),
        size_main_arr_(1),
        chunks(new T *[size_main_arr_]) {
    chunks[0] = new T[KChunkSize];
}

template<typename T>
Deque<T>::Deque(int capacity) :
        capacity(capacity),
        begin_(*this, ((capacity / KChunkSize) + 1) / 2, KChunkSize / 2), // middle chunk, middle index
        end_(begin_),
        size_main_arr_((capacity / KChunkSize) + 1),
        chunks(new T *[size_main_arr_])
        {
    for (size_t i = 0; i < size_main_arr_; i++) {
        chunks[i] = new T[KChunkSize];
    }
}

template<typename T>
Deque<T>::Deque(int size, T item)
        : Deque(size) {
    for (size_t i = 0; i < size; i++) {
        this->push_back(item);
    }
}

template<typename T>
Deque<T>::Deque(const Deque &deq) :
        end_({*this, deq.end_.getChunk(), deq.end_.getIndex()}),
        begin_({*this, deq.begin_.getChunk(), deq.begin_.getIndex()}) {
    *this = deq;
}

template<typename T>
Deque<T> &Deque<T>::operator=(const Deque &deq) {
    if (chunks) {
        for (size_t i = 0; i < size_main_arr_; i++) {
            delete[] chunks[i];
        }
        delete[] chunks;
    }
    capacity = deq.capacity;
    sz_ = deq.sz_;
    offset = deq.offset;
    size_main_arr_ = deq.size_main_arr_;
    chunks = new T *[size_main_arr_];
    for (size_t i = 0; i < size_main_arr_; i++) {
        chunks[i] = new T[KChunkSize];
    }
    auto tmp = begin_;
    for (auto tmp_begin : deq) {
        *tmp = tmp_begin;
        tmp++;
    }
    return *this;
}

template<typename T>
void Deque<T>::push_back(T item) {
    if (begin_ == end_) {
        *begin_ = item;
    } else if (end_.getChunk() == size_main_arr_ and !end_.getIndex()) {
        relocation_end();
        *end_ = item;
    } else {
        *end_ = item;
    }
    end_++;
    sz_++;
}

template<typename T>
void Deque<T>::push_front(T item) {
    if (begin_ == end_) {
        *begin_ = item;
        end_++;
    } else if ((begin_.getChunk() + offset) == 0 and !begin_.getChunk()) {
        relocation_begin();
        *--begin_ = item;
    } else {
        *(--begin_) = item;
    }
    sz_++;
}

template<typename T>
void Deque<T>::pop_back() {
    if (end_ != begin_) {
        --end_;
        --sz_;
    }
}

template<typename T>
void Deque<T>::pop_front() {
    if (end_ != begin_) {
        ++begin_;
        --sz_;
    }
}

template<typename T>
T &Deque<T>::operator[](size_t index) {
    return *(begin_ + index);
}

template<typename T>
const T &Deque<T>::operator[](size_t index) const {
    return *(begin_ + index);
}

template<typename T>
T &Deque<T>::at(size_t index) {
    if (index > sz_ - 1) {
        throw std::out_of_range("bad index");
    }
    return *(begin_ + index);
}

template<typename T>
void Deque<T>::relocation_begin() {
    size_main_arr_++;
    T **new_arr = new T *[size_main_arr_];
    new_arr[0] = new T[KChunkSize];
    offset++;
    for (size_t i = 0; i < size_main_arr_ - 1; i++) {
        new_arr[i + 1] = chunks[i];
    }
    delete[] chunks;
    chunks = new_arr;
}

template<typename T>
void Deque<T>::relocation_end() {
    T **new_arr = new T *[size_main_arr_ + 1];
    for (size_t i = 0; i < size_main_arr_; ++i) {
        new_arr[i] = chunks[i];
    }
    new_arr[size_main_arr_] = new T[KChunkSize];
    delete[] chunks;
    chunks = new_arr;
    ++size_main_arr_;
}

template<typename T>
size_t Deque<T>::size() const {
    return sz_;
}

template<typename T>
Deque<T>::~Deque() {
    for (size_t i = 0; i < size_main_arr_; i++) {
        delete[] chunks[i];
    }
    delete[] chunks;
}
