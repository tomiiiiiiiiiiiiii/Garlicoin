// Copyright (c) 2011-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_PAYMENTSERVER_H
#define BITCOIN_QT_PAYMENTSERVER_H

// Handles normal garlicoin: payment URIs and URI-based application switching.
// Legacy BIP70 PaymentRequest support has been removed.

#include <qt/walletmodel.h>

#include <QObject>
#include <QString>

class OptionsModel;

QT_BEGIN_NAMESPACE
class QLocalServer;
QT_END_NAMESPACE

class PaymentServer : public QObject
{
    Q_OBJECT

public:
    // Parse payment URIs on the command line.
    static void ipcParseCommandLine(int argc, char *argv[]);

    // Forward command-line URIs to an already-running GUI instance.
    static bool ipcSendCommandLine();

    explicit PaymentServer(QObject* parent, bool startLocalServer = true);
    ~PaymentServer();

    // Retained for compatibility with the existing GUI initialization flow.
    void setOptionsModel(OptionsModel *optionsModel);

Q_SIGNALS:
    // Fired when a valid normal garlicoin: payment URI is received.
    void receivedPaymentRequest(SendCoinsRecipient);

    // Fired when a message should be reported to the user.
    void message(const QString &title, const QString &message, unsigned int style);

public Q_SLOTS:
    // Signal this when the main window's UI is ready to display queued URIs.
    void uiReady();

    // Handle an incoming normal garlicoin: URI. Legacy BIP70 inputs are rejected.
    void handleURIOrFile(const QString& s);

private Q_SLOTS:
    void handleURIConnection();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    bool saveURIs;
    QLocalServer* uriServer;
    OptionsModel *optionsModel;
};

#endif // BITCOIN_QT_PAYMENTSERVER_H
