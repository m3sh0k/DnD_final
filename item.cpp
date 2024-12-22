#include "item.h"
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QMessageBox>

// Конструктор класса Item
Item::Item(const QString &name, const QString &description)
    : name(name), description(description) {}

// Метод для сохранения данных предмета в JSON файл
bool Item::saveToFile(const QString &filePath) const {
    // Создаем JSON объект с данными предмета
    QJsonObject itemObject;
    itemObject["name"] = name;
    itemObject["description"] = description;

    QJsonDocument doc(itemObject);

    // Открываем файл для записи
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Не удалось открыть файл для записи:" << filePath;
        return false;
    }

    file.write(doc.toJson()); // Записываем JSON в файл
    file.close();
    return true;
}

// Статический метод для загрузки данных предмета из JSON файла
Item Item::loadFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть файл для чтения:" << filePath;
        return Item();
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isObject()) {
        qWarning() << "Некорректный формат JSON в файле:" << filePath;
        return Item();
    }

    QJsonObject itemObject = doc.object();
    QString name = itemObject.value("name").toString();
    QString description = itemObject.value("description").toString();

    return Item(name, description);
}
