#pragma once

#include "logger.hpp"

namespace report {

    std::string EraseEnd(std::string s, const std::string& rm) {
        if (s.size() < rm.size()) return s;
        if (s.substr(s.size() - rm.size()) == rm) {
            s.erase(s.size() - rm.size());
        }
        return s;
    }

    template <Count maxSz = 256>
    std::string FormatNow(const std::string& format) {
        time_t now = std::time(nullptr);
        char buffer[maxSz];
        std::strftime(buffer, sizeof(buffer), format.c_str(), localtime(&now));
        return buffer;
    }

    std::experimental::filesystem::path GetUnique(std::experimental::filesystem::path p) {
        Index index = 1;
        auto prev = p;
        while (std::experimental::filesystem::exists(p)) {
            p = prev.string() + "__" + std::to_string(index);
        }
        return p;
    }

}