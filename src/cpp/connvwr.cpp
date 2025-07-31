#include <connvwr.hpp>

Viewer::Viewer() {
    // Initialize ncurses
    initscr();              // Start curses mode
    cbreak();               // Disable line buffering
    noecho();               // Don't echo user input
    start_color();          // Allow color pairs
    use_default_colors();   // Give access to default colors
    curs_set(0);            // Hide the cursor
    refresh();              // refresh the screen

    // set color pair(s) that can be used later
    init_pair(1, COLOR_RED, -1);

    drawDefaultBox();

    // create reactive windows (update displays, user-input fields, etc)
    ReactiveWindow* connWindow = new ReactiveWindow(termHeight - 4, termWidth/2, 2, 2);
    Component* dText = connWindow->addComponent(new Text(1, 1));
    reactiveWindows.push_back(connWindow);

    ReactiveWindow* menuWindow = new ReactiveWindow(termHeight - 4, termWidth/4, 2, termWidth - (termWidth/3));
    Component* dMenu = menuWindow->addComponent(new Menu({
        new FlagEvent("flag"), // mark an address to assign color coding
        new FlagEvent("unflag", true), // remove a flagged address
        
        new KillEvent("kill"), // attempt to kill connection

        new SaveEvent("save"), // export current connection list to file
        new ExitEvent("exit")  // exit program
    }));
    reactiveWindows.push_back(menuWindow);

    // render loop everything outside of this while loop only gets rendered once
    actionMode mode = actionMode::DEFAULT;
    while (mode != actionMode::EXIT) {
        // dynamically updates tracked Text Component
        collectConnections();
        if (Text* textComp = dynamic_cast<Text*>(dText)) {
            // record the connections for potential exporting
            ConnectionRecorder::getInstance().setContent(connectionStrings());

            // keep record of Connection objects for potential later use
            ConnectionRecorder::getInstance().setConnections(*getConnections());

            // update text component content
            textComp->setLines(connectionStrings()); // Only in Text
        }

        // update any reactive windows
        update(mode);

        // give CPU breathing room
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

// Thread Target for Updating Window Display 
void Viewer::update(actionMode& mode) {
    // update the reactive window and its components
    for (auto& window : reactiveWindows) {
        window->update(mode);
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

void Viewer::collectConnections() {
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
            std::string inodeStr = entryFragments[9];
            
            // no value entry
            if (rem_address == "00000000:0000") continue;
            conns.emplace_back(rem_address, inodeStr);
        }
    }
}

std::vector<Connection>* Viewer::getConnections() {
    return &conns;
}