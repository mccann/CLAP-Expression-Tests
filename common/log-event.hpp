#pragma once

#include <clap/clap.h>
#include <vector>
#include <cassert>

struct LogEvent
{
    union {
        clap_event_header           event;
        clap_event_note             note;
        clap_event_midi             midi;
        clap_event_midi_sysex       midi_sysex;
        clap_event_midi2            midi2;
        clap_event_note_expression  note_expression;
        clap_event_param_value      param_value;
        clap_event_param_mod        param_mod;
        clap_event_param_gesture    param_gesture;
        clap_event_transport        transport;
    };
    uint32_t    block_count; 
    uint32_t    block_size;  

    static LogEvent from(const clap_event_header* event, uint64_t block_count, uint32_t block_size) {
                        assert(event);
                        assert(event->size <= sizeof(LogEvent));
                        LogEvent ret;
                        memcpy(&ret,event,event->size);
                        ret.block_count = block_count;
                        ret.block_size  = block_size;
                        return ret;
                    }

    using Buffer = std::vector<LogEvent>;
};



