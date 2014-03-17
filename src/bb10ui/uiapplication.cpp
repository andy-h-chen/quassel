#include "uiapplication.h"

#include "bb10ui.h"

using namespace bb::cascades;

UiApplication::UiApplication(int argc, char **argv)
    : Application(argc, argv)
    , Quassel()
{

}

bool UiApplication::init()
{
    new Bb10Ui(this);
    return true;
}
