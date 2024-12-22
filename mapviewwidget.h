#ifndef MAPVIEWWIDGET_H
#define MAPVIEWWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QInputDialog>
#include <QGraphicsSceneMouseEvent>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLabel>


struct CharacterIcon {
    QString name;
    QString iconPath;
};

namespace Ui {
class mapviewwidget;
}

class mapviewwidget : public QWidget
{
    Q_OBJECT

public:
    explicit mapviewwidget(QWidget *parent = nullptr);
    ~mapviewwidget();

    // Добавьте переменную для хранения пути к изображению
    QString imagePath;

    QLabel *label_icon;  // Добавьте переменную для вашего QLabel, если она не определена
    // Загрузка карты из файла
    void loadMap(const QString &filePath);
    void createContextMenu(QPoint position);
    //bool eventFilter(QObject *object, QEvent *event);
    //void addCharacterIcon();
    //void removeCharacterIcon();
    void mousePressEvent(QMouseEvent *event);
   //QList<CharacterIcon> loadCharacterIconsFromJson(const QString &filePath);
    void handleIconRightClick(QGraphicsSceneMouseEvent *event);
    void createCharacterIconContextMenu(QGraphicsPixmapItem *item, QPoint position);
    void removeCharacterIcon(QGraphicsPixmapItem *item);


public slots:
    void addCharacterIcon(const QPixmap &iconPixmap);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;
    bool Event_Filter(QObject *object, QEvent *event);

signals:
    void iconMoved(const QPointF &newPosition); // Объявление сигнала

private:
    Ui::mapviewwidget *ui;

    QGraphicsScene *scene; // Сцена для отображения
    double currentScale;   // Текущий масштаб

    QGraphicsPixmapItem *characterItem; // Указатель на иконку персонажа

    // Новые методы для добавления и перетаскивания иконки персонажа

    void startDraggingCharacter(QMouseEvent *event);
    void stopDraggingCharacter(QMouseEvent *event);
    void moveCharacter(QMouseEvent *event);
    void updateCharacterPosition(const QPointF &newPosition);

    // Вспомогательные методы
    void setupGraphicsView();            // Настройка GraphicsView
    void handleDrag(QMouseEvent *event); // Перетаскивание карты
    void handleZoom(QWheelEvent *event); // Масштабирование карты
};

#endif // MAPVIEWWIDGET_H
