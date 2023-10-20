#include "registerDock.h"
#include "translatedockwidget.h"
#include "plugin-support.h"

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
	// Register the dock
	obs_frontend_add_dock(dock);

	parent->addDockWidget(Qt::BottomDockWidgetArea, dock);
}
