#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    detailsNames << "Куб 1x1"
                 << "Куб 1x2"
                 << "Куб 1x3"
                 << "Куб 1x4"
                 << "Куб 1x6"
                 << "Куб 1x8"
                 << "Куб 1x10"
                 << "Куб 2x2"
                 << "Куб 2x3"
                 << "Куб 2x4"
                 << "Куб 2x6"
                 << "Куб 2x8"
                 << "Куб 2x10"
                 << "Куб 4x4"
                 << "Куб 4x6"
                 << "Куб 4x8"
                 << "Куб 4x10"
                 << "Куб 8x8"
                 << "Плата 1x1"
                 << "Плата 1x2"
                 << "Плата 1x3"
                 << "Плата 1x4"
                 << "Плата 1x6"
                 << "Плата 1x8"
                 << "Плата 1x10"
                 << "Плата 2x2"
                 << "Плата 2x3"
                 << "Плата 2x4"
                 << "Плата 2x6"
                 << "Плата 2x8"
                 << "Плата 2x10"
                 << "Плата 4x4"
                 << "Плата 4x6"
                 << "Плата 4x8"
                 << "Плата 4x10"
                 << "Плата 8x8"
                 << "Арка 1x3"
                 << "Арка 1x4"
                 << "Арка 1x6"
                 << "Угол 1x2"
                 << "Угол 1x3"
                 << "Угол 2x2"
                 << "Угол 2x3"
                 << "Обр.Угол 2x2"
                 << "Обр.Угол 2x3";

    ui->tableEdDetailLimit->setRowCount(detailsNames.size());
    ui->tableEdDetailLimit->setColumnWidth(1, 105);
    ui->tableEdWorkLimit->setRowCount(detailsNames.size());
    ui->tableEdWorkLimit->setColumnWidth(1, 105);
    for (int i = 0; i < detailsNames.size(); i++)
    {
        ui->typeChooseBox->addItem(detailsNames[i]);
        ui->typeEdChooseBox->addItem(detailsNames[i]);
        ui->tableEdDetailLimit->setItem(i, 0, new QTableWidgetItem);
        ui->tableEdDetailLimit->setItem(i, 1, new QTableWidgetItem);
        ui->tableEdWorkLimit->setItem(i, 0, new QTableWidgetItem);
        ui->tableEdWorkLimit->setItem(i, 1, new QTableWidgetItem);
        ui->tableEdDetailLimit->item(i, 0)->setText(detailsNames[i]);
        ui->tableEdWorkLimit->item(i, 0)->setText(detailsNames[i]);
        ui->tableEdDetailLimit->item(i, 1)->setText(QString("0"));
    }

    connect(this, SIGNAL(sendSize(int, int)), ui->dw, SLOT(getSize(int, int)));
    connect(this, SIGNAL(sendEdSize(int, int)), ui->ew, SLOT(getSize(int, int)));
    connect(this, SIGNAL(addDetail(int, QColor)), ui->dw, SLOT(getDetail(int, QColor)));
    connect(this, SIGNAL(addEdDetail(int, QColor)), ui->ew, SLOT(getDetail(int, QColor)));

    modelColor = Qt::red;
    ui->modelFrame->setAutoFillBackground(true);
    pal.setColor(QPalette::Background, modelColor);
    ui->modelFrame->setPalette(pal);
    modelEdColor = Qt::red;
    ui->modelEdFrame->setAutoFillBackground(true);
    pal.setColor(QPalette::Background, modelEdColor);
    ui->modelEdFrame->setPalette(pal);

    emit sendSize(ui->dw->width(), ui->dw->height());
    emit sendEdSize(ui->ew->width(), ui->ew->height());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_modelColorButton_clicked()
{
    QColor tmp = QColorDialog::getColor();
    if (tmp.isValid())
    {
        pal.setColor(QPalette::Background, tmp);
        ui->modelFrame->setPalette(pal);
        modelColor = tmp;
    }
}

void MainWindow::on_addButton_clicked()
{
    if (ui->dw->isPlant == 0)
        emit addDetail(ui->typeChooseBox->currentIndex(), modelColor);
    else
    {
        QMessageBox msg;
        msg.setText("Нажмите Enter или Esc, чтобы установить текущую деталь и добавить следующую.");
        msg.show();
        msg.exec();
    }
    ui->dw->setFocus();
}

void MainWindow::on_modelEdColorButton_clicked()
{
    QColor tmp = QColorDialog::getColor();
    if (tmp.isValid())
    {
        pal.setColor(QPalette::Background, tmp);
        ui->modelEdFrame->setPalette(pal);
        modelEdColor = tmp;
    }
}

void MainWindow::on_addEdButton_clicked()
{
    if (ui->ew->isPlant == 0)
    {
        int tmp = ui->tableEdWorkLimit->item(ui->typeEdChooseBox->currentIndex(), 1)->text().toInt();
        if (tmp != 0)
        {
            tmp--;
            ui->tableEdWorkLimit->item(ui->typeEdChooseBox->currentIndex(), 1)->setText(QString::number(tmp)); // Порядок важен!
            emit addEdDetail(ui->typeEdChooseBox->currentIndex(), modelEdColor);
        }
        else
        {
            QMessageBox msg;
            msg.setText("Данный тип детали недоступен.");
            msg.show();
            msg.exec();
        }
    }
    else
    {
        QMessageBox msg;
        msg.setText("Нажмите Enter или Esc, чтобы установить текущую деталь и добавить следующую.");
        msg.show();
        msg.exec();
    }
    ui->ew->setFocus();
}

void MainWindow::on_modeTabWidget_tabBarClicked(int index)
{
    if (index == ui->modeTabWidget->indexOf(ui->educate))
    {
        ui->tableEdDetailLimit->show();
    }
}

void MainWindow::on_closeTableButton_clicked()
{
    for (int i = 0; i < detailsNames.size(); i++)
        ui->tableEdWorkLimit->item(i, 1)->setText(ui->tableEdDetailLimit->item(i, 1)->text());
    ui->tableEdDetailLimit->hide();
    ui->closeTableButton->hide();
}

void MainWindow::on_openTableButton_clicked()
{
    ui->tableEdDetailLimit->show();
    ui->closeTableButton->show();
}
