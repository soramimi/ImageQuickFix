#ifndef IMAGEGENERATOR_H
#define IMAGEGENERATOR_H

#include <QImage>



class ImageGenerator {
public:
	static QImage squared1k(QImage const &image);
	static QImage rotateRight(QImage const &image);
	static QImage rotateLeft(QImage const &image);
	static QImage rotate180deg(const QImage &image);
	static QImage flipVertical(QImage const &image);
	static QImage flipHorizontal(const QImage &image);
};

#endif // IMAGEGENERATOR_H
