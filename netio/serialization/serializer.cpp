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
std::map<int, DeserializeFunction> Serializable::class_deserializers = {};

Serializable::Serializable(){
    // TODO
}

std::vector<SerializableVariant> Serializable::getNetIOFields(){
    return std::vector<SerializableVariant>{};
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

VectorSerializableVariant* Serializable::serialize(bool serialize_by_id){
    if (get_class_info(this).primitive){
        // primitives are serialized fully, they do not send their id, class id or anything else, they are just serialized fully.
        return this->primitive_serialize();
    }
    int class_id = get_class_id(this);
    if (get_class_info(this).by_id){
        // note that by_id means that the internal id will be used.
        return new VectorSerializableVariant{std::vector<SerializableVariant>{class_id, this->get_id()}};
    }
    if (serialize_by_id){
        // this iimplies that this object was already sent to the other side, and this is only for referensing to this object
        return new VectorSerializableVariant{std::vector<SerializableVariant>{class_id, serializable_object_id}};
    }
    VectorSerializableVariant* data = new VectorSerializableVariant(); // TODO do not garbage the heap! please in iterative way do not create a lot of new objects
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

}