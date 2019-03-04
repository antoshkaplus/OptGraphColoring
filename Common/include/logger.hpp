#pragma once

namespace report {

    class Logger {

        std::mutex m;
        ofstream out;

    public:
        Logger(const std::string& path) : out(path) {}

        template <typename ...Args>
        void Log(Args&&... args) {
            std::lock_guard<std::mutex> guard(m);
            ant::Println(out, std::forward<Args>(args)...);
        }
    };
}