/*
Plugin Name
Copyright (C) <Year> <Developer> <Email Address>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <obs-module.h>
#include <plugin-support.h>
#include "translation-service/translation.h"
#include "ui/registerDock.h"
#include "utils/config-data.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

bool obs_module_load(void)
{
	obs_log(LOG_INFO, "plugin loaded successfully (version %s)", PLUGIN_VERSION);

	resetContext();
	registerDock();

	// load plugin settings from config
	if (loadConfig() == OBS_POLYGLOT_CONFIG_SUCCESS) {
		obs_log(LOG_INFO, "Loaded config from config file");
	} else {
		obs_log(LOG_INFO, "Failed to load config from config file");
	}
	// build the translation context
	if (build_translation_context() != OBS_POLYGLOT_TRANSLATION_INIT_SUCCESS) {
		obs_log(LOG_ERROR, "Failed to build translation context");
	} else {
		obs_log(LOG_INFO, "Built translation context");
	}

	return true;
}

void obs_module_unload(void)
{
	freeContext();
	obs_log(LOG_INFO, "plugin unloaded");
}
