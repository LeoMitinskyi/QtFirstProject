#include "secondwindow.h"
#include "ui_secondwindow.h"
#include <QTimer>
#include <QDateTime>
#include <QtWidgets>

SecondWindow::SecondWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SecondWindow)
{
    ui->setupUi(this);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start();

    QDateTime dateTime = QDateTime::currentDateTime();
    ui->DateTime->setText(dateTime.toString("dd.MM.yyyy"));
}

SecondWindow::~SecondWindow()
{
    delete ui;
}

void SecondWindow::showTime()
{
    QTime time = QTime::currentTime();
    QString time_text = time.toString("hh : mm : ss");
    ui->Digital_clock->setText(time_text);
}

void SecondWindow::on_QuitButton_clicked()
{
    close();
}

void SecondWindow::slotNameGetter(const QString &name, const QString &sirname)
{
    ui->nameLabel->setText(name);
    ui->sirnameLabel->setText(sirname);
}

void SecondWindow::slotlQJsonArrayGetter(QJsonArray docAr)
{
    model = new QStandardItemModel(nullptr);
    model->setHorizontalHeaderLabels(QStringList() << "Name" << "Weight");

    for (int i = 0; i < docAr.count(); i++)
    {
        if (i % 2 == 0){
            QList<QStandardItem*> lst;
            QStandardItem* col = new QStandardItem(docAr[i].toObject().value("Name").toString());
            QStandardItem* col2 = new QStandardItem(docAr[i + 1].toObject().value("Weight").toString());
            lst.append(col);
            lst.append(col2);
            model->appendRow(lst);
        }
    }
    ui->tableView->setModel(model);
}

void SecondWindow::slotSocketGetter(QTcpSocket *socket)
{
    this->socket = socket;
}

void SecondWindow::on_tableView_clicked(const QModelIndex &index)
{
    m_row = index.row();
}

void SecondWindow::on_deleteButton_clicked()
{
    if (m_row != NULL){
        QList<QStandardItem*> lst = model->takeRow(m_row);
        qDebug() << lst[0]->text() << lst[1]->text();
        socket->write("{\"type\":\"deleteItem\", \"name\":\"" + lst[0]->text().toLocal8Bit() + "\", \"weight\":\"" + lst[1]->text().toLocal8Bit() + "\"}");
    }
}


void SecondWindow::on_addButton_clicked()
{
    model->insertRow(model->rowCount());
}


void SecondWindow::on_pushButton_clicked()
{
    QList<QStandardItem*> lst = model->takeRow(m_row);
    model->appendRow(lst);
    qDebug() << lst[0]->text() << lst[1]->text();
    socket->write("{\"type\":\"insertItem\", \"name\":\"" + lst[0]->text().toLocal8Bit() + "\", \"weight\":\"" + lst[1]->text().toLocal8Bit() + "\"}");
}

