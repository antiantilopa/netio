#ifndef BASE_READER_H
#define BASE_READER_H

#include "data.h"
#include "serializer.h"

namespace NetIO{

class BaseReader {
protected:
    static std::mutex reader_lock; 
public:
    static Message decode(netio::socket conn);
    static SerializableVariant _read_field(netio::socket conn);
    static std::string _read_string(netio::socket conn);
    static bool _read_boolean(netio::socket conn);
    static long long _read_int(netio::socket conn, unsigned int nbytes);
    static float _read_float32(netio::socket conn);
    static double _read_float64(netio::socket conn);
    static std::vector<SerializableVariant> _read_object(netio::socket conn);
    static SerializableVariant _bytes_to_serialized(Data data, unsigned long long start);
};
    
}

#endif