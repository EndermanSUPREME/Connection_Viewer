#ifndef MENU_EVENT
#define MENU_EVENT

#include <iostream>
#include <string>
#include <vector>

// base class
class MenuEvent {
public:
    MenuEvent(std::string str=""): description(str) {};
    MenuEvent(const MenuEvent& rhs) { description = rhs.description; };

    // pure virtual
    virtual void execute() =0;
    // virtual dtor
    virtual ~MenuEvent(){};

    std::string ToString();
    const char* ToCString();

protected:
    std::string description;
};

class PsuedoEvent : public MenuEvent {
public:
    // delegate to base ctor to fill in protected inherited member
    PsuedoEvent(std::string str=""): MenuEvent(str) {};
    PsuedoEvent(const PsuedoEvent& rhs) { description = rhs.description; };

    void execute() override {};
};

// Exit Event triggers program shutdown
class ExitEvent : public MenuEvent {
public:
    // delegate to base ctor to fill in protected inherited member
    ExitEvent(std::string str=""): MenuEvent(str), exitState(false) {};
    ExitEvent(const ExitEvent& rhs) { description = rhs.description; };

    void execute() override;
    bool getExitState() { return exitState; };
private:
    bool exitState;
};

#endif