#ifndef PARSER_H
#define PARSER_H

#include <string_view>
#include <functional>

class Parser {
 public:
    using DefaultCallBack = std::function<void(void)>;
    using StringCallBack  = std::function<void(std::string_view)>;
    using NumberCallBack  = std::function<void(uint64_t)>;

 public:
    void set_start_callback(DefaultCallBack startCB);
    void set_number_callback(NumberCallBack numberCB);
    void set_string_callback(StringCallBack stringCB);
    void set_end_callback(DefaultCallBack endCB);

    void run(std::string_view strv_to_parse) const;

 private:
    DefaultCallBack _startCB = []{};
    NumberCallBack  _numCB   = [](uint64_t){};
    StringCallBack  _strCB   = [](std::string_view){};
    DefaultCallBack _endCB   = []{};
};

#endif  // PARSER_H
