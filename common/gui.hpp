#pragma once

#include <clap/clap.h>

#include "imgui-clap-support/imgui-clap-editor.h"

#include "../common/plugin.hpp"


template <typename T>
constexpr auto makeExtensionGUI() -> clap_plugin_gui_t {
    return {
	.is_api_supported = [](const clap_plugin_t *_plugin, const char* api, bool isFloating) {
        if (isFloating)
            return false;
        #if IS_MAC
            if (strcmp(api, CLAP_WINDOW_API_COCOA) == 0)
                return true;
        #endif

        #if IS_WIN
            if (strcmp(api, CLAP_WINDOW_API_WIN32) == 0)
                return true;
        #endif

        #if IS_LINUX
            if (strcmp(api, CLAP_WINDOW_API_X11) == 0)
                return true;
        #endif
            return false;
    }, 
    .get_preferred_api = [](const clap_plugin_t *_plugin, const char **api, bool *is_floating) { return false; },
    .create = [](const clap_plugin_t *_plugin, const char *api, bool is_floating) {
        auto plugin = plugin_cast<T*>(_plugin);
        plugin->gui = plugin->makeUniqueGUI();
        auto timer = (const clap_host_timer_support_t *)
            plugin->host->get_extension(plugin->host, CLAP_EXT_TIMER_SUPPORT);
        return imgui_clap_guiCreateWith(plugin->gui.get(), timer);
    },
    .destroy = [](const clap_plugin_t *_plugin) {
        auto plugin = plugin_cast<T*>(_plugin);
        auto timer = (const clap_host_timer_support_t *)
            plugin->host->get_extension(plugin->host, CLAP_EXT_TIMER_SUPPORT);
        imgui_clap_guiDestroyWith(plugin->gui.get(), timer);
        plugin->gui = nullptr;
    },
    .set_scale = [](const clap_plugin_t *_plugin, double scale) {
        return false;
    },
    .get_size = [](const clap_plugin_t *plugin, uint32_t *width, uint32_t *height) {
        *width = 640;
        *height = 480;
        return true;
    },
    .can_resize = [](const clap_plugin_t *plugin) { return true; },
    .get_resize_hints = [](const clap_plugin_t *plugin, clap_gui_resize_hints_t *hints) { return false; },
    .adjust_size = [](const clap_plugin_t *plugin, uint32_t *width, uint32_t *height) { return true; },
    .set_size = [](const clap_plugin_t *_plugin, uint32_t width, uint32_t height) { 
        return imgui_clap_guiSetSizeWith(plugin_cast<T*>(_plugin)->gui.get(), width, height);
    },
    .set_parent = [](const clap_plugin_t *_plugin, const clap_window_t *window) -> bool {
        return imgui_clap_guiSetParentWith(plugin_cast<T*>(_plugin)->gui.get(), window);
    },
    .set_transient = [](const clap_plugin_t *plugin, const clap_window_t *window) { return false; },
    .suggest_title = [](const clap_plugin_t *plugin, const char *title) { },
    .show = [](const clap_plugin_t *plugin) { return false; },
    .hide = [](const clap_plugin_t *plugin) { return false; }
    };
}

