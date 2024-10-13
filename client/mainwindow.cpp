#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QtNetwork>
#include <QPixmap>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    const QString &name = QHostInfo::localHostName(); // find out name of this machine

    if(!name.isEmpty())
    {
        ui->hostCombo->addItem(name);

        const QString &domain = QHostInfo::localDomainName();

        if (!domain.isEmpty())
            ui->hostCombo->addItem(name + QChar('.') + domain);
    }

    if(name != QLatin1String("localhost"))
        ui->hostCombo->addItem(QString("localhost"));

    const QList<QHostAddress> &ipAddressesList = QNetworkInterface::allAddresses(); // find out IP addresses of this machine

    // add non-localhost addresses
    for(const QHostAddress &entry: ipAddressesList)
    {
        if (!entry.isLoopback())
            ui->hostCombo->addItem(entry.toString());
    }

    // add localhost addresses
    for(const QHostAddress &entry: ipAddressesList)
    {
        if(entry.isLoopback())
            ui->hostCombo->addItem(entry.toString());
    }

    connect(ui->streamButton, &QAbstractButton::clicked, this, [this] { client.startStreaming(ui->hostCombo->currentText(), ui->portLineEdit->text().toInt()); } );
    connect(&client, &Client::sendFrame, this, [this] (const QPixmap &pixmap) { ui->desktopView->setPixmap(pixmap.scaled(ui->desktopView->size())); });
}
