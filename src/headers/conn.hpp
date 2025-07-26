#ifndef CONN
#define CONN

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

class Connection {
public:
    Connection(std::string hex_addr);

    // getters
    std::string getAddress() const { return connAddr; };
    int getPort() const { return connPort; }

    friend std::ostream& operator<<(std::ostream& out, const Connection& conn);
private:
    std::pair<std::vector<std::string>,std::string> getHexBytes(const std::string hexStr);
    std::pair<std::string,int> fromHex(const std::string hex_addr);

    std::string connAddr;
    int connPort;
};

#endif