#include <sstream>

#include "test_runner.h"
#include "serialize.h"

namespace {

template<typename Data>
void doCorrectTest(Data&& data, const std::string& s) {
    std::stringstream stream;
    Serializer serializer(stream);
    ASSERT(serializer.save(data) == Error::NoError);
    ASSERT_EQUAL(stream.str(), s);
    Data tmp;
    Deserializer deserializer(stream);
    ASSERT(deserializer.load(tmp) == Error::NoError);
    ASSERT(data == tmp);
}

struct SimpleData {
    uint64_t a;
    bool b;

    template <class Serializer>
    Error serialize(Serializer& serializer) {
        return serializer(a, b);
    }
    template <class Deserializer>
    Error deserialize(Deserializer& deserializer) {
        return deserializer(a, b);
    }
    friend bool operator==(SimpleData lhs, SimpleData rhs) {
        return std::tie(lhs.a, lhs.b) == std::tie(rhs.a, rhs.b);
    }
};


void testSimpleData() {
    doCorrectTest(SimpleData{ 1ull, true }, "1 true ");
    doCorrectTest(SimpleData{ 0ull, false }, "0 false ");
    doCorrectTest(SimpleData{ 100, 1 }, "100 true ");
}

struct VariousData {
    uint64_t a;
    bool b;
    bool c;
    uint64_t d;
    bool e;
    uint64_t f;

    template <class Serializer>
    Error serialize(Serializer& serializer) {
        return serializer(a, b, c, d, e, f);
    }
    template <class Deserializer>
    Error deserialize(Deserializer& deserializer) {
        return deserializer(a, b, c, d, e, f);
    }
    friend bool operator==(VariousData lhs, VariousData rhs) {
        return std::tie(lhs.a, lhs.b, lhs.c,
                        lhs.d, lhs.e, lhs.f) ==
               std::tie(rhs.a, rhs.b, rhs.c,
                        rhs.d, rhs.e, rhs.f);
    }
};

void testVariousData() {
    doCorrectTest(VariousData{1, 1, 1, 1, 1, 1}, "1 true true 1 true 1 ");
    doCorrectTest(VariousData{0, 0, 0, 0, 0, 0}, "0 false false 0 false 0 ");
}

struct IncorrectData {
    uint64_t a;
    bool b;
    double c;

    template <class Serializer>
    Error serialize(Serializer& serializer) {
        return serializer(a, b, c);
    }
    template <class Deserializer>
    Error deserialize(Deserializer& deserializer) {
        return deserializer(a, b, c);
    }
};

template<typename Data>
void doCorruptedTest(const std::vector<std::string>& corruptv) {
    for (auto&& corrupted_data : corruptv) {
        std::stringstream ss(corrupted_data);
        Deserializer ds(ss);
        Data sd;
        ASSERT(ds.load(sd) == Error::CorruptedArchive);
    }
}

struct BooleanData {
    bool a;

    template <class Serializer>
    Error serialize(Serializer& serializer) {
        return serializer(a);
    }
    template <class Deserializer>
    Error deserialize(Deserializer& deserializer) {
        return deserializer(a);
    }
    friend bool operator==(BooleanData lhs, BooleanData rhs) {
        return lhs.a == rhs.a;
    }
};

void testBoolean() {
    {
        doCorrectTest(BooleanData{true}, "true ");
        doCorrectTest(BooleanData{false}, "false ");
    }
    {
        doCorruptedTest<BooleanData>(std::vector<std::string>({
            "", " ",
            "true", "false",  // no separator
            "1 ", "0 ", "qweqr",
            "0true", "true0", "atrue", "truea",
            "TRUE", "FALSE", "trUe", "fAlse"
        }));
    }
}
void testCorruptedArchive() {
    for (auto&& corrupted_data : {"", "1", "true", "true 1",
            "1 1 1 1 1", "111111111111111111111111111111111 true",
            "1 1", "1 0", "qwe ewq", "100 TRUE", "10a0 true"}) {
        std::stringstream ss(corrupted_data);
        Deserializer ds(ss);
        SimpleData sd;
        ASSERT(ds.load(sd) == Error::CorruptedArchive);
    }
    std::stringstream ss;
    Serializer s(ss);
    IncorrectData id{1, true, 0.2};
    ASSERT(s.save(id) == Error::CorruptedArchive);

    Deserializer ds(ss);
    IncorrectData tmp;
    ASSERT(ds.load(tmp) == Error::CorruptedArchive);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, testSimpleData);
    RUN_TEST(tr, testVariousData);
    RUN_TEST(tr, testBoolean);
    RUN_TEST(tr, testCorruptedArchive);
}
