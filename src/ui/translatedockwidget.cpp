#include "translatedockwidget.h"
#include "ui_translatedockwidget.h"
#include "settingsdialog.h"
#include "translation-service/httpserver.h"
#include "utils/config-data.h"

TranslateDockWidget::TranslateDockWidget(QWidget *parent)
	: QDockWidget(parent), ui(new Ui::TranslateDockWidget)
{
	ui->setupUi(this);

	ui->errorLabel->hide();
	ui->label_status->hide();
	this->updateErrorLabel(global_context.error_message);

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

void TranslateDockWidget::updateStatusLabel(const std::string &message)
{
	if (message.empty()) {
		ui->label_status->hide();
		return;
	}
	ui->label_status->setText(QString::fromStdString(message));
}

void TranslateDockWidget::updateErrorLabel(const std::string &error_message)
{
	// if there is an error message, show the error label
	if (!error_message.empty()) {
		// write the error text in red
		ui->errorLabel->setText(QString("<font color='red'>%1</font>")
						.arg(QString::fromStdString(error_message)));
		// show the error label
		ui->errorLabel->show();
		// disable the start/stop http server button
		ui->startStopHTTPServer->setEnabled(false);
	} else {
		ui->errorLabel->hide();
		// enable the start/stop http server button
		ui->startStopHTTPServer->setEnabled(true);
	}
}
