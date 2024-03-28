#pragma once

#include "../common/event-logger-gui.hpp"

#include "imgui-clap-support/imgui-clap-editor.h"

#include <array>
#include <atomic>


using want_sends_t = std::array<std::atomic<bool>*,3>;

struct NoteGeneratorGUI : public imgui_clap_editor
{
    NoteGeneratorGUI(EventLogger& events,want_sends_t&& snds) 
    : events_logger{events}
    , want_sends{std::move(snds)} 
    {}

    void onRender() override;
private:
    EventLogger&    events_logger;
    ImGuiEventLog   log_gui;
    want_sends_t    want_sends;

};
