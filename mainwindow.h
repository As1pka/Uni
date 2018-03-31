#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColorDialog>
#include "baseview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    QPalette pal;
    QColor modelColor, modelEdColor;
    QVector<QString> detailsNames;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void addDetail(int idDetail, QColor color);
    void addEdDetail(int idDetail, QColor color);
    void sendSize(int w, int h);
    void sendEdSize(int w, int h);

private slots:
    void on_modelColorButton_clicked();
    void on_addButton_clicked();
    void on_modelEdColorButton_clicked();
    void on_addEdButton_clicked();
    void on_modeTabWidget_tabBarClicked(int index);
    void on_closeTableButton_clicked();
    void on_openTableButton_clicked();
};

#endif // MAINWINDOW_H
