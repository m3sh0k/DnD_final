#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QJsonObject>
#include <QRegion>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();



    void loadFromJsonFile(const QString &filePath);  // Метод загрузки из JSON
    void saveToJsonFile(const QString &filePath);   // Метод сохранения в JSON
    void onImageChanged(const QString &newImagePath);

signals:
    void dataChanged();  // Сигнал для уведомления об изменениях
    void imageSelected(const QString &path);

    void sendIconToMap(const QPixmap &pixmap);  // Объявление сигнала

protected:
    void mousePressEvent(QMouseEvent *event) override;  // Метод для обработки нажатия на виджет

private:
    Ui::Form *ui;
    QString imagePath;  // Переменная для хранения пути к изображению

    void onImageClicked();  // Метод для обработки нажатия на изображение
    void connectWidgets();  // Метод для подключения сигналов виджетов к dataChanged
};

#endif // FORM_H
