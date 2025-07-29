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

void SaveEvent::execute() {
    std::string savePath = drawTextInput(3, 75, 4, 10, "/tmp/connections.txt");
    try {
        std::ofstream outputFile(savePath);
        if (outputFile) {
            for (const std::string& line : *(ConnectionRecorder::getInstance().getContent())) {
                outputFile << line << std::endl;
            }

            // show affordance upon successful exporting
            std::string saveMsg = "saved connections: " + savePath;
            
            int height, width;
            getmaxyx(stdscr, height, width);
            drawPopUp(3, height/2,10, 2,saveMsg);
        } else {

        }
    } catch (std::exception error) {
        // error catch pop-up
    }
}

void ViewEvent::execute() {
}

void KillEvent::execute() {
}

void FlagEvent::execute() {
}