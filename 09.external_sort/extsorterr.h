#ifndef EXTERNAL_SORT_ERROR_H
#define EXTERNAL_SORT_ERROR_H

#include <stdexcept>

struct ExternalSortError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct FileError : ExternalSortError {
    using ExternalSortError::ExternalSortError;
};


#endif  // EXTERNAL_SORT_ERROR_H
