#ifndef BB10UI_H
#define BB10UI_H

#include <bb/cascades/Application>
 
class Bb10Ui : public QObject
{
    Q_OBJECT
public:
    Bb10Ui(bb::cascades::Application *app);
    virtual ~Bb10Ui() { }

    void init();
};

#endif
