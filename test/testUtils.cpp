#include <utils.hpp>

bool checkPids(std::vector<std::string>& results) {
    results = getPIDs();
    bool capturedPids = !results.empty();

    std::cerr << std::boolalpha << "[*] PIDS captured? "
    << capturedPids << " | " << results.size() << std::endl;

    return capturedPids && results[0] == "/proc/1";
}

// debugging test function against checkPids()
void readFolder(const char* path = "/proc") {
    DIR* dir = opendir(path);
    if (!dir) {
        std::cerr << "[*] Error reading pids from path" << std::endl;
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        std::cerr << "|___" << name << std::boolalpha << " | Captured: " << isInteger(name) << std::endl;
    }

    closedir(dir);
}

int main() {
    // readFolder();

    if (std::vector<std::string> results; !checkPids(results)) {
        for (const std::string& result : results) {
            std::cerr << "[*] PID Element -> " << result << std::endl;
        }
    }
    return 0;
}