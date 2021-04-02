#include <exception>
#include <algorithm>
#include <utility>

#include "matrix.h"

Matrix::Matrix(size_t nrows, size_t ncols)
    : nrows_(nrows)
    , ncols_(ncols)
    , data_(std::make_unique<int32_t[]>(nrows_ * ncols_)) {}

Matrix::Matrix(const Matrix& other)
    : nrows_(other.nrows_)
    , ncols_(other.ncols_)
    , data_(std::make_unique<int32_t[]>(nrows_ * ncols_)) {
    std::copy(other.data_.get(), other.data_.get() + size(), data_.get());
}

Matrix& Matrix::operator=(const Matrix& other) {
    if (other.size() <= size()) {
        std::copy(other.data_.get(), other.data_.get() + other.size(), data_.get());
        nrows_ = other.nrows_;
        ncols_ = other.ncols_;
    } else {
        Matrix tmp(other);
        std::swap(tmp.nrows_, nrows_);
        std::swap(tmp.ncols_, ncols_);
        std::swap(tmp.data_, data_);
    }
    return *this;
}

Matrix::Matrix(Matrix&& other)
    : nrows_(other.nrows_)
    , ncols_(other.ncols_)
    , data_(std::move(other.data_)) {
    other.ncols_ = other.nrows_ = 0ul;
    other.data_ = nullptr;
}

Matrix& Matrix::operator=(Matrix&& other) {
    nrows_ = std::exchange(other.nrows_, 0ul);
    ncols_ = std::exchange(other.ncols_, 0ul);
    data_  = std::exchange(other.data_, nullptr);
    return *this;
}

int32_t  Matrix::Proxy::operator[](size_t j) const {
    matrix.is_indices_valid(i, j);
    return matrix.data_[i * matrix.ncols_ + j];
}
int32_t& Matrix::Proxy::operator[](size_t j) {
    matrix.is_indices_valid(i, j);
    return matrix.data_[i * matrix.ncols_ + j];
}

const Matrix::Proxy  Matrix::operator[](size_t i) const {
    return {*this, i};
}
Matrix::Proxy  Matrix::operator[](size_t i) {
    return {*this, i};
}

int32_t& Matrix::at(size_t i, size_t j) {
    is_indices_valid(i, j);
    return data_[i * ncols_ + j];
}
int32_t  Matrix::at(size_t i, size_t j) const {
    is_indices_valid(i, j);
    return data_[i * ncols_ + j];
}

Matrix Matrix::operator+(const Matrix& other) const {
    if (std::tie(ncols_, nrows_) != std::tie(other.ncols_, other.nrows_))
        throw std::logic_error("Different dimensions");
    Matrix res(nrows_, ncols_);
    for (size_t i = 0; i < nrows_ * ncols_; i++)
        res.data_[i] = data_[i] + other.data_[i];
    return res;
}

Matrix& Matrix::operator*=(int32_t alpha) {
    std::for_each_n(data_.get(), size(), [&](int32_t& el) { el *= alpha; });
    return *this;
}

bool Matrix::operator==(const Matrix& other) const {
    return std::tie(ncols_, nrows_) == std::tie(other.ncols_, other.nrows_) &&
           std::equal(data_.get(), data_.get() + size(), other.data_.get());
}
bool Matrix::operator!=(const Matrix& other) const {
    return !(*this == other);
}

size_t Matrix::size()  const { return nrows_ * ncols_; }
size_t Matrix::nrows() const { return nrows_; }
size_t Matrix::ncols() const { return ncols_; }

void Matrix::is_indices_valid(size_t i, size_t j) const {
    if (i >= nrows_ || j >= ncols_)
        throw std::out_of_range("Bad indices");
}

std::ostream& operator<<(std::ostream& os, const Matrix& matr) {
    for (size_t i = 0; i < matr.nrows(); ++i)
        for (size_t j = 0; j <  matr.ncols(); ++j)
            os << matr[i][j] << " \n"[j == matr.ncols() - 1];
    return os;
}

std::istream& operator>>(std::istream& is, Matrix& m) {
    Matrix tmp(m.nrows(), m.ncols());
    for (size_t i = 0; i < tmp.nrows(); ++i)
        for (size_t j = 0; j <  tmp.ncols(); ++j)
            is >> tmp[i][j];
    if (!is)
        throw std::runtime_error("Not enough elems!");
    m = std::move(tmp);
    return is;
}
