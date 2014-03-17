#ifndef MONOAPPLICATION_H_
#define MONOAPPLICATION_H_

#include "uiapplication.h"

class CoreApplicationInternal;

class MonolithicApplication : public UiApplication
{
    Q_OBJECT
public:
    MonolithicApplication(int, char **);
    ~MonolithicApplication();

    bool init();

private slots:
    void startInternalCore();

private:
    CoreApplicationInternal *_internal;
    bool _internalInitDone;
};


#endif
