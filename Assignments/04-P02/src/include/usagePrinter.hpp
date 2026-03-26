/*
 * The class UsagePrinter prints help blocks with color.
 */

#include "termcolor.hpp"
#include <algorithm>
#include <iomanip> // where setw is declared
#include <iostream>
#include <string>
#include <vector>

struct Option {
    std::string flag;
    std::string arg;
    std::string desc;
};

class UsagePrinter {
private:
    std::vector<Option> options;
    std::vector<std::string> examples;

public:
    void add(std::string flag, std::string arg, std::string desc) {
        options.push_back({flag, arg, desc});
    }

    void addExample(std::string ex) {
        examples.push_back(ex);
    }

    void print(const std::string &programName) {

        using namespace termcolor;

        std::cout << green << bold << "Usage:\n"
                  << reset;
        std::cout << "  " << bold << programName << reset
                  << " " << cyan << "[OPTIONS]\n\n"
                  << reset;

        std::cout << green << bold << "Options:\n"
                  << reset;

        size_t flagWidth = 0;
        size_t argWidth = 0;

        for (auto &o : options) {
            flagWidth = std::max(flagWidth, o.flag.size());
            argWidth = std::max(argWidth, o.arg.size());
        }

        for (auto &o : options) {

            std::cout << "  "
                      << cyan << std::left
                      << std::setw(flagWidth) << o.flag
                      << reset << " ";

            if (!o.arg.empty()) {
                std::cout << yellow
                          << std::setw(argWidth) << o.arg
                          << reset;
            } else {
                std::cout << std::setw(argWidth) << " ";
            }

            std::cout << "   " << o.desc << "\n";
        }
        std::cout << std::endl
                  << green << bold << "Examples:" << reset << std::endl;
        for (auto &ex : examples) {
            std::cout << yellow << ex << reset << std::endl;
        }

        std::cout << "\n";
    }
};