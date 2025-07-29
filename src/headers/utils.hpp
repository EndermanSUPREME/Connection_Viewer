#ifndef UTILS
#define UTILS

#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <thread>

enum class actionMode { DEFAULT, FLAG, KILL, VIEW, EXIT };

std::vector<std::string> splitLine(const std::string line, char delimiter = ' ');
std::string get_line(std::ifstream& file);

std::shared_ptr<WINDOW*> drawBox(int height, int width, int y=0, int x=0);
void drawPopUp(int height, int y=0, int x=0, int duration=3, std::string content="");
// int drawMenu(std::shared_ptr<WINDOW*>& parentWindow, const std::vector<MenuEvent*> options);
void drawLines(std::shared_ptr<WINDOW*>& parentWindow, int startRow, int column, const std::vector<std::string> lines);
std::string drawTextInput(int height, int width, int y=0, int x=0, std::string defaultValue="");

void deleteWindow(std::shared_ptr<WINDOW *>& window);

// Singleton responsible for recording lines that potentially redirect to output files
class ConnectionRecorder {
public:
    // thread safe singleton
    static ConnectionRecorder& getInstance();
    void setContent(std::vector<std::string> content_={}) { content = content_; };
    std::vector<std::string>* getContent() { return &content; };
private:
    ConnectionRecorder(){};
    ConnectionRecorder(const ConnectionRecorder&) = delete;
    ConnectionRecorder& operator=(const ConnectionRecorder&) = delete;

    std::vector<std::string> content;
};

#endif