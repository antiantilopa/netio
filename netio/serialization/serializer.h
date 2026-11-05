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

struct VectorSerializableVariant;

struct ClassInfo {
    int class_id;
    bool primitive;
    bool by_id;
};


class Serializable {
    // destructor should be the first thing here, so that typeid works correctly.
    virtual ~Serializable() = default;
public:
    typedef Serializable* (*HandleSerializables)(VectorSerializableVariant, Serializable*, bool, bool);
    
private:
    static std::map<int, int> subclasses; // id to typeid hash_code
    static std::map<int, ClassInfo> class_infos; // typeid hash_code to ClassInfo
    static std::map<int, std::function<Serializable*(VectorSerializableVariant, bool, HandleSerializables)>> class_deserializers; // typeid hash_code to deserializer function
protected:
    int class_id;
    int id;
    VectorSerializableVariant last_serialization;

    // deserialize works with a pointer to a subclass of Serializable, and returns an instance of that subclass.
    // should be called from inside of a parser, not from outside.
    template <class T> static T* deserialize(T* class_ptr, VectorSerializableVariant data, bool ignore_class_id = false, HandleSerializables handle_serializables = nullptr);
public:
    bool is_created = false;

    Serializable();

    // getters
    static Serializable* get_class(int id);
    template <typename T> static int get_class_id(T* class_ptr);
    template <typename T> static bool get_class_primitive(T* class_ptr);
    template <typename T> static bool get_class_by_id(T* class_ptr);
    template <typename T> static ClassInfo get_class_info(T* class_ptr);

    // class_ptr should be the type of pointer to a subclass of Serializable, and should be nullptr.
    template <typename T> static void sync_key(T* class_ptr, std::string key, bool primitive = false, bool by_id = false);
    static int hash(std::string key);

    // parse works with any pointer, and will return the same thing that was given to it.
    // CREATES OBJECT ON HEAP, MUST BE DELETED AFTER USE.
    template <typename T> static T* parse(VectorSerializableVariant data, T* class_ptr, bool ignore_class_id = false, HandleSerializables handle_serializables = nullptr);

    VectorSerializableVariant serialize(Serializable* as_class_ptr = nullptr, bool serialize_by_id = false);
};


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


}
#endif