#include "translation.h"
#include <ctranslate2/translator.h>
#include "plugin-support.h"
#include <obs-module.h>
#include <ctranslate2/models/sequence_to_sequence.h>
#include <sentencepiece_processor.h>
#include <regex>

static std::vector<std::string_view>
get_vocabulary_tokens(const ctranslate2::Vocabulary &vocabulary)
{
	std::vector<std::string_view> tokens;
	const size_t size = vocabulary.size();
	tokens.reserve(size);
	for (size_t i = 0; i < size; ++i)
		tokens.emplace_back(vocabulary.to_token(i));
	return tokens;
}

const std::string model_path = std::string(getenv("USERPROFILE")) +
			       "/Downloads/nllb-200-distilled-600M-int8/";
const std::string sp_model_path =
	model_path + "flores200_sacrebleu_tokenizer_spm.model";

int translate()
{
	try {
		sentencepiece::SentencePieceProcessor processor;
		const auto status = processor.Load(sp_model_path);
		if (!status.ok()) {
			obs_log(LOG_ERROR, status.ToString().c_str());
		}

		auto tokenizer = [&processor](const std::string &text) {
			std::vector<std::string> tokens;
			processor.Encode(text, &tokens);
			return tokens;
		};
		auto detokenizer =
			[&processor](const std::vector<std::string> &tokens) {
				std::string text;
				processor.Decode(tokens, &text);
				return std::regex_replace(
					text, std::regex("<unk>"), "UNK");
			};

		const std::string text = "Hello world! How are you?";
		std::vector<std::string> tokens = tokenizer(text);
		tokens.insert(tokens.begin(), "<s>");
		tokens.insert(tokens.begin(), "eng_Latn");
		tokens.push_back("</s>");

		obs_log(LOG_INFO, "Loading model...");
		ctranslate2::Translator translator(
			model_path, ctranslate2::Device::CPU,
			ctranslate2::ComputeType::AUTO);
		obs_log(LOG_INFO, "Model loaded");

		ctranslate2::TranslationOptions options;
		options.beam_size = 1;
		options.max_decoding_length = 10;
		options.use_vmap = true;
		options.return_scores = false;

		const std::vector<std::vector<std::string>> batch = {tokens};
		obs_log(LOG_INFO, "Translating...");
		const std::vector<std::vector<std::string>> target_prefix = {
			{"spa_Latn"}};
		const std::vector<ctranslate2::TranslationResult> results =
			translator.translate_batch(batch, target_prefix,
						   options);
		obs_log(LOG_INFO, "Translation done");
		obs_log(LOG_INFO, "Result: %s",
			detokenizer(results[0].output()).c_str());
	} catch (std::exception &e) {
		obs_log(LOG_ERROR, "Error: %s", e.what());
		return 1;
	}
	return 0;
}
