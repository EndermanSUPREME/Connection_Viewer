#ifndef REACTIVE_WINDOW
#define REACTIVE_WINDOW

#include <ncurses.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "windowComponent.hpp"

class ReactiveWindow {
public:
    ReactiveWindow(int height, int width, int y, int x);
    ~ReactiveWindow();

    // getter
    auto getComponents() const { return &components; };

    void update();
    Component* addComponent(Component* comp);
private:
    std::shared_ptr<WINDOW*> window;
    std::vector<Component*> components;
};

#endif