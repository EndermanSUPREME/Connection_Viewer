#include <utils.hpp>

bool isInteger(const std::string& input, int* number) {
    try {
        size_t pos;

        if (number != nullptr) {
            // strip optional wrapper and access the reference
            *number = std::stoi(input, &pos);
        } else {
            std::stoi(input, &pos);
        }

        return pos == input.length();
    } catch (std::invalid_argument&) {
        // Not a number at all
        return false;
    }
}

// convert unsigned integer into pid_t
pid_t int2pid(const unsigned int& pid) {
    return static_cast<pid_t>(pid);
}

// Collect list of current PID directories
std::vector<std::string> getPIDs() {
    const char* proc_path = "/proc";
    std::vector<std::string> pidPaths;

    DIR* dir = opendir(proc_path);
    if (!dir) {
        return {"Error reading pids from /proc"};
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (isInteger(name)) {
            std::string pidPath;

            // increment create final pid path str
            pidPath += proc_path;
            pidPath += "/";
            pidPath += name;

            pidPaths.push_back(pidPath);
        }
    }

    closedir(dir);
    return pidPaths;
}

// Check if a given fd_path contains a socket based on given inode value
bool matchingInode(const std::string& fdPath, const unsigned int& inode) {
    DIR* dir = opendir(fdPath.c_str());
    if (!dir) {
        return false;
    }

    bool inodeFound = false;
    struct dirent* entry;
    while (!inodeFound && (entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;

        char target[PATH_MAX];
        ssize_t len = readlink(name.c_str(), target, sizeof(target) - 1);

        if (len == -1) {
            perror("readlink");
            return 1;
        }

        target[len] = '\0';

        if (strncmp(target, "socket:[", 8) == 0) {
        }
    }

    closedir(dir);
    return inodeFound;
}

// attempt to kill process based on pid
bool killProc(const int& pid) {
    return (kill(int2pid(pid), SIGTERM) == 0);
}

std::vector<std::string> splitLine(const std::string line, char delimiter) {
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

// Draw a box on screen with given height, width, row, column
std::shared_ptr<WINDOW*> drawBox(int height, int width, int y, int x) {
    // Create window with custom dimensions
    WINDOW* win = newwin(height, width, y, x);

    // Draw border and add label
    box(win, 0, 0);

    // Refresh to draw
    wrefresh(win);

    // refresh the screen
    refresh();

    return std::make_shared<WINDOW*>(win);
}

// Create a pop up with a given height, row/col position, duration, and content
// pop ups halt current thread for duration (in seconds)
void drawPopUp(int height, int y, int x, int duration, std::string content) {
    refresh();

    int padding = 3;
    std::shared_ptr<WINDOW*> window = drawBox(height, content.length() + (padding * 2), y, x);
    mvprintw(y+1, x+padding, content.c_str());
    
    wrefresh(*window);
    refresh();

    // pop up shows for 3 seconds before disappearing
    std::this_thread::sleep_for(std::chrono::seconds(duration));

    deleteWindow(window);
}

std::string drawTextInput(const char* prompt, int height, int width, int y, int x, std::string defaultValue) {
    std::string inputValue = defaultValue;
    int cursorPosition = inputValue.size();
    std::shared_ptr<WINDOW*> window = drawBox(height,width,y,x);
    curs_set(1); // show cursor
    keypad(*window, TRUE); // allow arrow key inputs

    while (true) {
        werase(*window);
        box(*window, 0, 0);
        mvwprintw(*window, 1, 1, prompt, inputValue.c_str());
        wmove(*window, 1, cursorPosition + std::strlen(prompt) - 1); // move cursor to the end of the recently printed string
        wrefresh(*window);

        int c = wgetch(*window); // needed to accurately read arrow keys

        if (c == 10) break; // Enter
        
        if (c == KEY_LEFT && cursorPosition > 0) {
            cursorPosition--; // move cursor left
        } else if (c == KEY_RIGHT && cursorPosition < (int)inputValue.size()) {
            cursorPosition++; // move cursor right
        } else if (c == KEY_BACKSPACE || c == 127 || c == 8) {
            // delete character
            if (cursorPosition > 0) {
                inputValue.erase(cursorPosition - 1, 1);
                cursorPosition--;
            }
        } else if (isprint(c)) {
            inputValue.insert(cursorPosition, 1, (char)c);
            cursorPosition++;
        }
    }
    curs_set(0); // hide cursor

    // destroy and deallocate window
    deleteWindow(window);
    return inputValue;
}

// Fully Erase the Window and Deallocate it from memory
void deleteWindow(std::shared_ptr<WINDOW *>& window) {
    werase(*window);
    wrefresh(*window);
    delwin(*window);
}

/*
// Draw interactable menu inside a given parent window and list of choices
int drawMenu(std::shared_ptr<WINDOW*>& parentWindow, std::vector<MenuEvent*> options) {
    if (!parentWindow) return -1;
    
    int parentWidth, parentHeight;
    getmaxyx(*parentWindow, parentHeight, parentWidth);
    keypad(*parentWindow, TRUE);   // Enable function & arrow keys
    
    bool submit = false;

    // maintain state between function invokations
    static size_t choice = 0;
    static size_t highlight = 0;

    for (size_t i = 0; i < options.size(); ++i) {
        if (i == highlight) {
            wattron(*parentWindow, A_REVERSE);
        }

        mvwprintw(*parentWindow, i+1, 1, options[i]->ToCString());
        wattroff(*parentWindow, A_REVERSE);
    }

    // disables blocking mode on this particular window
    nodelay(*parentWindow, TRUE);
    choice = wgetch(*parentWindow);

    switch(choice) {
        case KEY_UP:
            highlight--;
            if (highlight == -1) highlight = options.size()-1;
        break;
        case KEY_DOWN:
            highlight++;
            if (highlight == options.size()) highlight = 0;
        break;
        case 10: // ENTER KEY
            submit = true;
        break;
        
        default:
        break;
    }

    // exit option selected
    if (submit) {
        // exit options
        if (highlight+1 == options.size()) {
            return -1;
        }

        // execute event
        options[highlight]->execute();

        // idle
        return 0;
    }

    // idle
    return 0;
}
*/

// Write lines to parent window from top-down
void drawLines(std::shared_ptr<WINDOW*>& parentWindow, int startRow, int column, std::vector<std::string> lines) {
    if (!parentWindow) return;
    int currentLine = startRow;

    for (const std::string& line : lines) {
        mvwprintw(*parentWindow, currentLine++, column, line.c_str());
        // Refresh to draw
        wrefresh(*parentWindow);
    }
}

ConnectionRecorder& ConnectionRecorder::getInstance() {
    static ConnectionRecorder instance;
    return instance;
}

ConnectionFlagger& ConnectionFlagger::getInstance() {
    static ConnectionFlagger instance;
    return instance;
}

// add entry to flag list if entry doesnt exist
void ConnectionFlagger::flagEntry(std::string entry) {
    auto entryExists = std::find(entries.begin(), entries.end(), entry);
    if (entryExists == entries.end()) {
        entries.push_back(entry); // entry : 127.0.0.1:1234
    }
}
// attempt to remove entry if it exists
void ConnectionFlagger::removeEntry(std::string entry) {
    // remove erase vector idiom
    entries.erase(std::remove(entries.begin(), entries.end(), entry), entries.end());
}