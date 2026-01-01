#include "Coordinate.h"



QPointF Coordinate::posRealFromView(const QPointF &view_pt) const
{
	double x = (view_pt.x() - view_size.width() / 2) / scale + real_x;
	double y = (view_pt.y() - view_size.height() / 2) / scale + real_y;
	return {x, y};
}

QPointF Coordinate::posViewFromReal(const QPointF &real_pt) const
{
	double x = (real_pt.x() - real_x) * scale + view_size.width() / 2;
	double y = (real_pt.y() - real_y) * scale + view_size.height() / 2;
	return {x, y};
}
