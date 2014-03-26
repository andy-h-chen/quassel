#ifndef UIAPPLICATION_H
#define UIAPPLICATION_H

#include <bb/cascades/Application>

#include "quassel.h"

class UiApplication : public bb::cascades::Application, public Quassel
{
    Q_OBJECT
public:
    UiApplication(int argc, char **argv);
    virtual ~UiApplication();

    bool init();
};

#endif
