#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QButtonGroup>
#include <QTreeView>
#include <QFileSystemModel>
#include <QInputDialog>
#include <QMainWindow>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QModelIndex>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QPoint>
#include <QTextEdit>
#include <QTabWidget>
#include "mapviewwidget.h"  // Ваш виджет для отображения карт
#include "item.h"
#include "spell.h"
#include "form.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void setupSideMenu();                           // Настройка бокового меню
    QPushButton *activeButton = nullptr;            // Текущая активная кнопка

    void setupTreeView();                           // Настройка файлового дерева
    QFileSystemModel *fileSystemModel;              //
    QFileSystemModel *monstersModel;                //
    QFileSystemModel *spellsModel;                  //
    QFileSystemModel *itemsModel;                   //

    void addMapTab(const QString &jsonPath);                  // Функция для добавления новой вкладки с картой
    QString extractMapPathFromJson(const QString &jsonPath);  // Извлечение пути карты из JSON

    void createMonsterJson(const QString &name, const QString &imagePath);  // Объявление метода
    void createEmptyJsonFile(const QString &parentDirPath);
    void addCharacterTab(const QString &jsonPath);  // <-- Объявляем метод для добавления вкладки для персонажа

    void onTabCloseRequested(int index); // Объявление функции

    Form *formInstance;  // Указатель на форму
    mapviewwidget *mapviewWidgetInstance;  // Указатель на виджет карты

private slots:
    void on_monsters_pushButton_released();         // Переключение на страницу "Бестиарий"
    void on_spells_pushButton_released();           // Переключение на страницу "Заклинания"
    void on_item_pushButton_released();             // Переключение на страницу "Предметы"
    void on_cart_pushButton_released();             // Переключение на страницу "Карты"
    void on_category_pushButton_released(QWidget* page, QPushButton* button, int index);

    void on_cart_treeView_customContextMenuRequested(const QPoint &pos);
    void on_spells_treeView_customContextMenuRequested(const QPoint &pos);
    void on_item_treeView_customContextMenuRequested(const QPoint &pos);
    void createUniqueFolder(QDir &dir, const QString &baseName);
    void removeFileOrFolder(const QModelIndex &index);
    void addMapToDirectory(const QString &parentDirPath, const QString &filePath);
    void addSpellToDirectory(const QString &parentDirPath, const QString &filePath);
    void addItemToDirectory(const QString &parentDirPath, const QString &filePath);
    void createMapJson(const QString &jsonFilePath, const QString &imagePath);
    void on_cartItemDoubleClicked(const QModelIndex &index);  // Слот для обработки двойного клика на файле
    //void on_monster_treeView_customContextMenuRequested(const QPoint &pos);


    void on_spellItemDoubleClicked(const QModelIndex &index);
    void displaySpellContentInTab(const QString &jsonPath);
    QString extractSpellContentFromJson(const QString &jsonPath);
    void on_itemItemDoubleClicked(const QModelIndex &index);
    void displayItemContentInTab(const QString &jsonPath);
    QString extractItemContentFromJson(const QString &jsonPath);
    void createJsonFileWithDialog(const QString &parentDirPath);

    void createNewSpell(const QString &parentDirPath);
    void createNewItem(const QString &parentDirPath);
    void on_monster_treeView_customContextMenuRequested(const QPoint &pos);
    void on_monster_treeView_doubleClicked(const QModelIndex &index); // <-- Добавьте эту строку

};
#endif // MAINWINDOW_H
