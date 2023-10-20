#ifndef TRANSLATEDOCKWIDGET_H
#define TRANSLATEDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class TranslateDockWidget;
}

class TranslateDockWidget : public QDockWidget {
	Q_OBJECT

public:
	explicit TranslateDockWidget(QWidget *parent = nullptr);
	~TranslateDockWidget();

private slots:
	void openSettingsDialog();

private:
	Ui::TranslateDockWidget *ui;
};

#endif // TRANSLATEDOCKWIDGET_H
