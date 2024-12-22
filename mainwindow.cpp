#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonObject>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , formInstance(new Form(this))  // Создаем экземпляр формы
    , mapviewWidgetInstance(new mapviewwidget(this))  // Создаем экземпляр виджета карты
{
    ui->setupUi(this);
    setupSideMenu();  // Настраиваем боковое меню
    setupTreeView();  // Настраиваем файловое дерево

    connect(ui->cart_treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::on_cart_treeView_customContextMenuRequested);
    connect(ui->cart_treeView, &QTreeView::doubleClicked, this, &MainWindow::on_cartItemDoubleClicked);
    //connect(ui->monster_treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::on_monster_treeView_customContextMenuRequested);

    connect(ui->spells_treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::on_spells_treeView_customContextMenuRequested); // Подключение контекстного меню для заклинаний
    connect(ui->spells_treeView, &QTreeView::doubleClicked, this, &MainWindow::on_spellItemDoubleClicked);
    connect(ui->item_treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::on_item_treeView_customContextMenuRequested);     // Подключение контекстного меню для предметов
    connect(ui->item_treeView, &QTreeView::doubleClicked, this, &MainWindow::on_itemItemDoubleClicked);

    connect(ui->monster_treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::on_monster_treeView_customContextMenuRequested);     // Подключение контекстного меню для предметов
    connect(ui->monster_treeView, &QTreeView::doubleClicked, this, &MainWindow::on_monster_treeView_doubleClicked);

    // Включаем кнопки закрытия вкладок
    ui->tabWidget->setTabsClosable(true);

    // Подключаем сигнал закрытия вкладки
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onTabCloseRequested(int index)
{
    // Получаем виджет вкладки
    QWidget *tab = ui->tabWidget->widget(index);
    if (tab) {
        ui->tabWidget->removeTab(index); // Удаляем вкладку из TabWidget
        delete tab; // Удаляем сам виджет, чтобы освободить память
    }
}


// Настройка бокового меню
void MainWindow::setupSideMenu()
{
    // Создаем группу кнопок и добавляем кнопки
    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->monsters_pushButton);  // Кнопка "Бестиарий"
    buttonGroup->addButton(ui->spells_pushButton);    // Кнопка "Заклинания"
    buttonGroup->addButton(ui->item_pushButton);      // Кнопка "Предметы"
    buttonGroup->addButton(ui->cart_pushButton);      // Кнопка "Карты"
    buttonGroup->setExclusive(true);                 // Группа делает кнопки эксклюзивными
}

// Универсальная функция для переключения вкладок
void MainWindow::on_category_pushButton_released(QWidget* /*page*/, QPushButton* button, int index) {
    if (activeButton == button) {
        ui->extended_side_menu->setVisible(!ui->extended_side_menu->isVisible());
    } else {
        activeButton = button;
        ui->extended_side_menu->setCurrentIndex(index); // Устанавливаем нужную страницу
        ui->extended_side_menu->setVisible(true);
    }
}

// Функции для кнопок
void MainWindow::on_monsters_pushButton_released()
{
    on_category_pushButton_released(ui->extended_side_menu, ui->monsters_pushButton, 0);
}

void MainWindow::on_spells_pushButton_released()
{
    on_category_pushButton_released(ui->extended_side_menu, ui->spells_pushButton, 1);
}

void MainWindow::on_item_pushButton_released()
{
    on_category_pushButton_released(ui->extended_side_menu, ui->item_pushButton, 2);
}

void MainWindow::on_cart_pushButton_released()
{
    on_category_pushButton_released(ui->extended_side_menu, ui->cart_pushButton, 3);
}

// Настройка файлового дерева
void MainWindow::setupTreeView()
{
    // Путь к папкам
    QString mapsPath = QDir::currentPath() + "/map";
    QString monstersPath = QDir::currentPath() + "/monsters";
    QString spellsPath = QDir::currentPath() + "/spells";
    QString itemsPath = QDir::currentPath() + "/items";

    // Проверка и создание папок, если их нет
    QDir dir;
    dir.mkpath(mapsPath);
    dir.mkpath(monstersPath);
    dir.mkpath(spellsPath);
    dir.mkpath(itemsPath);

    // Создание моделей файлов для каждой категории
    fileSystemModel = new QFileSystemModel(this);
    fileSystemModel->setRootPath(mapsPath);
    ui->cart_treeView->setModel(fileSystemModel);
    ui->cart_treeView->setRootIndex(fileSystemModel->index(mapsPath));

    monstersModel = new QFileSystemModel(this);
    monstersModel->setRootPath(monstersPath);
    ui->monster_treeView->setModel(monstersModel);
    ui->monster_treeView->setRootIndex(monstersModel->index(monstersPath));

    spellsModel = new QFileSystemModel(this);
    spellsModel->setRootPath(spellsPath);
    ui->spells_treeView->setModel(spellsModel);
    ui->spells_treeView->setRootIndex(spellsModel->index(spellsPath));

    itemsModel = new QFileSystemModel(this);
    itemsModel->setRootPath(itemsPath);
    ui->item_treeView->setModel(itemsModel);
    ui->item_treeView->setRootIndex(itemsModel->index(itemsPath));

    // Скрытие ненужных столбцов
    QList<QTreeView*> treeViews = {ui->cart_treeView, ui->monster_treeView, ui->spells_treeView, ui->item_treeView};
    for (auto treeView : treeViews) {
        treeView->hideColumn(1);  // Скрываем "Size"
        treeView->hideColumn(2);  // Скрываем "Type"
        treeView->hideColumn(3);  // Скрываем "Date"
        treeView->setHeaderHidden(true);  // Отключаем заголовки
        treeView->setContextMenuPolicy(Qt::CustomContextMenu);  // Включаем контекстное меню
    }
}

void MainWindow::on_cart_treeView_customContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu(this);
    QAction *addAction = contextMenu.addAction("Добавить папку");
    QAction *addMapAction = contextMenu.addAction("Добавить карту");
    QAction *removeAction = contextMenu.addAction("Удалить");

    // Показываем меню в точке клика
    QAction *selectedAction = contextMenu.exec(ui->cart_treeView->mapToGlobal(pos));

    QModelIndex index = ui->cart_treeView->indexAt(pos);
    QString parentDirPath = index.isValid() ? fileSystemModel->filePath(index) : QDir::currentPath() + "/map";

    QDir dir(parentDirPath);

    // Создаем папку /map, если она не существует
    if (!dir.exists() && !parentDirPath.endsWith("/map")) {
        dir.mkdir(parentDirPath);
    }

    if (selectedAction == addAction) {
        createUniqueFolder(dir, "Новая папка");
    } else if (selectedAction == addMapAction) {
        QString filePath = QFileDialog::getOpenFileName(this, tr("Выберите карту"), QDir::homePath(),
                                                        tr("Images (*.png *.jpg *.jpeg);;PDF Files (*.pdf);;All Files (*)"));
        if (filePath.isEmpty()) return;
        addMapToDirectory(parentDirPath, filePath);  // Теперь добавляем только JSON
    } else if (selectedAction == removeAction) {
        if (index.isValid()) {
            removeFileOrFolder(index);
        }
    }
}

void MainWindow::createUniqueFolder(QDir &dir, const QString &baseName)
{
    QString folderName = baseName;
    int counter = 1;

    // Проверяем, существует ли папка с таким именем
    while (dir.exists(folderName)) {
        folderName = baseName + " " + QString::number(counter++);
    }

    if (dir.mkdir(folderName)) {
        fileSystemModel->setRootPath(QDir::currentPath());
    }
}

void MainWindow::addMapToDirectory(const QString &parentDirPath, const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString jsonFilePath = parentDirPath + "/" + fileInfo.completeBaseName() + ".json";  // Имя для JSON файла

    // Создаем JSON с путем к изображению
    createMapJson(jsonFilePath, filePath);

    // Перезагружаем модель
    fileSystemModel->setRootPath(QDir::currentPath());
}

void MainWindow::createMapJson(const QString &jsonFilePath, const QString &imagePath)
{
    QJsonObject mapObject;
    mapObject["imagePath"] = imagePath;
    mapObject["width"] = 1000;  // Пример: ширина карты (если нужно)
    mapObject["height"] = 800;  // Пример: высота карты (если нужно)

    QJsonDocument doc(mapObject);
    QFile jsonFile(jsonFilePath);
    if (jsonFile.open(QIODevice::WriteOnly)) {
        jsonFile.write(doc.toJson());
        jsonFile.close();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось создать JSON файл.");
    }
}

void MainWindow::removeFileOrFolder(const QModelIndex &index)
{
    QString filePath = fileSystemModel->filePath(index);
    QFileInfo fileInfo(filePath);

    if (fileInfo.isDir()) {
        QDir dir(filePath);
        dir.removeRecursively();
    } else {
        QFile file(filePath);
        file.remove();
    }
}

void MainWindow::on_cartItemDoubleClicked(const QModelIndex &index)
{
    // Получаем путь к JSON файлу
    QString jsonPath = fileSystemModel->filePath(index);

    // Проверим, что это файл .json
    if (jsonPath.endsWith(".json")) {
        // Если файл JSON - добавляем карту
        addMapTab(jsonPath);  // Ваша функция добавления новой вкладки с картой
    } else {
        qDebug() << "Выбран не JSON файл";
    }
}

void MainWindow::addMapTab(const QString &jsonPath)
{
    // Создание нового виджета для отображения карты
    mapviewwidget *newMapWidget = new mapviewwidget(ui->tabWidget);  // Важно указать родительский элемент, например ui->tabWidget

    // Загружаем путь к изображению из JSON
    QString mapPath = extractMapPathFromJson(jsonPath);
    newMapWidget->loadMap(mapPath);  // Загружаем карту

    // Устанавливаем атрибут для удаления при закрытии
    newMapWidget->setAttribute(Qt::WA_DeleteOnClose);

    // Добавляем новую вкладку в tabWidget
    int index = ui->tabWidget->addTab(newMapWidget, QFileInfo(jsonPath).baseName());
    ui->tabWidget->setCurrentIndex(index);
}

QString MainWindow::extractMapPathFromJson(const QString &jsonPath)
{
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть JSON файл:" << jsonPath;
        return "";
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isObject()) {
        qWarning() << "Некорректный формат JSON";
        return "";
    }

    // Извлекаем путь к изображению
    QJsonObject obj = doc.object();
    return obj["imagePath"].toString();  // Убедитесь, что путь к изображению правильно записан в JSON
}



// Добавляем новые функции для обработки контекстного меню в боковом меню для вкладок "Предметы" и "Заклинания"

// Функция для вызова контекстного меню в боковом меню вкладки "Заклинания"
void MainWindow::on_spells_treeView_customContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu(this);
    QAction *addAction = contextMenu.addAction("Добавить папку");
    QAction *addSpellAction = contextMenu.addAction("Создать заклинание");
    QAction *removeAction = contextMenu.addAction("Удалить");

    QAction *selectedAction = contextMenu.exec(ui->spells_treeView->mapToGlobal(pos));

    QModelIndex index = ui->spells_treeView->indexAt(pos);
    QString parentDirPath = index.isValid() ? spellsModel->filePath(index) : QDir::currentPath() + "/spells";

    QDir dir(parentDirPath);

    if (!dir.exists() && !parentDirPath.endsWith("/spells")) {
        dir.mkdir(parentDirPath);
    }

    if (selectedAction == addAction) {
        createUniqueFolder(dir, "Новая папка");
    } else if (selectedAction == addSpellAction) {
        createNewSpell(parentDirPath);  // Заменяем вызов на правильный
    } else if (selectedAction == removeAction) {
        if (index.isValid()) {
            removeFileOrFolder(index);
        }
    }
}

void MainWindow::createNewSpell(const QString &parentDirPath)
{
    // Диалог для ввода имени предмета
    QString spellName = QInputDialog::getText(this, "Создание заклинания", "Введите название заклинания:");
    if (spellName.isEmpty()) return;

    QString spellDescription = QInputDialog::getText(this, "Создание заклинания", "Введите описание заклинания:");
    if (spellDescription.isEmpty()) return;


    // Создаем экземпляр предмета
    Spell newSpell(spellName, spellDescription);

    // Определяем путь для JSON файла
    QString jsonFilePath = parentDirPath + "/" + spellName + ".json";

    // Сохраняем данные предмета в JSON файл
    if (!newSpell.saveToFile(jsonFilePath)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить предмет.");
    }

    // Обновляем модель файловой системы
    spellsModel->setRootPath(QDir::currentPath());
}

// Функция для вызова контекстного меню в боковом меню вкладки "Предметы"
void MainWindow::on_item_treeView_customContextMenuRequested(const QPoint &pos) {
    QMenu contextMenu(this);
    QAction *addAction = contextMenu.addAction("Добавить папку");
    QAction *addItemAction = contextMenu.addAction("Добавить предмет");
    QAction *removeAction = contextMenu.addAction("Удалить");

    QAction *selectedAction = contextMenu.exec(ui->item_treeView->mapToGlobal(pos));

    QModelIndex index = ui->item_treeView->indexAt(pos);
    QString parentDirPath = index.isValid() ? itemsModel->filePath(index) : QDir::currentPath() + "/items";

    QDir dir(parentDirPath);

    if (!dir.exists() && !parentDirPath.endsWith("/items")) {
        dir.mkdir(parentDirPath);
    }

    if (selectedAction == addAction) {
        createUniqueFolder(dir, "Новая папка");
    } else if (selectedAction == addItemAction) {
        createNewItem(parentDirPath); // Новый метод для создания предмета
    } else if (selectedAction == removeAction) {
        if (index.isValid()) {
            removeFileOrFolder(index);
        }
    }
}

void MainWindow::createNewItem(const QString &parentDirPath) {
    // Диалог для ввода имени предмета
    QString itemName = QInputDialog::getText(this, "Создание предмета", "Введите название предмета:");
    if (itemName.isEmpty()) return;

    // Диалог для ввода описания предмета
    QString itemDescription = QInputDialog::getText(this, "Создание предмета", "Введите описание предмета:");
    if (itemDescription.isEmpty()) return;

    // Создаем экземпляр предмета
    Item newItem(itemName, itemDescription);

    // Определяем путь для JSON файла
    QString jsonFilePath = parentDirPath + "/" + itemName + ".json";

    // Сохраняем данные предмета в JSON файл
    if (!newItem.saveToFile(jsonFilePath)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить предмет.");
    }

    // Обновляем модель файловой системы
    itemsModel->setRootPath(QDir::currentPath());
}

// Функция для добавления объекта заклинания в папку
void MainWindow::addSpellToDirectory(const QString &parentDirPath, const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString spellFilePath = parentDirPath + "/" + fileInfo.fileName(); // Получаем путь для копирования

    QFile::copy(filePath, spellFilePath); // Копируем файл в нужное место

    spellsModel->setRootPath(QDir::currentPath()); // Перезагружаем модель для отображения изменений
}

// Функция для добавления объекта предмета в папку
void MainWindow::addItemToDirectory(const QString &parentDirPath, const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString itemFilePath = parentDirPath + "/" + fileInfo.fileName(); // Получаем путь для копирования

    QFile::copy(filePath, itemFilePath); // Копируем файл в нужное место

    itemsModel->setRootPath(QDir::currentPath()); // Перезагружаем модель для отображения изменений
}


// Метод для обработки двойного клика по элементу в дереве заклинаний
void MainWindow::on_spellItemDoubleClicked(const QModelIndex &index)
{
    // Получаем полный путь к выбранному JSON файлу заклинания
    QString jsonPath = spellsModel->filePath(index);

    // Проверяем, что выбранный файл имеет расширение .json
    if (jsonPath.endsWith(".json")) {
        // Если файл JSON, извлекаем и отображаем его содержимое в новой вкладке
        displaySpellContentInTab(jsonPath);  // Вызовем новый метод для отображения содержимого заклинания
    } else {
        // Если файл не является JSON, выводим сообщение в лог
        qDebug() << "Выбран не JSON файл";
    }
}

// Метод для отображения содержимого заклинания в новой вкладке
void MainWindow::displaySpellContentInTab(const QString &jsonPath)
{
    qDebug() << "Displaying spell content in new tab";

    // Создание нового виджета для отображения содержимого заклинания
    QTextEdit *spellTextEdit = new QTextEdit(ui->tabWidget);  // создаем QTextEdit для отображения текста заклинания
    spellTextEdit->setReadOnly(true); // Устанавливаем текстовый редактор в режим "только чтение"

    // Загружаем данные из JSON файла с заклинанием
    QString spellContent = extractSpellContentFromJson(jsonPath);
    if (spellContent.isEmpty()) {
        qWarning() << "No content to display";
    }

    // Форматируем текст в HTML для стилизации
    QString htmlContent = QString("<html><head>"
                                  "<style>"
                                  "h1 { color: blue; font-family: 'Arial'; }"  // Стили для заголовка заклинания
                                  "p { font-size: 14px; line-height: 1.5; }"  // Стили для описания
                                  "</style>"
                                  "</head><body>"
                                  "<h1>%1</h1>"  // Название заклинания
                                  "<p>%2</p>"    // Описание заклинания
                                  "</body></html>")
                              .arg(spellContent.section("\n\nОписание:\n", 0, 0))  // Извлекаем заголовок из текста
                              .arg(spellContent.section("\n\nОписание:\n", 1));   // Извлекаем описание

    spellTextEdit->setHtml(htmlContent);  // Устанавливаем HTML содержимое в текстовый редактор

    // Устанавливаем атрибут для виджета, чтобы он удалялся при закрытии вкладки
    spellTextEdit->setAttribute(Qt::WA_DeleteOnClose);

    // Добавляем новый виджет в вкладки (tabWidget) с названием, основанным на имени файла
    int index = ui->tabWidget->addTab(spellTextEdit, QFileInfo(jsonPath).baseName());
    qDebug() << "Added tab at index: " << index;

    // Устанавливаем новый индекс вкладки как текущий, чтобы сразу переключиться на неё
    ui->tabWidget->setCurrentIndex(index);  // Переключаемся на новую вкладку
}



// Метод для извлечения данных из JSON файла заклинания и формирования строки для отображения
QString MainWindow::extractSpellContentFromJson(const QString &jsonPath)
{
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть JSON файл:" << jsonPath;
        return "";  // Возвращаем пустую строку, если файл не открыт
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();  // Закрываем файл после чтения

    if (!doc.isObject()) {
        qWarning() << "Некорректный формат JSON";
        return "";  // Возвращаем пустую строку, если формат некорректен
    }

    // Получаем данные заклинания из JSON
    QJsonObject obj = doc.object();
    QString spellName = obj["name"].toString();
    QString spellDescription = obj["description"].toString();

    // Проверим, что данные корректны
    qDebug() << "Spell Name: " << spellName;
    qDebug() << "Spell Description: " << spellDescription;

    // Формируем строку для отображения, которая содержит название и описание заклинания
    QString spellContent = "Заклинание: " + spellName + "\n\nОписание:\n" + spellDescription;
    return spellContent;
}


// Метод для обработки двойного клика по элементу в дереве заклинаний
void MainWindow::on_itemItemDoubleClicked(const QModelIndex &index)
{
    // Получаем полный путь к выбранному JSON файлу заклинания
    QString jsonPath = itemsModel->filePath(index);

    // Проверяем, что выбранный файл имеет расширение .json
    if (jsonPath.endsWith(".json")) {
        // Если файл JSON, извлекаем и отображаем его содержимое в новой вкладке
        displayItemContentInTab(jsonPath);  // Вызовем новый метод для отображения содержимого заклинания
    } else {
        // Если файл не является JSON, выводим сообщение в лог
        qDebug() << "Выбран не JSON файл";
    }
}

// Метод для отображения содержимого заклинания в новой вкладке
void MainWindow::displayItemContentInTab(const QString &jsonPath)
{
    qDebug() << "Displaying item content in new tab";

    // Создание нового виджета для отображения содержимого предмета
    QTextEdit *itemTextEdit = new QTextEdit(ui->tabWidget);  // создаем QTextEdit для отображения текста предмета
    itemTextEdit->setReadOnly(true); // Устанавливаем текстовый редактор в режим "только чтение"

    // Загружаем данные из JSON файла с предметом
    QString itemContent = extractItemContentFromJson(jsonPath);
    if (itemContent.isEmpty()) {
        qWarning() << "No content to display";
    }

    // Форматируем текст в HTML для стилизации
    QString htmlContent = QString("<html><head>"
                                  "<style>"
                                  "h1 { color: green; font-family: 'Verdana'; }"  // Стили для заголовка предмета
                                  "p { font-size: 14px; line-height: 1.5; color: #555; }"  // Стили для описания
                                  "</style>"
                                  "</head><body>"
                                  "<h1>%1</h1>"  // Название предмета
                                  "<p>%2</p>"    // Описание предмета
                                  "</body></html>")
                              .arg(itemContent.section("\n\nОписание:\n", 0, 0))  // Извлекаем заголовок из текста
                              .arg(itemContent.section("\n\nОписание:\n", 1));   // Извлекаем описание

    itemTextEdit->setHtml(htmlContent);  // Устанавливаем HTML содержимое в текстовый редактор

    // Устанавливаем атрибут для виджета, чтобы он удалялся при закрытии вкладки
    itemTextEdit->setAttribute(Qt::WA_DeleteOnClose);

    // Добавляем новый виджет в вкладки (tabWidget) с названием, основанным на имени файла
    int index = ui->tabWidget->addTab(itemTextEdit, QFileInfo(jsonPath).baseName());
    qDebug() << "Added tab at index: " << index;

    // Устанавливаем новый индекс вкладки как текущий, чтобы сразу переключиться на неё
    ui->tabWidget->setCurrentIndex(index);  // Переключаемся на новую вкладку
}



// Метод для извлечения данных из JSON файла заклинания и формирования строки для отображения
QString MainWindow::extractItemContentFromJson(const QString &jsonPath)
{
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть JSON файл:" << jsonPath;
        return "";  // Возвращаем пустую строку, если файл не открыт
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();  // Закрываем файл после чтения

    if (!doc.isObject()) {
        qWarning() << "Некорректный формат JSON";
        return "";  // Возвращаем пустую строку, если формат некорректен
    }

    // Получаем данные заклинания из JSON
    QJsonObject obj = doc.object();
    QString spellName = obj["name"].toString();
    QString spellDescription = obj["description"].toString();

    // Проверим, что данные корректны
    qDebug() << "Item Name: " << spellName;
    qDebug() << "Item Description: " << spellDescription;

    // Формируем строку для отображения, которая содержит название и описание заклинания
    QString spellContent = "Предмет: " + spellName + "\n\nОписание:\n" + spellDescription;
    return spellContent;
}

void MainWindow::createEmptyJsonFile(const QString &parentDirPath)
{
    // Формируем путь для нового JSON файла
    QString newFilePath = parentDirPath + "/new_character.json";

    // Открываем файл для записи
    QFile jsonFile(newFilePath);
    if (!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Не удалось создать файл: " << newFilePath;
        return;
    }

    // Создаем пустой JSON объект (абсолютно пустой)
    QJsonObject emptyObject;

    // Переводим объект в QJsonDocument (это стандартный способ работы с JSON в Qt)
    QJsonDocument jsonDoc(emptyObject);

    // Записываем JSON документ в файл
    jsonFile.write(jsonDoc.toJson());

    // Закрываем файл
    jsonFile.close();

    qDebug() << "Пустой JSON файл создан: " << newFilePath;
}

void MainWindow::createJsonFileWithDialog(const QString &parentDirPath) {
    // Показываем диалог для ввода имени файла
    bool ok;
    QString fileName = QInputDialog::getText(this, tr("Создать персонажа"),
                                             tr("Введите имя JSON файла без расширения:"),
                                             QLineEdit::Normal, tr("new_character"), &ok);

    // Проверяем, нажал ли пользователь OK и введено ли имя файла
    if (!ok || fileName.isEmpty()) {
        qDebug() << "Создание JSON файла отменено.";
        return;
    }

    // Добавляем расширение .json к имени файла
    if (!fileName.endsWith(".json")) {
        fileName += ".json";
    }

    // Формируем полный путь для нового JSON файла
    QString newFilePath = parentDirPath + "/" + fileName;

    // Проверяем, существует ли файл с таким именем
    if (QFile::exists(newFilePath)) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Файл с таким именем уже существует."));
        return;
    }

    // Открываем файл для записи
    QFile jsonFile(newFilePath);
    if (!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Не удалось создать файл: " << newFilePath;
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось создать файл: %1").arg(newFilePath));
        return;
    }

    // Создаем пустой JSON объект
    QJsonObject emptyObject;

    // Переводим объект в QJsonDocument
    QJsonDocument jsonDoc(emptyObject);

    // Записываем JSON документ в файл
    jsonFile.write(jsonDoc.toJson());

    // Закрываем файл
    jsonFile.close();

    qDebug() << "JSON файл создан: " << newFilePath;
    QMessageBox::information(this, tr("Успех"), tr("JSON файл успешно создан: %1").arg(newFilePath));
}

void MainWindow::on_monster_treeView_customContextMenuRequested(const QPoint &pos) {
    QMenu contextMenu(this);
    QAction *addAction = contextMenu.addAction("Добавить папку");  // Добавить папку
    QAction *addCharacterAction = contextMenu.addAction("Добавить персонажа");  // Добавить персонажа (новый JSON)
    QAction *removeAction = contextMenu.addAction("Удалить");  // Удалить файл или папку

    // Получаем индекс элемента в дереве, по которому был клик
    QModelIndex index = ui->monster_treeView->indexAt(pos);

    // Если индекс действителен, получаем путь к родительскому каталогу, иначе используем путь по умолчанию
    QString parentDirPath = index.isValid() ? monstersModel->filePath(index) : QDir::currentPath() + "/monsters";

    // Конвертируем строку пути в объект QDir
    QDir dir(parentDirPath);

    // Если директория не существует, создаем ее
    if (!dir.exists()) {
        dir.mkpath(parentDirPath);  // mkpath создает структуру папок
    }

    // Ожидаем выбора действия в контекстном меню
    QAction *selectedAction = contextMenu.exec(ui->monster_treeView->mapToGlobal(pos));

    if (selectedAction == addAction) {
        // Создаем новую папку в выбранной директории
        createUniqueFolder(dir, "Новая папка");
    } else if (selectedAction == addCharacterAction) {
        // Создаем новый файл персонажа (JSON) с указанием имени
        createJsonFileWithDialog(parentDirPath);
    } else if (selectedAction == removeAction) {
        // Удаление выбранного файла или папки
        if (index.isValid()) {
            removeFileOrFolder(index);
        }
    }
}

void MainWindow::on_monster_treeView_doubleClicked(const QModelIndex &index)
{
    qDebug() << "on_monster_treeView_doubleClicked triggered for:" << fileSystemModel->filePath(index);

    if (sender() != ui->monster_treeView) {
        return;
    }

    QString jsonPath = monstersModel->filePath(index);

    if (jsonPath.endsWith(".json")) {
        // Проверяем, существует ли уже вкладка
        for (int i = 0; i < ui->tabWidget->count(); ++i) {
            if (ui->tabWidget->tabText(i) == QFileInfo(jsonPath).fileName()) {
                qDebug() << "Вкладка уже открыта для файла:" << jsonPath;
                return;
            }
        }

        // Добавляем новую вкладку с формой персонажа
        addCharacterTab(jsonPath);
    } else {
        qDebug() << "Выбран не JSON файл";
    }
}


void MainWindow::addCharacterTab(const QString &filePath)
{
    // Создаем новую форму персонажа
    Form *characterForm = new Form(this);

    // Загружаем данные из JSON файла
    characterForm->loadFromJsonFile(filePath);

    // Добавляем вкладку с формой
    ui->tabWidget->addTab(characterForm, QFileInfo(filePath).fileName());

    // Настраиваем автоматическое сохранение изменений в файл
    connect(characterForm, &Form::dataChanged, [characterForm, filePath]() {
        characterForm->saveToJsonFile(filePath);
    });
}
