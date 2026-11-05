#include "../util/socket.h"
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

const char* special_types::NOTHING = &special_types::__NOTHING;
const char* special_types::END = &special_types::__END;

std::map<int, int> Serializable::subclasses = {};
std::map<int, ClassInfo> Serializable::class_infos = {};

Serializable::Serializable(){
    // TODO
}

template <typename T> 
int Serializable::get_class_id(T* class_ptr){
    int hash_code = get_hash_code(class_ptr);
    if (!class_infos.count(hash_code)){
        throw std::invalid_argument("NetIO: class not registered");
    }
    return class_infos[hash_code].class_id;
}
template <typename T> 
bool Serializable::get_class_primitive(T* class_ptr){
    int hash_code = get_hash_code(class_ptr);
    if (!class_infos.count(hash_code)){
        throw std::invalid_argument("NetIO: class not registered");
    }
    return class_infos[hash_code].primitive;
}
template <typename T> 
bool Serializable::get_class_by_id(T* class_ptr){
    int hash_code = get_hash_code(class_ptr);
    if (!class_infos.count(hash_code)){
        throw std::invalid_argument("NetIO: class not registered");
    }
    return class_infos[hash_code].by_id;
}
template <typename T> 
ClassInfo Serializable::get_class_info(T* class_ptr){
    int hash_code = get_hash_code(class_ptr);
    if (!class_infos.count(hash_code)){
        throw std::invalid_argument("NetIO: class not registered");
    }
    return class_infos[hash_code];
}

template <typename T> 
DeserializeFunction Serializable::get_class_deserializer(T* class_ptr){
    int hash_code = get_hash_code(class_ptr);
    if (!class_deserializers.count(hash_code)){
        throw std::invalid_argument("NetIO: class not registered");
    }
    return class_deserializers[hash_code];
}

std::vector<SerializableVariant> Serializable::getNetIOFields(){
    return std::vector<SerializableVariant>{};
}

template <typename T> 
void Serializable::sync_key(T* class_ptr, std::string key, bool primitive, bool by_id){
    int class_id = hash(key);
    int hash_code = get_hash_code(class_ptr);
    if (subclasses.count(class_id)){
        std::cout << "class_id: " << class_id << "\n";
        throw std::invalid_argument("NetIO: tried to synchronize class by the key already existing");
    }
    subclasses[class_id] = hash_code;
    class_infos[hash_code] = ClassInfo{class_id, primitive, by_id};
    class_deserializers[hash_code] = [class_ptr](VectorSerializableVariant data, bool ignore_class_id, HandleSerializables handle_serializables) -> T* {
        return deserialize(class_ptr, data, ignore_class_id, handle_serializables);
    };
}

int Serializable::hash(std::string key){
    // DJB2 hash function
    // for full DJB2 turn 'int' into 'long'.
    int hash = 5381;
    for (char c : key) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

template<typename T>
VectorSerializableVariant* Serializable::serialize(T* as_class_ptr, bool serialize_by_id){
    

    // TODO

    return new VectorSerializableVariant{std::vector<SerializableVariant>{0}};
}

VectorSerializableVariant* Serializable::serialize(bool serialize_by_id){
    if (get_class_info(this).primitive){
        // primitives are serialized fully, they do not send their id, class id or anything else, they are just serialized fully.
        return this->primitive_serialize();
    }
    if (get_class_info(this).by_id){
        // note that by_id means that the internal id will be used.
        return new VectorSerializableVariant{std::vector<SerializableVariant>{class_id, this->get_id()}};
    }
    if (serialize_by_id){
        // this iimplies that this object was already sent to the other side, and this is only for referensing to this object
        return new VectorSerializableVariant{std::vector<SerializableVariant>{class_id, serializable_object_id}};
    }
    VectorSerializableVariant* data = new VectorSerializableVariant();
    data->children.push_back(class_id);
    data->children.push_back(serializable_object_id);

    for (SerializableVariant field : this->getNetIOFields()){
        if (std::holds_alternative<Serializable*>(field)){
            Serializable* field_ptr = std::get<Serializable*>(field);
            if (field_ptr == nullptr){
                throw std::invalid_argument("NetIO: tried to serialize a null pointer to Serializable");
            }
            else{
                VectorSerializableVariant* inner_data = field_ptr->serialize(serialize_by_id);
                data->children.push_back(std::make_shared<VectorSerializableVariant>(std::move(*inner_data)));
                delete inner_data;
            }
        }
        else{
            data->children.push_back(field);
        }
    }
    return data;
}

template<typename T>
int get_hash_code(T* class_ptr){
    return typeid(*class_ptr).hash_code();
}

}