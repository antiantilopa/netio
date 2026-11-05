#include "../util/socket.h"
#include "data.h"
#include "serializer.h"
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

unsigned long long inline next_power_2(unsigned long long n){
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    n++;
    return n;
}

Data::Data(){
    buffer = nullptr;
    _len = 0;
    _capacity = 0;
}

Data::Data(unsigned long long capacity){
    this->_capacity = next_power_2(capacity);
    this->buffer = std::make_shared<char*>(new char[this->_capacity]);
    this->_len = 0;
}

Data::Data(char* buffer, unsigned long long len, unsigned long long capacity){
    if (len == 0 && capacity == 0){
        buffer = nullptr;
        len = 0;
        _capacity = 0;
    }

    if ((capacity > 0) && (capacity > len)){
        this->_capacity = next_power_2(capacity);
    } 
    else{
        this->_capacity = next_power_2(len);
    }
    this->_len = len;
    this->buffer = std::make_shared<char*>(new char[this->_capacity]);
    std::memcpy(*this->buffer.get(), buffer, len);
}

Data::Data(Data& data){
    this->_len = data._len;
    this->_capacity = data._capacity;
    if (this->_len == 0){
        this->buffer = nullptr;
        return;
    }
    this->buffer = std::make_shared<char*>(new char[this->_capacity]);
    std::memcpy(*this->buffer.get(), *data.buffer.get(), this->_len);
}

Data::~Data(){
    this->buffer.reset();
}

unsigned long long Data::len() const{
    return this->_len;
}
unsigned long long Data::capacity() const{
    return this->_capacity;
}

void Data::extend(char* buffer, unsigned long long len){
    if (this->_len + len > this->_capacity){
        this->_capacity = next_power_2(this->_len + len);
        char* new_buffer = new char[_capacity];
        std::memcpy(new_buffer, *this->buffer.get(), this->_len);
        this->buffer.reset();
        this->buffer = std::make_shared<char*>(new_buffer);
    }
    std::memcpy(*this->buffer.get() + this->_len, buffer, len);
    this->_len += len;
}
void Data::extend(char value){
    if (this->_capacity == 0){
        this->_capacity = 1;
        char* new_buffer = new char[_capacity];
        this->buffer = std::make_shared<char*>(new_buffer);
    }
    else if (this->_len + 1 > this->_capacity){
        this->_capacity >>= 1;
        char* new_buffer = new char[_capacity];
        std::memcpy(new_buffer, *this->buffer.get(), this->_len);
        this->buffer.reset();
        this->buffer = std::make_shared<char*>(new_buffer);
    }
    *(*this->buffer.get() + this->_len) = value;
    this->_len += 1;
}
void Data::extend(const Data& data){
    this->extend(*data.buffer.get(), data.len());
}

char Data::get(unsigned long long index){
    return *(*this->buffer.get() + index);
}

const char HEX[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

std::string Data::hex(){
    char result[this->_len * 2];
    for (int i = 0; i < this->_len; i++){
        unsigned char val = *(*((unsigned char**)this->buffer.get()) + i);
        result[2 * i] = HEX[val / 16];
        result[2 * i + 1] = HEX[val % 16];
    }
    std::string result_str = std::string(result, this->_len * 2);
    return result_str;
}

}