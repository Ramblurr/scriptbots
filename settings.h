#ifndef SETTINGS_H
#define SETTINGS_H
namespace conf {
    
    const int WIDTH = 1800;  //width and height of simulation
    const int HEIGHT = 1000;
    const int WWIDTH = 1800;  //window width and height
    const int WHEIGHT = 1000;
    
    const int CZ = 50; //cell size in pixels, for food squares. Should divide well into Width Height
    
    const int NUMBOTS=30; //initially, and minimally
    const float BOTRADIUS=10; //for drawing
    const float BOTSPEED= 0.1;
    const float SPIKESPEED= 0.005; //how quickly can attack spike go up?
    const float SPIKEMULT= 0.5; //essentially the strength of every spike impact
    const int BABIES=2; //number of babies per agent when they reproduce
    const float BOOSTSIZEMULT=2; //how much boost do agents get? when boost neuron is on
    const float REPRATEH=7; //reproduction rate for herbivors
    const float REPRATEC=7; //reproduction rate for carnivors

    const float DIST= 150;		//how far can the eyes see on each bot?
    const float EYE_SENSITIVITY= 2; //how sensitive are the eyes?
    const float BLOOD_SENSITIVITY= 2; //how sensitive are blood sensors?
    const float METAMUTRATE1= 0.002; //what is the change in MUTRATE1 and 2 on reproduction? lol
    const float METAMUTRATE2= 0.05;

    const float FOODGROWTH= 0;//0.00001; //how quickly does food grow on a square?
    const float FOODINTAKE= 0.00325; //how much does every agent consume?
    const float FOODWASTE= 0.001; //how much food disapears if agent eats?
    const float FOODMAX= 0.5; //how much food per cell can there be at max?
    const int FOODADDFREQ= 30; //how often does random square get to full food?

    const float FOODTRANSFER= 0.001; //how much is transfered between two agents trading food? per iteration
    const float FOOD_SHARING_DISTANCE= 50; //how far away is food shared between bots?

    int CLOSED=0; //if environment is closed, then no random bots are added per time interval

    const float FOOD_DISTRIBUTION_RADIUS=100; //when bot is killed, how far is its body distributed?

}
#endif