#ifndef SPELL_H
#define SPELL_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QMessageBox>

// Класс для представления заклинания
class Spell {
public:
    // Конструктор для создания нового заклинания
    Spell(const QString &name = "", const QString &description = "")
        : name(name), description(description) {}

    // Метод для сохранения заклинания в JSON файл
    bool saveToFile(const QString &filePath) {
        // Создаем JSON объект для заклинания
        QJsonObject spellObject;
        spellObject["name"] = name;              // Сохраняем имя заклинания
        spellObject["description"] = description; // Сохраняем описание заклинания

        // Конвертируем JSON объект в текст
        QJsonDocument doc(spellObject);

        // Открываем файл для записи
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            // Показать сообщение об ошибке, если файл нельзя открыть
            QMessageBox::warning(nullptr, "Ошибка", "Не удалось создать JSON файл: " + filePath);
            return false;
        }

        // Записываем JSON данные в файл
        file.write(doc.toJson());
        file.close();
        return true; // Успешно сохранили файл
    }

    // Метод для загрузки заклинания из JSON файла
    bool loadFromFile(const QString &filePath) {
        // Открываем файл для чтения
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            // Показать сообщение об ошибке, если файл нельзя открыть
            QMessageBox::warning(nullptr, "Ошибка", "Не удалось открыть файл для чтения: " + filePath);
            return false;
        }

        // Читаем содержимое файла
        QByteArray fileData = file.readAll();
        file.close();

        // Парсим JSON данные
        QJsonDocument doc = QJsonDocument::fromJson(fileData);
        if (doc.isNull() || !doc.isObject()) {
            QMessageBox::warning(nullptr, "Ошибка", "Ошибка в формате JSON в файле: " + filePath);
            return false;
        }

        // Извлекаем данные из JSON объекта
        QJsonObject spellObject = doc.object();
        if (spellObject.contains("name") && spellObject["name"].isString()) {
            name = spellObject["name"].toString();
        }
        if (spellObject.contains("description") && spellObject["description"].isString()) {
            description = spellObject["description"].toString();
        }

        return true; // Успешно загрузили данные
    }

private:
    QString name;         // Имя заклинания
    QString description;  // Описание заклинания
};

#endif // SPELL_H
