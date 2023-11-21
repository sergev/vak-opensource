#include "ModelCxx.h"
#include "Gui-Swift.h"

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
