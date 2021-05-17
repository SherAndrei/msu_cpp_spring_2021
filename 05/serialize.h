#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <iostream>
#include <utility>

#include "sererr.h"


class ISerializer {
 public:
    template<typename... Args>
    Error operator()(Args&&... args) {
        return process(std::forward<Args>(args)...);
    }

 protected:
    template<typename T, typename... Args>
    Error process(T&& val, Args&&... args) {
        Error er = process(val);
        return (er == Error::NoError)
               ? process(std::forward<Args>(args)...)
               : er;
    }
    template<typename T>
    Error process(T&) {
        return Error::CorruptedArchive;
    }
    virtual Error process(bool&) = 0;
    virtual Error process(uint64_t&) = 0;
};

class Serializer : public ISerializer {
 public:
    static constexpr char Separator = ' ';

 public:
    explicit Serializer(std::ostream& out)
        : out_(out) {}

    template<typename T>
    Error save(T& obj) {
        return obj.serialize(*this);
    }

 private:
    Error process(bool&) override;
    Error process(uint64_t&) override;
 private:
    std::ostream& out_;
};

class Deserializer : public ISerializer {
 public:
    explicit Deserializer(std::istream& in)
        : in_(in) {}

    template<typename T>
    Error load(T& obj) {
       return obj.deserialize(*this);
    }

 private:
    Error process(bool&) override;
    Error process(uint64_t&) override;
 private:
    std::istream& in_;
};

#endif  // SERIALIZE_H
