#include <connvwr.hpp>

std::vector<std::string> splitLine(const std::string line, char delimiter = ' ') {
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

std::vector<std::string> readTcpConnections() {
    std::ifstream tcpFile("/proc/net/tcp");
    std::vector<std::string> lines;
    
    if (!tcpFile) {
        std::cout << "[-] Error: Unable to read tcp file!" << std::endl;
        return lines;
    }

    // std::cout << "################################################################################" << std::endl;
    // std::cout << "|___ TCP" << std::endl;
    while (!tcpFile.eof()) {
        // std::cout << get_line(tcpFile) << std::endl;
        lines.push_back(get_line(tcpFile));
    }
    // std::cout << "################################################################################" << std::endl;

    // remove the first line of the file as its just col markers
    if (lines.size() > 0) lines.erase(lines.begin());
    
    return lines;
}

std::vector<std::string> readTcp6Connections() {
    std::ifstream tcp6File("/proc/net/tcp6");
    std::vector<std::string> lines;

    if (!tcp6File) {
        std::cout << "[-] Error: Unable to read tcp6 file!" << std::endl;
        return lines;
    }

    // std::cout << "################################################################################" << std::endl;
    // std::cout << "|___ TCP 6" << std::endl;
    while (!tcp6File.eof()) {
        // std::cout << get_line(tcp6File) << std::endl;
        lines.push_back(get_line(tcp6File));
    }
    // std::cout << "################################################################################" << std::endl;
    
    // remove the first line of the file as its just col markers
    if (lines.size() > 0) lines.erase(lines.begin());
    
    return lines;
}

void getConnections() {
    std::vector<std::string> tcpEntries = readTcpConnections();
    std::vector<std::string> tcp6Entries = readTcp6Connections();

    // pull rem_address from each entry
    for (const std::string entry : tcpEntries) {
        // check if we can index the vector safely
        std::vector<std::string> entryFragments = splitLine(entry);
 
        if (entryFragments.size() > 2) {
            // rem_address | hex_address:hex_port
            std::string rem_address = entryFragments[2];
            
            // no value entry
            if (rem_address == "00000000:0000") continue;

            std::cout << rem_address << std::endl;
            Connection conn(rem_address);
        }
    }
}

int main() {
    getConnections();
}