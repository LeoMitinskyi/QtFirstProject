#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "form.h"
#include "secondwindow.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QVector>
#include <QTcpSocket>
#include <QSqlError>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    SecondWindow *window;
    Form *form;
    QSqlDatabase db;
    QSqlQuery *query;
    QSqlTableModel *model;
    QTcpSocket *socket;
    QByteArray Data;
    void SendToServer(QString, QString, QString);
    void SendToServer(QString, QString, QString, QString, QString);
    quint16 nextBlockSize;

public slots:
    void slotForm(QVector<QString> vector);
    void slotReadyRead();

signals:
    void signalNameGetter(const QString&, const QString&);
};
#endif // MAINWINDOW_H
