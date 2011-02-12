#include "Agent.h"

#include "settings.h"
#include "helpers.h"
#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;
Agent::Agent()
{
    pos= Vector2f(randf(0,WIDTH),randf(0,HEIGHT));
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

Agent::Agent(const Agent& other)
{

}

Agent::~Agent()
{

}

Agent& Agent::operator=(const Agent& other)
{
    return *this;
}

bool Agent::operator==(const Agent& other) const
{
///TODO: return ...;
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
