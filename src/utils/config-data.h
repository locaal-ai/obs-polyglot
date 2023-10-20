#ifndef POLYGLOT_CONFIG_DATA_H
#define POLYGLOT_CONFIG_DATA_H

#ifdef __cplusplus

#include <string>
#include <vector>
#include <functional>

struct polyglot_config_data {
	// local model path
	std::string local_model_path;
	// local spm path
	std::string local_spm_path;

	// max output tokens
	uint16_t max_output_tokens;

	// http server port
	uint16_t http_server_port;

	// For Future Use:
	// local or cloud
	bool local;
	// cloud model name
	std::string cloud_model_name;
	// cloud API key
	std::string cloud_api_key;
};

// forward declaration
namespace ctranslate2 {
class TranslationOptions;
class Translator;
}
namespace sentencepiece {
class SentencePieceProcessor;
}
namespace httplib {
class Server;
}

struct polyglot_global_context {
	// error message
	std::string error_message;
	// ctranslate2 options
	ctranslate2::TranslationOptions *options;
	// ctranslate2 translator
	ctranslate2::Translator *translator;
	// sentencepiece processor
	sentencepiece::SentencePieceProcessor *processor;
	// tokenizer
	std::function<std::vector<std::string>(const std::string &)> tokenizer;
	// detokenizer
	std::function<std::string(const std::vector<std::string> &)> detokenizer;
	// http server
	httplib::Server *svr;
};

extern polyglot_config_data global_config;
extern polyglot_global_context global_context;

#endif // __cplusplus

#define OBS_POLYGLOT_CONFIG_FAIL -1
#define OBS_POLYGLOT_CONFIG_SUCCESS 0

#ifdef __cplusplus
extern "C" {
#endif

void resetContext();
int saveConfig(bool create_if_not_exist);
int loadConfig();

#ifdef __cplusplus
}
#endif

#endif // POLYGLOT_CONFIG_DATA_H
