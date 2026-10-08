// Copyright (c) 2011-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/paymentserver.h>

#include <qt/guiutil.h>

#include <base58.h>
#include <chainparams.h>
#include <ui_interface.h>
#include <util.h>

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QFileOpenEvent>
#include <QHash>
#include <QList>
#include <QLocalServer>
#include <QLocalSocket>
#include <QMessageBox>
#include <QUrl>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif

const int BITCOIN_IPC_CONNECT_TIMEOUT = 1000; // milliseconds
const QString BITCOIN_IPC_PREFIX("garlicoin:");

//
// Create a name that is unique for:
//  testnet / non-testnet
//  data directory
//
static QString ipcServerName()
{
    QString name("GarlicoinQt");

    // Append a simple hash of the datadir.
    // GetDataDir(true) differs between mainnet and testnet.
    QString ddir(GUIUtil::boostPathToQString(GetDataDir(true)));
    name.append(QString::number(qHash(ddir)));

    return name;
}

// Store payment URIs received before the main GUI window is ready.
static QList<QString> savedPaymentRequests;

void PaymentServer::ipcParseCommandLine(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        QString arg(argv[i]);
        if (arg.startsWith("-"))
            continue;

        if (arg.startsWith(BITCOIN_IPC_PREFIX, Qt::CaseInsensitive))
        {
            savedPaymentRequests.append(arg);

            // Preserve the existing network-selection behavior for normal
            // garlicoin: URIs containing an address.
            SendCoinsRecipient r;
            if (GUIUtil::parseBitcoinURI(arg, &r) && !r.address.isEmpty())
            {
                auto tempChainParams = CreateChainParams(CBaseChainParams::MAIN);

                if (IsValidDestinationString(r.address.toStdString(), *tempChainParams)) {
                    SelectParams(CBaseChainParams::MAIN);
                } else {
                    tempChainParams = CreateChainParams(CBaseChainParams::TESTNET);
                    if (IsValidDestinationString(r.address.toStdString(), *tempChainParams)) {
                        SelectParams(CBaseChainParams::TESTNET);
                    }
                }
            }
        }
        else if (QFile::exists(arg))
        {
            // BIP70 PaymentRequest files are no longer supported and are not
            // parsed. Log this here because the GUI may not be ready yet.
            qWarning() << "PaymentServer::ipcParseCommandLine: BIP70 payment request files are no longer supported: " << arg;
        }
        else
        {
            qWarning() << "PaymentServer::ipcParseCommandLine: URI or file does not exist: " << arg;
        }
    }
}

bool PaymentServer::ipcSendCommandLine()
{
    bool fResult = false;
    for (const QString& r : savedPaymentRequests)
    {
        QLocalSocket* socket = new QLocalSocket();
        socket->connectToServer(ipcServerName(), QIODevice::WriteOnly);
        if (!socket->waitForConnected(BITCOIN_IPC_CONNECT_TIMEOUT))
        {
            delete socket;
            return false;
        }

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << r;
        out.device()->seek(0);

        socket->write(block);
        socket->flush();
        socket->waitForBytesWritten(BITCOIN_IPC_CONNECT_TIMEOUT);
        socket->disconnectFromServer();

        delete socket;
        fResult = true;
    }

    return fResult;
}

PaymentServer::PaymentServer(QObject* parent, bool startLocalServer) :
    QObject(parent),
    saveURIs(true),
    uriServer(0),
    optionsModel(0)
{
    // Install the global file-open event filter used for garlicoin: links.
    if (parent)
        parent->installEventFilter(this);

    QString name = ipcServerName();

    // Clean up an old local socket left behind by a crash.
    QLocalServer::removeServer(name);

    if (startLocalServer)
    {
        uriServer = new QLocalServer(this);

        if (!uriServer->listen(name)) {
            QMessageBox::critical(0, tr("Payment request error"),
                tr("Cannot start garlicoin: click-to-pay handler"));
        }
        else {
            connect(uriServer, SIGNAL(newConnection()), this, SLOT(handleURIConnection()));
        }
    }
}

PaymentServer::~PaymentServer()
{
}

bool PaymentServer::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *fileEvent = static_cast<QFileOpenEvent*>(event);
        if (!fileEvent->file().isEmpty())
            handleURIOrFile(fileEvent->file());
        else if (!fileEvent->url().isEmpty())
            handleURIOrFile(fileEvent->url().toString());

        return true;
    }

    return QObject::eventFilter(object, event);
}

void PaymentServer::uiReady()
{
    saveURIs = false;
    for (const QString& s : savedPaymentRequests)
        handleURIOrFile(s);
    savedPaymentRequests.clear();
}

void PaymentServer::handleURIOrFile(const QString& s)
{
    if (saveURIs)
    {
        savedPaymentRequests.append(s);
        return;
    }

    if (s.startsWith(BITCOIN_IPC_PREFIX, Qt::CaseInsensitive))
    {
#if QT_VERSION < 0x050000
        QUrl uri(s);
#else
        QUrlQuery uri((QUrl(s)));
#endif
        // r= is the legacy BIP70 PaymentRequest mechanism. Never fetch it.
        if (uri.hasQueryItem("r"))
        {
            Q_EMIT message(tr("URI handling"),
                tr("BIP70 payment requests are no longer supported. Please use a normal garlicoin: URI with a payment address."),
                CClientUIInterface::ICON_WARNING);
            return;
        }

        SendCoinsRecipient recipient;
        if (GUIUtil::parseBitcoinURI(s, &recipient))
        {
            if (!IsValidDestinationString(recipient.address.toStdString())) {
                Q_EMIT message(tr("URI handling"),
                    tr("Invalid payment address %1").arg(recipient.address),
                    CClientUIInterface::MSG_ERROR);
            }
            else {
                Q_EMIT receivedPaymentRequest(recipient);
            }
        }
        else {
            Q_EMIT message(tr("URI handling"),
                tr("URI cannot be parsed! This can be caused by an invalid Garlicoin address or malformed URI parameters."),
                CClientUIInterface::ICON_WARNING);
        }
        return;
    }

    if (QFile::exists(s))
    {
        Q_EMIT message(tr("Payment request file handling"),
            tr("BIP70 payment request files are no longer supported. Please use a normal garlicoin: URI with a payment address."),
            CClientUIInterface::ICON_WARNING);
    }
}

void PaymentServer::handleURIConnection()
{
    QLocalSocket *clientConnection = uriServer->nextPendingConnection();

    while (clientConnection->bytesAvailable() < (int)sizeof(quint32))
        clientConnection->waitForReadyRead();

    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));

    QDataStream in(clientConnection);
    in.setVersion(QDataStream::Qt_4_0);
    if (clientConnection->bytesAvailable() < (int)sizeof(quint16))
        return;

    QString msg;
    in >> msg;
    handleURIOrFile(msg);
}

void PaymentServer::setOptionsModel(OptionsModel *_optionsModel)
{
    optionsModel = _optionsModel;
}
