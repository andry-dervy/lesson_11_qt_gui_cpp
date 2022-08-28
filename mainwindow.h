#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QSqlRelationalTableModel>
#include <QItemDelegate>
#include <QSpinBox>
#include <QLineEdit>
#include <QDateEdit>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <map>
#include <utility>

class DelegateProgressValidator: public QItemDelegate
{
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
    {
        QSpinBox* editor = new QSpinBox(parent);
        editor->setMinimum(0);
        editor->setMaximum(10);
        return editor;
    }
};

class DelegateDateValidator: public QItemDelegate
{
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
    {
        QDateEdit *editor = new QDateEdit(parent);
        editor->setDisplayFormat("yyyy-MM-dd");
        return editor;
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum class  eTYPE_HOTKEY
    {
        NewTask, DeleteTask, Quit,
    };
    enum class TableColumns {
        Id, NameTask,DescTask,Progress,Date
    };


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    const int WIDTH = 1000;
    const int HIGHT = 500;
    const int QUANTITY_OF_COLUMNS = 5;

private:
    QSqlRelationalTableModel* model;
    QTableView* table;
    QSqlDatabase db;

private:
    void resizeTable();
    void setSettingsTable();

    void selectTable(const QString& table);
    void setHotKeys();
    void openBD();

    void newTask();
    void deleteTask();
    void quit();

private:
    std::map<eTYPE_HOTKEY,std::pair<int,Qt::KeyboardModifiers>> hotkeys;
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};
#endif // MAINWINDOW_H
