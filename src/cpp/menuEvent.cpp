#include <menuEvent.hpp>

std::string MenuEvent::ToString() {
    return description;
}
const char* MenuEvent::ToCString() {
    return description.c_str();
}

void ExitEvent::execute() {
    exitState = true;
}

void SaveEvent::execute() {
    std::string savePath = drawTextInput("Save To: %s", 3, 75, 4, 10, "/tmp/connections.txt");
    try {
        std::ofstream outputFile(savePath);
        if (outputFile) {
            for (const std::string& line : *(ConnectionRecorder::getInstance().getContent())) {
                outputFile << line << std::endl;
            }

            // show affordance upon successful exporting
            std::string saveMsg = "saved connections: " + savePath;
            
            int height, width;
            getmaxyx(stdscr, height, width);
            drawPopUp(3, height/2,10, 2,saveMsg);
        } else {
            std::string saveMsg = "Error saving to: " + savePath;
            
            int height, width;
            getmaxyx(stdscr, height, width);
            drawPopUp(3, height/2,10, 2,saveMsg);
        }
    } catch (std::exception error) {
        // error catch pop-up
        int height, width;
        getmaxyx(stdscr, height, width);
        drawPopUp(3, height/2,10, 2,"I/O Exception Error!");
    }
}

void KillEvent::execute() {
    // gather current pids
    std::vector<std::string> pidPaths = getPIDs();
    if (pidPaths.empty()) {
        int height, width;
        getmaxyx(stdscr, height, width);
        std::string msg = pidPaths.front();
        drawPopUp(3, height/2,10, 2,"Error reading pids from /proc");
        return;
    }

    std::string connIndexStr = drawTextInput("Enter line: %s", 3, 75, 4, 10, "0");
    int index = -1;

    // ensure the line is a valid index
    while (!isInteger(connIndexStr, &index) ||
            index < 0 || index >= ConnectionRecorder::getInstance().getConnections()->size()) {
        connIndexStr = drawTextInput("Enter Line: %s", 5, 75, 4, 10, "0");
    }

    std::vector<Connection>& conns = *(ConnectionRecorder::getInstance().getConnections());
    std::string inodeStr = conns[index].getInode();

    int height, width;
    getmaxyx(stdscr, height, width);

    // iterate each pidPath and find a matching socket
    // Reading -> /proc/12/fd/*
    bool killedProc = false;
    for (const std::string& pidPath : pidPaths) {
        // Pull pid from path: /proc/12 resolves to 12
        int pid = std::stoi(pidPath.substr(pidPath.find_last_of('/')+1));
    
        if (matchingInode(pidPath + "/fd", inodeStr)) {
            // attempt killing PID
            killedProc = killProc(pid);
            return;
        }
    }

    // if a kill failed, this is becasue the user running the binary
    // doesnt have sufficient permission to read the PIDs FD path contents
    if (!killedProc) {
        drawPopUp(3, height/2,10, 2,"Kill PID Failed, potentially requires SUDO!");
    }
}

void FlagEvent::execute() {
    std::string lineIndex = drawTextInput("Enter Line: %s", 5, 75, 4, 10, "0");
    int line = -1;

    // ensure the line is a valid index
    while (!isInteger(lineIndex, &line) ||
            line < 0 || line >= ConnectionRecorder::getInstance().getContent()->size()) {
        lineIndex = drawTextInput("Enter Line: %s", 5, 75, 4, 10, "0");
    }

    if (isFlagRemover()) {
        ConnectionFlagger::getInstance().removeEntry(
            ConnectionRecorder::getInstance().getContent()->at(line)
        );
    } else {
        ConnectionFlagger::getInstance().flagEntry(
            ConnectionRecorder::getInstance().getContent()->at(line)
        );
    }
}