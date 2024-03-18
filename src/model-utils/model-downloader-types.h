#ifndef MODEL_DOWNLOADER_TYPES_H
#define MODEL_DOWNLOADER_TYPES_H

#include <functional>
#include <map>
#include <string>

// Information about a model
struct ModelInfo {
	std::string name;
	std::vector<std::string> urls;
	std::string localPath;
	std::string spmUrl;
	std::string localSpmPath;
};

// Callback for when the download is finished
typedef std::function<void(int download_status, const ModelInfo &modelInfo)>
	download_finished_callback_t;

extern std::map<std::string, ModelInfo> models_info;

#endif // MODEL_DOWNLOADER_TYPES_H
