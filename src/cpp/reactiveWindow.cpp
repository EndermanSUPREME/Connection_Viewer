#include <reactiveWindow.hpp>

// Create a normal window with given height, width, y, x
ReactiveWindow::ReactiveWindow(int height, int width, int y, int x) {
    if (!window) {
        window = std::make_shared<WINDOW*>(newwin(height, width, y, x));
    }
}

// Deallocate WINDOW* and Components on object destruct
ReactiveWindow::~ReactiveWindow() {
    if (window) {
        delwin(*window);
    };

    // components are allocated via new-keyword
    for (size_t i = 0; i < components.size(); ++i) {
        delete components[i];
    }
}

Component* ReactiveWindow::addComponent(Component* comp) {
    if (window) {
        comp->setWindow(window);
        components.push_back(comp);
        return comp;
    }
    return nullptr;
}

// Iterates over components and updates them
void ReactiveWindow::update(actionMode& mode) {
    if (!window) return;

    // Draw Box border
    box(*window, 0, 0);

    // Refresh window to draw new content
    wrefresh(*window);
    refresh();

    // update connected components
    for (auto& comp : components) {
        comp->update(mode);
    }
}