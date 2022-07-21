#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QVector>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Form *ui;

signals:
    void signalRegistrationDataGetter(QString, QString, QString, QString);
};

#endif // FORM_H
