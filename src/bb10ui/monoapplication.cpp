#include "monoapplication.h"
#include "coreapplication.h"
#include "client.h"
#include "core.h"

class InternalPeer;

MonolithicApplication::MonolithicApplication(int argc, char **argv)
    : UiApplication(argc, argv),
    _internalInitDone(false)
{
    _internal = new CoreApplicationInternal(); // needed for parser options
    disableCrashhandler();
    QStringList paths;
    paths << findDataDirPaths();
    paths << QCoreApplication::applicationDirPath() + "/apps/quassel/";
    setDataDirPaths(paths);
    setRunMode(Quassel::Monolithic);
    qDebug() << "xxxxx MonolithicApplication constructor";
}


bool MonolithicApplication::init()
{
    qDebug() << "xxxxx MonolithicApplication::init before Quassel::init";
    if (!Quassel::init()) // parse args
        return false;
    qDebug() << "xxxxx MonolithicApplication::init";
    connect(Client::coreConnection(), SIGNAL(startInternalCore()), SLOT(startInternalCore()));

    // FIXME what's this for?
    //if (isOptionSet("port")) {
    //    startInternalCore();
    //}

    return UiApplication::init();
}


MonolithicApplication::~MonolithicApplication()
{
    // Client needs to be destroyed first
    Client::destroy();
    delete _internal;
}


void MonolithicApplication::startInternalCore()
{
    qDebug() << "xxxxx MonolithicApplication::startInternalCore";
    if (!_internalInitDone) {
        _internal->init();
        _internalInitDone = true;
    }
    Core *core = Core::instance();
    CoreConnection *connection = Client::coreConnection();
    connect(connection, SIGNAL(connectToInternalCore(InternalPeer*)), core, SLOT(setupInternalClientSession(InternalPeer*)));
    connect(core, SIGNAL(sessionState(Protocol::SessionState)), connection, SLOT(internalSessionStateReceived(Protocol::SessionState)));
}
