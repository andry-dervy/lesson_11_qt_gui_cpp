#include "qapplication.h"
#include "mainwindow.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QDate>
#include <QKeyEvent>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , model(new QSqlRelationalTableModel(this, QSqlDatabase::addDatabase("QSQLITE")))
{
    setWindowTitle(tr("Список задач"));

    resize(WIDTH,HIGHT);

    table = new QTableView(this);
    setCentralWidget(table);
    table->setItemDelegateForColumn(static_cast<int>(TableColumns::Progress), new DelegateProgressValidator());
    table->setItemDelegateForColumn(static_cast<int>(TableColumns::Date), new DelegateDateValidator());

    openBD();

    setHotKeys();
    installEventFilter(this);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setHotKeys()
{
    hotkeys.insert({eTYPE_HOTKEY::Quit,
                    std::make_pair<int,Qt::KeyboardModifiers>(81,
                        Qt::KeyboardModifier::ControlModifier)});
    hotkeys.insert({eTYPE_HOTKEY::NewTask,
                    std::make_pair<int,Qt::KeyboardModifiers>(78,
                        Qt::KeyboardModifier::ControlModifier)});
    hotkeys.insert({eTYPE_HOTKEY::DeleteTask,
                    std::make_pair<int,Qt::KeyboardModifiers>(68,
                        Qt::KeyboardModifier::ControlModifier)});
}

void MainWindow::resizeTable()
{
    if(table->horizontalHeader()->count() != QUANTITY_OF_COLUMNS)
    {
         qDebug() << "Quantity of columns is less 4.";
         return;
    }
    table->horizontalHeader()->resizeSection(static_cast<int>(TableColumns::NameTask),size().rwidth()/4);
    table->horizontalHeader()->resizeSection(static_cast<int>(TableColumns::Date),100);
}

void MainWindow::setSettingsTable()
{
    table->setColumnHidden(static_cast<int>(TableColumns::Id),true);
    table->horizontalHeader()->setSectionResizeMode(static_cast<int>(TableColumns::NameTask),QHeaderView::Fixed);
    table->horizontalHeader()->setSectionResizeMode(static_cast<int>(TableColumns::DescTask),QHeaderView::Stretch);

    model->setHeaderData(static_cast<int>(TableColumns::NameTask), Qt::Horizontal, tr("Название задачи"));
    model->setHeaderData(static_cast<int>(TableColumns::DescTask), Qt::Horizontal, tr("Описание задачи"));
    model->setHeaderData(static_cast<int>(TableColumns::Progress), Qt::Horizontal, tr("Прогресс"));
    model->setHeaderData(static_cast<int>(TableColumns::Date), Qt::Horizontal, tr("Cрок"));

    resizeTable();
}

void MainWindow::openBD()
{
    db = model->database();
    db.setDatabaseName("./TaskList.db");

    if (!db.open())
    {
        qDebug() << "DB is not open!";
    }

    table->setModel(model);

    selectTable("tasks");
}

void MainWindow::selectTable(const QString& nameTable)
{
    model->setTable(nameTable);
    model->select();
    setSettingsTable();
}

void MainWindow::newTask()
{
    QSqlQuery  query{db};

    QString date = QDate::currentDate().toString("yyyy-MM-dd");
    QString execute = "insert into tasks (nameTask,descriptionTask,progress,limitDate) "
        "values('Name task', 'Description task', 0, '" + date + "')";
    if (!query.exec(execute)) {
        qDebug() << "Can't insert";
    }

    selectTable("tasks");
}

void MainWindow::deleteTask()
{
    auto index = table->currentIndex();
    if(!index.isValid()) return;

    qDebug() << model->data(index);

    index = model->index(index.row(),0);
    auto id = model->data(index).toString();
    QSqlQuery  query{db};

    QString execute = "delete from tasks where id = '" + id + "'";
    if (!query.exec(execute)) {
        qDebug() << "Can't insert";
    }

    selectTable("tasks");
}

void MainWindow::quit()
{
    qApp->quit();
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::Resize)
    {
        if(this == obj)
        {
            resizeTable();
        }
    }
    if (event->type() == QEvent::KeyPress)
    {
        auto keyEvent = static_cast<QKeyEvent*>(event);

        if(keyEvent == nullptr) return false;

        auto hotKey = std::make_pair(keyEvent->key(), keyEvent->modifiers());

        qDebug() << "codeKey: " << keyEvent->key() << " modifiers:" << keyEvent->modifiers();

        auto hk_newFile = hotkeys.find(eTYPE_HOTKEY::NewTask);
        if(hk_newFile != hotkeys.end())
        {
            if(hk_newFile->second == hotKey)
            {
                newTask();
                return true;
            }
        }

        auto hk_openFile = hotkeys.find(eTYPE_HOTKEY::DeleteTask);
        if(hk_openFile != hotkeys.end())
        {
            if(hk_openFile->second == hotKey)
            {
                deleteTask();
                return true;
            }
        }

        auto hk_Quit = hotkeys.find(eTYPE_HOTKEY::Quit);
        if(hk_Quit != hotkeys.end())
        {
            if(hk_Quit->second == hotKey)
            {
                quit();
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

