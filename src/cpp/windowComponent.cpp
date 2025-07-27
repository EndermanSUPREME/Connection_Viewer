#include <windowComponent.hpp>

void Text::update() {
    if (!window) return;

    int currentLine = row;

    int width = getmaxx(*window);

    // write content
    for (const std::string& line : lines) {
        mvwprintw(*window, currentLine, col, std::string(width - col - 1, ' ').c_str());
        mvwprintw(*window, currentLine++, col, line.c_str());
    }

    // refresh window after writing all content
    wrefresh(*window);
    refresh();
}