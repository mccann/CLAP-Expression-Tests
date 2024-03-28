#pragma once

#include "../common/event-logger-gui.hpp"
#include "imgui-clap-support/imgui-clap-editor.h"

struct NoteLoggerGUI : public imgui_clap_editor
{
    NoteLoggerGUI(EventLogger& events);
    void onRender() override;
private:
    EventLogger&        events_logger;
    ImGuiEventLog       log_gui;
};
