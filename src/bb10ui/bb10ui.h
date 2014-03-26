#ifndef BB10UI_H
#define BB10UI_H

#include <bb/cascades/Application>

#include "abstractui.h"
#include "types.h"

using namespace bb::cascades;

class MessageModel;
class UiMessageProcessor;

namespace bb
{
    namespace cascades
    {
        class Container;
        class ListView;
        class NavigationPane;
        class Page;
    }
}

class Bb10Ui : public AbstractUi
{
    Q_OBJECT
public:
    Bb10Ui(Application *app);
    virtual ~Bb10Ui() { }

    MessageModel *createMessageModel(QObject *parent);
    AbstractMessageProcessor *createMessageProcessor(QObject *parent);

    void init();
    //Page *createChannelList();
protected slots:
    void connectedToCore();
    void setConnectedState();
private slots:
    void clientNetworkCreated(NetworkId);
    void showCoreConfigWizard(const QVariantList &);
    void handleCoreConnectionError(const QString &error);
    void showCoreSetupSuccess();
    void saveIdentityAndServer();
private:
    void showNewIdentityPage();
    NavigationPane *m_navPane;
};

#endif
