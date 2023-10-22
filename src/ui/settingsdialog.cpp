#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "utils/config-data.h"
#include "plugin-support.h"
#include "translation-service/translation.h"

#include <QFileDialog>
#include <obs-module.h>

namespace {
// get home directory
std::string getHomeDir()
{
#ifdef _WIN32
	return std::string(getenv("USERPROFILE"));
#else
	return std::string(getenv("HOME"));
#endif
}
}

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	// populate the UI with the current settings
	this->ui->modelFile->setText(QString::fromStdString(global_config.local_model_path));
	this->ui->spmFile->setText(QString::fromStdString(global_config.local_spm_path));
	this->ui->httpPort->setText(QString::number(global_config.http_server_port));

	// Model folder selection dialog
	connect(this->ui->modelFileBtn, &QPushButton::clicked, this, [=]() {
		// Allow selection of a folder only
		QString dir = QFileDialog::getExistingDirectory(
			this, tr("Open Directory"), QString::fromStdString(getHomeDir()),
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
		if (dir != "") {
			this->ui->modelFile->setText(dir);
		}
	});

	// SPM folder selection dialog
	connect(this->ui->spmFileBtn, &QPushButton::clicked, this, [=]() {
		// Allow selection of a .model file only
		QString file = QFileDialog::getOpenFileName(this, tr("Open File"),
							    QString::fromStdString(getHomeDir()),
							    tr("Model Files (*.model)"));
		if (file != "") {
			this->ui->spmFile->setText(file);
		}
	});

	// connect to the dialog Cancel action to close the dialog
	this->connect(this->ui->buttonBox, &QDialogButtonBox::rejected, this, [=]() {
		// close the dialog
		this->close();
	});
	// connect to the dialog Save action to save the settings
	this->connect(this->ui->buttonBox, &QDialogButtonBox::accepted, this, [=]() {
		// get settings from UI into config struct
		global_config.local_model_path = this->ui->modelFile->text().toStdString();
		global_config.local_spm_path = this->ui->spmFile->text().toStdString();
		global_config.http_server_port = this->ui->httpPort->text().toUShort();

		// serialize to json and save to the OBS module settings
		if (saveConfig(false) == OBS_POLYGLOT_CONFIG_SUCCESS) {
			obs_log(LOG_INFO, "Saved settings");

			// update the plugin
			freeContext();
			if (build_translation_context() == OBS_POLYGLOT_TRANSLATION_INIT_SUCCESS) {
				obs_log(LOG_INFO, "Translation context updated");
			} else {
				obs_log(LOG_ERROR, "Failed to update translation context");
			}
		} else {
			obs_log(LOG_ERROR, "Failed to save settings");
		}

		// close the dialog
		this->close();
	});
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}
