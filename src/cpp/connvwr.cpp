#include <connvwr.hpp>

Viewer::Viewer() {
    // Initialize ncurses
    initscr();              // Start curses mode
    cbreak();               // Disable line buffering
    noecho();               // Don't echo user input
    curs_set(0);            // Hide the cursor

    DrawDefaultBox();

    std::shared_ptr<WINDOW*> connBox = DrawBox(termHeight - 4, termWidth/2, 2, 2);
    DrawLines(connBox, 1, 1, {
        "Line 1",
        "Line 2",
        "Line 3",
        "Line 4"
    });

    std::shared_ptr<WINDOW*> menuBox = DrawBox(termHeight - 4, termWidth/4, 2, termWidth - (termWidth/3));
    DrawMenu(menuBox, {
        "option A",
        "option B",
        "option C",
        "option D"
    });
}

Viewer::~Viewer() {
    // deallocate all windows created
    for (auto& window : windows) {
        delwin(*window);
    }
    endwin();  // End curses mode
}

// Draw a box that encompasses the entire terminal
void Viewer::DrawDefaultBox() {
    // draw a box that covers the full screen
    int height, width;
    getmaxyx(stdscr, height, width);

    // mark terminal specs
    termWidth = width;
    termHeight = height;
    
    // draw box that encompasses the full terminal
    box(stdscr, 0, 0);
    // move cursor down one draw and over 2 cols
    std::string title = "    CONNECTION VIEWER";
    title += "|(";
    title += std::to_string(termWidth);
    title += ":";
    title += std::to_string(termHeight);
    title += ")";
    mvprintw(1, 2, title.c_str());
    refresh();
}

// Draw a box on screen with given height, width, row, column
std::shared_ptr<WINDOW*> Viewer::DrawBox(int height, int width, int y, int x) {
    // Create window with custom dimensions
    WINDOW* win = newwin(height, width, y, x);
    
    // track windows created
    windows.push_back(std::make_shared<WINDOW*>(win));

    // refresh the screen
    refresh();

    // Draw border and add label
    box(win, 0, 0);

    // Refresh to draw
    wrefresh(win);

    return windows.back();
}

// Draw interactable menu inside a given parent window and list of choices
void Viewer::DrawMenu(std::shared_ptr<WINDOW*>& parentWindow, std::vector<std::string> options) {
    if (!parentWindow) return;
    
    int parentWidth, parentHeight;
    getmaxyx(*parentWindow, parentHeight, parentWidth);
    keypad(*parentWindow, TRUE);   // Enable function & arrow keys
    
    bool quit = false;
    size_t choice = 0;
    size_t highlight = 0;

    while (!quit) {
        for (size_t i = 0; i < options.size(); ++i) {
            if (i == highlight) {
                wattron(*parentWindow, A_REVERSE);
            }

            mvwprintw(*parentWindow, i+1, 1, options[i].c_str());
            wattroff(*parentWindow, A_REVERSE);
        }

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
            case 10: // Enter Key
                quit = true;
            break;
            
            default:
            break;
        }
    }
}

// Write lines to parent window from top-down
void Viewer::DrawLines(std::shared_ptr<WINDOW*>& parentWindow, int startRow, int column, std::vector<std::string> lines) {
    if (!parentWindow) return;
    int currentLine = startRow;

    for (const std::string& line : lines) {
        mvwprintw(*parentWindow, currentLine++, column, line.c_str());
        // Refresh to draw
        wrefresh(*parentWindow);
    }
}

// Thread Target for Updating Window Display 
void Viewer::Update() {
    while (!isDestroyed) {
        for (auto& window : reactiveWindows) {
            // refresh the window (update whats displayed)
            wrefresh(*window);
        }
    }
    // clear vector
    reactiveWindows.clear();
}

std::vector<std::string> Viewer::readTcpConnections() {
    std::ifstream tcpFile("/proc/net/tcp");
    std::vector<std::string> lines;
    
    if (!tcpFile) {
        std::cout << "[-] Error: Unable to read tcp file!" << std::endl;
        return lines;
    }

    while (!tcpFile.eof()) {
        lines.push_back(get_line(tcpFile));
    }

    // remove the first line of the file as its just col markers
    if (lines.size() > 0) lines.erase(lines.begin());
    
    return lines;
}

std::vector<std::string> Viewer::readTcp6Connections() {
    std::ifstream tcp6File("/proc/net/tcp6");
    std::vector<std::string> lines;

    if (!tcp6File) {
        std::cout << "[-] Error: Unable to read tcp6 file!" << std::endl;
        return lines;
    }

    while (!tcp6File.eof()) {
        lines.push_back(get_line(tcp6File));
    }

    // remove the first line of the file as its just col markers
    if (lines.size() > 0) lines.erase(lines.begin());
    
    return lines;
}

void Viewer::getConnections() {
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
            conns.emplace_back(rem_address);
        }
    }
}