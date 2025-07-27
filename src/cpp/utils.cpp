#include <utils.hpp>

std::vector<std::string> splitLine(const std::string line, char delimiter) {
    std::vector<std::string> results;
    std::string fragment;

    for (const char c : line) {
        if (c != delimiter) {
            fragment += c;
        } else {
            if (!fragment.empty()) results.push_back(fragment);
            fragment.clear();
        }
    }

    // ensure we get the final entry if one exists
    if (!fragment.empty()) results.push_back(fragment);

    return results;
}

std::string get_line(std::ifstream& file) {
    std::string line;
    char c;

    while (!file.eof() && file.get(c)) {
        // read line char by char
        if (c != '\n') {
            line += c;
        } else {
            // exit loop on newline
            break;
        }
    }

    return line;
}