#include "mapviewwidget.h" // Подключаем заголовочный файл, который определяет класс mapviewwidget
#include "ui_mapviewwidget.h" // Подключаем файл, который содержит автоматически сгенерированный код для интерфейса

// Конструктор класса mapviewwidget
mapviewwidget::mapviewwidget(QWidget *parent)
    : QWidget(parent), // Инициализируем базовый класс QWidget с передачей родительского виджета
    ui(new Ui::mapviewwidget), // Создаем объект интерфейса
    scene(new QGraphicsScene(this)), // Создаем сцену для отображения графики
    currentScale(1.0), // Устанавливаем начальный масштаб (100%)
    characterItem(nullptr) // Инициализируем указатель на иконку персонажа
{
    ui->setupUi(this); // Инициализируем элементы интерфейса

    setupGraphicsView(); // Настраиваем виджет для отображения графики
    ui->graphicsView->viewport()->installEventFilter(this); // Устанавливаем фильтр событий для viewport графического вида

    // Подключаем сигнал iconMoved к слоту для обновления позиции персонажа
    connect(this, &mapviewwidget::iconMoved, this, &mapviewwidget::updateCharacterPosition);


}

// Деструктор класса mapviewwidget
mapviewwidget::~mapviewwidget()
{
    delete ui; // Освобождаем память, занятую объектом интерфейса
}

// Метод для настройки QGraphicsView
void mapviewwidget::setupGraphicsView()
{
    ui->graphicsView->setScene(scene); // Устанавливаем сцену для отображения в графическом виде
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключаем горизонтальную полосу прокрутки
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключаем вертикальную полосу прокрутки
    ui->graphicsView->setRenderHint(QPainter::Antialiasing); // Включаем сглаживание для отрисовки
    ui->graphicsView->setDragMode(QGraphicsView::NoDrag); // Отключаем режим перетаскивания
}

// Метод для загрузки карты
void mapviewwidget::loadMap(const QString &mapPath)
{
    QPixmap pixmap(mapPath); // Загружаем изображение карты из указанного пути
    if (pixmap.isNull()) { // Проверяем, удалось ли загрузить изображение
        qWarning() << "Failed to load map:" << mapPath; // Выводим предупреждение в случае ошибки
        return; // Завершаем выполнение метода
    }

    scene->clear(); // Очищаем текущую сцену
    scene->addPixmap(pixmap); // Добавляем загруженное изображение на сцену
    ui->graphicsView->setScene(scene); // Назначаем сцену графическому виду
}



// Метод для обработки событий перетаскивания
void mapviewwidget::handleDrag(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) { // Проверяем, была ли нажата левая кнопка мыши
        ui->graphicsView->setDragMode(
            event->type() == QEvent::MouseButtonPress
                ? QGraphicsView::ScrollHandDrag // Включаем режим перетаскивания при нажатии кнопки
                : QGraphicsView::NoDrag); // Выключаем режим перетаскивания при отпускании кнопки
    }
}

// Метод для обработки событий масштабирования
void mapviewwidget::handleZoom(QWheelEvent *event)
{
    const double scaleFactor = 1.15; // Коэффициент масштабирования
    if (event->angleDelta().y() > 0) { // Если колесо прокручено вперед (увеличение)
        ui->graphicsView->scale(scaleFactor, scaleFactor); // Увеличиваем масштаб
    } else { // Если колесо прокручено назад (уменьшение)
        ui->graphicsView->scale(1.0 / scaleFactor, 1.0 / scaleFactor); // Уменьшаем масштаб
    }
}
/*
void mapviewwidget::createContextMenu(QPoint position)
{
    QMenu contextMenu;

    QAction *zoomInAction = new QAction("Zoom In", this);
    QAction *zoomOutAction = new QAction("Zoom Out", this);
    QAction *resetViewAction = new QAction("Reset View", this);
    QAction *addIconAction = new QAction("Add Character Icon", this);
    QAction *removeIconAction = new QAction("Remove Character Icon", this);

    contextMenu.addAction(zoomInAction);
    contextMenu.addAction(zoomOutAction);
    contextMenu.addAction(resetViewAction);
    contextMenu.addAction(addIconAction);
    contextMenu.addAction(removeIconAction);

    connect(zoomInAction, &QAction::triggered, this, [this]() {
        ui->graphicsView->scale(1.15, 1.15);
    });

    connect(zoomOutAction, &QAction::triggered, this, [this]() {
        ui->graphicsView->scale(1.0 / 1.15, 1.0 / 1.15);
    });

    connect(resetViewAction, &QAction::triggered, this, [this]() {
        ui->graphicsView->resetTransform();
    });


    contextMenu.exec(ui->graphicsView->mapToGlobal(position));
}
*/


// Дополняем метод eventFilter для обработки щелчка правой кнопкой мыши
bool mapviewwidget::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->graphicsView->viewport()) { // Проверяем, что событие относится к viewport
        switch (event->type()) { // Определяем тип события
        case QEvent::MouseButtonPress: { // Обработка нажатия мыши
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event); // Приводим событие к типу QMouseEvent
            if (mouseEvent->button() == Qt::RightButton) { // Проверяем, что нажата правая кнопка мыши
                createContextMenu(mouseEvent->pos()); // Вызываем метод для отображения контекстного меню // Вызываем обработчик для нажатия правой кнопки мыши
                return true; // Блокируем дальнейшую обработку события
            }
            handleDrag(mouseEvent); // Передаем событие в обработчик перетаскивания для других кнопок
            break;
        }
        case QEvent::MouseButtonRelease: { // Обработка отпускания мыши
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event); // Приводим событие к типу QMouseEvent
            handleDrag(mouseEvent); // Передаем событие в обработчик перетаскивания
            break;
        }
        case QEvent::Wheel: // Обработка прокрутки колеса мыши
            handleZoom(static_cast<QWheelEvent *>(event)); // Передаем событие в обработчик масштабирования
            return true; // Блокируем стандартное поведение прокрутки колеса
        default:
            break; // Игнорируем другие типы событий
        }
    }
    return QWidget::eventFilter(object, event); // Вызываем стандартный обработчик событий
}


void mapviewwidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // Если нажата левая кнопка мыши, начинаем перетаскивание
        QPointF scenePos = ui->graphicsView->mapToScene(event->pos()); // Получаем позицию в сцене
        QList<QGraphicsItem*> selectedItems = scene->items(scenePos);  // Получаем все элементы на этой позиции

        if (!selectedItems.isEmpty()) {
            // Если элемент выбран, выделяем его
            selectedItems.first()->setSelected(true);
        }
    }
    else if (event->button() == Qt::RightButton) {
        // При правом клике ничего не делаем в mousePressEvent
        // Это для предотвращения конфликта между перетаскиванием карты и контекстным меню.
    }
}


// Метод для добавления иконки персонажа на карту
void mapviewwidget::addCharacterIcon(const QPixmap &icon)
{
    // Получаем размеры графического представления
    QSize viewSize = ui->graphicsView->size();

    // Выбираем меньшую сторону для создания квадратной маски и делим на 15
    int size = qMin(viewSize.width(), viewSize.height()) / 15;

    // Преобразуем QPixmap в QImage для большего контроля над изображением
    QImage img = icon.toImage();

    // Масштабируем изображение с использованием фильтра Lanczos для более качественного уменьшения
    QImage scaledImage = img.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::TransformationMode::SmoothTransformation);

    // Создаем маску для круга
    QPixmap scaledPixmap = QPixmap::fromImage(scaledImage);

    // Обрезаем изображение до квадратной формы
    QPixmap croppedPixmap = scaledPixmap.copy(0, 0, size, size);

    // Создаем маску для круга
    QBitmap mask(size, size);
    mask.fill(Qt::color0);  // Заполняем маску прозрачным цветом

    // Рисуем круговую маску
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::color1);  // Белый цвет для маски (непрозрачный)
    painter.drawEllipse(0, 0, size, size);
    painter.end();

    // Применяем маску к изображению
    croppedPixmap.setMask(mask);

    // Получаем сцену, привязанную к QGraphicsView
    QGraphicsScene *scene = ui->graphicsView->scene();

    if (!scene) {
        // Если сцена еще не была создана, создаем ее вручную
        scene = new QGraphicsScene(this);
        ui->graphicsView->setScene(scene);  // Привязываем сцену к QGraphicsView
    }

    // Создаем элемент для отображения на сцене
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(croppedPixmap);

    // Устанавливаем флаг для перетаскивания
    item->setFlag(QGraphicsItem::ItemIsMovable);

    // Устанавливаем начальную позицию иконки на карте
    item->setPos(200, 200);  // Устанавливаем позицию, можно настроить по вашему усмотрению

    // Добавляем элемент на сцену
    scene->addItem(item);
}


// Реализация других методов для добавления и перетаскивания
void mapviewwidget::startDraggingCharacter(QMouseEvent *event)
{
    if (characterItem && characterItem->isUnderMouse()) {
        characterItem->setSelected(true);
    }
}

void mapviewwidget::stopDraggingCharacter(QMouseEvent *event)
{
    if (characterItem) {
        characterItem->setSelected(false);
    }
}


void mapviewwidget::moveCharacter(QMouseEvent *event)
{
    if (characterItem && characterItem->isSelected()) {
        QPointF scenePos = ui->graphicsView->mapToScene(event->pos());
        characterItem->setPos(scenePos);

        // Излучаем сигнал iconMoved с новой позицией
        emit iconMoved(scenePos);
    }
}

// Метод для обновления позиции иконки
void mapviewwidget::updateCharacterPosition(const QPointF &newPosition)
{
    if (characterItem) {
        characterItem->setPos(newPosition); // Обновляем позицию иконки
    }
}


// Переопределение метода createContextMenu для добавления иконки персонажа
// Обновляем метод createContextMenu
void mapviewwidget::createContextMenu(QPoint position)
{
    QMenu contextMenu;

    QAction *addIconAction = new QAction("Add Character Icon", this);
    contextMenu.addAction(addIconAction);

    connect(addIconAction, &QAction::triggered, this, [this]() {
        // Открываем диалог выбора файла JSON
        QString fileName = QFileDialog::getOpenFileName(this, tr("Choose Character JSON"), "", tr("JSON Files (*.json)"));
        if (fileName.isEmpty()) {
            return;
        }

        // Читаем файл JSON
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Failed to open file:" << fileName;
            return;
        }

        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            qWarning() << "Invalid JSON format in file:" << fileName;
            return;
        }

        QJsonObject jsonObject = doc.object();

        // Извлекаем путь к иконке
        if (!jsonObject.contains("imagePath") || !jsonObject["imagePath"].isString()) {
            qWarning() << "No icon path found in JSON.";
            return;
        }

        QString iconPath = jsonObject["imagePath"].toString();
        QPixmap iconPixmap(iconPath);
        if (iconPixmap.isNull()) {
            qWarning() << "Failed to load icon from path:" << iconPath;
            return;
        }

        // Добавляем иконку на карту
        addCharacterIcon(iconPixmap);
    });

    contextMenu.exec(ui->graphicsView->mapToGlobal(position));
}

/*
// Метод для удаления иконки с карты
void mapviewwidget::removeCharacterIcon(QGraphicsPixmapItem *item)
{
    if (item) {
        scene->removeItem(item); // Удаляем элемент из сцены
        delete item; // Освобождаем память, занятую элементом
    }
}

// Метод для создания контекстного меню для иконки персонажа
void mapviewwidget::createCharacterIconContextMenu(QGraphicsPixmapItem *pixmapItem, const QPoint &pos)
{
    // Здесь создается контекстное меню для удаления изображения персонажа
    QMenu menu;
    QAction *deleteAction = new QAction("Удалить изображение персонажа", this);
    connect(deleteAction, &QAction::triggered, this, [this, pixmapItem]() {
        scene->removeItem(pixmapItem);  // Удаляем изображение персонажа
    });
    menu.addAction(deleteAction);
    menu.exec(ui->graphicsView->mapToGlobal(pos));  // Отображаем меню в нужной позиции
}


// Метод для обработки события нажатия правой кнопкой мыши на иконке
void mapviewwidget::handleIconRightClick(QGraphicsSceneMouseEvent *event)
{
    // Проверяем, есть ли на позиции элемент, который является иконкой персонажа
    QGraphicsItem *item = scene->itemAt(event->scenePos(), QTransform());

    // Если элемент существует и это QGraphicsPixmapItem (иконка персонажа)
    if (item && qgraphicsitem_cast<QGraphicsPixmapItem*>(item)) {
        QGraphicsPixmapItem *pixmapItem = qgraphicsitem_cast<QGraphicsPixmapItem*>(item);
        createCharacterIconContextMenu(pixmapItem, event->scenePos().toPoint()); // Открываем контекстное меню для иконки
    }
}
*/

