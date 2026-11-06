#ifndef NETIO_SERIALIZER_IMPL_H
#define NETIO_SERIALIZER_IMPL_H

#include "../util/socket.h"
#include "serializer.h"

namespace NetIO {

template<typename T>
int get_hash_code(T* class_ptr){
    if (class_ptr != nullptr){
        return typeid(*class_ptr).hash_code();
    }
    std::cout << "getting hash code: " << typeid(T).name() << " => " << (int)typeid(T).hash_code() << "\n";
    return typeid(T).hash_code();
}

template <typename T> 
int Serializable::get_class_id(T* class_ptr){
    int hash_code = get_hash_code<T>(class_ptr);
    if (!class_infos.count(hash_code)){
        throw std::invalid_argument("NetIO: class not registered");
    }
    return class_infos[hash_code].class_id;
}
template <typename T> 
bool Serializable::get_class_primitive(T* class_ptr){
    int hash_code = get_hash_code<T>(class_ptr);
    if (!class_infos.count(hash_code)){
        throw std::invalid_argument("NetIO: class not registered");
    }
    return class_infos[hash_code].primitive;
}
template <typename T> 
bool Serializable::get_class_by_id(T* class_ptr){
    int hash_code = get_hash_code<T>(class_ptr);
    if (!class_infos.count(hash_code)){
        throw std::invalid_argument("NetIO: class not registered");
    }
    return class_infos[hash_code].by_id;
}
template <typename T> 
ClassInfo Serializable::get_class_info(T* class_ptr){
    int hash_code = get_hash_code<T>(class_ptr);
    if (!class_infos.count(hash_code)){
        throw std::invalid_argument("NetIO: class not registered");
    }
    return class_infos[hash_code];
}

template <typename T> 
DeserializeFunction Serializable::get_class_deserializer(T* class_ptr){
    int hash_code = get_hash_code<T>(class_ptr);
    if (!class_deserializers.count(hash_code)){
        throw std::invalid_argument("NetIO: class not registered");
    }
    return class_deserializers[hash_code];
}

template <typename T> 
void Serializable::sync_key(T* class_ptr, std::string key, bool primitive, bool by_id){
    int class_id = hash(key);
    int hash_code = get_hash_code<T>(class_ptr);
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

template<typename T>
VectorSerializableVariant* Serializable::serialize(T* as_class_ptr, bool serialize_by_id){
    

    // TODO

    return new VectorSerializableVariant{std::vector<SerializableVariant>{0}};
}

template <class T>
T* Serializable::deserialize(T* class_ptr, VectorSerializableVariant data, bool ignore_class_id, HandleSerializables handle_serializables){
    return nullptr; // TODO
}

}


#endif // NETIO_SERIALIZER_IMPL_H