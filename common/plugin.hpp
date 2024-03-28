#pragma once

#include <memory>

#include <clap/clap.h>

template <typename T>
auto    plugin_cast(const clap_plugin* _plugin) {
            return  static_cast<T>(_plugin->plugin_data);
        }


template <bool INPUT>
constexpr auto makeExtensionNotePorts() -> clap_plugin_note_ports_t {
    return {
	.count = [] (const clap_plugin_t *plugin, bool isInput) -> uint32_t {
		return (isInput==INPUT) ? 1 : 0;
	},

	.get = [] (const clap_plugin_t *plugin, uint32_t index, bool isInput, clap_note_port_info_t *info) -> bool {
        if ((isInput!=INPUT) || index != 0) {
            return false;
        }
		info->id = 0;
		info->supported_dialects = CLAP_NOTE_DIALECT_CLAP;
		info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
        strncpy(info->name,"Note Port",sizeof(info->name));
		return true;
	},
    };
}

template <typename T,const clap_plugin_note_ports_t* notePorts, const clap_plugin_gui_t* guiPlugin>
constexpr auto makePluginClass(const clap_plugin_descriptor_t* desc) -> clap_plugin_t {
    return {
	.desc = desc,
	.plugin_data = nullptr,
    .init = [] (const clap_plugin *) { return true; },
	.destroy = [] (const clap_plugin * _plugin) {
        // born as unique_ptr, die as one
        std::unique_ptr<T>(plugin_cast<T*>(_plugin)) = nullptr;
    },

	.activate = [] (const clap_plugin *_plugin, double sampleRate, uint32_t, uint32_t) { 
        plugin_cast<T*>(_plugin)->onActivate(sampleRate);
        return true; 
    },
	.deactivate = [] (const clap_plugin *) {},

	.start_processing = [] (const clap_plugin *_plugin) {
        plugin_cast<T*>(_plugin)->onStartProcessing();
        return true; 
    },
	.stop_processing = [] (const clap_plugin *) {},

	.reset = [] (const clap_plugin *_plugin) {
        plugin_cast<T*>(_plugin)->onResetProcessing();
	},

	.process = [] (const clap_plugin *_plugin, const clap_process_t *process) -> clap_process_status {
		return plugin_cast<T*>(_plugin)->onProcess(process);
	},

	.get_extension = [] (const clap_plugin *plugin, const char *id) -> const void * {
		if (0 == strcmp(id, CLAP_EXT_NOTE_PORTS )) return notePorts;
        if (0 == strcmp(id, CLAP_EXT_GUI )) return guiPlugin;
		return nullptr;
	},

	.on_main_thread = [] (const clap_plugin *_plugin) {}
    };
}

template <typename T,const clap_plugin_t* pluginClass>
constexpr auto makePluginFactory() -> clap_plugin_factory_t {
    return {
	.get_plugin_count = [] (const clap_plugin_factory *factory) -> uint32_t { 
		return 1; 
	},

	.get_plugin_descriptor = [] (const clap_plugin_factory *factory, uint32_t index) -> const clap_plugin_descriptor_t * { 
		return index == 0 ? pluginClass->desc : nullptr; 
	},

	.create_plugin = [] (const clap_plugin_factory *factory, const clap_host_t *host, const char *pluginID) -> const clap_plugin_t * {
		if (!clap_version_is_compatible(host->clap_version) || strcmp(pluginID, pluginClass->desc->id)) {
			return nullptr;
		}

		auto plugin = std::make_unique<T>();
		plugin->host = host;
		plugin->plugin = *pluginClass;
		plugin->plugin.plugin_data = plugin.get();
		return &plugin.release()->plugin;
	}
    };
}



template <const clap_plugin_factory_t* factory>
constexpr clap_plugin_entry_t makeEntry() {
    return {
	.clap_version = CLAP_VERSION_INIT,
	.init = [] (const char *path) -> bool { 
		return true; 
	},
	.deinit = [] () {},
	.get_factory = [] (const char *factoryID) -> const void * {
		return strcmp(factoryID, CLAP_PLUGIN_FACTORY_ID) ? nullptr : factory;
	}
    };
}
