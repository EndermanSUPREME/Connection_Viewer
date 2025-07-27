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

class Viewer {
public:
    Viewer();
    ~Viewer();
private:
    void getConnections();
    std::vector<std::string> readTcpConnections();
    std::vector<std::string> readTcp6Connections();

    void Update();

    void DrawDefaultBox();
    std::shared_ptr<WINDOW*> DrawBox(int height, int width, int y=0, int x=0);

    void DrawMenu(std::shared_ptr<WINDOW*>& parentWindow, const std::vector<std::string> options);
    void DrawLines(std::shared_ptr<WINDOW*>& parentWindow, int startRow, int column, const std::vector<std::string> lines);
    
    int termWidth;
    int termHeight;
    bool isDestroyed;
    std::vector<Connection> conns;
    std::vector<std::shared_ptr<WINDOW*>> windows;
    // list of windows that will undergo updates at runtime
    // on a separate thread
    std::vector<std::shared_ptr<WINDOW*>> reactiveWindows;
};

#endif