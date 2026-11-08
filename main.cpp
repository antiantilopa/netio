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

class Unit: public NetIO::Serializable {
public:
    int a;
    std::string b;
    float c;

    Unit(int a, std::string b, float c): a(a), b(b), c(c) {}

    virtual std::vector<NetIO::SerializableVariant> getNetIOFields() override {
        return {a, b, c};
    }

    virtual NetIO::VectorSerializableVariant* primitive_serialize() override {
        return new NetIO::VectorSerializableVariant{std::vector<NetIO::SerializableVariant>{a, b, c}};
    }

    virtual int get_id() override {
        return 0; // Not used for this example
    }
};
// template void NetIO::Serializable::sync_key<Unit>(Unit* class_ptr, std::string key, bool primitive, bool by_id);
int main(){
    // float pi = 3.1415f;
    // void* ptr = &pi;
    // NetIO::Data d1 = NetIO::BaseWriter::_string_to_bytes("aboba png");
    // NetIO::Data d2 = NetIO::BaseWriter::_int_to_bytes((int) 31415, 4);
    // NetIO::Data d3 = NetIO::BaseWriter::_float_to_bytes(pi);
    // d2.extend(d1);
    // float f;
    // void* ptr2 = &f;
    // memcpy(ptr2, *d3.buffer.get(), sizeof(float));
    // std::cout << *d2.buffer.get() << *d3.buffer.get() << "\n\n";
    // std::cout << d2.hex() << "\n";
    // std::printf("%f\n", f);

    NetIO::Serializable::sync_key<Unit>((Unit*)nullptr, "unit", false, false);
    std::cout << "Registered class 'Unit' with class_id: " << NetIO::Serializable::get_class_id<Unit>((Unit*)nullptr) << std::endl;
    Unit unit(42, "Hello, World!", 2.718f);
    NetIO::Data serialized_data = NetIO::BaseWriter::_object_to_bytes(unit.serialize()->children);
    std::cout << "Serialized data (hex): " << serialized_data.hex() << std::endl;
    return 0;
}

