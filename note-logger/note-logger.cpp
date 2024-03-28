#include <string.h>
#include <stdlib.h>

#include "../common/plugin.hpp"
#include "../common/gui.hpp"
#include "../common/event-logger.hpp"

#include "note-logger-gui.hpp"

struct NoteLogger {
	clap_plugin_t       plugin;
	const clap_host_t*  host{};

    EventLogger logger;
    std::unique_ptr<NoteLoggerGUI> gui;

    void    onActivate(double sampleRate) {
                logger.reset();
            }
    void    onStartProcessing() {  logger.reset();   }
    void    onResetProcessing() {  logger.reset();   }
    auto    onProcess(const clap_process_t *process) {
                return logger.processInEvents(process);
            }

    auto    makeUniqueGUI() {
                return std::make_unique<NoteLoggerGUI>(logger);
            }
};

static const clap_plugin_descriptor_t pluginDescriptor = {
	.clap_version = CLAP_VERSION_INIT,
	.id = "CLAP-Expression-Tests.NoteLogger",
	.name = "NoteLogger",
	.vendor = "CLAP-Expression-Tests",
	.url = "https://github.com/mccann/CLAP-Expression-Tests",
	.manual_url = "https://github.com/mccann/CLAP-Expression-Tests",
	.support_url = "https://github.com/mccann/CLAP-Expression-Tests",
	.version = "1.0.0",
	.description = "Display a text log of CLAP events received on input port",

	.features = (const char *[]) {
		CLAP_PLUGIN_FEATURE_NOTE_EFFECT,
		NULL,
	},
};

static const clap_plugin_note_ports_t extensionNotePorts    = makeExtensionNotePorts<true>();
static const clap_plugin_gui_t extensionGUI                 = makeExtensionGUI<NoteLogger>();
static const clap_plugin_t pluginClass                      = makePluginClass<NoteLogger,&extensionNotePorts,&extensionGUI>(&pluginDescriptor);
static const clap_plugin_factory_t pluginFactory            = makePluginFactory<NoteLogger,&pluginClass>();
extern "C" const clap_plugin_entry_t clap_entry             = makeEntry<&pluginFactory>();
