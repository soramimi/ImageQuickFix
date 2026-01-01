#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MySettings.h"
#include "SettingsDialog.h"
#include "main.h"
#include "ImageGenerator.h"
#include "TrimDialog.h"
#include "Coordinate.h"
#include <QClipboard>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>

/**
 * @brief MainWindow のプライベート実装構造体。
 */
struct MainWindow::Private {
	QBrush checkerboard_brush;
	QImage current_image;
};

/**
 * @brief コンストラクタ。UI 初期化と設定値(フィット/ウィンドウ状態)復元を行う。
 * @param parent 親ウィジェット
 */
MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, m(new Private)
{
	ui->setupUi(this);
	
	// 画像フィット設定を復元
	{
		MySettings settings;
#if 0
		settings.beginGroup("global");
		bool f = settings.value("FitImageToView", false).toBool();
		settings.endGroup();
#endif
		bool f = true;
		ui->widget_image_viewer->fitImageToView(f);
		ui->action_fit_image_to_view->setChecked(f);
	}

	// ウィンドウ位置・サイズの復元
	{
		if (global->appsettings.remember_and_restore_window_position) {
			Qt::WindowStates state = windowState();

			MySettings settings;
			settings.beginGroup("MainWindow");
			bool maximized = settings.value("Maximized").toBool();
			restoreGeometry(settings.value("Geometry").toByteArray());
			settings.endGroup();

			if (maximized) {
				state |= Qt::WindowMaximized;
				setWindowState(state);
			}
		}
	}

	{
		QImage image(16, 16, QImage::Format_RGB888);
		for (int y = 0; y < 16; y++) {
			uint8_t *p = (uint8_t *)image.scanLine(y);
			for (int x = 0; x < 16; x++) {
				uint8_t v = ((x ^ y) & 8) ? 192 : 255;
				p[3 * x + 0] = v;
				p[3 * x + 1] = v;
				p[3 * x + 2] = v;
			}
		}
		m->checkerboard_brush = QBrush(image);
	}
}

/**
 * @brief デストラクタ。所有リソースを破棄。
 */
MainWindow::~MainWindow()
{
	delete m;
	delete ui;
}

/**
 *
 */
void MainWindow::setImage(QImage const &image)
{
	m->current_image = image;
	ui->widget_image_viewer->setImage(m->current_image);

	int w = m->current_image.width();
	int h = m->current_image.height();
	ui->action_remove_frame->setEnabled(w == 2048 && h == 1440);
}

/**
 * @brief 画像ファイルを開いて表示する。
 * @param path 画像ファイルパス
 * @return 成功時 true / 失敗時 false
 */
bool MainWindow::openFile(QString const &path)
{
	QImage image;
	image.load(path);
	if (!image.isNull()) {
		setImage(image);
		return true;
	}
	return false;
}

/**
 * @brief [ファイルを開く] アクション処理。ダイアログ表示後、選択画像を読み込む。
 */
void MainWindow::on_action_file_open_triggered()
{
	QString path;

	MySettings s;
	s.beginGroup("global");
	path = s.value("RecentFile").toString();

	path = QFileDialog::getOpenFileName(this, "Open", path, "Image Files (*.jpg *.jpeg *.png);;All Files (*.*)");

	if (!path.isEmpty()) {
		s.setValue("RecentFile", path);

		if (!openFile(path)) {
			QMessageBox::critical(this, tr("File Open Error"), tr("Failed to open the file."));
			return;
		}
	}
}

/**
 * @brief ウィンドウクローズ時の処理。位置/サイズを必要に応じて保存。
 * @param event クローズイベント
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
	MySettings settings;

	if (global->appsettings.remember_and_restore_window_position) {
		setWindowOpacity(0); // フリッカー抑制
		Qt::WindowStates state = windowState();
		bool maximized = (state & Qt::WindowMaximized) != 0;
		if (maximized) {
			state &= ~Qt::WindowMaximized;
			setWindowState(state);
		}
		{
			settings.beginGroup("MainWindow");
			settings.setValue("Maximized", maximized);
			settings.setValue("Geometry", saveGeometry());
			settings.endGroup();
		}
	}

	QMainWindow::closeEvent(event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if (QApplication::modalWindow()) return;

	if (event->mimeData()->hasUrls()) {
		event->setDropAction(Qt::CopyAction);
		event->accept();
		return;
	}
	QMainWindow::dragEnterEvent(event);
}

void MainWindow::dropEvent(QDropEvent *event)
{
	if (QApplication::modalWindow()) return;

	if (0) {
		QMimeData const *mimedata = event->mimeData();
		QByteArray encoded = mimedata->data("application/x-qabstractitemmodeldatalist");
		QDataStream stream(&encoded, QIODevice::ReadOnly);
		while (!stream.atEnd()) {
			int row, col;
			QMap<int,  QVariant> roledatamap;
			stream >> row >> col >> roledatamap;
		}
	}

	if (event->mimeData()->hasUrls()) {
		QStringList paths;
		QByteArray ba = event->mimeData()->data("text/uri-list");
		if (ba.size() > 4 && memcmp(ba.data(), "h\0t\0t\0p\0", 8) == 0) {
			QString path = QString::fromUtf16((ushort const *)ba.data(), ba.size() / 2);
			int i = path.indexOf('\n');
			if (i >= 0) {
				path = path.mid(0, i);
			}
			if (!path.isEmpty()) {
				paths.push_back(path);
			}
		} else {
			QList<QUrl> urls = event->mimeData()->urls();
			for (QUrl const &url : urls) {
				QString path = url.url();
				paths.push_back(path);
			}
		}
		for (QString const &path : paths) {
			if (path.startsWith("file://")) {
				int i = 7;
#ifdef Q_OS_WIN
				if (path.utf16()[i] == '/') {
					i++;
				}
#endif
				openFile(path.mid(i));
			} else if (path.startsWith("http://") || path.startsWith("https://")) {
			}
		}
	}
}

/**
 * @brief [フィット] チェック状態変更時の処理。内部ビューへ反映し設定保存。
 */
void MainWindow::on_action_fit_image_to_view_changed()
{
	bool f = ui->action_fit_image_to_view->isChecked();
	ui->widget_image_viewer->fitImageToView(f);

#if 0
	MySettings s;
	s.beginGroup("global");
	s.setValue("FitImageToView", f);
#endif
}

/**
 * @brief 現在表示中画像をクリップボードへコピー。
 */
void MainWindow::on_action_copy_triggered()
{
	QImage img = ui->widget_image_viewer->image();
	qApp->clipboard()->setImage(img);
}

/**
 * @brief 設定ダイアログ表示。OK の場合は設定を反映。
 */
void MainWindow::on_action_settings_triggered()
{
	SettingsDialog dlg(this);
	if (dlg.exec() == QDialog::Accepted) {
		ApplicationSettings const &newsettings = dlg.settings();
		global->appsettings = newsettings;
	}
}

void MainWindow::on_action_new_from_clipboard_triggered()
{
	QImage image = qApp->clipboard()->image();
	if (!image.isNull()) {
		setImage(image);
	}
}

void MainWindow::on_action_edit_squared_triggered()
{
	QImage image = ImageGenerator::squared1k(m->current_image);
	setImage(image);
}

void MainWindow::on_action_edit_rotate_right_triggered()
{
	QImage image = ImageGenerator::rotateRight(m->current_image);
	setImage(image);
}


void MainWindow::on_action_edit_rotate_left_triggered()
{
	QImage image = ImageGenerator::rotateLeft(m->current_image);
	setImage(image);
}

void MainWindow::on_action_edit_rotate_180deg_triggered()
{
	QImage image = ImageGenerator::rotate180deg(m->current_image);
	setImage(image);
}

void MainWindow::on_action_edit_flip_vertical_triggered()
{
	QImage image = ImageGenerator::flipVertical(m->current_image);
	setImage(image);
}


void MainWindow::on_action_edit_flip_horizontal_triggered()
{
	QImage image = ImageGenerator::flipHorizontal(m->current_image);
	setImage(image);
}

void MainWindow::on_action_save_square_image_triggered()
{
	QImage image = ImageGenerator::squared1k(m->current_image);
	if (image.width() > 0 && image.height() > 0) {
		MySettings s;
		s.beginGroup("global");
		QString path = s.value("SaveDir").toString();
		path = QFileDialog::getSaveFileName(this, "Save Squared Image", path, "PNG Image (*.png);;JPEG Image (*.jpg *.jpeg);;All Files (*.*)");
		if (!path.isEmpty()) {
			QString dir = QFileInfo(path).absolutePath();
			s.setValue("SaveDir", dir);
			image.save(path);
		}
	}
}

void MainWindow::on_action_save_as_triggered()
{
	if (m->current_image.width() > 0 && m->current_image.height() > 0) {
		MySettings s;
		s.beginGroup("global");
		QString path = s.value("SaveDir").toString();
		path = QFileDialog::getSaveFileName(this, "Save Image As", path, "PNG Image (*.png);;JPEG Image (*.jpg *.jpeg);;All Files (*.*)");
		if (!path.isEmpty()) {
			QString dir = QFileInfo(path).absolutePath();
			s.setValue("SaveDir", dir);
			m->current_image.save(path);
		}
	}
}

void MainWindow::on_action_remove_frame_triggered()
{
	int w = m->current_image.width();
	int h = m->current_image.height();
	if (w == 2048 && h == 1440) {
		TrimDialog dlg(this);
		dlg.setImage(m->current_image);
		if (dlg.exec() == QDialog::Accepted) {
			QImage image = dlg.trimmedImage();
			setImage(image);
		}
	}
}

