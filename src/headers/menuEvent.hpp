#ifndef MENU_EVENT
#define MENU_EVENT

#include <ncurses.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include "utils.hpp"

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

class SaveEvent : public MenuEvent {
public:
    // delegate to base ctor to fill in protected inherited member
    SaveEvent(std::string str=""): MenuEvent(str) {};
    SaveEvent(const SaveEvent& rhs) { description = rhs.description; };

    void execute() override;
};

class KillEvent : public MenuEvent {
public:
    // delegate to base ctor to fill in protected inherited member
    KillEvent(std::string str=""): MenuEvent(str) {};
    KillEvent(const KillEvent& rhs) { description = rhs.description; };

    void execute() override;
};

class FlagEvent : public MenuEvent {
public:
    // delegate to base ctor to fill in protected inherited member
    FlagEvent(std::string str="", bool unflag=false): MenuEvent(str), unflagMode(unflag) {};
    FlagEvent(const FlagEvent& rhs) { description = rhs.description; };

    void execute() override;
    bool isFlagRemover() const { return unflagMode; };
private:
    bool unflagMode;
};

#endif