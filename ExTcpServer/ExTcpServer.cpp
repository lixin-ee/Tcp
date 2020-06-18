/*
 * Copyright (C)  2019 ~ 2019 touwoyimuli.  All rights reserved.
 *
 * Author:  touwoyimuli <touwoyimuli@gmai.com>
 *
 * github:  https://github.com/touwoyimuli
 * blogs:   https://touwoyimuli.github.io/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://touwoyimuli.github.io/>.
 */
#include "ExTcpServer.h"
#include "ui_ExTcpServer.h"
#include <QMessageBox>

ExTcpServer::ExTcpServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ExTcpServer)
{
    ui->setupUi(this);

    m_labListen = new QLabel("监听状态：");
    m_labSocket = new QLabel("在线人数：");
    m_labListen->setMidLineWidth(200);
    m_labSocket->setMinimumWidth(200);
    ui->statusBar->addWidget(m_labListen);
    ui->statusBar->addWidget(m_labSocket);
    ui->plainTextEdit->setReadOnly(true);
    QString localeIp = getLocalIp();
    setWindowTitle(windowTitle() + "---IP地址：" + localeIp);
    ui->comboBox->addItem(localeIp);

    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

ExTcpServer::~ExTcpServer()
{
    delete ui;
}

QString ExTcpServer::getLocalIp()
{
    QString hostName = QHostInfo::localHostName();
    QHostInfo hostInfo = QHostInfo::fromName(hostName);
    ui->plainTextEdit->appendPlainText("本机名称：" + hostName);
    QString locaIp;

    QList<QHostAddress> list = hostInfo.addresses();

    if (list.empty())
        return "null QString";

    foreach (QHostAddress addr, list) {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
            locaIp = addr.toString();
            break;
        }
    }

    return locaIp;
}

void ExTcpServer::closeEvent(QCloseEvent *event)   //关闭窗口时候停止监听
{
    if (m_tcpServer->isListening())
        m_tcpServer->close();

    event->accept();
}

void ExTcpServer::on_actStart_triggered()
{
    QString Ip = ui->comboBox->currentText();
    quint16 port = ui->spinBox->value();

    m_tcpServer->listen(QHostAddress::Any, port);  //监听指定的 IP 和指定的 port
    ui->plainTextEdit->appendPlainText("服务器地址为:" + m_tcpServer->serverAddress().toString() + "   服务器端口：" + QString::number(m_tcpServer->serverPort()));
    ui->plainTextEdit->appendPlainText("开始监听...");

    ui->actStart->setEnabled(false);
    ui->actStop->setEnabled(true);
    m_labListen->setText("监听状态：正在监听...");
}

void ExTcpServer::on_actStop_triggered()
{
    if (!m_tcpServer->isListening())
        return;
    QString tempstr1="服务器断开连接";
    QByteArray str = tempstr1.toUtf8();
    str.append('\n');
    for(auto& x:m_tcpSocket)
    {
     x->write(str);
    }
    m_tcpServer->close();  //停止监听

    ui->actStart->setEnabled(true);
    ui->actStop->setEnabled(false);
    m_labListen->setText("监听状态：监听已经停止");

}

void ExTcpServer::on_actClear_triggered()
{
    ui->plainTextEdit->clear();
}

void ExTcpServer::on_actQuit_triggered()
{
    close();
}

void ExTcpServer::on_btnSend_clicked()
{
    QString msg = ui->lineEdit->text();
    if(msg==NULL) {QMessageBox::information(NULL,"提示","输入不能为空");}
    else
    {
     if(count==0){QMessageBox::information(NULL,"提示","无人在线");}
     else
     {
        ui->plainTextEdit->appendPlainText("[服务器:]" + msg+'\n');
        ui->lineEdit->clear();
        ui->plainTextEdit->hasFocus();
       QString tempstr1="[服务器:]" + msg;
       QByteArray str = tempstr1.toUtf8();
       str.append('\n');
       for(auto& x:m_tcpSocket)
       {
         x->write(str);
       }
     }
    }
}
void ExTcpServer::onSocketReadyRead()     //读取缓冲区行文本
{
    QString tempstr1;
    QString tempstr;
     QByteArray str1;
      QByteArray str;
      int tempcount=0;
    for(auto& x:m_tcpSocket)
    {
        while (x->canReadLine()) {
             tempstr1=x->readLine();
             if(clientname[tempcount]=="")
             {
               QStringList list=tempstr1.split(QRegExp("[{}]"));
               clientname[tempcount]=list[1];
                tempstr= "<"+QString::number(count,10)+">\n";
                str1 = tempstr.toUtf8();
               for(auto& x:m_tcpSocket)
               {
                x->write(str1);
               }
             }
                 ui->plainTextEdit->appendPlainText(tempstr1);
                  str =(tempstr1).toUtf8();
                 for(auto& q:m_tcpSocket)
                 {
                     if(q!=x)
                  q->write(str);
                 }
        } 
     tempcount++;
    }
}

void ExTcpServer::onNewConnection()
{
    m_tcpSocket.append(m_tcpServer->nextPendingConnection());
    clientname.append("");
    connect(m_tcpSocket[count], SIGNAL(connected()), this, SLOT(onClientConnected()));
    connect(m_tcpSocket[count], SIGNAL(disconnected()), this, SLOT(onClientDisonnected()));
    connect(m_tcpSocket[count], SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
    count++;
    QString tempstr= QString::number(count,10);
  m_labSocket->setText("在线人数:"+tempstr);
}
void::ExTcpServer::onClientConnected()
{
}
void::ExTcpServer::onClientDisonnected()
{
   QTcpSocket* tempsocket=qobject_cast<QTcpSocket*>(sender());
   int tempcount=0;
   QByteArray str;
   for (auto& x:m_tcpSocket)
   {
    if(tempsocket==x)
    {

        m_tcpSocket.erase(m_tcpSocket.begin()+tempcount);
        QString tempstr1="{"+clientname[tempcount]+"}已退出聊天\n";
        ui->plainTextEdit->appendPlainText(tempstr1);
        str = (tempstr1).toUtf8();
        for(auto& x:m_tcpSocket)
        {
         x->write(str);
        }
        clientname.erase(clientname.begin()+tempcount);
    }
    else tempcount++;
   }
   count=tempcount;
   QString tempstr= QString::number(count,10);
   m_labSocket->setText("在线人数:"+tempstr);
   QString tempstr1= "<"+tempstr+">\n";
   str = tempstr1.toUtf8();
   for(auto& x:m_tcpSocket)
   {
    x->write(str);
   }

}

