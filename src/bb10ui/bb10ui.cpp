#include "bb10ui.h"

#include <bb/cascades/ActionItem>
#include <bb/cascades/Application>
#include <bb/cascades/Container>
#include <bb/cascades/Divider>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Label>
#include <bb/cascades/LayoutOrientation>
#include <bb/cascades/ListView>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/NavigationPaneProperties>
#include <bb/cascades/Page>
#include <bb/cascades/ScrollView>
#include <bb/cascades/StackLayout>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/TextArea>
#include <bb/cascades/TextField>

#include <bb/cascades/resources/titlebar.h>
#include <bb/cascades/controls/chromevisibility.h>

#include <bb/platform/Notification>
#include <bb/platform/NotificationDialog>
#include <bb/platform/NotificationError>
#include <bb/platform/NotificationResult>

#include <QModelIndex>

#include "bb10uisettings.h"
#include "bb10uistyle.h"
#include "buffermodel.h"
#include "bufferviewoverlay.h"
#include "channellistviewfilter.h"
#include "chatlinemodel.h"
#include "chatview.h"
#include "client.h"
#include "clientbufferviewconfig.h"
#include "clientbufferviewmanager.h"
#include "clientidentity.h"
#include "clientignorelistmanager.h"
#include "datamodeladapter.h"
#include "simplesetuppage.h"
#include "uimessageprocessor.h"

Bb10Ui *Bb10Ui::s_instance = 0;
Bb10UiStyle* Bb10Ui::s_uiStyle = 0;

using namespace bb::platform;
using namespace bb::system;

Bb10Ui::Bb10Ui(Application *app)
    : m_simpleSetupPage(0)
    , m_navPane(0)
    , m_identity(0)
    , m_appState(FullScreen)
{
    if (s_instance != 0) {
        qWarning() << "Bb10Ui has been instantiated again!";
        return;
    }
    s_instance = this;

    bool connectResult;
    Q_UNUSED(connectResult);

    m_navPane = new NavigationPane();

    app->setScene(m_navPane);
}

Bb10Ui::~Bb10Ui()
{
    if (m_simpleSetupPage)
        delete m_simpleSetupPage;
}

void Bb10Ui::init()
{
    qDebug() << "xxxxx Bb10Ui::init";
    s_uiStyle = new Bb10UiStyle(this);
    m_chatListPage = Page::create();
    Container* container = new Container();
    DockLayout* chatListLayout = new DockLayout();
    container->setLayout(chatListLayout);
    m_channelListView = new ListView();
    container->add(m_channelListView);
    m_chatListPage->setContent(container);
    connect(m_channelListView, SIGNAL(triggered(const QVariantList)), this, SLOT(onChannelListTriggered(const QVariantList)));
    ActionItem* editIdentity = ActionItem::create()
    .title("Edit Identity");
    connect(editIdentity, SIGNAL(triggered()), Bb10Ui::instance(), SLOT(showNewIdentityPage()));
    m_chatListPage->addAction(editIdentity, ActionBarPlacement::OnBar);

    m_connect = ActionItem::create().title("Connect");
    m_chatListPage->addAction(m_connect, ActionBarPlacement::OnBar);
    connect(m_connect, SIGNAL(triggered()), this, SLOT(toggleConnection()));

    m_navPane->push(m_chatListPage);

    connect(Application::instance(), SIGNAL(fullscreen()), this, SLOT(onFullscreen));
    connect(Application::instance(), SIGNAL(thumbnail()), this, SLOT(onThumbnail));

    connect(Client::instance(), SIGNAL(networkCreated(NetworkId)), SLOT(clientNetworkCreated(NetworkId)));
    connect(Client::coreConnection(), SIGNAL(startCoreSetup(QVariantList)), SLOT(showCoreConfigWizard(QVariantList)));
    connect(Client::coreConnection(), SIGNAL(connectionErrorPopup(QString)), SLOT(handleCoreConnectionError(QString)));
    connect(Client::coreConnection(), SIGNAL(coreSetupSuccess), SLOT(showCoreSetupSuccess));
    connect(Client::messageModel(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), SLOT(messagesInserted(const QModelIndex &, int, int)));
    CoreConnection *conn = Client::coreConnection();
    if (!conn->connectToCore()) {
        qDebug() << "xxxxx !conn->connectToCore";
        // No autoconnect selected (or no accounts)
        //showCoreConnectionDlg();
    } else
        qDebug() << "xxxxx conn->connectToCore success!";

    m_invokeRequest.setAction("bb.action.OPEN");
    m_invokeRequest.setTargetTypes(InvokeTarget::Application);
    m_invokeRequest.setMimeType("text/plain");
    m_invokeRequest.setTarget("irc");
    
}

void Bb10Ui::clientNetworkCreated(NetworkId id)
{
    qDebug() << "xxxxx clientNetworkCreated id = " << id;
}

void Bb10Ui::showCoreConfigWizard(const QVariantList &backend)
{
    Q_UNUSED(backend);
    qDebug() << "xxxxx showCoreConfigWizard";
}

void Bb10Ui::showCoreSetupSuccess()
{
    qDebug() << "xxxxx showCoreSetupSuccess";
}

MessageModel *Bb10Ui::createMessageModel(QObject *parent)
{
    return new ChatLineModel(parent);
}

void Bb10Ui::handleCoreConnectionError(const QString &error)
{
    qDebug() << "xxxxx Core Connection Error " << error;
    //QMessageBox::critical(this, tr("Core Connection Error"), error, QMessageBox::Ok);
}

void Bb10Ui::connectedToCore()
{
    qDebug() << "xxxxx Bb10Ui::connectedToCore";
    connect(Client::bufferViewManager(), SIGNAL(bufferViewConfigAdded(int)), this, SLOT(bufferViewConfigAdded(int)));
    connect(Client::bufferViewManager(), SIGNAL(bufferViewConfigDeleted(int)), this, SLOT(bufferViewConfigDeleted(int)));
    connect(Client::bufferViewManager(), SIGNAL(initDone()), this, SLOT(bufferViewManagerInitDone()));
    setConnectedState();
}

void Bb10Ui::setConnectedState()
{
    // TODO: Update UI
    if (Client::networkIds().isEmpty()) {
        showNewIdentityPage();
    } else {
        NetworkId id = Client::networkIds()[0];
        const Network* net = Client::network(id);
        m_networkInfo = net->networkInfo();
        connect(net, SIGNAL(connectedSet(bool)), this, SLOT(updateConnectionState(bool)));
        if (net->connectionState() != Network::Disconnected)
            m_connect->setEnabled(false);
    }
}

void Bb10Ui::showNewIdentityPage()
{
    if (Client::identityIds().isEmpty()) {
        m_identity = new CertIdentity(-1, this);
        m_identity->setToDefaults();
        m_identity->setIdentityName(tr("Default Identity"));
    } else {
        m_identity = new CertIdentity(*Client::identity(Client::identityIds().first()), this);
    }

    QStringList defaultNets = Network::presetNetworks(true);
    QStringList defaultChannels;
    qDebug() << "xxxxx Bb10Ui::showNewIdentityPage defaultNets.isEmpty = " << defaultNets.isEmpty();
    if (!defaultNets.isEmpty()) {
        NetworkInfo info = Network::networkInfoFromPreset(defaultNets[0]);
        if (!info.networkName.isEmpty()) {
            m_networkInfo = info;
            qDebug() << "xxxxx Bb10Ui::showNewIdentityPage" << info.networkName << info.serverList.size();
            defaultChannels = Network::presetDefaultChannels(defaultNets[0]);
        }
    }

    Network::Server defaultServer;
    if (m_networkInfo.serverList.size()) {
        defaultServer = m_networkInfo.serverList[0];
    }
    if (!m_simpleSetupPage)
        m_simpleSetupPage = new SimpleSetupPage();
    m_navPane->push(m_simpleSetupPage->getPage());
    m_simpleSetupPage->displayIdentity(m_identity);
    m_simpleSetupPage->displayNetworkInfo(m_networkInfo);
    m_simpleSetupPage->setDefaultChannels(defaultChannels);
}

void Bb10Ui::saveIdentityAndServer()
{
    qDebug() << "xxxxx Bb10Ui::saveIdentityAndServer";
    m_simpleSetupPage->saveToIdentity(m_identity);
    if (m_identity->id().isValid()) {
        Client::updateIdentity(m_identity->id(), m_identity->toVariantMap());
        identityReady(m_identity->id());
    }
    else {
        connect(Client::instance(), SIGNAL(identityCreated(IdentityId)), this, SLOT(identityReady(IdentityId)));
        Client::createIdentity(*m_identity);
    }
    m_navPane->pop();
}

void Bb10Ui::identityReady(IdentityId id)
{
    disconnect(Client::instance(), SIGNAL(identityCreated(IdentityId)), this, SLOT(identityReady(IdentityId)));
    NetworkInfo networkInfo = m_networkInfo;
    m_simpleSetupPage->saveToNetworkInfo(networkInfo);
    networkInfo.identity = id;
    connect(Client::instance(), SIGNAL(networkCreated(NetworkId)), this, SLOT(networkReady(NetworkId)));
    Client::createNetwork(networkInfo, m_simpleSetupPage->channels());
}

void Bb10Ui::networkReady(NetworkId id)
{
    qDebug() << "xxxxx Bb10Ui::networkReady " << id;
    disconnect(Client::instance(), SIGNAL(networkCreated(NetworkId)), this, SLOT(networkReady(NetworkId)));
    const Network *net = Client::network(id);
    Q_ASSERT(net);
    net->requestConnect();
}

void Bb10Ui::bufferViewConfigAdded(int bufferViewConfigId)
{
    qDebug() << "xxxxx Bb10Ui::bufferViewConfigAdde id = " << bufferViewConfigId;
    ClientBufferViewConfig *config = Client::bufferViewManager()->clientBufferViewConfig(bufferViewConfigId);
    qDebug() << "xxxxx Bb10Ui::bufferViewConfigAdde config->networkId = " << config->networkId().toInt() << " bufferViewName = " << config->bufferViewName() << "bufferList = " << config->bufferList().size() << config->bufferList();
    Client::bufferModel()->sort(0);
    //m_channelListView->setDataModel(new DataModelAdapter(Client::bufferModel()));
    DataModelAdapter* model = new DataModelAdapter(new ChannelListViewFilter(Client::bufferModel(), config));
    connect(config, SIGNAL(configChanged()), model, SIGNAL(handleLayoutChanged()));
    m_channelListView->setDataModel(model);
}

void Bb10Ui::bufferViewConfigDeleted(int bufferViewConfigId)
{
    qDebug() << "xxxxx Bb10Ui::bufferViewConfigDeleted id = " << bufferViewConfigId;
}
void Bb10Ui::bufferViewManagerInitDone()
{
    qDebug() << "xxxxx bufferViewManagerInitDone";
}

AbstractMessageProcessor *Bb10Ui::createMessageProcessor(QObject *parent)
{
    return new UiMessageProcessor(parent);
}

void Bb10Ui::onChannelListTriggered(const QVariantList index)
{
    QModelIndex modelIndex = static_cast<DataModelAdapter*>(m_channelListView->dataModel())->getQModelIndex(index);
    BufferInfo bufferInfo = modelIndex.data(NetworkModel::BufferInfoRole).value<BufferInfo>();
    BufferId id = bufferInfo.bufferId();
    QString bufferName = bufferInfo.bufferName();
    
    qDebug() << "xxxxx Bb10Ui::onChannelListTriggered bufferInfo = " << bufferInfo << " index = " << index << " modelIndex = " << modelIndex;
    ChatView *view = qobject_cast<ChatView *>(m_chatViews.value(id));
    if (!view) {
        view = new ChatView(id, bufferName);
        m_chatViews[id] = view;
    }
    m_navPane->push(view->getPage());
}

void Bb10Ui::navPanePop()
{
    m_navPane->pop();
}

void Bb10Ui::navPanePush(Page* page)
{
    m_navPane->push(page);
}

void Bb10Ui::messagesInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    bool hasFocus = Application::instance()->isFullscreen();
    qDebug() << "xxxxx Bb10Ui::messagesInserted m_appState = " << m_appState << " Application::instance()->isFullScreen() = " << Application::instance()->isFullscreen();
    for (int i = start; i <= end; i++) {
        QModelIndex idx = Client::messageModel()->index(i, ChatLineModel::ContentsColumn);
        if (!idx.isValid()) {
            qDebug() << "Bb10Ui::messagesInserted(): Invalid model index!";
            continue;
        }
        Message::Flags flags = (Message::Flags)idx.data(ChatLineModel::FlagsRole).toInt();
        if (flags.testFlag(Message::Backlog) || flags.testFlag(Message::Self))
            continue;

        BufferId bufId = idx.data(ChatLineModel::BufferIdRole).value<BufferId>();
        BufferInfo::Type bufType = Client::networkModel()->bufferType(bufId);

        // check if bufferId belongs to the shown chatlists
        if (!m_chatViews.contains(bufId)) {
            qDebug() << "xxxxx Bb10Ui::messagesInserted line 297";
            continue;
        }

        // check if it's the buffer currently displayed
        if (hasFocus && bufId == Client::bufferModel()->currentBuffer()) {
            qDebug() << "xxxxx Bb10Ui::messagesInserted line 303";
            continue;
        }

        // only show notifications for higlights or queries
        if (bufType != BufferInfo::QueryBuffer && !(flags & Message::Highlight)) {
            qDebug() << "xxxxx Bb10Ui::messagesInserted line 309";
            continue;
        }

        // and of course: don't notify for ignored messages
        if (Client::ignoreListManager() && Client::ignoreListManager()->match(idx.data(MessageModel::MessageRole).value<Message>(), Client::networkModel()->networkName(bufId))) {
            qDebug() << "xxxxx Bb10Ui::messagesInserted line 315";
            continue;
        }

        // seems like we have a legit notification candidate!
        QModelIndex senderIdx = Client::messageModel()->index(i, ChatLineModel::SenderColumn);
        QString sender = senderIdx.data(ChatLineModel::EditRole).toString();
        QString contents = idx.data(ChatLineModel::DisplayRole).toString();
        /*
        AbstractNotificationBackend::NotificationType type;

        if (bufType == BufferInfo::QueryBuffer && !hasFocus)
            type = AbstractNotificationBackend::PrivMsg;
        else if (bufType == BufferInfo::QueryBuffer && hasFocus)
            type = AbstractNotificationBackend::PrivMsgFocused;
        else if (flags & Message::Highlight && !hasFocus)
            type = AbstractNotificationBackend::Highlight;
        else
            type = AbstractNotificationBackend::HighlightFocused;
        */
        //QtUi::instance()->invokeNotification(bufId, type, sender, contents);
        if (hasFocus) {
            // process in-app notification
        } else {
            QString bufferName = (bufType == BufferInfo::QueryBuffer) ? "Private msg" : Client::networkModel()->bufferName(bufId);
            Notification* pNotification = new Notification();

            pNotification->setTitle("Quassel IRC for BB10");
            pNotification->setBody("[" + bufferName + "] " + sender + " says: " + contents);
            pNotification->setInvokeRequest(m_invokeRequest);
            // FIXME: call setData to add bufferId, so that we can open the page.
            pNotification->notify();
        }
    }
}

void Bb10Ui::onFullscreen()
{
    qDebug() << "xxxxx Bb10Ui::onFullscreen";
    m_appState = FullScreen;
}

void Bb10Ui::onThumbnail()
{
    qDebug() << "xxxxx Bb10Ui::onThumbnail";
    m_appState = Thumbnail;
}

void Bb10Ui::toggleConnection()
{
    const Network *net = Client::network(m_networkInfo.networkId);
    if (net && net->isConnected() && m_connect->title() == "Disconnect") {
        m_connect->setEnabled(false);
        net->requestDisconnect();
        return;
    }
    if (net && !net->isConnected() && m_connect->title() == "Connect") {
        m_connect->setEnabled(false);
        net->requestConnect();
    }
        
}

void Bb10Ui::updateConnectionState(bool connected)
{
    if (connected)
        m_connect->setTitle("Disconnect");
    else
        m_connect->setTitle("Connect");
    m_connect->setEnabled(true);
}
