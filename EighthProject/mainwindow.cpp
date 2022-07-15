#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "secondwindow.h"
#include <QtWidgets>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    form = new Form;
    window = new SecondWindow();

    socket = new QTcpSocket(this);
    socket->connectToHost("127.0.0.1", 2323);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    nextBlockSize = 0;

    connect(form, &Form::signal, this, &MainWindow::slotForm);
    connect(this, &MainWindow::signalNameGetter, window, &SecondWindow::slotNameGetter);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString login = ui->loginEdit->text();
    QString password = ui->passwordEdit->text();

    SendToServer("authorization", login, password);
}

void MainWindow::on_pushButton_2_clicked()
{
    form->show();
}

void MainWindow::slotForm(QVector<QString> vector)
{
    SendToServer("registration", vector[0], vector[1], vector[2], vector[3]);
}

void MainWindow::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_3);
    if (in.status() == QDataStream::Ok)
    {
        for (;;)
        {
            if (nextBlockSize == 0)
            {
                if (socket->bytesAvailable() < 2)
                {
                    break;
                }
                in >> nextBlockSize;
            }
            if (socket->bytesAvailable() < nextBlockSize)
            {
                break;
            }
            QString type, name, sirname;
            QSqlTableModel model;
            in >> type >> name >> sirname;
            nextBlockSize = 0;
            if (type == "NotFound"){
                qDebug() << "Not Found";
                ui->statusbar->showMessage("Wrong data");
            } else if (type == "Success"){
                QMessageBox::information(this, "Notification", "Authorized successfully");
                hide();
                window->setModal(true);
                window->show();
                emit signalNameGetter(name, sirname);
            } else if (type == "Taken") {
                QMessageBox::warning(this, "Notification", "This login is taken!");
            } else if (type == "Added") {
                QMessageBox::information(this, "Notification", "User was added!");
            } else {
                QMessageBox::information(this, "Notification", "No answer from server!");
            }
        }
    }
    else
    {
        qDebug() << "read error";
    }
}

void MainWindow::SendToServer(QString type, QString str1, QString str2)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_3);
    out << quint16(0) << type << str1 << str2;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
}

void MainWindow::SendToServer(QString type, QString str1, QString str2, QString str3, QString str4)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_3);
    out << quint16(0) << type << str1 << str2 << str3 << str4;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
}
