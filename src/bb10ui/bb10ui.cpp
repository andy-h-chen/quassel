#include "bb10ui.h"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

Bb10Ui::Bb10Ui(Application *app)
    : QObject(app)
{
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    if (qml->hasErrors()) {
        qDebug() << "xxxxx qml->hasErrors()";
    }
    AbstractPane *root = qml->createRootObject<AbstractPane>();
    app->setScene(root);
}

void Bb10Ui::init()
{

}

