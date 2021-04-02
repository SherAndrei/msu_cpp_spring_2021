#include <sstream>

#include "test_runner.h"
#include "matrix.h"

void TestConstructor();
void TestAt();
void TestOperatorBrackets();
void TestComparison();
void TestSum();
void TestCoeff();

void TestConstructor() {
    {
        Matrix m(3, 2);
        ASSERT_EQUAL(2u, m.ncols());
        ASSERT_EQUAL(3u, m.nrows());
        ASSERT_EQUAL(6u, m.size());
    }
    {
        Matrix m(100, 0);
        ASSERT_EQUAL(0u, m.ncols());
        ASSERT_EQUAL(100u, m.nrows());
    }
}

namespace {

void DoAssert(const Matrix& m, const std::string& s) {
    std::ostringstream os;
    os << m;
    ASSERT_EQUAL(os.str(), s);
}

}  // namespace

void TestAt() {
    {
        Matrix m(1, 1);
        m.at(0, 0) = 1;
        ASSERT_EQUAL(m.at(0, 0), 1);
    }
    {
        Matrix m(4, 4);
        for (size_t i = 0; i < 4; i++)
            for (size_t j = 0; j < 4; j++)
                m.at(i, j) = i + j;
        DoAssert(m, "0 1 2 3\n1 2 3 4\n2 3 4 5\n3 4 5 6\n");
    }
    {
        Matrix m(1, 5);
        for (size_t i = 0; i < 5; i++)
            m.at(0, i) = i;
        DoAssert(m, "0 1 2 3 4\n");
    }
    {
        Matrix m(5, 1);
        for (size_t i = 0; i < 5; i++)
            m.at(i, 0) = i;
        DoAssert(m, "0\n1\n2\n3\n4\n");
    }
    try {
        Matrix m(1, 1);
        m.at(1, 1) = 1;
        ASSERT(false);
    } catch (std::out_of_range& ex) {
        ASSERT(true);
    }
    try {
        Matrix m(10, 10);
        m.at(5, -1) = 1;
        ASSERT(false);
    } catch (std::out_of_range& ex) {
        ASSERT(true);
    }
    try {
        Matrix m(10, 10);
        m.at(-1, 5) = 1;
        ASSERT(false);
    } catch (std::out_of_range& ex) {
        ASSERT(true);
    }
}

void TestOperatorBrackets() {
    {
        Matrix m(3, 3);
        std::istringstream is("1 2 3\n4 5 6\n7 8 9\n");
        is >> m;
        for (size_t i = 0; i < 3; i++)
            m[1][i] = i;
        DoAssert(m, "1 2 3\n0 1 2\n7 8 9\n");
        for (size_t j = 0; j < 3; j++)
            m[j][j] = 100;
        DoAssert(m, "100 2 3\n0 100 2\n7 8 100\n");
        for (size_t k = 0; k < 3; k++)
            m[k][2] = k;
        DoAssert(m, "100 2 0\n0 100 1\n7 8 2\n");
    }
    try {
        Matrix m(1, 1);
        m[1][1] = 1;
        ASSERT(false);
    } catch(std::out_of_range& ex) {
        ASSERT(true);
    }
    try {
        Matrix m(20, 30);
        m[-1][20] = 1;
        ASSERT(false);
    } catch(std::out_of_range& ex) {
        ASSERT(true);
    }
    try {
        Matrix m(30, 20);
        m[20][-1] = 1;
        ASSERT(false);
    } catch(std::out_of_range& ex) {
        ASSERT(true);
    }
}

void TestComparison() {
    Matrix lhs(2, 2);
    std::istringstream is("1 2\n3 4\n");
    is >> lhs;
    {
        Matrix rhs(1, 4);
        for (size_t i = 0; i < 4; i++)
            rhs[0][i] = i + 1;
        ASSERT(rhs != lhs);
    }
    {
        Matrix rhs(4, 1);
        for (size_t i = 0; i < 4; i++)
            rhs[i][0] = i + 1;
        ASSERT(rhs != lhs);
    }
    {
        Matrix rhs(2, 2);
        rhs[0][0] = 1;
        rhs[0][1] = 2;
        rhs[1][0] = 3;
        rhs[1][1] = 4;
        ASSERT(rhs == lhs);
    }
    {
        Matrix rhs(2, 2);
        rhs[0][0] = 4;
        rhs[0][1] = 3;
        rhs[1][0] = 2;
        rhs[1][1] = 1;
        ASSERT(rhs != lhs);
    }
}

void TestSum() {
    try {
        Matrix a = Matrix(1, 2) + Matrix(2, 1);
        ASSERT(false);
    } catch(std::logic_error& e) {
        ASSERT(true);
    }
    auto init = [](Matrix& m, const std::string& s) {
        std::istringstream is(s);
        is >> m;
    };
    {
        Matrix lhs(2, 2);
        init(lhs, "-1 -2\n -3 -4\n");
        Matrix rhs(2, 2);
        init(rhs, "1 2\n 3 4\n");
        DoAssert(lhs + rhs, "0 0\n0 0\n");
    }
}

void TestCoeff() {
    auto do_init = []() {
        Matrix m(2, 3);
        std::istringstream is("1 2 3\n2 3 4\n");
        is >> m;
        return m;
    };
    {
        Matrix m = do_init();
        m *= 0;
        DoAssert(m, "0 0 0\n0 0 0\n");
    }
    {
        Matrix m = do_init();
        m *= 3;
        DoAssert(m, "3 6 9\n6 9 12\n");
    }
    {
        Matrix m = do_init();
        m *= -1;
        DoAssert(m, "-1 -2 -3\n-2 -3 -4\n");
    }
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestConstructor);
    RUN_TEST(tr, TestAt);
    RUN_TEST(tr, TestOperatorBrackets);
    RUN_TEST(tr, TestComparison);
    RUN_TEST(tr, TestSum);
    RUN_TEST(tr, TestCoeff);
}
