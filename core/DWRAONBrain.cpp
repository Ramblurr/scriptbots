#include "DWRAONBrain.h"
using namespace std;


Box::Box()
{
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
        boxes[i] = a;

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

DWRAONBrain::DWRAONBrain(const DWRAONBrain& other)
{
    boxes = other.boxes;
}

DWRAONBrain& DWRAONBrain::operator=(const DWRAONBrain& other)
{
    if( this != &other )
        boxes = other.boxes;
    return *this;
}


void DWRAONBrain::init()
{

}

void DWRAONBrain::tick(float in[], float out[])
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

void DWRAONBrain::mutate(float MR, float MR2)
{
    for (int j=0;j<BRAINSIZE;j++) {

        if (randf(0,1)<MR*3) {
            boxes[j].bias+= randn(0, MR2);
//             a2.mutations.push_back("bias jiggled\n");
        }

        if (false && randf(0,1)<MR*3) {
            boxes[j].kp+= randn(0, MR2);
            if (boxes[j].kp<0.01) boxes[j].kp=0.01;
            if (boxes[j].kp>1) boxes[j].kp=1;
//             a2.mutations.push_back("kp jiggled\n");
        }

        if (randf(0,1)<MR*3) {
            int rc= randi(0, CONNS);
            boxes[j].w[rc]+= randn(0, MR2);
            if (boxes[j].w[rc]<0.01) boxes[j].w[rc]= 0.01;
//             a2.mutations.push_back("weight jiggled\n");
        }

        //more unlikely changes here
        if (randf(0,1)<MR) {
            int rc= randi(0, CONNS);
            int ri= randi(0,BRAINSIZE);
            boxes[j].id[rc]= ri;
//             a2.mutations.push_back("connectivity changed\n");
        }

        if (randf(0,1)<MR) {
            int rc= randi(0, CONNS);
            boxes[j].notted[rc]= !boxes[j].notted[rc];
//             a2.mutations.push_back("notted was flipped\n");
        }

        if (randf(0,1)<MR) {
            boxes[j].type= 1-boxes[j].type;
//             a2.mutations.push_back("type of a box was changed\n");
        }
    }
}

DWRAONBrain DWRAONBrain::crossover(const DWRAONBrain& other)
{
    //this could be made faster by returning a pointer
    //instead of returning by value
    DWRAONBrain newbrain(*this);
    
    for (int i=0;i< BRAINSIZE ; i++) {
        newbrain.boxes[i].bias= randf(0,1)<0.5 ? this->boxes[i].bias : other.boxes[i].bias;
        newbrain.boxes[i].kp= randf(0,1)<0.5 ? this->boxes[i].kp : other.boxes[i].kp;
        newbrain.boxes[i].type= randf(0,1)<0.5 ? this->boxes[i].type : other.boxes[i].type;

        for (int j=0;j< CONNS;j++) {
            newbrain.boxes[i].id[j] = randf(0,1)<0.5 ? this->boxes[i].id[j] : other.boxes[i].id[j];
            newbrain.boxes[i].notted[j] = randf(0,1)<0.5 ? this->boxes[i].notted[j] : other.boxes[i].notted[j];
            newbrain.boxes[i].w[j] = randf(0,1)<0.5 ? this->boxes[i].w[j] : other.boxes[i].w[j];
        }
    }
    return newbrain;
}

