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

Serializable* Serializable::get_class(int id){
    if (!subclasses.count(id)){
        std::cout << "id: " << id << "\n";
        throw std::invalid_argument("NetIO: no class with given id was found");
    }
    int hash = subclasses[id];
    
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


VectorSerializableVariant Serializable::serialize(Serializable* as_class_ptr, bool serialize_by_id){
    throw "NotImplementedYet ahh error";
    return VectorSerializableVariant{std::vector<SerializableVariant>{0}};
}

template<typename T>
int get_hash_code(T* class_ptr){
    return typeid(*class_ptr).hash_code();
}

}