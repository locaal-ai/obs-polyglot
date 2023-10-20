#pragma once

#ifdef __cplusplus
extern "C" {
#endif
int build_translation_context();
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include <string>
int translate(const std::string &text, const std::string &source_lang,
	      const std::string &target_lang, std::string &result);
#endif

#define OBS_POLYGLOT_TRANSLATION_INIT_FAIL -1
#define OBS_POLYGLOT_TRANSLATION_INIT_SUCCESS 0
#define OBS_POLYGLOT_TRANSLATION_SUCCESS 0
#define OBS_POLYGLOT_TRANSLATION_FAIL -1
