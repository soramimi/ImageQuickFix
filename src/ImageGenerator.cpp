#include "ImageGenerator.h"

#include <QPainter>




QImage ImageGenerator::squared1k(const QImage &image)
{
	QImage new_image = image.convertToFormat(QImage::Format_RGBA8888);
	int w = new_image.width();
	int h = new_image.height();
	if (w < 1 || h < 1) return {};

	int size = std::max(w, h);
	size = std::min(size, 1024);

	QImage scaled_image;
	if (w > size || h > size) {
		if (w > h) {
			scaled_image = new_image.scaled(size, h * size / w, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		} else {
			scaled_image = new_image.scaled(w * size / h, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		}
	} else {
		scaled_image = new_image;
	}
	new_image = QImage(size, size, QImage::Format_RGBA8888);
	new_image.fill(Qt::transparent);
	{
		QPainter pr(&new_image);
		pr.drawImage((size - scaled_image.width()) / 2, (size - scaled_image.height()) / 2, scaled_image);
	}
	return new_image;
}

QImage ImageGenerator::rotateRight(const QImage &image)
{
	QImage src_image = image.convertToFormat(QImage::Format_RGBA8888);
	int w = src_image.width();
	int h = src_image.height();
	if (w < 1 || h < 1) return {};
	QImage new_image(h, w, QImage::Format_RGBA8888);
	for (int y = 0; y < h; y++) {
		uint32_t const *src = (uint32_t const *)src_image.scanLine(y);
		uint32_t *dst = (uint32_t *)new_image.scanLine(0) + h - y - 1;
		for (int x = 0; x < w; x++) {
			*dst = src[x];
			dst += h;
		}
	}
	return new_image;
}

QImage ImageGenerator::rotateLeft(const QImage &image)
{
	QImage src_image = image.convertToFormat(QImage::Format_RGBA8888);
	int w = src_image.width();
	int h = src_image.height();
	if (w < 1 || h < 1) return {};
	QImage new_image(h, w, QImage::Format_RGBA8888);
	for (int y = 0; y < h; y++) {
		uint32_t const *src = (uint32_t const *)src_image.scanLine(y);
		uint32_t *dst = (uint32_t *)new_image.scanLine(0) + h * w + y;
		for (int x = 0; x < w; x++) {
			dst -= h;
			*dst = src[x];
		}
	}
	return new_image;
}

QImage ImageGenerator::rotate180deg(const QImage &image)
{
	QImage src_image = image.convertToFormat(QImage::Format_RGBA8888);
	int w = src_image.width();
	int h = src_image.height();
	if (w < 1 || h < 1) return {};
	QImage new_image(w, h, QImage::Format_RGBA8888);
	for (int y = 0; y < h; y++) {
		uint32_t const *src = (uint32_t const *)src_image.scanLine(y);
		uint32_t *dst = (uint32_t *)new_image.scanLine(h - y - 1);
		for (int x = 0; x < w; x++) {
			dst[w - x - 1] = src[x];
		}
	}
	return new_image;
}

QImage ImageGenerator::flipVertical(const QImage &image)
{
	QImage src_image = image.convertToFormat(QImage::Format_RGBA8888);
	int w = src_image.width();
	int h = src_image.height();
	if (w < 1 || h < 1) return {};
	QImage new_image(w, h, QImage::Format_RGBA8888);
	for (int y = 0; y < h; y++) {
		uint32_t const *src = (uint32_t const *)src_image.scanLine(y);
		uint32_t *dst = (uint32_t *)new_image.scanLine(h - y - 1);
		for (int x = 0; x < w; x++) {
			dst[x] = src[x];
		}
	}
	return new_image;
}

QImage ImageGenerator::flipHorizontal(const QImage &image)
{
	QImage src_image = image.convertToFormat(QImage::Format_RGBA8888);
	int w = src_image.width();
	int h = src_image.height();
	if (w < 1 || h < 1) return {};
	QImage new_image(w, h, QImage::Format_RGBA8888);
	for (int y = 0; y < h; y++) {
		uint32_t const *src = (uint32_t const *)src_image.scanLine(y);
		uint32_t *dst = (uint32_t *)new_image.scanLine(y);
		for (int x = 0; x < w; x++) {
			dst[w - x - 1] = src[x];
		}
	}
	return new_image;
}
