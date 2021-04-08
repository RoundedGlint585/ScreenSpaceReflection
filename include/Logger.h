//
// Created by roundedglint585 on 2/14/21.
//

#ifndef PROJECT_LOGGER_H
#define PROJECT_LOGGER_H

#include <iostream>
// iostream defines ERROR
#undef ERROR

namespace logger {
    template <typename T>
    static void inline INFO(const T& message) {
        std::cout << "\033[32m" << "INFO: " << message << "\033[0m" << std::endl;
    }

    template<typename T>
    static void inline WARNING(const T& message) {
        std::cout << "\033[31m" << "WARNING: " << message << "\033[0m" << std::endl;
    }

    template<typename T>
    static void inline ERROR(const T& message) {
        std::cout << "\033[31m" << "ERROR: " << message << "\033[0m" << std::endl;
    }
}
#endif //PROJECT_LOGGER_H
