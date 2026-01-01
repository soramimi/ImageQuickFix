#ifndef COORDINATE_H
#define COORDINATE_H

#include <QPointF>
#include <QSize>



class Coordinate {
public:
	double scale = 1;              //!< 拡大率
	double real_x = 0;             //!< 表示中心の実座標X
	double real_y = 0;             //!< 表示中心の実座標Y
	double real_w = 0;             //!< 画像実幅
	double real_h = 0;             //!< 画像実高
	QSize view_size;
public:
	/** @brief ビュー座標→実座標変換。 */
	QPointF posRealFromView(QPointF const &view_pt) const;

	/** @brief 実座標→ビュー座標変換。 */
	QPointF posViewFromReal(QPointF const &real_pt) const;
};


#endif // COORDINATE_H
