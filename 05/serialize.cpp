#include <string>

#include "serialize.h"
#include "sererr.h"

namespace {

Error check(std::ios& ios) {
    return (!ios) ? Error::CorruptedArchive : Error::NoError;
}

}  // namespace

Error Serializer::process(bool& var) {
    return check(out_ << std::boolalpha << var << Separator);
}
Error Serializer::process(uint64_t& var) {
    return check(out_ << var << Separator);
}

namespace {

Error check_istream(std::istream& is) {
    return (!is || is.peek() != Serializer::Separator) ? Error::CorruptedArchive : Error::NoError;
}

}  // namespace

Error Deserializer::process(bool& var) {
    return check_istream(in_ >> std::boolalpha >> var);
}
Error Deserializer::process(uint64_t& var) {
    return check(in_ >> var);
}
