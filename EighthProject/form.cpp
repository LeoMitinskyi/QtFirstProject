#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
}

Form::~Form()
{
    delete ui;
}

void Form::on_pushButton_clicked()
{
    auto name = ui->nameEdit->text();
    auto sirname = ui->sirnameEdit->text();
    auto login = ui->loginEdit->text();
    auto password = ui->passwordEdit->text();

    QVector<QString> vector;

    vector.push_back(name);
    vector.push_back(sirname);
    vector.push_back(login);
    vector.push_back(password);

    emit signal(vector);
    hide();
}

