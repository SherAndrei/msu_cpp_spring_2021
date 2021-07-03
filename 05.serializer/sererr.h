#ifndef SERIALIZE_ERR_H
#define SERIALIZE_ERR_H

enum class Error {
    Unknown = -1,
    NoError,
    CorruptedArchive
};

#endif  // SERIALIZE_ERR_H
