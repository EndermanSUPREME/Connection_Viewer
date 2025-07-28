#include <connvwr.hpp>

Viewer::Viewer() {
    // Initialize ncurses
    initscr();              // Start curses mode
    cbreak();               // Disable line buffering
    noecho();               // Don't echo user input
    curs_set(0);            // Hide the cursor
    refresh();              // refresh the screen

    // updateThread = std::thread(&Viewer::Update, this);

    drawDefaultBox();

    // std::shared_ptr<WINDOW*> connBox = DrawBox(termHeight - 4, termWidth/2, 2, 2);
    // DrawLines(connBox, 1, 1, connectionStrings());

    ReactiveWindow* connBox = new ReactiveWindow(termHeight - 4, termWidth/2, 2, 2);
    Component* comp = connBox->addComponent(new Text(1, 1));
    reactiveWindows.push_back(connBox);

    std::shared_ptr<WINDOW*> menuBox = drawBox(termHeight - 4, termWidth/4, 2, termWidth - (termWidth/3));

    // render loop everything outside of this while loop only gets rendered once
    while (true) {
        // dynamically updates tracked Text Component
        getConnections();
        if (Text* textComp = dynamic_cast<Text*>(comp)) {
            textComp->setLines(connectionStrings()); // Only in Text
        }

        // update any reactive windows
        update();

        int status = drawMenu(menuBox, {
            "flag", // mark an address to assign color coding
            "kill", // attempt to kill connection
            "view", // attempt to view connection
            "save", // export current connection list to file
            "exit"  // exit program
        });
        if (status == -1) break;

        // give CPU breathing room
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

// Thread Target for Updating Window Display 
void Viewer::update() {
    // update the reactive window and its components
    for (auto& window : reactiveWindows) {
        window->update();
    }
}

Viewer::~Viewer() {
    // ensure thread safely closes
    // updateThread.join();

    // deallocate reactive windows
    for (auto& window : reactiveWindows) {
        delete window;
    }

    // deallocate all windows created
    for (auto& window : windows) {
        delwin(*window);
    }
    endwin();  // End curses mode
}

// Draw a box that encompasses the entire terminal
void Viewer::drawDefaultBox() {
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
std::shared_ptr<WINDOW*> Viewer::drawBox(int height, int width, int y, int x) {
    // Create window with custom dimensions
    WINDOW* win = newwin(height, width, y, x);
    
    // track windows created
    windows.push_back(std::make_shared<WINDOW*>(win));

    // Draw border and add label
    box(win, 0, 0);

    // Refresh to draw
    wrefresh(win);

    // refresh the screen
    refresh();

    return windows.back();
}

// Draw interactable menu inside a given parent window and list of choices
int Viewer::drawMenu(std::shared_ptr<WINDOW*>& parentWindow, std::vector<std::string> options) {
    if (!parentWindow) return -1;
    
    int parentWidth, parentHeight;
    getmaxyx(*parentWindow, parentHeight, parentWidth);
    keypad(*parentWindow, TRUE);   // Enable function & arrow keys
    
    bool submit = false;
    bool quitMenu = false;

    // maintain state between function invokations
    static size_t choice = 0;
    static size_t highlight = 0;

    for (size_t i = 0; i < options.size(); ++i) {
        if (i == highlight) {
            wattron(*parentWindow, A_REVERSE);
        }

        mvwprintw(*parentWindow, i+1, 1, options[i].c_str());
        wattroff(*parentWindow, A_REVERSE);
    }

    submit = false;

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
        if (highlight+1 == options.size()) {
            return -1;
        }

        // idle
        return 0;
    } else {
        // idle
        return 0;
    }
}

// Write lines to parent window from top-down
void Viewer::drawLines(std::shared_ptr<WINDOW*>& parentWindow, int startRow, int column, std::vector<std::string> lines) {
    if (!parentWindow) return;
    int currentLine = startRow;

    for (const std::string& line : lines) {
        mvwprintw(*parentWindow, currentLine++, column, line.c_str());
        // Refresh to draw
        wrefresh(*parentWindow);
    }
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

std::vector<std::string> Viewer::connectionStrings() {
    std::vector<std::string> connList;
    for (const Connection& conn : conns) {
        connList.push_back(conn.ToString());
    }
    return connList;
}

void Viewer::getConnections() {
    std::vector<std::string> tcpEntries = readTcpConnections();
    std::vector<std::string> tcp6Entries = readTcp6Connections();
    conns.clear();

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