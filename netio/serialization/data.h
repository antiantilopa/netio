#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <variant>
#include <vector>
#include <tuple>
#include <memory>
#include <cstring>

namespace NetIO {

struct Data {
private:
    unsigned long long _len;
    unsigned long long _capacity;
public:
    std::shared_ptr<char*> buffer;
    Data();
    Data(unsigned long long capacity);
    Data(char* buffer, unsigned long long len, unsigned long long capacity = 0);
    Data(Data& data);
    ~Data();
    unsigned long long len() const;
    unsigned long long capacity() const;
    void extend(char* buffer, unsigned long long len);
    void extend(char value);
    void extend(const Data& data);
    char get(unsigned long long index);
    std::string hex();
};
}

#endif