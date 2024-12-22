#include "form.h"
#include "ui_form.h"
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QPixmap>
#include <QMouseEvent>
#include <QBuffer>
#include <QPainter>
#include "mapviewwidget.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form),
    imagePath("")  // Инициализируем путь к изображению пустым значением
{
    ui->setupUi(this);
    // Устанавливаем многострочный текст в QLabel
    ui->label_icon->setText("Выберите изображение");

    // Включаем перенос текста
    ui->label_icon->setWordWrap(true);

    // Подключаем сигнал dataChanged к нужным виджетам
    connectWidgets();

    // Сигнал для передачи иконки в карту
    auto mapWidget = dynamic_cast<mapviewwidget*>(parent);
    if (mapWidget) {
        connect(this, &Form::sendIconToMap, mapWidget, &mapviewwidget::addCharacterIcon);
    } else {
        qWarning() << "Parent is not of type mapviewwidget!";
    }
}

Form::~Form()
{
    delete ui;
}

void Form::connectWidgets()
{
    connect(ui->lineEdit_name, &QLineEdit::textChanged, this, &Form::dataChanged);
    connect(ui->lineEdit_race, &QLineEdit::textChanged, this, &Form::dataChanged);
    connect(ui->lineEdit_Size, &QLineEdit::textChanged, this, &Form::dataChanged);
    connect(ui->spinBox_health, QOverload<int>::of(&QSpinBox::valueChanged), this, &Form::dataChanged);
    connect(ui->spinBox_mastership_bonus, QOverload<int>::of(&QSpinBox::valueChanged), this, &Form::dataChanged);
    connect(ui->lineEdit_worldview, &QLineEdit::textChanged, this, &Form::dataChanged);
    connect(ui->lineEdit_class, &QLineEdit::textChanged, this, &Form::dataChanged);
    connect(ui->comboBox_The_main_characteristic, &QComboBox::currentTextChanged, this, &Form::dataChanged);
}

void Form::mousePressEvent(QMouseEvent *event)
{
    // Проверяем, был ли клик по label_icon
    if (ui->label_icon->rect().contains(event->pos())) {
        onImageClicked();  // Если да, то вызываем функцию для выбора изображения
    }
    QWidget::mousePressEvent(event);  // Важно вызвать базовый класс для нормальной работы других событий
}

void Form::onImageClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Выберите изображение"), "", tr("Images (*.png *.jpg *.bmp *.jfif)"));
    if (!fileName.isEmpty()) {
        QPixmap pixmap(fileName);
        if (!pixmap.isNull()) {
            QPixmap scaledPixmap = pixmap.scaled(ui->label_icon->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            QPixmap croppedPixmap = scaledPixmap.copy(0, 0, ui->label_icon->width(), ui->label_icon->height());

            QBitmap mask(ui->label_icon->size());
            mask.fill(Qt::color0);

            QPainter painter(&mask);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setBrush(Qt::color1);
            painter.drawEllipse(0, 0, mask.width(), mask.height());
            painter.end();

            croppedPixmap.setMask(mask);
            ui->label_icon->setPixmap(croppedPixmap);
            imagePath = fileName;
            emit imageSelected(fileName);

            // Генерируем сигнал для передачи иконки в карту
            emit sendIconToMap(croppedPixmap);  // Здесь все в порядке
        }
    }
}

void Form::onImageChanged(const QString &newImagePath) {
    // Обновляем путь к изображению
    imagePath = newImagePath;
}

void Form::saveToJsonFile(const QString &filePath)
{
    QJsonObject jsonObject;

    // Сохраняем данные из виджетов
    jsonObject["name"] = ui->lineEdit_name->text();
    jsonObject["race"] = ui->lineEdit_race->text();
    jsonObject["size"] = ui->lineEdit_Size->text();
    jsonObject["health"] = ui->spinBox_health->value();
    jsonObject["mastership_bonus"] = ui->spinBox_mastership_bonus->value();
    jsonObject["worldview"] = ui->lineEdit_worldview->text();
    jsonObject["class"] = ui->lineEdit_class->text();
    jsonObject["main_characteristic"] = ui->comboBox_The_main_characteristic->currentText();

    // Сохраняем изображение в файл
    if (!imagePath.isEmpty()) {
        // Проверяем существование папки "images/"
        QDir dir("images");
        if (!dir.exists()) {
            dir.mkpath(".");  // Создаем папку, если она не существует
        }

        QFile imageFile(imagePath);
        QFileInfo fileInfo(imageFile);
        QString newImagePath = QDir::current().absoluteFilePath("images/" + fileInfo.fileName());

        // Копируем изображение в новую папку
        QFile::copy(imagePath, newImagePath);
        jsonObject["imagePath"] = newImagePath;  // Сохраняем новый путь изображения
    }

    QJsonDocument jsonDoc(jsonObject);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Не удалось открыть файл для записи:" << filePath;
        return;
    }

    file.write(jsonDoc.toJson());
    file.close();
}




void Form::loadFromJsonFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть файл:" << filePath;
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isObject()) {
        qWarning() << "Файл не содержит валидный JSON объект:" << filePath;
        return;
    }

    QJsonObject jsonObject = doc.object();

    // Заполняем виджеты данными
    ui->lineEdit_name->setText(jsonObject["name"].toString());
    ui->lineEdit_race->setText(jsonObject["race"].toString());
    ui->lineEdit_Size->setText(jsonObject["size"].toString());
    ui->spinBox_health->setValue(jsonObject["health"].toInt());
    ui->spinBox_mastership_bonus->setValue(jsonObject["mastership_bonus"].toInt());
    ui->lineEdit_worldview->setText(jsonObject["worldview"].toString());
    ui->lineEdit_class->setText(jsonObject["class"].toString());
    ui->comboBox_The_main_characteristic->setCurrentText(jsonObject["main_characteristic"].toString());

    // Загружаем изображение
    imagePath = jsonObject["imagePath"].toString();  // Обновляем путь к изображению
    qDebug() << "Загружаем путь к изображению:" << imagePath;

    // Проверяем, существует ли файл изображения
    QFileInfo fileInfo(imagePath);
    if (fileInfo.exists()) {
        QPixmap pixmap(imagePath);
        if (!pixmap.isNull()) {
            // Масштабируем изображение
            QPixmap scaledPixmap = pixmap.scaled(ui->label_icon->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

            // Центрируем обрезку по области label_icon
            QPixmap croppedPixmap = scaledPixmap.copy(0, 0, ui->label_icon->width(), ui->label_icon->height());

            // Создаем маску в виде круга с использованием QBitmap
            QBitmap mask(ui->label_icon->size());
            mask.fill(Qt::color0);  // Начинаем с черного фона

            QPainter painter(&mask);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setBrush(Qt::color1);  // Белый круг
            painter.drawEllipse(0, 0, mask.width(), mask.height());
            painter.end();

            // Применяем маску
            croppedPixmap.setMask(mask);

            // Устанавливаем обрезанное и маскированное изображение в QLabel
            ui->label_icon->setPixmap(croppedPixmap);
        } else {
            qWarning() << "Не удалось загрузить изображение по пути:" << imagePath;
        }
    } else {
        qWarning() << "Файл изображения не существует:" << imagePath;
    }
}




