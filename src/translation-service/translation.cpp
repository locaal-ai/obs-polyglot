#include "translation.h"
#include "plugin-support.h"
#include "utils/config-data.h"

#include <ctranslate2/translator.h>
#include <obs-module.h>
#include <ctranslate2/models/sequence_to_sequence.h>
#include <sentencepiece_processor.h>
#include <regex>

int build_translation_context()
{
	obs_log(LOG_INFO, "Building translation context...");
	try {
		obs_log(LOG_INFO, "Loading SPM from %s", global_config.local_spm_path.c_str());
		global_context.processor = new sentencepiece::SentencePieceProcessor();
		const auto status = global_context.processor->Load(global_config.local_spm_path);
		if (!status.ok()) {
			obs_log(LOG_ERROR, status.ToString().c_str());
			return OBS_POLYGLOT_TRANSLATION_INIT_FAIL;
		}

		global_context.tokenizer = [](const std::string &text) {
			std::vector<std::string> tokens;
			global_context.processor->Encode(text, &tokens);
			return tokens;
		};
		global_context.detokenizer = [](const std::vector<std::string> &tokens) {
			std::string text;
			global_context.processor->Decode(tokens, &text);
			return std::regex_replace(text, std::regex("<unk>"), "UNK");
		};

		obs_log(LOG_INFO, "Loading CT2 model from %s",
			global_config.local_model_path.c_str());
		global_context.translator = new ctranslate2::Translator(
			global_config.local_model_path, ctranslate2::Device::CPU,
			ctranslate2::ComputeType::AUTO);
		obs_log(LOG_INFO, "CT2 Model loaded");

		global_context.options = new ctranslate2::TranslationOptions;
		global_context.options->beam_size = 1;
		global_context.options->max_decoding_length = global_config.max_output_tokens;
		global_context.options->use_vmap = true;
		global_context.options->return_scores = false;
	} catch (std::exception &e) {
		obs_log(LOG_ERROR, "Error: %s", e.what());
		return OBS_POLYGLOT_TRANSLATION_INIT_FAIL;
	}
	return OBS_POLYGLOT_TRANSLATION_INIT_SUCCESS;
}

int translate(const std::string &text, const std::string &source_lang,
	      const std::string &target_lang, std::string &result)
{
	try {
		// get tokens
		std::vector<std::string> tokens = global_context.tokenizer(text);
		tokens.insert(tokens.begin(), "<s>");
		tokens.insert(tokens.begin(), source_lang);
		tokens.push_back("</s>");

		const std::vector<std::vector<std::string>> batch = {tokens};

		obs_log(LOG_DEBUG, "Translating...");

		const std::vector<std::vector<std::string>> target_prefix = {{target_lang}};
		const std::vector<ctranslate2::TranslationResult> results =
			global_context.translator->translate_batch(batch, target_prefix,
								   *global_context.options);

		obs_log(LOG_DEBUG, "Translation done");
		// detokenize starting with the 2nd token
		const auto &tokens_result = results[0].output();
		result = global_context.detokenizer(
			std::vector<std::string>(tokens_result.begin() + 1, tokens_result.end()));
		obs_log(LOG_DEBUG, "Result: %s", result.c_str());
	} catch (std::exception &e) {
		obs_log(LOG_ERROR, "Error: %s", e.what());
		return OBS_POLYGLOT_TRANSLATION_FAIL;
	}
	return OBS_POLYGLOT_TRANSLATION_SUCCESS;
}
