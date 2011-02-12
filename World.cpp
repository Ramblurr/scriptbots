#include "World.h"

#include <ctime>

#include "settings.h"
#include "helpers.h"
#include "vmath.h"

using namespace std;

World::World() :
        modcounter(0),
        epoch(0),
        idcounter(0),
        FW(conf::WIDTH/conf::CZ),
        FH(conf::HEIGHT/conf::CZ),
        CLOSED(false)
{
    addRandomBots(conf::NUMBOTS);
    //inititalize food layer

    for(int x=0;x<FW;x++){
        for(int y=0;y<FH;y++){
            food[x][y]= 0;
        }
    }
}

void World::update()
{
    modcounter++;

    //Process periodic events
    //Age goes up!
    if (modcounter%100==0) {
        for (int i=0;i<agents.size();i++) {
            agents[i].age+= 1;    //agents age...
        }
    }
    if (modcounter%1000==0) writeReport();
    if (modcounter>=10000) {
        modcounter=0;
        epoch++;
    }
    if (modcounter%conf::FOODADDFREQ==0) {
        fx=randi(0,FW);
        fy=randi(0,FH);
        food[fx][fy]= conf::FOODMAX;
    }

    //give input to every agent. Sets in[] array
    setInputs();

    //brains tick. computes in[] -> out[]
    brainsTick();

    //read output and process consequences of bots on environment. requires out[]
    processOutputs();

    //process bots: health and deaths
    for (int i=0;i<agents.size();i++) {
        float baseloss= 0.0002 + 0.0001*(abs(agents[i].w1) + abs(agents[i].w2))/2;
        if (agents[i].w1<0.1 && agents[i].w2<0.1) baseloss=0.0001; //hibernation :p
        baseloss += 0.00005*agents[i].soundmul; //shouting costs energy. just a tiny bit

        if (agents[i].boost) {
            //boost carries its price, and it's pretty heavy!
            agents[i].health -= baseloss*conf::BOOSTSIZEMULT*2;
        } else {
            agents[i].health -= baseloss;
        }
    }

    //remove dead agents.
    //first distribute foods
    for (int i=0;i<agents.size();i++) {
        if (agents[i].health<=0) {
            //distribute its food. It will be erased soon
            //first figure out how many are around, to distribute this evenly
            int numaround=0;
            for (int j=0;j<agents.size();j++) {
                if (agents[j].health>0) {
                    float d= (agents[i].pos-agents[j].pos).length();
                    if (d<conf::FOOD_DISTRIBUTION_RADIUS) {
                        numaround++;
                    }
                }
            }
            if (numaround>0) {
                //distribute its food evenly
                for (int j=0;j<agents.size();j++) {
                    if (agents[j].health>0 && agents[j].herbivore<0.7) {
                        float d= (agents[i].pos-agents[j].pos).length();
                        if (d<conf::FOOD_DISTRIBUTION_RADIUS) {
                            agents[j].health += 3*(1-agents[j].herbivore)*(1-agents[j].herbivore)/numaround;
                            agents[j].repcounter -= 2*(1-agents[j].herbivore)*(1-agents[j].herbivore)/numaround; //good job, can use spare parts to make copies
                            if (agents[j].health>2) agents[j].health=2; //cap it!
                            agents[j].initEvent(30,1,1,1); //white means they ate! nice
                        }
                    }
                }
            }

        }
    }
    vector<Agent>::iterator iter= agents.begin();
    while (iter != agents.end()) {
        if (iter->health <=0) {
            iter= agents.erase(iter);
        } else {
            ++iter;
        }
    }

    //handle reproduction
    for (int i=0;i<agents.size();i++) {
        if (agents[i].repcounter<0 && agents[i].health>0.65) { //agent is healthy and is ready to reproduce
            //agents[i].health= 0.8; //the agent is left vulnerable and weak, a bit
            reproduce(i, agents[i].MUTRATE1, agents[i].MUTRATE2); //this adds conf::BABIES new agents to agents[]
            agents[i].repcounter= agents[i].herbivore*randf(conf::REPRATEH-0.1,conf::REPRATEH+0.1) + (1-agents[i].herbivore)*randf(conf::REPRATEC-0.1,conf::REPRATEC+0.1);
        }
    }

    //environment tick
    for (int x=0;x<FW;x++) {
        for (int y=0;y<FH;y++) {
            food[x][y]+= conf::FOODGROWTH; //food grows
            if (food[x][y]>conf::FOODMAX)food[x][y]=conf::FOODMAX; //cap at conf::FOODMAX
        }
    }

    //add new agents, if environment isn't closed
    if (!CLOSED) {
        //make sure environment is always populated with at least NUMBOTS bots
        if (agents.size()<conf::NUMBOTS
           ) {
            //add new agent
            addRandomBots(1);
        }
        if (modcounter%200==0) {
            if (randf(0,1)<0.5)
                addRandomBots(1); //every now and then add random bots in
            else
                addNewByCrossover(); //or by crossover
        }
    }

    //show FPS
    /*int currentTime = glutGet( GLUT_ELAPSED_TIME );
    frames++;
    if ((currentTime - lastUpdate) >= 1000) {
        int numherb=0;
        int numcarn=0;
        for (int i=0;i<agents.size();i++) {
            if (agents[i].herbivore>0.5) numherb++;
            else numcarn++;
        }
        sprintf( buf, "FPS: %d NumAgents: %d Carnivors: %d Herbivors: %d Epoch: %d", frames, agents.size(), numcarn, numherb, epoch );
        glutSetWindowTitle( buf );
        frames = 0;
        lastUpdate = currentTime;
    }
    if (skipdraw<=0 && draw) {
        clock_t endwait;
        float mult=-0.005*(skipdraw-1); //ugly, ah well
        endwait = clock () + mult * CLOCKS_PER_SEC ;
        while (clock() < endwait) {}
    }*/
}

void World::setInputs()
{
    //P1 R1 G1 B1 FOOD P2 R2 G2 B2 SOUND SMELL HEALTH P3 R3 G3 B3 CLOCK1 CLOCK 2 HEARING     BLOOD_SENSOR
    //0   1  2  3  4   5   6  7 8   9     10     11   12 13 14 15 16       17      18           19

    float PI8=M_PI/8/2; //pi/8/2
    float PI38= 3*PI8; //3pi/8/2
    for (int i=0;i<agents.size();i++) {
        Agent* a= &agents[i];

        //HEALTH
        a->in[11]= cap(a->health/2); //divide by 2 since health is in [0,2]

        //FOOD
        int cx= (int) a->pos.x/conf::CZ;
        int cy= (int) a->pos.y/conf::CZ;
        a->in[4]= food[cx][cy]/conf::FOODMAX;

        //SOUND SMELL EYES
        float p1,r1,g1,b1,p2,r2,g2,b2,p3,r3,g3,b3;
        p1=0;
        r1=0;
        g1=0;
        b1=0;
        p2=0;
        r2=0;
        g2=0;
        b2=0;
        p3=0;
        r3=0;
        g3=0;
        b3=0;
        float soaccum=0;
        float smaccum=0;
        float hearaccum=0;

        //BLOOD ESTIMATOR
        float blood= 0;

        for (int j=0;j<agents.size();j++) {
            if (i==j) continue;
            Agent* a2= &agents[j];

            if (a->pos.x<a2->pos.x-conf::DIST || a->pos.x>a2->pos.x+conf::DIST
                    || a->pos.y>a2->pos.y+conf::DIST || a->pos.y<a2->pos.y-conf::DIST) continue;

            float d= (a->pos-a2->pos).length();

            if (d<conf::DIST) {

                //smell
                smaccum+= 0.3*(conf::DIST-d)/conf::DIST;

                //sound
                soaccum+= 0.4*(conf::DIST-d)/conf::DIST*(max(fabs(a2->w1),fabs(a2->w2)));

                //hearing. Listening to other agents
                hearaccum+= a2->soundmul*(conf::DIST-d)/conf::DIST;

                float ang= (a2->pos- a->pos).get_angle(); //current angle between bots

                //left and right eyes
                float leyeangle= a->angle - PI8;
                float reyeangle= a->angle + PI8;
                float backangle= a->angle + M_PI;
                float forwangle= a->angle;
                if (leyeangle<-M_PI) leyeangle+= 2*M_PI;
                if (reyeangle>M_PI) reyeangle-= 2*M_PI;
                if (backangle>M_PI) backangle-= 2*M_PI;
                float diff1= leyeangle- ang;
                if (fabs(diff1)>M_PI) diff1= 2*M_PI- fabs(diff1);
                diff1= fabs(diff1);
                float diff2= reyeangle- ang;
                if (fabs(diff2)>M_PI) diff2= 2*M_PI- fabs(diff2);
                diff2= fabs(diff2);
                float diff3= backangle- ang;
                if (fabs(diff3)>M_PI) diff3= 2*M_PI- fabs(diff3);
                diff3= fabs(diff3);
                float diff4= forwangle- ang;
                if (fabs(forwangle)>M_PI) diff4= 2*M_PI- fabs(forwangle);
                diff4= fabs(diff4);

                if (diff1<PI38) {
                    //we see this agent with left eye. Accumulate info
                    float mul1= conf::EYE_SENSITIVITY*((PI38-diff1)/PI38)*((conf::DIST-d)/conf::DIST);
                    //float mul1= 100*((conf::DIST-d)/conf::DIST);
                    p1 += mul1*(d/conf::DIST);
                    r1 += mul1*a2->red;
                    g1 += mul1*a2->gre;
                    b1 += mul1*a2->blu;
                }

                if (diff2<PI38) {
                    //we see this agent with left eye. Accumulate info
                    float mul2= conf::EYE_SENSITIVITY*((PI38-diff2)/PI38)*((conf::DIST-d)/conf::DIST);
                    //float mul2= 100*((conf::DIST-d)/conf::DIST);
                    p2 += mul2*(d/conf::DIST);
                    r2 += mul2*a2->red;
                    g2 += mul2*a2->gre;
                    b2 += mul2*a2->blu;
                }

                if (diff3<PI38) {
                    //we see this agent with back eye. Accumulate info
                    float mul3= conf::EYE_SENSITIVITY*((PI38-diff3)/PI38)*((conf::DIST-d)/conf::DIST);
                    //float mul2= 100*((conf::DIST-d)/conf::DIST);
                    p3 += mul3*(d/conf::DIST);
                    r3 += mul3*a2->red;
                    g3 += mul3*a2->gre;
                    b3 += mul3*a2->blu;
                }

                if (diff4<PI38) {
                    float mul4= conf::BLOOD_SENSITIVITY*((PI38-diff4)/PI38)*((conf::DIST-d)/conf::DIST);
                    //if we can see an agent close with both eyes in front of us
                    blood+= mul4*(1-agents[j].health/2); //remember: health is in [0 2]
                    //agents with high life dont bleed. low life makes them bleed more
                }
            }
        }

        a->in[0]= cap(p1);
        a->in[1]= cap(r1);
        a->in[2]= cap(g1);
        a->in[3]= cap(b1);
        a->in[5]= cap(p2);
        a->in[6]= cap(r2);
        a->in[7]= cap(g2);
        a->in[8]= cap(b2);
        a->in[9]= cap(soaccum);
        a->in[10]= cap(smaccum);
        a->in[12]= cap(p3);
        a->in[13]= cap(r3);
        a->in[14]= cap(g3);
        a->in[15]= cap(b3);
        a->in[16]= abs(sin(modcounter/a->clockf1));
        a->in[17]= abs(sin(modcounter/a->clockf2));
        a->in[18]= cap(hearaccum);
        a->in[19]= cap(blood);
    }
}

void World::processOutputs()
{
    //assign meaning
    //LEFT RIGHT R G B SPIKE BOOST SOUND_MULTIPLIER GIVING
    // 0    1    2 3 4   5     6         7             8
    for (int i=0;i<agents.size();i++) {
        Agent* a= &agents[i];

        a->red= a->out[2];
        a->gre= a->out[3];
        a->blu= a->out[4];
        a->w1= a->out[0]; //-(2*a->out[0]-1);
        a->w2= a->out[1]; //-(2*a->out[1]-1);
        a->boost= a->out[6]>0.5;
        a->soundmul= a->out[7];
        a->give= a->out[8];

        //spike length should slowly tend towards out[5]
        float g= a->out[5];
        if (a->spikeLength<g)
            a->spikeLength+=conf::SPIKESPEED;
        else if (a->spikeLength>g)
            a->spikeLength= g; //its easy to retract spike, just hard to put it up
    }

    //move bots
    //#pragma omp parallel for
    for (int i=0;i<agents.size();i++) {
        Agent* a= &agents[i];

        Vector2f v(conf::BOTRADIUS/2, 0);
        v.rotate(a->angle + M_PI/2);

        Vector2f w1p= a->pos+ v; //wheel positions
        Vector2f w2p= a->pos- v;

        float BW1= conf::BOTSPEED*a->w1;
        float BW2= conf::BOTSPEED*a->w2;
        if (a->boost) {
            BW1=BW1*conf::BOOSTSIZEMULT;
        }
        if (a->boost) {
            BW2=BW2*conf::BOOSTSIZEMULT;
        }

        //move bots
        Vector2f vv= w2p- a->pos;
        vv.rotate(-BW1);
        a->pos= w2p-vv;
        a->angle -= BW1;
        if (a->angle<-M_PI) a->angle= M_PI - (-M_PI-a->angle);
        vv= a->pos - w1p;
        vv.rotate(BW2);
        a->pos= w1p+vv;
        a->angle += BW2;
        if (a->angle>M_PI) a->angle= -M_PI + (a->angle-M_PI);

        //wrap around the map
        if (a->pos.x<0) a->pos.x= conf::WIDTH+a->pos.x;
        if (a->pos.x>=conf::WIDTH) a->pos.x= a->pos.x-conf::WIDTH;
        if (a->pos.y<0) a->pos.y= conf::HEIGHT+a->pos.y;
        if (a->pos.y>=conf::HEIGHT) a->pos.y= a->pos.y-conf::HEIGHT;
    }

    //process food intake for herbivors
    for (int i=0;i<agents.size();i++) {

        int cx= (int) agents[i].pos.x/conf::CZ;
        int cy= (int) agents[i].pos.y/conf::CZ;
        float f= food[cx][cy];
        if (f>0 && agents[i].health<2) {
            //agent eats the food
            float itk=min(f,conf::FOODINTAKE);
            float speedmul= (1-(abs(agents[i].w1)+abs(agents[i].w2))/2)/2 + 0.5;
            itk= itk*agents[i].herbivore*agents[i].herbivore*speedmul; //herbivores gain more from ground food
            agents[i].health+= itk;
            agents[i].repcounter -= 3*itk;
            food[cx][cy]-= min(f,conf::FOODWASTE);
        }
    }

    //process giving and receiving of food
    for (int i=0;i<agents.size();i++) {
        agents[i].dfood=0;
    }
    for (int i=0;i<agents.size();i++) {
        if (agents[i].give>0.5) {
            for (int j=0;j<agents.size();j++) {
                float d= (agents[i].pos-agents[j].pos).length();
                if (d<conf::FOOD_SHARING_DISTANCE) {
                    //initiate transfer
                    if (agents[j].health<2) agents[j].health += conf::FOODTRANSFER;
                    agents[i].health -= conf::FOODTRANSFER;
                    agents[j].dfood += conf::FOODTRANSFER; //only for drawing
                    agents[i].dfood -= conf::FOODTRANSFER;
                }
            }
        }
    }

    //process spike dynamics for carnivors
    if (modcounter%2==0) { //we dont need to do this TOO often. can save efficiency here since this is n^2 op in #agents
        for (int i=0;i<agents.size();i++) {
            for (int j=0;j<agents.size();j++) {
                if (i==j || agents[i].spikeLength<0.2 || agents[i].w1<0.3 || agents[i].w2<0.3) continue;
                float d= (agents[i].pos-agents[j].pos).length();

                if (d<2*conf::BOTRADIUS) {
                    //these two are in collision and agent i has extended spike and is going decent fast!
                    Vector2f v(1,0);
                    v.rotate(agents[i].angle);
                    float diff= v.angle_between(agents[j].pos-agents[i].pos);
                    if (fabs(diff)<M_PI/8) {
                        //bot i is also properly aligned!!! that's a hit
                        float mult=1;
                        if (agents[i].boost) mult= conf::BOOSTSIZEMULT;
                        float DMG= conf::SPIKEMULT*agents[i].spikeLength*max(fabs(agents[i].w1),fabs(agents[i].w2))*conf::BOOSTSIZEMULT;

                        agents[j].health-= DMG;

                        if (agents[i].health>2) agents[i].health=2; //cap health at 2
                        agents[i].spikeLength= 0; //retract spike back down

                        agents[i].initEvent(40*DMG,1,1,0); //yellow event means bot has spiked other bot. nice!

                        Vector2f v2(1,0);
                        v2.rotate(agents[j].angle);
                        float adiff= v.angle_between(v2);
                        if (fabs(adiff)<M_PI/2) {
                            //this was attack from the back. Retract spike of the other agent (startle!)
                            //this is done so that the other agent cant right away "by accident" attack this agent
                            agents[j].spikeLength= 0;
                        }
                    }
                }
            }
        }
    }
}

void World::brainsTick()
{
#pragma omp parallel for
    for (int i=0;i<agents.size();i++) {
        agents[i].tick();
    }
}

void World::addRandomBots(int num)
{
    for (int i=0;i<num;i++) {
        Agent a;
        a.id= idcounter;
        idcounter++;
        agents.push_back(a);
    }
}

void World::addNewByCrossover()
{

    //find two success cases
    int i1= randi(0, agents.size());
    int i2= randi(0, agents.size());
    for (int i=0;i<agents.size();i++) {
        if (agents[i].age > agents[i1].age && randf(0,1)<0.1) {
            i1= i;
        }
        if (agents[i].age > agents[i2].age && randf(0,1)<0.1 && i!=i1) {
            i2= i;
        }
    }

    Agent* a1= &agents[i1];
    Agent* a2= &agents[i2];


    //cross brains
    Agent anew = a1->crossover(*a2);


    //maybe do mutation here? I dont know. So far its only crossover
    anew.id= idcounter;
    idcounter++;
    agents.push_back(anew);
}

void World::reproduce(int ai, float MR, float MR2)
{
    if (randf(0,1)<0.04) MR= MR*randf(1, 10);
    if (randf(0,1)<0.04) MR2= MR2*randf(1, 10);

    agents[ai].initEvent(30,0,0.8,0); //green event means agent reproduced.
    for (int i=0;i<conf::BABIES;i++) {

        Agent a2 = agents[ai].reproduce(MR,MR2);
        a2.id= idcounter;
        idcounter++;
        agents.push_back(a2);

        //record this
        //FILE* fp = fopen("log.txt", "a");
        //fprintf(fp, "%i %i %i\n", 1, this->id, a2.id); //1 marks the event: child is born
        //fclose(fp);
    }
}

void World::writeReport()
{

}


void World::reset()
{
    agents.clear();
    addRandomBots(conf::NUMBOTS);
}

void World::setClosed(bool close)
{
    CLOSED = close;
}

bool World::isClosed() const
{
    return CLOSED;
}


void World::draw(View* view)
{
    vector<Agent>::const_iterator it;
    for( it = agents.begin(); it != agents.end(); ++it) {
        view->drawAgent(*it);
    }
        
}

