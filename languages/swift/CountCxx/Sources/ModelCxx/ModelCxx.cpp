#include "ModelCxx.h"
#include "Gui-Swift.h"

void ModelCxx::setup()
{
    Gui::updateCount(count);
}

void ModelCxx::increment()
{
    count++;
    Gui::updateCount(count);
}

void ModelCxx::decrement()
{
    count--;
    Gui::updateCount(count);
}
