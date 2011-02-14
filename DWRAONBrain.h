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
	/* changed from <bool> to <char> because vector<bool> doesn't 
	   have true random access, and thus slows the simulation to a crawl.
	   http://www.informit.com/guides/content.aspx?g=cplusplus&seqNum=98
	   It still stores boolean values at the cost of space overhead,
	   but with a significant speed improvement.
	*/
	std::vector<char> notted; //is this input notted before coming in?
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
