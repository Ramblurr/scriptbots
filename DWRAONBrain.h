#ifndef DWRAONBRAIN_H
#define DWRAONBRAIN_H

#include "settings.h"
#include "helpers.h"

#include <vector>

#define INPUTSIZE 20
#define OUTPUTSIZE 9

#define BRAINSIZE 100
#define CONNS 3


class Box; // A private class for DWRAONBrain
/**
 * Damped Weighted Recurrent AND/OR Network
 */
class DWRAONBrain
{
public:

    std::vector<Box> boxes;

    DWRAONBrain();

    void tick(std::vector<float>& in, std::vector<float>& out);
private:
    void init();
};

#endif
