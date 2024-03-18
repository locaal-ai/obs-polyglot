#include "registerDock.h"
#include "translatedockwidget.h"
#include "plugin-support.h"
#include "utils/config-data.h"

#include <obs-module.h>
#include <obs-frontend-api.h>

#include <httplib.h>

#include <QMainWindow>

void registerDock()
{
	obs_log(LOG_INFO, "Registering dock...");
	// This function is called when the plugin is loaded.
	// It registers a dock widget with OBS.
	QMainWindow *parent = (QMainWindow *)obs_frontend_get_main_window();
	// Create the dock
	TranslateDockWidget *dock = new TranslateDockWidget((QWidget *)parent);
	// set the error callback on the global conetxt
	global_context.error_callback = [=](const std::string &error_message) {
		global_context.error_message = error_message;
		dock->updateErrorLabel(error_message);
	};
	global_context.status_callback = [=](const std::string &message) {
		global_context.status_message = message;
		dock->updateStatusLabel(message);
	};
	// Register the dock
	obs_frontend_add_dock(dock);

	parent->addDockWidget(Qt::BottomDockWidgetArea, dock);
}
