#include "base_reader.h"

namespace NetIO{

std::mutex BaseReader::reader_lock = std::mutex();

Message BaseReader::decode(netio::socket conn){
    int tp = _read_int(conn, 2);
    std::string route = _read_string(conn);
    std::vector<SerializableVariant> data = _read_object(conn);
    return Message{tp, route, data};
}

SerializableVariant BaseReader::_read_field(netio::socket conn){
    char tp = '\r';
    conn.recv(&tp, 1);

    switch ((unsigned char) tp)
    {
        case serialization_types::STRING:
            return _read_string(conn);
        case serialization_types::INT8:
            return (char)_read_int(conn, 1);
        case serialization_types::UINT8:
            return (unsigned char)_read_int(conn, 1);
        case serialization_types::INT32:
            return (int)_read_int(conn, 4);
        case serialization_types::UINT32:
            return (unsigned int)_read_int(conn, 4);
        case serialization_types::INT64:
            return (long long)_read_int(conn, 8);
        case serialization_types::UINT64:
            return (unsigned long long)_read_int(conn, 8);
        case serialization_types::FLOAT32:
            return _read_float32(conn);
        case serialization_types::FLOAT64:
            return _read_float64(conn);
        case serialization_types::OBJECT:
            return std::make_shared<VectorSerializableVariant>(VectorSerializableVariant{_read_object(conn)});
        case serialization_types::LIST:
            return std::make_shared<VectorSerializableVariant>(VectorSerializableVariant{_read_object(conn)});
        case serialization_types::NONE:
            return nullptr;
        case serialization_types::NOTHING:
            return special_types::NOTHING;
        case serialization_types::END_OF_OBJECT:
            return special_types::END;
    default:
        throw "TypeNotFound ahh error";
    }
    return nullptr;
}
std::string BaseReader::_read_string(netio::socket conn){
    std::string result = std::string();
    char byte = '\0';
    conn.recv(&byte, 1);
    while (byte != '\0') {
        result.push_back(byte);
        conn.recv(&byte, 1);
    }
    return result;
}
bool BaseReader::_read_boolean(netio::socket conn){
    char byte = '\0';
    conn.recv(&byte, 1);
    return byte;
}
long long BaseReader::_read_int(netio::socket conn, unsigned int nbytes){
    if (nbytes > 8){
        throw "Cannot read integers larger than 8 bytes";
    }
    char* bytes = new char[nbytes] {};
    conn.recv(bytes, nbytes);
    long long result = 0;
    memcpy(((char*)&result), bytes, nbytes);
    delete[] bytes;
    return result;
}
float BaseReader::_read_float32(netio::socket conn){
    char* bytes = new char[4] {};
    float result;
    conn.recv(bytes, 4);
    memcpy(&result, bytes, 4);
    delete[] bytes;
    return result;
}
double BaseReader::_read_float64(netio::socket conn){
    char* bytes = new char[8] {};
    double result;
    conn.recv(bytes, 8);
    memcpy(&result, bytes, 8);
    delete[] bytes;
    return result;
}
std::vector<SerializableVariant> BaseReader::_read_object(netio::socket conn){
    std::vector<SerializableVariant> result;
    SerializableVariant field = _read_field(conn);
    while (!(std::holds_alternative<char*>(field) && std::get<char*>(field) == special_types::END)){
        result.push_back(field);
        field = _read_field(conn);
    }
    return result;
}
SerializableVariant BaseReader::_bytes_to_serialized(Data data, unsigned long long start){
    throw "NotImplementedYet ahh error";
}

}