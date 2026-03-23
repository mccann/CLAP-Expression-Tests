#pragma once

#include <clap/clap.h>

inline constexpr auto makeExtensionPolyphonicVoiceInfo() -> clap_plugin_voice_info_t {
    return {
    .get =  [](const clap_plugin_t* _plugin, clap_voice_info_t* info) {
                info->voice_count = 256;
                info->voice_capacity = 256;
                info->flags = CLAP_VOICE_INFO_SUPPORTS_OVERLAPPING_NOTES;
                return true;
            }
    };
}

    