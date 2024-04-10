#pragma once

#include <string>

#include <clap/clap.h>
#include <fmt/format.h>


inline auto NoteEventString(int32_t note_id, int16_t port_index, int16_t channel, int16_t key, double value, std::string_view label) {
    return  fmt::format(
                "note_id:{:7d} port:{:2d} channel:{:2d} key:{:3d} | {:3f} {}"
                ,note_id, port_index, channel, key, value, label
            );
}

inline auto NoteEventString( const clap_event_note& note ) {
    return NoteEventString( note.note_id, note.port_index, note.channel, note.key, note.velocity, "VELOCITY" );
}

inline auto NoteExpressionString(const clap_event_note_expression& note_expression) {

    using namespace std::literals;

    std::string      strExpressionType;
    std::string_view svExpresionType;
    switch (note_expression.expression_id) {
    case CLAP_NOTE_EXPRESSION_VOLUME:
        svExpresionType = "VOLUME"sv;
        break;
    case CLAP_NOTE_EXPRESSION_PAN:
        svExpresionType = "PAN"sv;
        break;
    case CLAP_NOTE_EXPRESSION_TUNING:
        svExpresionType = "TUNING"sv;
        break;
    case CLAP_NOTE_EXPRESSION_VIBRATO:
        svExpresionType = "VIBRATO"sv;
        break;
    case CLAP_NOTE_EXPRESSION_EXPRESSION:
        svExpresionType = "EXPRESSION"sv;
        break;
    case CLAP_NOTE_EXPRESSION_BRIGHTNESS:
        svExpresionType = "BRIGHTNESS"sv;
        break;
    case CLAP_NOTE_EXPRESSION_PRESSURE:
        svExpresionType = "PRESSURE"sv;
        break;
    default:
        strExpressionType = fmt::format("UNKNOWN: {:d}", note_expression.expression_id );
        svExpresionType = strExpressionType;
    }
    return  NoteEventString(
                note_expression.note_id, note_expression.port_index, 
                note_expression.channel, note_expression.key, note_expression.value, 
                svExpresionType 
            );
}

inline auto ParamID(clap_id param_id) {
    return fmt::format("ParamID: {}",param_id);
}

inline auto ParamValueString(const clap_event_param_value& param_value) {
    return  NoteEventString(
                param_value.note_id, param_value.port_index, 
                param_value.channel, param_value.key, param_value.value, 
                ParamID(param_value.param_id) 
            );

}
inline auto ParamModString(const clap_event_param_mod& param_mod) {
    return  NoteEventString(
                param_mod.note_id, param_mod.port_index, 
                param_mod.channel, param_mod.key, param_mod.amount, 
                ParamID(param_mod.param_id)
            );

}

inline auto ParamGestureString(const clap_event_param_gesture& param_gesture) {
    return ParamID(param_gesture.param_id);
}

inline auto EventMidiString(const clap_event_midi& midi) {
    return  fmt::format(
                // aligned with note-events
                "                port:{:2d} data: {:2x} {:2x} {:2x}"
                ,midi.port_index, midi.data[0], midi.data[1], midi.data[2]
            );
}

inline auto EventMidiSysExString(const clap_event_midi_sysex& midi_sysex) {
    return  fmt::format(
                // aligned with note-events
                "                port:{:2d} size: {}"
                ,midi_sysex.port_index, midi_sysex.size
            );
}

inline auto EventMidi2String(const clap_event_midi2& midi2) {
    return  fmt::format(
                // aligned with note-events
                "                port:{:2d} data: {:8x} {:8x} {:8x} {:8x}"
                ,midi2.port_index, midi2.data[0], midi2.data[1], midi2.data[2], midi2.data[3]
            );
}

inline auto EventString(const clap_event_header* ev) -> std::string {

    using namespace std::literals;

    std::string      strExtra;
    std::string      strType;
    std::string_view svType;
    
    switch( ev->type ) {
    case CLAP_EVENT_NOTE_ON:
        svType = "NOTE_ON"sv;
        strExtra = NoteEventString(*reinterpret_cast<const clap_event_note*>(ev));
        break;
    case CLAP_EVENT_NOTE_OFF:
        svType = "NOTE_OFF"sv;
        strExtra = NoteEventString(*reinterpret_cast<const clap_event_note*>(ev));
        break;
    case CLAP_EVENT_NOTE_CHOKE:
        svType = "NOTE_CHOKE"sv;
        strExtra = NoteEventString(*reinterpret_cast<const clap_event_note*>(ev));
        break;
    case CLAP_EVENT_NOTE_END:
        svType = "NOTE_END"sv;
        strExtra = NoteEventString(*reinterpret_cast<const clap_event_note*>(ev));
        break;
    case CLAP_EVENT_NOTE_EXPRESSION:
        svType = "NOTE_EXPRESSION"sv;
        strExtra = NoteExpressionString(*reinterpret_cast<const clap_event_note_expression*>(ev));
        break;
    case CLAP_EVENT_PARAM_VALUE:
        svType = "PARAM_VALUE"sv;
        strExtra = ParamValueString(*reinterpret_cast<const clap_event_param_value*>(ev));
        break;
    case CLAP_EVENT_PARAM_MOD:
        svType = "PARAM_MOD"sv;
        strExtra = ParamModString(*reinterpret_cast<const clap_event_param_mod*>(ev));
        break;
    case CLAP_EVENT_PARAM_GESTURE_BEGIN:
        svType = "PARAM_GESTURE_BEGIN"sv;
        strExtra = ParamGestureString(*reinterpret_cast<const clap_event_param_gesture*>(ev));
        break;
    case CLAP_EVENT_PARAM_GESTURE_END:
        svType = "PARAM_GESTURE_END"sv;
        strExtra = ParamGestureString(*reinterpret_cast<const clap_event_param_gesture*>(ev));
        break;
    case CLAP_EVENT_TRANSPORT:
        svType = "TRANSPORT"sv;
        break;
    case CLAP_EVENT_MIDI:
        svType = "MIDI"sv;
        strExtra = EventMidiString(*reinterpret_cast<const clap_event_midi*>(ev));
        break;
    case CLAP_EVENT_MIDI_SYSEX:
        svType = "MIDI_SYSEX"sv;
        strExtra = EventMidiSysExString(*reinterpret_cast<const clap_event_midi_sysex*>(ev));
        break;
    case CLAP_EVENT_MIDI2:
        svType = "MIDI2"sv;
        strExtra = EventMidi2String(*reinterpret_cast<const clap_event_midi2*>(ev));
        break;
    default:
        strType = fmt::format("UNKNOWN: {:d}", ev->type );
        svType = strType;
    }

    return  fmt::format( "{:04d}:{:x} {:20s} {}", ev->time, ev->flags, svType, strExtra );
};

