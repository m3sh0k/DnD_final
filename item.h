#ifndef ITEM_H
#define ITEM_H

#include <QString>

// Класс для представления предмета
class Item {
public:
    // Конструкторы
    Item() = default;
    Item(const QString &name, const QString &description);

    // Методы
    bool saveToFile(const QString &filePath) const; // Сохранение в JSON файл
    static Item loadFromFile(const QString &filePath); // Загрузка из JSON файла

    // Доступ к свойствам
    QString getName() const { return name; }
    QString getDescription() const { return description; }

private:
    QString name;         // Название предмета
    QString description;  // Описание предмета
};

#endif // ITEM_H
