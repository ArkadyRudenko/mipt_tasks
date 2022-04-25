#pragma once

#include <iostream>
#include <cstring>

class String {
private:
    size_t sz = 0;
    size_t capacity = 0;
    char *str = nullptr;
public:
    String() = default;

    String(size_t n, char ch) : sz(n), capacity(n * 2), str(new char[capacity]) {
        memset(str, ch, sz);
    }

    String(const char *data_) {
        int i = 0;
        int size_ = 0;
        while (data_[i++] != '\0') {
            size_++;
        }
        sz = size_;
        capacity = sz * 2;
        str = new char[capacity];
        i = 0;
        while (data_[i] != '\0') {
            str[i] = data_[i];
            i++;
        }
//        str[i] = '\0';
    }

    String(const String &str_) : String(str_.sz, 0) {
        memcpy(str, str_.str, sz);
    }

    size_t size() const {
        return sz;
    }

    void pop_back() {
        if(sz)
            --sz;
    }

    bool empty() const {
        return !sz;
    }

    void clear() {
        sz = 0;
    }

    const char& front() const {
        return str[0];
    }

    const char& back() const {
        return str[sz - 1];
    }

    void push_back(char ch) {
        if(sz != capacity) {
            str[sz++] = ch;
        } else {
            capacity = sz * 2;
            char *additionally = new char[capacity];
            memcpy(additionally, str, sz);
            delete[] str;
            str = additionally;
            str[sz++] = ch;
        }
    }

    ~String() {
        delete[] str;
    }

    char operator[](size_t index) const {
        return str[index];
    }

    String &operator+=(const String &s) {
        if (s.size() > capacity - sz) {
            sz += s.size();
            capacity = sz * 2;
            char *additionally = new char[capacity];
            memcpy(additionally, str, sz - s.size());
            memcpy(additionally + (sz - s.size()), s.str, s.size());
            delete[] str;
            str = additionally;
        } else {
            memcpy((str + sz), s.str, s.size());
            sz += s.size();
        }
        return *this;
    }

//    String &operator+=(char ch) {
//
//    }

    String operator+(const String &s) {
        String copy = *this;
        copy += s;
        return copy;
    }

    String& operator=(const String &s) {
        sz = s.sz;
        capacity = s.capacity;
        delete[] str;
        str = new char[capacity];
        memcpy(str, s.str, sz);
        return *this;
    }

    char &operator[](size_t index) {
        return str[index];
    }

};

std::ostream &operator<<(std::ostream &out, const String &str);
void operator>>(std::istream &in, String &str); // TODO