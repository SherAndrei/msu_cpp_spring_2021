#include <sstream>
#include <limits>

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

template<typename Data>
void doCorruptedTests(const std::vector<std::string>& corruptv) {
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
        doCorruptedTests<BooleanData>(std::vector<std::string>({
            "", " ",
            "true", "false",  // no separator
            "true\n", "false\t", "true,",  // wrong separator
            "1 ", "0 ", "qweqr",
            "0true", "true0", "atrue", "truea",
            "TRUE", "FALSE", "trUe", "fAlse"
        }));
    }
}

struct IntegralData {
    uint64_t a;

    template <class Serializer>
    Error serialize(Serializer& serializer) {
        return serializer(a);
    }
    template <class Deserializer>
    Error deserialize(Deserializer& deserializer) {
        return deserializer(a);
    }
    friend bool operator==(IntegralData lhs, IntegralData rhs) {
        return lhs.a == rhs.a;
    }
};

using uint64_lim = std::numeric_limits<uint64_t>;

void testIntegral() {
    doCorrectTest(IntegralData{uint64_lim::min()}, "0 ");
    doCorrectTest(IntegralData{123123}, "123123 ");
    doCorrectTest(IntegralData{uint64_lim::max()}, std::to_string(uint64_lim::max()) + ' ');

    doCorruptedTests<IntegralData>({
        "", " ",
        "0", std::to_string(uint64_lim::max()),  // no separator
        "1,", "2\n", "0\t",  // wrong separator
        "true ", "false ",
        "111111111111111111111111111111111111111111",
        "123a123", "a123123", "123123a", ".1231"
    });
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


void testSimple() {
    doCorrectTest(SimpleData{ 1ull, true }, "1 true ");
    doCorrectTest(SimpleData{ 0ull, false }, "0 false ");
    doCorrectTest(SimpleData{ 100, 1 }, "100 true ");

    doCorruptedTests<SimpleData>({
        "", " ",
        "1true", "1 true",  // no separator
        "1\ttrue\t", "1\nfalse\n", "1;true.",  // wrong separator
        "1 ", "1  ", "1  q true",
        "qwe eqw", "11t rue",
        "0 0", "1 1",
    });
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

void testVarious() {
    doCorrectTest(VariousData{1, 1, 1, 1, 1, 1}, "1 true true 1 true 1 ");
    doCorrectTest(VariousData{0, 0, 0, 0, 0, 0}, "0 false false 0 false 0 ");
    doCorruptedTests<VariousData>({
        "", " ", "1 ", "1 true ", "1 true true ",
        "1 true true 1 ", "1 true true 1 true ",
        "1 1 1 1 1 1 ",
        "0 0 0 0 0 0 ",
        "1truetrue1true1", "1 true true 1 true 1" ,  // no separator
        "1,true,true,1,true,1,", "1\ntrue\ntrue\n1\ntrue\n1\n"  // wrong separator
    });
}

struct IncorrectData {
    bool a;
    std::string s;
    double c;

    template <class Serializer>
    Error serialize(Serializer& serializer) {
        return serializer(a, s, c);
    }
    template <class Deserializer>
    Error deserialize(Deserializer& deserializer) {
        return deserializer(a, s, c);
    }
};

void testIncorrect() {
    {
        std::stringstream ss;
        Serializer s(ss);
        IncorrectData id{true, "sample", 0.2};
        ASSERT(s.save(id) == Error::CorruptedArchive);

        Deserializer ds(ss);
        IncorrectData tmp;
        ASSERT(ds.load(tmp) == Error::CorruptedArchive);
    }
}

}  // namespace

int main() {
    TestRunner tr;
    RUN_TEST(tr, testBoolean);
    RUN_TEST(tr, testIntegral);
    RUN_TEST(tr, testSimple);
    RUN_TEST(tr, testVarious);
    RUN_TEST(tr, testIncorrect);
}
