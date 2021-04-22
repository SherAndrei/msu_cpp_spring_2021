#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <concepts>
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

template<typename T, typename Serializer>
concept Serializable = requires(T x, Serializer s) {
    { x.serialize(s) } -> std::same_as<Error>;
};

class Serializer : public ISerializer {
    static constexpr char Separator = ' ';
 public:
    explicit Serializer(std::ostream& out)
        : out_(out) {}

    template<typename T> requires Serializable<T, Serializer>
    Error save(T obj) {
        return obj.serialize(*this);
    }

 private:
    Error process(bool& var) override;
    Error process(uint64_t& var) override;
 private:
    std::ostream& out_;
};

template<typename T, typename Deserializer>
concept Deserializable = requires(T x, Deserializer s) {
    { x.serialize(s) } -> std::same_as<Error>;
};

class Deserializer : public ISerializer {
 public:
    explicit Deserializer(std::istream& in)
        : in_(in) {}

    template<typename T> requires Deserializable<T, Deserializer>
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
