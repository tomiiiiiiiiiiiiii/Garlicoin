// Copyright (c) 2011-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/paymentserver.h>

#include <qt/guiutil.h>

#include <base58.h>
#include <chainparams.h>
#include <ui_interface.h>
#include <util.h>

#include <memory>

#include <openssl/x509_vfy.h>

#include <QApplication>
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

#if QT_VERSION < 0x050000
#include <QUrl>
#else
#include <QUrl>
#include <QUrlQuery>
#endif

const int BITCOIN_IPC_CONNECT_TIMEOUT = 1000; // milliseconds
const QString BITCOIN_IPC_PREFIX("garlicoin:");

struct X509StoreDeleter {
    void operator()(X509_STORE* store) const
    {
        X509_STORE_free(store);
    }
};

namespace {
std::unique_ptr<X509_STORE, X509StoreDeleter> certStore;
}

//
// Create a name that is unique for:
//  testnet / non-testnet
//  data directory
//
static QString ipcServerName()
{
    QString name("GarlicoinQt");

    // Append a simple hash of the datadir
    // Note that GetDataDir(true) returns a different path
    // for -testnet versus main net
    QString ddir(GUIUtil::boostPathToQString(GetDataDir(true)));
    name.append(QString::number(qHash(ddir)));

    return name;
}

// We store payment URIs received before the main GUI window is ready.
// Legacy BIP70 files may also be queued only so we can reject them cleanly
// once the UI is available.
static QList<QString> savedPaymentRequests;

void PaymentServer::LoadRootCAs(X509_STORE* store)
{
    // BIP70 is disabled. Keep this compatibility entry point temporarily so
    // callers built around the legacy PaymentServer interface still compile.
    // No system certificates are loaded and no certificate parsing is done.
    if (store) {
        certStore.reset(store);
    } else {
        certStore.reset(X509_STORE_new());
    }
}

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

            // Preserve existing network selection for normal BIP21-style URIs.
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
            // Keep the path long enough to show a clear unsupported-BIP70
            // message after the GUI is ready. Do not parse the file.
            savedPaymentRequests.append(arg);
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
            socket = nullptr;
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
        socket = nullptr;
        fResult = true;
    }

    return fResult;
}

PaymentServer::PaymentServer(QObject* parent, bool startLocalServer) :
    QObject(parent),
    saveURIs(true),
    uriServer(0),
    netManager(0),
    optionsModel(0)
{
    if (parent)
        parent->installEventFilter(this);

    QString name = ipcServerName();

    // Clean up old socket leftover from a crash.
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

void PaymentServer::initNetManager()
{
    // BIP70 network fetching has been removed. Kept as a temporary no-op
    // until the legacy interface is deleted in the cleanup commit.
}

void PaymentServer::uiReady()
{
    saveURIs = false;
    for (const QString& s : savedPaymentRequests)
    {
        handleURIOrFile(s);
    }
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
        // BIP70 used the r= parameter to point at a remote PaymentRequest.
        // Never fetch or parse it. Normal garlicoin: URIs continue below.
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
                Q_EMIT message(tr("URI handling"), tr("Invalid payment address %1").arg(recipient.address),
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
        return;
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
    if (clientConnection->bytesAvailable() < (int)sizeof(quint16)) {
        return;
    }
    QString msg;
    in >> msg;

    handleURIOrFile(msg);
}

bool PaymentServer::readPaymentRequestFromFile(const QString& filename, PaymentRequestPlus& request)
{
    Q_UNUSED(filename);
    Q_UNUSED(request);
    return false;
}

bool PaymentServer::processPaymentRequest(const PaymentRequestPlus& request, SendCoinsRecipient& recipient)
{
    Q_UNUSED(request);
    Q_UNUSED(recipient);
    return false;
}

void PaymentServer::fetchRequest(const QUrl& url)
{
    Q_UNUSED(url);
}

void PaymentServer::fetchPaymentACK(CWallet* wallet, const SendCoinsRecipient& recipient, QByteArray transaction)
{
    Q_UNUSED(wallet);
    Q_UNUSED(recipient);
    Q_UNUSED(transaction);
}

void PaymentServer::netRequestFinished(QNetworkReply* reply)
{
    Q_UNUSED(reply);
}

void PaymentServer::reportSslErrors(QNetworkReply* reply, const QList<QSslError>& errs)
{
    Q_UNUSED(reply);
    Q_UNUSED(errs);
}

void PaymentServer::setOptionsModel(OptionsModel *_optionsModel)
{
    this->optionsModel = _optionsModel;
}

void PaymentServer::handlePaymentACK(const QString& paymentACKMsg)
{
    Q_UNUSED(paymentACKMsg);
}

bool PaymentServer::verifyNetwork(const payments::PaymentDetails& requestDetails)
{
    return requestDetails.network() == Params().NetworkIDString();
}

bool PaymentServer::verifyExpired(const payments::PaymentDetails& requestDetails)
{
    return requestDetails.has_expires() && (int64_t)requestDetails.expires() < GetTime();
}

bool PaymentServer::verifySize(qint64 requestSize)
{
    return requestSize <= BIP70_MAX_PAYMENTREQUEST_SIZE;
}

bool PaymentServer::verifyAmount(const CAmount& requestAmount)
{
    return MoneyRange(requestAmount);
}

X509_STORE* PaymentServer::getCertStore()
{
    return certStore.get();
}
