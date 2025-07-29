#ifndef CONN_VIEWER
#define CONN_VIEWER

#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <memory>

#include "conn.hpp"
#include "utils.hpp"
#include "reactiveWindow.hpp"
#include "menuEvent.hpp"

class Viewer {
public:
    Viewer();
    ~Viewer();
private:
    void getConnections();
    std::vector<std::string> readTcpConnections();
    std::vector<std::string> readTcp6Connections();
    std::vector<std::string> connectionStrings();

    void update(actionMode& mode);

    void drawDefaultBox();
    int termWidth;
    int termHeight;
    
    std::vector<Connection> conns;
    std::vector<std::shared_ptr<WINDOW*>> windows;
    // list of windows that will undergo updates at runtime
    // on a separate thread
    std::vector<ReactiveWindow*> reactiveWindows;
};

#endif