#ifndef TRIMDIALOG_H
#define TRIMDIALOG_H

#include <QDialog>

namespace Ui {
class TrimDialog;
}

class TrimDialog : public QDialog {
	Q_OBJECT

private:
	Ui::TrimDialog *ui;
public:
	explicit TrimDialog(QWidget *parent = nullptr);
	~TrimDialog();
	void setImage(QImage const &image);
	QImage trimmedImage() const;
};

#endif // TRIMDIALOG_H
