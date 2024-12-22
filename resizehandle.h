#ifndef RESIZEHANDLE_H
#define RESIZEHANDLE_H

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>

class ResizeHandle : public QGraphicsRectItem
{
public:
    ResizeHandle(QGraphicsItem *parent = nullptr);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QPointF dragStartPos;  // Начальная позиция при перетаскивании
};

#endif // RESIZEHANDLE_H
