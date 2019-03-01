#pragma once

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
            p = prev;
            p += "__" + std::to_string(index);
        }
        return p;
    }

}
//
//time_t now = std::time(0);
//char buffer[64];
//std::strftime(buffer, sizeof(buffer), "__%Y_%m_%d_%H_%M", localtime(&now));
//std::string analysis_dir = pp + buffer + "%s";

// have to increament path

//std::experimental::filesystem::path analysis_path{result_dir};
//for (;;) {
//auto prev = analysis_path;
//analysis_path /= analysis_dir
//}