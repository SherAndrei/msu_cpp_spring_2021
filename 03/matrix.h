#ifndef MATRIX_H
#define MATRIX_H

#include <memory>
#include <iostream>

class Matrix {
 public:
    Matrix(size_t nrows, size_t ncols);

    Matrix(const Matrix&);
    Matrix& operator=(const Matrix&);

    Matrix(Matrix&&);
    Matrix& operator=(Matrix&&);

 public:
    struct Proxy {
        const Matrix& matrix;
        size_t i;

        int32_t  operator[](size_t j) const;
        int32_t& operator[](size_t j);
    };

    const Proxy operator[](size_t i) const;
    Proxy operator[](size_t i);

    int32_t  at(size_t i, size_t j) const;
    int32_t& at(size_t i, size_t j);

 public:
    Matrix  operator+(const Matrix& other) const;
    Matrix& operator*=(int32_t alpha);

    bool operator==(const Matrix& other) const;
    bool operator!=(const Matrix& other) const;

 public:
    size_t size()  const;
    size_t nrows() const;
    size_t ncols() const;

 private:
    void is_indices_valid(size_t i, size_t j) const;

 private:
    size_t nrows_ = 0ul;
    size_t ncols_ = 0ul;
    std::unique_ptr<int32_t[]> data_;
};

std::ostream& operator<<(std::ostream& os, const Matrix&);
std::istream& operator>>(std::istream& is, Matrix&);

#endif  // MATRIX_H
