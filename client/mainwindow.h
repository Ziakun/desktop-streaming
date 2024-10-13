#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void init();

private:
    Ui::MainWindow *ui;
    Client client;
};
#endif // MAINWINDOW_H
