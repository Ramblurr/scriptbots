#include "Agent.h"

#include "settings.h"
#include "helpers.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include "DWRAONBrain.h"
using namespace std;
Agent::Agent()
{
    pos= Vector2f(randf(0,conf::WIDTH),randf(0,conf::HEIGHT));
    angle= randf(-M_PI,M_PI);
    health= 1.0+randf(0,0.1);
    age=0;
    spikeLength=0;
    red= 0;
    gre= 0;
    blu= 0;
    w1=0;
    w2=0;
    soundmul=1;
    give=0;
    clockf1= randf(5,100);
    clockf2= randf(5,100);
    boost=false;
    indicator=0;
    gencount=0;
    selectflag=0;
    ir=0;
    ig=0;
    ib=0;
    hybrid= false;
    herbivore= randf(0,1);
    repcounter= herbivore*randf(conf::REPRATEH-0.1,conf::REPRATEH+0.1) + (1-herbivore)*randf(conf::REPRATEC-0.1,conf::REPRATEC+0.1);

    id=0;

    MUTRATE1= 0.003;
    MUTRATE2= 0.05;

    in.resize(INPUTSIZE, 0);
    out.resize(OUTPUTSIZE, 0);
}

void Agent::printSelf()
{
    printf("Agent age=%i\n", age);
    for (int i=0;i<mutations.size();i++) {
        cout << mutations[i];
    }
}

void Agent::initEvent(float size, float r, float g, float b)
{
    indicator=size;
    ir=r;
    ig=g;
    ib=b;
}

void Agent::tick()
{
    brain.tick(in, out);
}
Agent Agent::reproduce(float MR, float MR2)
{
    Agent a2;

    //spawn the baby somewhere closeby behind agent
    //we want to spawn behind so that agents dont accidentally eat their young right away
    Vector2f fb(conf::BOTRADIUS,0);
    fb.rotate(-a2.angle);
    a2.pos= this->pos + fb + Vector2f(randf(-conf::BOTRADIUS*2,conf::BOTRADIUS*2), randf(-conf::BOTRADIUS*2,conf::BOTRADIUS*2));
    if (a2.pos.x<0) a2.pos.x= conf::WIDTH+a2.pos.x;
    if (a2.pos.x>=conf::WIDTH) a2.pos.x= a2.pos.x-conf::WIDTH;
    if (a2.pos.y<0) a2.pos.y= conf::HEIGHT+a2.pos.y;
    if (a2.pos.y>=conf::HEIGHT) a2.pos.y= a2.pos.y-conf::HEIGHT;

    a2.gencount= this->gencount+1;
    a2.repcounter= a2.herbivore*randf(conf::REPRATEH-0.1,conf::REPRATEH+0.1) + (1-a2.herbivore)*randf(conf::REPRATEC-0.1,conf::REPRATEC+0.1);

    //noisy attribute passing
    a2.MUTRATE1= this->MUTRATE1;
    a2.MUTRATE2= this->MUTRATE2;
    if (randf(0,1)<0.2) a2.MUTRATE1= randn(this->MUTRATE1, conf::METAMUTRATE1);
    if (randf(0,1)<0.2) a2.MUTRATE2= randn(this->MUTRATE2, conf::METAMUTRATE2);
    if (this->MUTRATE1<0.001) this->MUTRATE1= 0.001;
    if (this->MUTRATE2<0.02) this->MUTRATE2= 0.02;
    a2.herbivore= cap(randn(this->herbivore, MR2*4));
    if (randf(0,1)<MR*5) a2.clockf1= randn(a2.clockf1, MR2);
    if (a2.clockf1<2) a2.clockf1= 2;
    if (randf(0,1)<MR*5) a2.clockf2= randn(a2.clockf2, MR2);
    if (a2.clockf2<2) a2.clockf2= 2;

    //mutate brain here
    a2.brain= this->brain;
    a2.brain.mutate(MR,MR2);
    
    return a2;

}

Agent Agent::crossover(const Agent& other)
{
    //this could be made faster by returning a pointer
    //instead of returning by value
    Agent anew;
    anew.hybrid=true; //set this non-default flag
    anew.gencount= this->gencount;
    if (other.gencount<anew.gencount) anew.gencount= other.gencount;

    //agent heredity attributes
    anew.clockf1= randf(0,1)<0.5 ? this->clockf1 : other.clockf1;
    anew.clockf2= randf(0,1)<0.5 ? this->clockf2 : other.clockf2;
    anew.herbivore= randf(0,1)<0.5 ? this->herbivore : other.herbivore;
    anew.MUTRATE1= randf(0,1)<0.5 ? this->MUTRATE1 : other.MUTRATE1;
    anew.MUTRATE2= randf(0,1)<0.5 ? this->MUTRATE2 : other.MUTRATE2;
    
    anew.brain= this->brain.crossover(other.brain);
    
    return anew;
}
