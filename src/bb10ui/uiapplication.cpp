#include "uiapplication.h"

#include "bb10ui.h"
#include "client.h"

using namespace bb::cascades;

UiApplication::UiApplication(int argc, char **argv)
    : Application(argc, argv)
    , Quassel()
{

}

bool UiApplication::init()
{
    if (Quassel::init()) {
        QSettings::Format format = QSettings::NativeFormat;
        QString newFilePath = Quassel::configDirPath() + "quasselclient"
                              + ((format == QSettings::NativeFormat) ? QLatin1String(".conf") : QLatin1String(".ini"));
        QSettings newSettings(newFilePath, format);


        if (newSettings.value("Config/Version").toUInt() == 0) {
            QString org = "Quassel Project";
            QSettings oldSettings(org, "Quassel Client");
            if (oldSettings.allKeys().count()) {
                qWarning() << "\n\n*** IMPORTANT: Config and data file locations have changed. Attempting to auto-migrate your client settings...";
                foreach(QString key, oldSettings.allKeys())
                newSettings.setValue(key, oldSettings.value(key));
                newSettings.setValue("Config/Version", 1);
                qWarning() << "*   Your client settings have been migrated to" << newSettings.fileName();
                qWarning() << "*** Migration completed.\n\n";
            }
        }

        // session resume
        Bb10Ui *gui = new Bb10Ui(this);
        Client::init(gui);
        gui->init();
        qDebug() << "xxxxx UiApplication::init done.";
        //resumeSessionIfPossible();
        return true;
    }
    return false;
}

UiApplication::~UiApplication()
{
    Client::destroy();
}
