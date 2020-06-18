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
#include "ExTcpClient.h"
#include "ui_ExTcpClient.h"

ExTcpClient::ExTcpClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ExTcpClient)
{
    ui->setupUi(this);

    m_labSocket = new QLabel("socket状态：");
    zaixian=new QLabel("在线人数：");
    m_labSocket->setMidLineWidth(200);
    zaixian->setMinimumWidth(200);
    ui->statusBar->addWidget(m_labSocket);
    ui->statusBar->addWidget(zaixian);
    m_tcpSocket = new QTcpSocket(this);
    QString localIp =getLocalIp() ;
    this->setWindowTitle(windowTitle() + "----本机IP:" + localIp);
    ui->comboBox->addItem("182.254.145.163");
    ui->spinBox->setValue(24798);
    ui->plainTextEdit->setReadOnly(true);
    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(m_tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChange(QAbstractSocket::SocketState)));
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
}

ExTcpClient::~ExTcpClient()
{
    delete ui;
}

QString ExTcpClient::getLocalIp()
{
    QString hostName = QHostInfo::localHostName();
    QHostInfo hostInfo = QHostInfo::fromName(hostName);
    ui->plainTextEdit->appendPlainText("本机名称：" + hostName);
    QString localIp;

    foreach (QHostAddress addr, hostInfo.addresses()) {
        if (QAbstractSocket::IPv4Protocol == addr.protocol()) {
            localIp = addr.toString();

            break;
        }
    }
    return localIp;
}

void ExTcpClient::closeEvent(QCloseEvent *event)
{
    if (m_tcpSocket->state() == QAbstractSocket::ConnectedState)
        m_tcpSocket->disconnectFromHost();

    event->accept();
}

void ExTcpClient::onConnected()
{
    ui->plainTextEdit->appendPlainText("你已连接至服务器，( •̀ ω •́ )y\n");
    ui->actConnect->setEnabled(false);
    ui->actDisconnect->setEnabled(true);
    QString tempstr1="{"+name+"}已加入聊天";
    QByteArray str = tempstr1.toUtf8();
    str.append('\n');
    m_tcpSocket->write(str);
}

void ExTcpClient::onDisconnected()
{
    ui->plainTextEdit->appendPlainText("你已经断开与服务器的连接┭┮﹏┭┮\n");
    zaixian->setText("在线人数:");
    ui->actConnect->setEnabled(true);
    ui->actDisconnect->setEnabled(false);
}

void ExTcpClient::onSocketReadyRead()
{
    QString tempstr1;
    while (m_tcpSocket->canReadLine())
    {
        tempstr1=m_tcpSocket->readLine();
        if(tempstr1.startsWith("<")&&tempstr1.endsWith(">\n"))
        {
            count =tempstr1.mid(1,1);
            zaixian->setText("在线人数:"+count);
        }
        else
        ui->plainTextEdit->appendPlainText(tempstr1);
    }
}

void ExTcpClient::onSocketStateChange(QAbstractSocket::SocketState socketState)
{
    switch (socketState) {
    case QAbstractSocket::UnconnectedState:
        m_labSocket->setText("socket状态：UnconnectedState");
        break;
    case QAbstractSocket::HostLookupState:
        m_labSocket->setText("socket状态：HostLookupState");
        break;
    case QAbstractSocket::ConnectingState:
        m_labSocket->setText("socket状态：ConnectingState");
        break;
    case QAbstractSocket::ConnectedState:
        m_labSocket->setText("socket状态：ConnectedState");
        break;
    case QAbstractSocket::BoundState:
        m_labSocket->setText("socket状态：BoundState");
        break;
    case QAbstractSocket::ClosingState:
        m_labSocket->setText("socket状态：ClosingState");
        break;
    case QAbstractSocket::ListeningState:
        m_labSocket->setText("socket状态：ListeningState");
        break;
    default:
        m_labSocket->setText("socket状态：其他未知状态...");
        break;
    }
}

void ExTcpClient::on_actConnect_triggered()
{
      bool isOK=false;
    while(name==NULL&&(!isOK))
    {
        name = QInputDialog::getText(NULL, "设置昵称", "Please input your name", QLineEdit::Normal,NULL, &isOK);
    if(isOK)
      {
       if(name==NULL) {QMessageBox::information(NULL,"提示","昵称不能为空");isOK=false;}

      }
    else isOK=true;
    }

  if(name!=NULL)
  {
    QString addr = ui->comboBox->currentText();
    quint16 port = ui->spinBox->value();
    m_tcpSocket->connectToHost(addr, port);
  }
}

void ExTcpClient::on_actDisconnect_triggered()
{
    if(m_tcpSocket->state() == QAbstractSocket::ConnectedState)
        m_tcpSocket->disconnectFromHost();
}

void ExTcpClient::on_actClear_triggered()
{
    ui->plainTextEdit->clear();
}

void ExTcpClient::on_actQuit_triggered()
{
    close();
}

void ExTcpClient::on_btnSend_clicked()
{
    if(m_tcpSocket->state() == QAbstractSocket::UnconnectedState)
    {
        QMessageBox::information(NULL,"提示","您的连接已断开");
    }
    else
    {

    QString msg = ui->lineEdit->text();
    if(msg==NULL) {QMessageBox::information(NULL,"提示","输入不能为空");}
    else
    {
       ui->plainTextEdit->appendPlainText("["+name+"]:" + msg+'\n');
       ui->lineEdit->clear();
       ui->lineEdit->setFocus();
       QString tempstr="["+name+"]:" + msg;
       QByteArray str = tempstr.toUtf8();
       str.append('\n');
       m_tcpSocket->write(str);
    }
    }
}
