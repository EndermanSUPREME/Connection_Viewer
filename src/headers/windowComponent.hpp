#ifndef WINDOW_COMPONENT
#define WINDOW_COMPONENT

#include <ncurses.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

// component base class
class Component {
public:
    // pure virtual
    virtual void update() =0;
    // virtual dtor
    virtual ~Component(){ /* Reactive Window dtor Deallocates Window */ window = nullptr; };

    void setWindow(std::shared_ptr<WINDOW*>& targetWindow) { window = targetWindow; };
    std::shared_ptr<WINDOW*> getWindow() const { return window; };
protected:
    std::shared_ptr<WINDOW*> window;
};

class Text : public Component {
public:
    Text(int row_ = 1, int col_ = 1, std::vector<std::string> lines_ = {}): lines(lines_), row(row_), col(col_) {};
    
    // getters
    int getRow() const { return row; };
    int getCol() const { return col; };

    // setters
    void setLines(std::vector<std::string> lines_) { lines = lines_; };

    void update() override;
private:
    int row;
    int col;
    std::vector<std::string> lines;
};

#endif