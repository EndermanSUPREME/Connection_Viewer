#include <utils.hpp>
#include <cassert>

bool checkPids(std::vector<std::string>& results) {
    results = getPIDs();
    bool capturedPids = !results.empty();

    std::cerr << std::boolalpha << " |___ PIDS captured? "
    << capturedPids << " .... Count " << results.size() << std::endl;

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

    std::cout << "[*] Testing getPIDs. . ." << std::endl;

    std::vector<std::string> results;
    bool foundPIDs = checkPids(results);

    if (!foundPIDs) {
        for (const std::string& result : results) {
            std::cerr << "[*] PID Element -> " << result << std::endl;
        }
    }
    assert(foundPIDs);

    std::cout << "[*] Testing matchingInode. . ." << std::endl;
    std::cerr << " |___ First PID Path: " << results[0] << std::endl;
    std::cerr << " |___ substr results : " << results[0].substr(results[0].find_last_of('/')+1) << std::endl;

    bool foundLink = false;
    for (const std::string& pidPath : results) {
        int pid = std::stoi(pidPath.substr(pidPath.find_last_of('/')+1));

        std::cerr << " |_______ Extracted PID: " << pid << std::endl;

        foundLink = matchingInode(pidPath + "/fd", "/dev/null");
        if (foundLink) break;
    }
    assert(foundLink);

    std::cout << "[*] Testing int2pid. . ." << std::endl;
    unsigned int num = 1234;
    pid_t pid = int2pid(num);
    std::cerr << " |___PID:" << pid << std::endl;
    assert(pid == num);

    std::cout << "[+] Tests Passed!" << std::endl;

    return 0;
}