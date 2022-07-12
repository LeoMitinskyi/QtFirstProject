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

    db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName("./usersDB.db");

    if(!db.open()){
        qDebug() << db.lastError().text();
    }

    query = new QSqlQuery(db);
    query->exec("CREATE TABLE ListOfProducts(Name TEXT, Weight INT, ExpirationDate DATE, Fabricator TEXT);");

    model = new QSqlTableModel(this, db);
    model->setTable("ListOfProducts");
    model->select();

    ui->tableView->setModel(model);
    QHeaderView* header = ui->tableView->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
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
    hide();
}

void SecondWindow::on_addButton_clicked()
{
    model->insertRow(model->rowCount());
}

void SecondWindow::on_tableView_clicked(const QModelIndex &index)
{
    m_row = index.row();
}

void SecondWindow::on_deleteButton_clicked()
{
    model->removeRow(m_row);
}

void SecondWindow::slotNameGetter(const QString &name, const QString &sirname)
{
    ui->nameLabel->setText(name);
    ui->sirnameLabel->setText(sirname);
}


