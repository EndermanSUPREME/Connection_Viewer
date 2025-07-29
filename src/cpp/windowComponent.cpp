#include <windowComponent.hpp>

void Text::update(actionMode& mode) {
    if (!window) return;

    int currentLine = row;

    int width = getmaxx(*window);

    // write content
    for (size_t i = 0; i < lines.size(); ++i) {
        std::string line = lines[i];

        // clears the current line
        mvwprintw(*window, currentLine, col, std::string(width - col - 1, ' ').c_str());

        std::vector<std::string>& flaggedConns = *(ConnectionFlagger::getInstance().getFlaggedEntries());
        bool isFlagged = std::find(flaggedConns.begin(), flaggedConns.end(), line) != flaggedConns.end();

        // modify final line
        line = std::to_string(i) + ". " + "Connection | " + line;

        // have flagged connections be displayed in color
        if (isFlagged) {
            wattron(*window, COLOR_PAIR(1));
        } else {
            // if line was colored reset the colors
            wattroff(*window, A_ATTRIBUTES);
        }

        // writes line
        mvwprintw(*window, currentLine++, col, line.c_str());

        if (isFlagged) {
            wattroff(*window, COLOR_PAIR(1));
        }
    }

    // refresh window after writing all content
    wrefresh(*window);
    refresh();
}

Menu::Menu(std::vector<MenuEvent*> events_): events(events_), closeMenu(false) {
    exitEvent = nullptr;

    // after setting this menus events vector
    // we connect the events to their parent menu
    for (size_t i = 0; i < events.size(); ++i) {
        if (ExitEvent* exitEvt = dynamic_cast<ExitEvent*>(events[i])) {
            // only one ExitEvent should exist per Menu
            assert(exitEvent == nullptr);
            exitEvent = exitEvt;
        }
    }
}

Menu::~Menu() {
    for (size_t i = 0; i < events.size(); ++i) {
        delete events[i];
    }
}

void Menu::update(actionMode& mode) {
    if (!window) return;
    
    int parentWidth, parentHeight;
    getmaxyx(*window, parentHeight, parentWidth);
    keypad(*window, TRUE);   // Enable function & arrow keys
    
    bool submit = false;

    // maintain state between function invokations
    static size_t choice = 0;
    static size_t highlight = 0;

    for (size_t i = 0; i < events.size(); ++i) {
        if (i == highlight) {
            wattron(*window, A_REVERSE);
        }

        mvwprintw(*window, i+1, 1, events[i]->ToCString());
        wattroff(*window, A_REVERSE);
    }

    // disables blocking mode on this particular window
    nodelay(*window, TRUE);
    choice = wgetch(*window);

    switch(choice) {
        case KEY_UP:
            highlight--;
            if (highlight == -1) highlight = events.size()-1;
        break;
        case KEY_DOWN:
            highlight++;
            if (highlight == events.size()) highlight = 0;
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
        if (highlight+1 == events.size()) {
        }

        // execute event
        events[highlight]->execute();

        // check if exit event was triggered
        if (exitEvent != nullptr && exitEvent->getExitState()) {
            mode = actionMode::EXIT;
            return;
        }
    }

    // refresh window after writing all content
    wrefresh(*window);
    refresh();
}