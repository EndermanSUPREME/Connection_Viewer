#include <conn.hpp>

Connection::Connection(std::string hex_addr) {
    std::pair<std::string,int> connInfo = fromHex(hex_addr);
    connAddr = connInfo.first;
    connPort = connInfo.second;
};

std::pair<std::vector<std::string>,std::string> Connection::getHexBytes(const std::string hexStr) {
    std::vector<std::string> ipHexBytes;
    std::string portHexStr;

    std::string hexByteStr;

    // Extract Hex Bytes for IP Address
    int portStrLen = 5;
    int hexAddrLen = 8;

    // calculate the position where the hex address starts (tcp/tcp6)
    int start = hexStr.length() - portStrLen - hexAddrLen;
    int index = 0;
    
    // extract hex bytes into a list
    while (index+1 < 8) {
        hexByteStr = "";

        hexByteStr += hexStr[start + index];
        hexByteStr += hexStr[start + index+1];
        ipHexBytes.push_back(hexByteStr);

        index+=2;
    }
    
    // Extract Hex Bytes for Address Port
    start = hexStr.length() - portStrLen + 1;
    portHexStr = hexStr.substr(start);

    return std::make_pair(ipHexBytes, portHexStr);
};

std::pair<std::string,int> Connection::fromHex(const std::string hex_addr) {
    std::string ipAddr;
    int port;

    std::pair<std::vector<std::string>,std::string> hexBytes = getHexBytes(hex_addr);
    std::vector<std::string> ipHexBytes = hexBytes.first;
    std::string portHexBytes = hexBytes.second;

    // hex for port does not need operations to convert from:
    // convert the hex string into decimal value
    port = std::stoi(portHexBytes, nullptr, 16);
    
    // hex bytes are in little endian so we need to
    // traverse backwords so we do not write the
    // address as| 1.0.0.127

    for (auto ritr = ipHexBytes.rbegin(); ritr != ipHexBytes.rend(); ++ritr) {
        std::string hexByte = *ritr;
        // convert the hex byte into decimal value
        int decValue = std::stoi(hexByte, nullptr, 16);
        
        // concat values to create dotted ip address
        ipAddr += std::to_string(decValue);
        if (ritr != ipHexBytes.rend()-1) {
            ipAddr += ".";
        }
    }

    return std::make_pair(ipAddr, port);
};

std::string Connection::ToString() const {
    return getAddress() + ":" + std::to_string(getPort());
}

std::ostream& operator<<(std::ostream& out, const Connection& conn) {
    out << "Connection | " << conn.getAddress() << ":" << conn.getPort() << std::endl;
    return out;    
};