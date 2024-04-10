#include <string.h>
#include <stdlib.h>
#include <atomic>
#include <algorithm>

#include "../common/plugin.hpp"
#include "../common/gui.hpp"
#include "../common/event-logger.hpp"

#include "note-generator-gui.hpp"

class UniqueNoteIDGenerator {
    // Bitwig appears to max out at 20bits, so do the same.
    // use ids >= 1024 to not overlap with "key", to ease reading logs.
    static constexpr int32_t    min_note_id = 1024; 
    static constexpr int32_t    end_note_id = 1<<20; 
    int32_t                     counter = min_note_id;

public:
    int32_t     getNextNoteID() {
                    // should only ever be used from audio thread, so no need for atomics
                    if (!(counter < end_note_id)) {
                        counter = min_note_id;
                    }
                    return counter++;
                }
};

auto    MakeNoteEvent(uint32_t note_id, int16_t key, uint16_t note_event_type, double velocity) {
            clap_event_note ev;
            ev.header.size = sizeof(clap_event_note);
            ev.header.type = note_event_type;
            ev.header.time = 0;
            ev.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
            ev.header.flags = 0;
            ev.note_id = note_id;
            ev.port_index = 0;
            ev.channel = 0;
            ev.key = key;
            ev.velocity = velocity;
            return ev;
        };

auto    MakeNoteExpression(uint32_t note_id, int16_t key, clap_note_expression id, double value) {
            clap_event_note_expression ev;
            ev.header.size = sizeof(clap_event_note_expression);
            ev.header.type = (uint16_t)CLAP_EVENT_NOTE_EXPRESSION;
            ev.header.time = 0;
            ev.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
            ev.header.flags = 0;
            ev.note_id = note_id;
            ev.port_index = 0;
            ev.channel = 0;
            ev.key = key;
            ev.expression_id = id;
            ev.value = value;
            return ev;
        };


struct SingleNoteGenerator
{
    SingleNoteGenerator(int16_t key, clap_note_expression id)
    : send_key{key}
    , send_id{id}
    {}

    void    init(double sampleRate) {
                period = (uint64_t)sampleRate;
            }

    void    reset() {
                want_send_on  = false;
                want_send_off = false;
            }

    void    process ( UniqueNoteIDGenerator&    note_id_generator
                    , EventLogger&              logger
                    , const clap_process_t*     process
            ) {
                // GUI requested send?
                if (want_send.exchange(false,std::memory_order_relaxed)) {
                    // drop request if currently sending
                    if (!want_send_on && !want_send_off) {
                        want_send_on  = true;
                        want_send_off = true;
                    }
                }

                auto sendEvent =
                    [&](clap_event_header&& event) {
                        logger.pushEvent(&event);
                        process->out_events->try_push( process->out_events, &event);
                    };

                uint64_t base_time  = logger.sample_time;

                if (want_send_on) {
                    auto note_id = note_id_generator.getNextNoteID();
                    // send NOTE ON
                    sendEvent( MakeNoteEvent( note_id, send_key, CLAP_EVENT_NOTE_ON, 1.0 ).header );

                    // send EXPRESSION with same sample time-stamp (0)
                    sendEvent( MakeNoteExpression( note_id, send_key, send_id, 1.0 ).header ); 

                    want_send_on = false;
                    note_send_time = base_time;
                }
                else if (want_send_off) {
                    if ( base_time < (note_send_time + period) ) {
                        return;
                    }

                    // test case just needs note on for longer than smoothing window
                    sendEvent( MakeNoteEvent( -1, send_key, CLAP_EVENT_NOTE_OFF, 1.0 ).header );

                    want_send_off = false;
                }
            }

    uint64_t    note_send_time{};
    uint64_t    period{};
	bool        want_send_on{};
    bool        want_send_off{};
    int16_t     send_key;
    clap_note_expression send_id;
    std::atomic<bool>   want_send;
};

struct NoteGenerator {
	clap_plugin_t           plugin;
	const clap_host_t*      host{};

    EventLogger             logger;
    UniqueNoteIDGenerator   node_id_generator;

    std::array<SingleNoteGenerator,3> notes{
            SingleNoteGenerator{60,CLAP_NOTE_EXPRESSION_TUNING},       // Bitwig defaults to 0.0
            SingleNoteGenerator{64,CLAP_NOTE_EXPRESSION_PRESSURE},     // Bitwig defaults to 0.0
            SingleNoteGenerator{67,CLAP_NOTE_EXPRESSION_BRIGHTNESS}    // Bitwig defaults to 0.5
        };
    std::array<std::atomic<bool>,3>   want_sends;
    std::unique_ptr<NoteGeneratorGUI> gui;

    void    onActivate(double sampleRate) {
                for( auto& note : notes) {
                    note.init(sampleRate);
                }
            }
    void    onStartProcessing() {  
                for( auto& note : notes) {
                    note.reset();   
                }
            }
    void    onResetProcessing() {  
                onStartProcessing();   
            }
    auto    onProcess(const clap_process_t *process) {
                logger.processBegin(process);
                for( auto& note : notes) {
                    note.process(node_id_generator,logger,process);
                }
                logger.processEnd();
                return CLAP_PROCESS_CONTINUE;
            }
    auto    makeUniqueGUI() {
                return std::make_unique<NoteGeneratorGUI>(
                    logger,
                    want_sends_t{
                        &notes[0].want_send,
                        &notes[1].want_send,
                        &notes[2].want_send
                    }
                );
            }

};

static const clap_plugin_descriptor_t pluginDescriptor = {
	.clap_version = CLAP_VERSION_INIT,
	.id = "CLAP-Expression-Tests.NoteGenerator",
	.name = "NoteGenerator",
	.vendor = "CLAP-Expression-Tests",
	.url = "https://github.com/mccann/CLAP-Expression-Tests",
	.manual_url = "https://github.com/mccann/CLAP-Expression-Tests",
	.support_url = "https://github.com/mccann/CLAP-Expression-Tests",
	.version = "1.0.0",
	.description = "Minimal Test case for bug report regarding note expression smoothing of CLAP output in Bitwig",

	.features = (const char *[]) {
		CLAP_PLUGIN_FEATURE_NOTE_EFFECT,
		NULL,
	},
};
static const clap_plugin_note_ports_t extensionNotePorts    = makeExtensionNotePorts<false>();
static const clap_plugin_gui_t extensionGUI                 = makeExtensionGUI<NoteGenerator>();
static const clap_plugin_t pluginClass                      = makePluginClass<NoteGenerator,&extensionNotePorts,&extensionGUI>(&pluginDescriptor);
static const clap_plugin_factory_t pluginFactory            = makePluginFactory<NoteGenerator,&pluginClass>();
extern "C" const clap_plugin_entry_t clap_entry             = makeEntry<&pluginFactory>();

