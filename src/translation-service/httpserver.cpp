#include "httpserver.h"
#include "utils/config-data.h"
#include "plugin-support.h"
#include "translation.h"

#include <httplib.h>
#include <obs-module.h>
#include <thread>
#include <nlohmann/json.hpp>

// start the http server
void start_http_server()
{
	obs_log(LOG_INFO, "Starting Polyglot http server thread...");

	std::thread([]() {
		// create the server
		if (global_context.svr != nullptr) {
			obs_log(LOG_INFO, "Polyglot Http server already running, stopping...");
			stop_http_server();
		}
		global_context.svr = new httplib::Server();

		global_context.svr->set_pre_routing_handler([](const httplib::Request &,
							       httplib::Response &res) {
			res.set_header("Access-Control-Allow-Origin", "*");
			res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
			res.set_header("Access-Control-Allow-Headers",
				       "Content-Type, Authorization");
			return httplib::Server::HandlerResponse::Unhandled;
		});

		// set an echo handler
		global_context.svr->Post("/echo", [](const httplib::Request &req,
						     httplib::Response &res,
						     const httplib::ContentReader &content_reader) {
			UNUSED_PARAMETER(req);
			obs_log(LOG_DEBUG, "Received request on /echo");
			std::string body;
			content_reader([&](const char *data, size_t data_length) {
				body.append(data, data_length);
				return true;
			});
			res.set_content(body, "text/plain");
		});
		// set a translation handler
		global_context.svr->Post(
			"/translate", [](const httplib::Request &req, httplib::Response &res,
					 const httplib::ContentReader &content_reader) {
				UNUSED_PARAMETER(req);
				obs_log(LOG_DEBUG, "Received request on /translate");
				std::string body;
				content_reader([&](const char *data, size_t data_length) {
					body.append(data, data_length);
					return true;
				});

				std::string result;
				int ret = translate_from_json(body, result);
				if (ret == OBS_POLYGLOT_TRANSLATION_SUCCESS) {
					res.set_content(result, "text/plain");
				} else {
					res.set_content("Translation failed", "text/plain");
					res.status = 500;
				}
			});

		// listen on the port
		obs_log(LOG_INFO, "Polyglot Http server starting on port %d",
			global_config.http_server_port);
		try {
			global_context.svr->listen("127.0.0.1", global_config.http_server_port);
		} catch (const std::exception &e) {
			obs_log(LOG_ERROR, "Polyglot Http server start error: %s", e.what());
		}
		obs_log(LOG_INFO, "Polyglot Http server stopped.");
	}).detach();

	global_context.status_callback("Ready for requests at http://localhost:" +
				       std::to_string(global_config.http_server_port) +
				       "/translate");
}

// stop the http server
void stop_http_server()
{
	obs_log(LOG_INFO, "Stopping Polyglot http server...");
	if (global_context.svr == nullptr) {
		obs_log(LOG_INFO, "Polyglot Http server not running.");
	} else {
		global_context.svr->stop();
		delete global_context.svr;
		global_context.svr = nullptr;
		global_context.status_callback("");
	}
}
