#include <iostream>
#include <vector>
#include <variant>
#include <cstdint>
#include "netio/util/socket.h"
#include "netio/serialization/serializer.h"
#include "netio/serialization/data.h"
#include "netio/serialization/base_writer.h"
#include "netio/serialization/base_reader.h"
#include <vector>
#include <memory>

int main(){
    float pi = 3.1415f;
    void* ptr = &pi;
    NetIO::Data d1 = NetIO::BaseWriter::_string_to_bytes("aboba png");
    NetIO::Data d2 = NetIO::BaseWriter::_int_to_bytes((int) 31415, 4);
    NetIO::Data d3 = NetIO::BaseWriter::_float_to_bytes(pi);
    d2.extend(d1);
    float f;
    void* ptr2 = &f;
    memcpy(ptr2, *d3.buffer.get(), sizeof(float));
    std::cout << *d2.buffer.get() << *d3.buffer.get() << "\n\n";
    std::cout << d2.hex() << "\n";
    std::printf("%f\n", f);
    return 0;
}

