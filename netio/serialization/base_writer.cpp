#include <mutex>
#include "base_writer.h"

namespace NetIO{

std::mutex BaseWriter::writer_lock = std::mutex();

void setBigEndian(char* buffer, int length){
    if (length == 1) return;
    else if (length == 2) *((unsigned short*)buffer) = htons(*((unsigned short*)buffer));
    else if (length == 4) *((unsigned long*)buffer) = htonl(*((unsigned long*)buffer));
    else if (length == 8) *((unsigned long long*)buffer) = htonll(*((unsigned long long*)buffer));
    else{
        std::cout << "(!!!) meh, I do not want to implement bytes reverser \n";
        throw std::invalid_argument("creator's laziness ahh error");
    }
}

void BaseWriter::encode(netio::socket conn, Message message){
    std::vector<SerializableVariant> data_vector;
    if (std::holds_alternative<SerializableVariant>(message.data)) {
        if (std::holds_alternative<Serializable*>(std::get<SerializableVariant>(message.data))){
            Serializable* serializable_ptr = std::get<Serializable*>(std::get<SerializableVariant>(message.data));
            VectorSerializableVariant* serialized_data = serializable_ptr->serialize();
            data_vector = serialized_data->children;
            delete serialized_data;
        }
        else {
            data_vector = std::vector<SerializableVariant>{std::get<SerializableVariant>(message.data)};
        }
    }
    else{
        data_vector = std::get<std::vector<SerializableVariant>>(message.data);
    }
    Data serialized_message = Data(32); // HACK find more optimal size for this, or make it dynamic.
    serialized_message.extend(_int_to_bytes(message.type, 2));
    serialized_message.extend(_string_to_bytes(message.route));
    serialized_message.extend(_object_to_bytes(data_vector));
    writer_lock.lock();
    conn.send(*serialized_message.buffer.get(), serialized_message.len());
    writer_lock.unlock();
}
Data BaseWriter::_int_to_bytes(long long value, unsigned int nbytes){
    char* bytes = new char[nbytes];
    for (long long i = 0; i < nbytes; i++) {
        bytes[i] = (value >> (8 * (nbytes - 1 - i))) & 0xFF;
    }
    return Data{bytes, nbytes};
}
Data BaseWriter::_string_to_bytes(std::string value){
    if (value.size() > INT_MAX) {
        std::cerr << "String too long to serialize" << std::endl;
        exit(1);
    }
    char* bytes = new char[value.size() + 1];
    value.copy(bytes, value.size());
    bytes[value.size()] = '\0';
    return Data{bytes, (unsigned long long)value.size() + 1};
    delete[] bytes;
}
Data BaseWriter::_boolean_to_bytes(bool value){
    char* bytes = new char[1];
    bytes[0] = value ? 1 : 0;
    return Data{bytes, 1};
}
Data BaseWriter::_float_to_bytes(float value){
    char* bytes = new char[4];
    memcpy(bytes, &value, 4);
    setBigEndian(bytes, 4);
    return Data{bytes, 4};
}
Data BaseWriter::_float_to_bytes(double value){
    char* bytes = new char[8];
    memcpy(bytes, &value, 8);
    setBigEndian(bytes, 8);
    return Data{bytes, 8};
}
Data BaseWriter::_object_to_bytes(std::vector<SerializableVariant> value){
    Data data = Data();
    for (int i = 0; i < value.size(); i++) {
        data.extend(_any_to_bytes((value)[i]));
    }
    data.extend(serialization_types::END_OF_OBJECT);
    return data;
}
Data BaseWriter::_any_to_bytes(SerializableVariant value){
    Data result;
    if (std::holds_alternative<int>(value)) {
        if (std::get<int>(value) >= 0 && std::get<int>(value) <= UINT8_MAX){
            result.extend(serialization_types::UINT8);
            result.extend(_int_to_bytes(std::get<int>(value), 1));
        }
        else if (std::get<int>(value) >= INT8_MIN && std::get<int>(value) <= INT8_MAX){
            result.extend(serialization_types::INT8);
            result.extend(_int_to_bytes(std::get<int>(value), 1));
        }
        else if (std::get<int>(value) >= 0 && std::get<int>(value) <= UINT32_MAX){
            result.extend(serialization_types::UINT32);
            result.extend(_int_to_bytes(std::get<int>(value), 4));
        }
        else if (std::get<int>(value) >= INT32_MIN && std::get<int>(value) <= INT32_MAX){
            result.extend(serialization_types::INT32);
            result.extend(_int_to_bytes(std::get<int>(value), 4));
        }
        else if (std::get<int>(value) >= 0 && std::get<int>(value) <= UINT64_MAX){
            result.extend(serialization_types::UINT64);
            result.extend(_int_to_bytes(std::get<int>(value), 8));
        }
        else if (std::get<int>(value) >= INT64_MIN && std::get<int>(value) <= INT64_MAX){
            result.extend(serialization_types::INT64);
            result.extend(_int_to_bytes(std::get<int>(value), 8));
        }
    }
    else if (std::holds_alternative<char>(value)) {
        result.extend(serialization_types::INT8);
        result.extend(std::get<char>(value));
    }
    else if (std::holds_alternative<std::string>(value)) {
        result.extend(serialization_types::STRING);
        result.extend(_string_to_bytes(std::get<std::string>(value)));
    }
    else if (std::holds_alternative<float>(value)) {
        result.extend(serialization_types::FLOAT32);
        result.extend(_float_to_bytes(std::get<float>(value)));
    }
    else if (std::holds_alternative<double>(value)) {
        result.extend(serialization_types::FLOAT64);
        result.extend(_float_to_bytes(std::get<double>(value)));
    }
    else if (std::holds_alternative<bool>(value)) {
        result.extend(serialization_types::INT8);
        result.extend(_boolean_to_bytes(std::get<bool>(value)));
    }
    else if (std::holds_alternative<std::shared_ptr<VectorSerializableVariant>>(value)) {
        result.extend(serialization_types::OBJECT);
        result.extend(_object_to_bytes(std::get<std::shared_ptr<VectorSerializableVariant>>(value)->children));
    }
    else {
        throw "No type matches given value.";
    }
    return result;
}

}