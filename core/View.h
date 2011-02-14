#ifndef VIEW_H
#define VIEW_H

#include "Agent.h"
class View
{
public:
    virtual void drawAgent(const Agent &a) = 0;
    virtual void drawFood(int x, int y, float quantity) = 0;
};

#endif // VIEW_H
