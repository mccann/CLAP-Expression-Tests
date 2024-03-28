#include "note-generator-gui.hpp"
#include "imgui.h"

void NoteGeneratorGUI::onRender() {

    if (ImGui::Begin("Note Generator", nullptr, 
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration)
    ){
        ImGui::Text( "Note Generator" );

        if (ImGui::Button("Send Note with Tuning")) {
            want_sends[0]->store(true,std::memory_order_relaxed);
        }
        if (ImGui::Button("Send Note with Pressure")) {
            want_sends[1]->store(true,std::memory_order_relaxed);
        }
        if (ImGui::Button("Send Note with Brightness")) {
            want_sends[2]->store(true,std::memory_order_relaxed);
        }

        ImGui::Separator();
        log_gui.render("Events Sent",events_logger);

    }
    // imgui-clap-support calls ImGui::End() automatically
}