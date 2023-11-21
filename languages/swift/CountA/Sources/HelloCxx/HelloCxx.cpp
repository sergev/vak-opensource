#include "HelloCxx.h"
#include "Gui-Swift.h"

void HelloCxx::increment()
{
    count++;
    Gui::updateCount(count);
}

void HelloCxx::decrement()
{
    count--;
    Gui::updateCount(count);
}
