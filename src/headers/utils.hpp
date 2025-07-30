#ifndef UTILS
#define UTILS

#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <sys/types.h>
#include <csignal>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cassert>
#include <utility>
#include <memory>
#include <thread>
#include <algorithm>

enum class actionMode { DEFAULT, FLAG, KILL, VIEW, EXIT };

// check if the given string can convert to a proper integer
// and saves the conversion result into a reference
bool isInteger(const std::string& input, int* number = nullptr);

std::vector<std::string> splitLine(const std::string line, char delimiter = ' ');
std::string get_line(std::ifstream& file);

std::shared_ptr<WINDOW*> drawBox(int height, int width, int y=0, int x=0);
void drawPopUp(int height, int y=0, int x=0, int duration=3, std::string content="");
// int drawMenu(std::shared_ptr<WINDOW*>& parentWindow, const std::vector<MenuEvent*> options);
void drawLines(std::shared_ptr<WINDOW*>& parentWindow, int startRow, int column, const std::vector<std::string> lines);
std::string drawTextInput(const char* prompt, int height, int width, int y=0, int x=0, std::string defaultValue="");

void deleteWindow(std::shared_ptr<WINDOW *>& window);

std::vector<std::string> getPIDs();
bool matchingInode(const std::string& fdPath, const unsigned int& inode);
bool killProc(const int& pid);
pid_t int2pid(const unsigned int& pid);

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

class ConnectionFlagger {
public:
    // thread safe singleton
    static ConnectionFlagger& getInstance();

    void flagEntry(std::string entry);
    void removeEntry(std::string entry);
    std::vector<std::string>* getFlaggedEntries() { return &entries; };
private:
    ConnectionFlagger(){};
    ConnectionFlagger(const ConnectionFlagger&) = delete;
    ConnectionFlagger& operator=(const ConnectionFlagger&) = delete;

    std::vector<std::string> entries;
};



#endif