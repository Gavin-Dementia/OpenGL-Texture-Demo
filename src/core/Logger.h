#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <ctime>

class Logger {
public:
    static void init(const std::string& file = "log.txt") {
        get().file.open(file, std::ios::out | std::ios::trunc);
    }

    static void shutdown() {
        if (get().file.is_open())
            get().file.close();
    }

    static void info(const std::string& msg) {
        get().write("[INFO] " + msg);
    }

    static void error(const std::string& msg) {
        get().write("[ERROR] " + msg);
    }

private:
    std::ofstream file;

    static Logger& get() {
        static Logger instance;
        return instance;
    }

    void write(const std::string& msg) {
        std::string line = timestamp() + " " + msg + "\n";

        if (file.is_open())
            file << line;

        // fallback
        std::cerr << line;
    }

    std::string timestamp() {
        std::time_t now = std::time(nullptr);
        char buf[64];
        std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&now));
        return buf;
    }
};

