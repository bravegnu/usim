#ifndef UTILS_H
#define UTILS_H

#include <stdexcept>
#include <cstring>
#include <string>

class IOError: public std::runtime_error {
public:
  IOError(int err, std::string message): std::runtime_error(message + strerror(err)) {}
};

#endif
