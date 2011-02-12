#include "DWRAONBrain.h"
using namespace std;
class Box {
public:

    Box();

    //props
    int type; //0: AND, 1:OR
    float kp; //kp: damping strength
    vector<float> w; //weight of each connecting box (in [0,inf])
    vector<int> id; //id in boxes[] of the connecting box
    vector<bool> notted; //is this input notted before coming in?
    float bias;

    //state variables
    float target; //target value this node is going toward
    float out; //current output, and history. 0 is farthest back. -1 is latest


};

Box::Box()
{

    w.resize(CONNS,0);
    id.resize(CONNS,0);
    notted.resize(CONNS,0);

    //constructor
    for (int i=0;i<CONNS;i++) {
        w[i]= randf(0.1,2);
        id[i]= randi(0,BRAINSIZE);
        if (randf(0,1)<0.2) id[i]= randi(0,INPUTSIZE); //20% of the brain AT LEAST should connect to input.
        notted[i]= randf(0,1)<0.5;
    }

    type= (randf(0,1)>0.5)?(0):(1);
    kp= randf(0.8,1);
    bias= randf(-1,1);

    out=0;
    target=0;
}

DWRAONBrain::DWRAONBrain()
{

    //constructor
    for (int i=0;i<BRAINSIZE;i++) {
        Box a; //make a random box and copy it over
        boxes.push_back(a);

        boxes[i].out= a.out;
        boxes[i].target= a.target;
        boxes[i].type= a.type;
        boxes[i].bias= a.bias;
        for (int j=0;j<CONNS;j++) {
            boxes[i].notted[j]= a.notted[j];
            boxes[i].w[j]= a.w[j];
            boxes[i].id[j]= a.id[j];

            if (randf(0,1)<0.05) boxes[i].id[j]=0;
            if (randf(0,1)<0.05) boxes[i].id[j]=5;
            if (randf(0,1)<0.05) boxes[i].id[j]=12;
            if (randf(0,1)<0.05) boxes[i].id[j]=4;

            //boxes[i].id[j]= max(min(BRAINSIZE-1, randi(i-10,i+10)), 0);
            if (i<BRAINSIZE/2) {
                boxes[i].id[j]= randi(0,INPUTSIZE);
            }
        }
    }

    //do other initializations
    init();
}
void DWRAONBrain::init()
{

}

void DWRAONBrain::tick(vector< float >& in, vector< float >& out)
{

    //do a single tick of the brain

    //take first few boxes and set their out to in[].
    for (int i=0;i<INPUTSIZE;i++) {
        boxes[i].out= in[i];
    }

    //then do a dynamics tick and set all targets
    for (int i=INPUTSIZE;i<BRAINSIZE;i++) {
        Box* abox= &boxes[i];

        if (abox->type==0) {

            //AND NODE
            float res=1;
            for (int j=0;j<CONNS;j++) {
                int idx=abox->id[j];
                float val= boxes[idx].out;
                if (abox->notted[j]) val= 1-val;
                //res= res * pow(val, abox->w[j]);
                res= res * val;
            }
            res*= abox->bias;
            abox->target= res;

        } else {

            //OR NODE
            float res=0;
            for (int j=0;j<CONNS;j++) {
                int idx=abox->id[j];
                float val= boxes[idx].out;
                if (abox->notted[j]) val= 1-val;
                res= res + val*abox->w[j];
            }
            res+= abox->bias;
            abox->target= res;
        }

        //clamp target
        if (abox->target<0) abox->target=0;
        if (abox->target>1) abox->target=1;
    }

    //make all boxes go a bit toward target
    for (int i=INPUTSIZE;i<BRAINSIZE;i++) {
        Box* abox= &boxes[i];
        abox->out =abox->out + (abox->target-abox->out)*abox->kp;
    }

    //finally set out[] to the last few boxes output
    for (int i=0;i<OUTPUTSIZE;i++) {
        out[i]= boxes[BRAINSIZE-1-i].out;
    }
}
