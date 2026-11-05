#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "../util/socket.h"
#include "data.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <tuple>
#include <variant>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace NetIO {

namespace serializable_primitives {
    typedef enum {
        INT,
        STRING,
        FLOAT,
        BOOL,
        TUPLE,
        LIST,
        NULLTYPE
    } SerializablePrimitive_t;
}
namespace serialization_types {
    typedef enum {
        STRING = 0,
        INT32 = 1,
        FLOAT32 = 2,
        OBJECT = 3,
        INT8 = 4,
        NONE = 5,
        NOTHING = 6,
        LIST = 7,
        UINT32 = 8,
        UINT8 = 9,
        FLOAT64 = 10,
        UINT64 = 11,
        INT64 = 12,
        END_OF_OBJECT = 255
    } SerializationTypes;
}
namespace special_types {
    const char __NOTHING = '\0';
    const char __END = '\0';
    extern const char* NOTHING;
    extern const char* END;
}

class Serializable;
struct VectorSerializableVariant;

using SerializableVariant = std::variant<
    unsigned char, char, 
    unsigned short, short, 
    unsigned int, int, 
    unsigned long, long, 
    unsigned long long, long long, 
    std::string, 
    float, double, bool, 
    std::shared_ptr<VectorSerializableVariant>, 
    Serializable*, 
    std::nullptr_t, 
    char* >;

struct VectorSerializableVariant{
    std::vector<SerializableVariant> children;
};

struct Message {
    int type;
    std::string route;
    std::variant<std::vector<SerializableVariant>, SerializableVariant> data;
};

struct ClassInfo {
    int class_id;
    bool primitive;
    bool by_id;
};

typedef Serializable* (*HandleSerializables)(VectorSerializableVariant, Serializable*, bool, bool);
typedef std::function<Serializable*(VectorSerializableVariant, bool, HandleSerializables)> DeserializeFunction;

class Serializable {
public:
    // destructor should be the first thing here, so that typeid works correctly.
    virtual ~Serializable() = default;    
private:
    int class_id;
    static std::map<int, int> subclasses; // id to typeid hash_code
    static std::map<int, ClassInfo> class_infos; // typeid hash_code to ClassInfo
    static std::map<int, DeserializeFunction> class_deserializers; // typeid hash_code to deserializer function
protected:
    int serializable_object_id;
    VectorSerializableVariant last_serialization;

    // deserialize works with a pointer to a subclass of Serializable, and returns an instance of that subclass.
    // should be called from inside of a parser, not from outside.
    template <class T> static T* deserialize(T* class_ptr, VectorSerializableVariant data, bool ignore_class_id = false, HandleSerializables handle_serializables = nullptr);
    
    // for subclasses that are primitive. should NOT contain class_id nor id.
    // should return a VectorSerializableVariant on the heap, which will be deleted after use.
    virtual VectorSerializableVariant* primitive_serialize() = 0; 
    
    // for subclasses that are by_id. should NOT contain class_id nor id.
    virtual int get_id() = 0; 
public:
    bool is_created = false;

    // every subclass should implement this for themselves.
    // should NOT contain class_id nor id.
    virtual std::vector<SerializableVariant> getNetIOFields() = 0;

    Serializable();

    // getters
    template <typename T> static int get_class_id(T* class_ptr);
    template <typename T> static bool get_class_primitive(T* class_ptr);
    template <typename T> static bool get_class_by_id(T* class_ptr);
    template <typename T> static ClassInfo get_class_info(T* class_ptr);
    template <typename T> static DeserializeFunction get_class_deserializer(T* class_ptr);

    // class_ptr should be the type of pointer to a subclass of Serializable, and should be nullptr.
    template <typename T> static void sync_key(T* class_ptr, std::string key, bool primitive = false, bool by_id = false);
    static int hash(std::string key);

    // parse works with any pointer, and will return the same thing that was given to it.
    // CREATES OBJECT ON HEAP, MUST BE DELETED AFTER USE.
    template <typename T> static T* parse(VectorSerializableVariant data, T* class_ptr, bool ignore_class_id = false, HandleSerializables handle_serializables = nullptr);

    // creates an object VectorSerializableVariant on the heap NEEDS TO BE DELETED AFTER USE.
    template <typename T> VectorSerializableVariant* serialize(T* as_class_ptr, bool serialize_by_id = false);
    // creates an object VectorSerializableVariant on the heap NEEDS TO BE DELETED AFTER USE.
    VectorSerializableVariant* serialize(bool serialize_by_id = false);
};

}
#endif