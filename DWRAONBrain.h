#ifndef DWRAONBRAIN_H
#define DWRAONBRAIN_H

#include "settings.h"
#include "helpers.h"

#include <vector>

#define INPUTSIZE 20
#define OUTPUTSIZE 9

#define BRAINSIZE 100
#define CONNS 3


class Box {
public:

    Box();

    //props
    int type; //0: AND, 1:OR
    float kp; //kp: damping strength
    std::vector<float> w; //weight of each connecting box (in [0,inf]
    std::vector<int> id; //id in boxes[] of the connecting box
    std::vector<bool> notted; //is this input notted before coming in?
    float bias;

    //state variables
    float target; //target value this node is going toward
    float out; //current output, and history. 0 is farthest back. -1 is latest


};
/**
 * Damped Weighted Recurrent AND/OR Network
 */
class DWRAONBrain
{
public:

    std::vector<Box> boxes;

    DWRAONBrain();
    DWRAONBrain(const DWRAONBrain &other);
    virtual DWRAONBrain& operator=(const DWRAONBrain& other);

    void tick(std::vector<float>& in, std::vector<float>& out);
    void mutate(float MR, float MR2);
    DWRAONBrain crossover( const DWRAONBrain &other );
private:
    void init();
};

#endif
