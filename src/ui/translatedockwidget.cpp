#include "translatedockwidget.h"
#include "ui_translatedockwidget.h"
#include "settingsdialog.h"
#include "translation-service/httpserver.h"

TranslateDockWidget::TranslateDockWidget(QWidget *parent)
	: QDockWidget(parent), ui(new Ui::TranslateDockWidget)
{
	ui->setupUi(this);

	// connect the settings button to the settings dialog
	connect(ui->settings, &QPushButton::clicked, this,
		&TranslateDockWidget::openSettingsDialog);
	// connect the startHTTPServer button to the startHTTPServer function
	connect(ui->startStopHTTPServer, &QPushButton::clicked, this, [=]() {
		// if the button is not checked, start the server
		if (this->ui->startStopHTTPServer->isChecked())
			start_http_server();
		// otherwise, stop the server
		else
			stop_http_server();
	});
}

TranslateDockWidget::~TranslateDockWidget()
{
	delete ui;
}

void TranslateDockWidget::openSettingsDialog()
{
	// create the settings dialog
	SettingsDialog *settingsDialog = new SettingsDialog(this);
	// show the settings dialog
	settingsDialog->show();
}
