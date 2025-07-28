#include <menuEvent.hpp>

std::string MenuEvent::ToString() {
    return description;
}
const char* MenuEvent::ToCString() {
    return description.c_str();
}

void ExitEvent::execute() {
    exitState = true;
}