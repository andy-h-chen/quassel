#ifndef SIMPLESETUPPAGE_H
#define SIMPLESETUPPAGE_H

#include <bb/cascades/TextArea>
#include <QObject>

#include "network.h"

using namespace bb::cascades;

namespace bb
{
    namespace cascades
    {
        class CheckBox;
        class Page;
        class TextArea;
        class TextField;
    }
}

class CertIdentity;

class SimpleSetupPage : public QObject
{
    Q_OBJECT
public:
    SimpleSetupPage(QObject* parent = 0);
    ~SimpleSetupPage();

    Page* getPage() { return m_page; }
    void displayIdentity(CertIdentity *id, CertIdentity *saveId = 0);
    void saveToIdentity(CertIdentity *id);
    void displayNetworkInfo(const NetworkInfo &networkInfo);
    void saveToNetworkInfo(NetworkInfo &networkInfo);
    void setDefaultChannels(const QStringList& channels);
    QStringList channels();
private:
    Page* m_page;
    TextField* m_realName;
    TextField* m_nickName;
    TextField* m_alterNick;
    TextField* m_awayMessage;
    TextField* m_networkName;
    TextField* m_serverAddr;
    TextField* m_port;
    TextField* m_password;
    CheckBox* m_useSSL;
    TextArea* m_autojoin;
};

#endif
