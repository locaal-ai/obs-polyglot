#include "config-data.h"
#include "plugin-support.h"

#include <obs-module.h>
#include <string>
#include <nlohmann/json.hpp>
#include <util/config-file.h>
#include <ctranslate2/translator.h>
#include <sentencepiece_processor.h>
#include <httplib.h>

polyglot_config_data global_config;
polyglot_global_context global_context;

void config_defaults()
{
	global_config.model_selection = 0;
	global_config.local = true;
	global_config.local_model_path = "";
	global_config.local_spm_path = "";
	global_config.cloud_model_name = "";
	global_config.cloud_api_key = "";
	global_config.max_output_tokens = 100;
	global_config.http_server_port = 18080;
}

void create_config_folder()
{
	char *config_folder_path = obs_module_config_path("");
	if (config_folder_path == nullptr) {
		obs_log(LOG_ERROR, "Failed to get config folder path");
		return;
	}
	std::filesystem::path config_folder_std_path(config_folder_path);
	bfree(config_folder_path);

	// create the folder if it doesn't exist
	if (!std::filesystem::exists(config_folder_std_path)) {
#ifdef _WIN32
		obs_log(LOG_INFO, "Config folder does not exist, creating: %S",
			config_folder_std_path.c_str());
#else
		obs_log(LOG_INFO, "Config folder does not exist, creating: %s",
			config_folder_std_path.c_str());
#endif
		// Create the config folder
		std::filesystem::create_directories(config_folder_std_path);
	}
}

int getConfig(config_t **config, bool create_if_not_exist = false)
{
	create_config_folder(); // ensure the config folder exists

	// Get the config file
	char *config_file_path = obs_module_config_path("config.ini");

	int ret = config_open(config, config_file_path,
			      create_if_not_exist ? CONFIG_OPEN_ALWAYS : CONFIG_OPEN_EXISTING);
	if (ret != CONFIG_SUCCESS) {
		obs_log(LOG_INFO, "Failed to open config file %s", config_file_path);
		return OBS_POLYGLOT_CONFIG_FAIL;
	}

	return OBS_POLYGLOT_CONFIG_SUCCESS;
}

std::string config_data_to_json(const polyglot_config_data &data);
polyglot_config_data config_data_from_json(const std::string &json);

int saveConfig(bool create_if_not_exist)
{
	config_t *config_file;
	if (getConfig(&config_file, create_if_not_exist) == OBS_POLYGLOT_CONFIG_SUCCESS) {
		std::string json = config_data_to_json(global_config);
		config_set_string(config_file, "general", "polyglot_config", json.c_str());
		config_save(config_file);
		config_close(config_file);
		return OBS_POLYGLOT_CONFIG_SUCCESS;
	}
	return OBS_POLYGLOT_CONFIG_FAIL;
}

int loadConfig()
{
	config_t *config_file;
	if (getConfig(&config_file) == OBS_POLYGLOT_CONFIG_SUCCESS) {
		const char *json = config_get_string(config_file, "general", "polyglot_config");
		if (json != nullptr) {
			global_config = config_data_from_json(json);
			config_close(config_file);
			return OBS_POLYGLOT_CONFIG_SUCCESS;
		}
		config_close(config_file);
	} else {
		obs_log(LOG_WARNING, "Failed to load config file. Creating a new one.");
		config_defaults();
		if (saveConfig(true) == OBS_POLYGLOT_CONFIG_SUCCESS) {
			obs_log(LOG_INFO, "Saved default settings");
			return OBS_POLYGLOT_CONFIG_SUCCESS;
		} else {
			obs_log(LOG_ERROR, "Failed to save settings");
		}
	}
	return OBS_POLYGLOT_CONFIG_FAIL;
}

// serialize config_data to a json string
std::string config_data_to_json(const polyglot_config_data &data)
{
	nlohmann::json j;
	j["model_selection"] = data.model_selection;
	j["local"] = data.local;
	j["local_model_path"] = data.local_model_path;
	j["local_spm_path"] = data.local_spm_path;
	j["cloud_model_name"] = data.cloud_model_name;
	j["cloud_api_key"] = data.cloud_api_key;
	j["max_output_tokens"] = data.max_output_tokens;
	j["http_server_port"] = data.http_server_port;
	return j.dump();
}

// deserialize config_data from a json string
polyglot_config_data config_data_from_json(const std::string &json)
{
	nlohmann::json j = nlohmann::json::parse(json);
	polyglot_config_data data;
	try {
		data.model_selection = j["model_selection"];
		data.local = j["local"];
		data.local_model_path = j["local_model_path"];
		data.local_spm_path = j["local_spm_path"];
		data.cloud_model_name = j["cloud_model_name"];
		data.cloud_api_key = j["cloud_api_key"];
		data.max_output_tokens = j["max_output_tokens"];
		data.http_server_port = j["http_server_port"];
	} catch (nlohmann::json::exception &e) {
		obs_log(LOG_ERROR, "Failed to parse config data: %s", e.what());
	}
	return data;
}

void resetContext()
{
	global_context.error_message = "";
	global_context.options = nullptr;
	global_context.translator = nullptr;
	global_context.processor = nullptr;
	global_context.svr = nullptr;
	global_context.error_callback = [](const std::string &error_message) {
		global_context.error_message = error_message;
		if (!error_message.empty()) {
			obs_log(LOG_ERROR, "Error (callback): %s", error_message.c_str());
		}
	};
	global_context.status_callback = [](const std::string &message) {
		global_context.status_message = message;
		if (!message.empty()) {
			obs_log(LOG_INFO, "Status (callback): %s", message.c_str());
		}
	};
	global_context.tokenizer = [](const std::string &) { return std::vector<std::string>(); };
	global_context.detokenizer = [](const std::vector<std::string> &) { return std::string(); };
}

void freeContext()
{
	if (global_context.options != nullptr) {
		delete global_context.options;
		global_context.options = nullptr;
	}
	if (global_context.translator != nullptr) {
		delete global_context.translator;
		global_context.translator = nullptr;
	}
	if (global_context.processor != nullptr) {
		delete global_context.processor;
		global_context.processor = nullptr;
	}
	if (global_context.svr != nullptr) {
		delete global_context.svr;
		global_context.svr = nullptr;
	}
	resetContext();
}
