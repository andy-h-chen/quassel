#ifndef BB10UI_H
#define BB10UI_H

#include <bb/cascades/Application>

#include "abstractui.h"
#include "network.h"
#include "types.h"

using namespace bb::cascades;

class Bb10UiStyle;
class CertIdentity;
class ChatView;
class MessageModel;
class SimpleSetupPage;
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
    friend class SimpleSetupPage;
    friend class ChatView;
    Bb10Ui(Application *app);
    virtual ~Bb10Ui();

    MessageModel *createMessageModel(QObject *parent);
    AbstractMessageProcessor *createMessageProcessor(QObject *parent);

    inline static Bb10Ui *instance() { return s_instance; }
    void init();

    inline static Bb10UiStyle *uiStyle();
    //Page *createChannelList();
protected slots:
    void connectedToCore();
    void setConnectedState();
    void saveIdentityAndServer();
    void identityReady(IdentityId);
    void networkReady(NetworkId);
    void bufferViewConfigAdded(int);
    void bufferViewConfigDeleted(int);
    void bufferViewManagerInitDone();
    void navPanePop();
    void navPanePush(Page*);

private slots:
    void clientNetworkCreated(NetworkId);
    void showCoreConfigWizard(const QVariantList &);
    void handleCoreConnectionError(const QString &error);
    void showCoreSetupSuccess();
    // Channel list clicked
    void onChannelListTriggered(const QVariantList);

private:
    void showNewIdentityPage();

    static Bb10Ui* s_instance;
    static Bb10UiStyle* s_uiStyle;
    SimpleSetupPage* m_simpleSetupPage;
    NavigationPane *m_navPane;
    Page* m_chatListPage;
    ListView* m_channelListView;
    CertIdentity *m_identity;
    NetworkInfo m_networkInfo;
    QHash<BufferId, ChatView*> m_chatViews;
};

Bb10UiStyle* Bb10Ui::uiStyle() { return s_uiStyle; }

#endif
