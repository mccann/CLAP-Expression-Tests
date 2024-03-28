#include "note-logger-gui.hpp"

NoteLoggerGUI::NoteLoggerGUI(EventLogger& events)
: events_logger{events}
{}


void NoteLoggerGUI::onRender() {
    if (ImGui::Begin("Note Logger", nullptr, 
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration)
    ){
        log_gui.render("Events Received",events_logger);
    }
    // imgui-clap-support calls ImGui::End() automatically
}


