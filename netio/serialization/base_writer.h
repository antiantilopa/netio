#ifndef BASE_WRITER_H
#define BASE_WRITER_H


#include "data.h"
#include "serializer.h"

namespace NetIO{

class BaseWriter {
protected:
    static std::mutex writer_lock;

public:
    static void encode(netio::socket conn, Message message);
    static Data _int_to_bytes(long long value, unsigned int nbytes);
    static Data _string_to_bytes(std::string value);
    static Data _boolean_to_bytes(bool value);
    static Data _float_to_bytes(float value);
    static Data _float_to_bytes(double value);
    static Data _any_to_bytes(SerializableVariant value);
    static Data _object_to_bytes(std::vector<SerializableVariant> value);
};

}
#endif