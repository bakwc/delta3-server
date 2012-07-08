#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onClick()));
    serv = new QTcpServer(this);
    serv->listen(QHostAddress::LocalHost, 1234);
    connect(serv, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
//    QProcess* process = new QProcess(this);
//    process->start("cmd");
//    if(process->waitForStarted(3000)){
//        ui->textEdit->append(process->readAllStandardOutput());

//        process->write("ipconfig\n");

//        process->waitForBytesWritten(3000);
//        process->waitForReadyRead(3000);
//        ui->textEdit->append(process->readAllStandardOutput());

//        process->close();
//    }
//    else ui->textEdit->append("error");
}

MainWindow::~MainWindow()
{
    delete ui;
}
bool hui = false;
void MainWindow::onClick()
{
    if(hui){
        QString textik = ui->lineEdit->text();

        ui->textEdit->append(QString("> %1").arg(textik));
        pClientSocket->write(qPrintable(textik));
        ui->lineEdit->clear();
    }
    else{
        ui->textEdit->append("| Client not connected");
    }
}

void MainWindow::slotNewConnection()
{
    pClientSocket = serv->nextPendingConnection();
    connect((QObject*) pClientSocket, SIGNAL(disconnected()),
            (QObject*) pClientSocket, SLOT(deleteLater())
           );
    connect((QObject*) pClientSocket, SIGNAL(disconnected()),
            this, SLOT(clientDC())
           );
    connect((QObject*) pClientSocket, SIGNAL(readyRead()),
            this,          SLOT(slotReadClient())
           );
    hui = true;
}

void MainWindow::slotReadClient(){
    ui->textEdit->append(QString("< %1").arg((QString)pClientSocket->readAll()));
}

void MainWindow::clientDC()
{
    hui = false;
}
