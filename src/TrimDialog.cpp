#include "Coordinate.h"
#include "TrimDialog.h"
#include "ui_TrimDialog.h"

#include <QPainter>
#include <QPainterPath>

TrimDialog::TrimDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::TrimDialog)
{
	ui->setupUi(this);

	auto fn = [](QPainter *painter, Coordinate const &coord, void *cookie){
		// TrimDialog *self = static_cast<TrimDialog *>(cookie);
		QPointF topleft = coord.posViewFromReal(QPointF(64, 69));
		QPointF bottomright = coord.posViewFromReal(QPointF(64 + 1920, 69 + 1080));
		int x = int(topleft.x());
		int y = int(topleft.y());
		int w = int(bottomright.x()) - x;
		int h = int(bottomright.y()) - y;
		{
			QPainterPath path;
			QPainterPath inner_path;
			path.addRect(0, 0, coord.view_size.width(), coord.view_size.height());
			inner_path.addRect(x, y, w, h);
			path = path.subtracted(inner_path);
			painter->fillPath(path, QColor(255, 0, 0, 128));
		}
		painter->setPen(QPen(Qt::red, 3));
		painter->setBrush(Qt::NoBrush);
		painter->drawRect(x, y, w, h);
	};
	ui->widget->setOverlayPainter(fn, this);
}

TrimDialog::~TrimDialog()
{
	delete ui;
}

void TrimDialog::setImage(const QImage &image)
{
	ui->widget->setImage(image);
	ui->widget->fitImageToView(true);
}

QImage TrimDialog::trimmedImage() const
{
	QImage const &image = ui->widget->image();
	return image.copy(64, 69, 1920, 1080);
}
