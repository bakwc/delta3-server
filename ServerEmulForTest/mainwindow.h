#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    QTcpServer * serv;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    

private:
    Ui::MainWindow *ui;
    QTcpSocket * pClientSocket;
protected slots:
       void onClick();
       void slotNewConnection();
       void slotReadClient();
       void clientDC();
};

#endif // MAINWINDOW_H
