#ifndef SECONDWINDOW_H
#define SECONDWINDOW_H

#include <QDialog>
#include <QtWidgets>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>


namespace Ui {
class SecondWindow;
}

class SecondWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SecondWindow(QWidget *parent = nullptr);
    ~SecondWindow();

private slots:
    void on_QuitButton_clicked();
    void showTime();

    void on_addButton_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_deleteButton_clicked();


private:
    Ui::SecondWindow *ui;
    QSqlDatabase db;
    QSqlQuery *query;
    QSqlTableModel *model;
    int m_row;

public slots:
    void slotNameGetter(const QString &name, const QString &sirname);
};

#endif // SECONDWINDOW_H
