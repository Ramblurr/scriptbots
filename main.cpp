/*
v1.2
Author: Andrej Karpathy
Licence: Do whatever you want with this code

TODO:
- right now AND gates dont use weights. because i suspect bug there. fix later?
- to implement: some kind of simulation stats? number of kills, deaths, etc?
- ability to take direct control of an agent using mouse/keyboard. Could even be turned into game! haha
- functionality for saving and loading agents, brains, or entire environments

- pheromone trails? agents can mark territory, etc.
- modularize everything? Brain, Agent, World, Simulation, Graphics
- ability to record movies, (i.e. save bmp screenshots?)
*/

#include "glut.h"
#include <vector>
#include <math.h>
#include <ctime>
#include <string>
#include <iostream>

#include "config.h" // tells us whether we have openmp or not
#ifdef HAVE_OPENMP
    #include <omp.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "DWRAONBrain.h"
#include "Agent.h"

#include "vmath.h"
#include "settings.h"
#include "helpers.h"
using namespace std;

#pragma warning( once : 4018 )


//for fps
int lastUpdate = 0;
int frames = 0;
char buf[100];
float fpstuner= 0.01;

//for drawing
char buf2[10];
int skipdraw=1;
bool drawFood=false;

//user interaction
bool paused=false;
bool draw=true;

//simulation, logging
int idcounter=0;
int epoch=0;
int modcounter=0;

//food
int FW= conf::WIDTH/conf::CZ;
int FH= conf::HEIGHT/conf::CZ;
float food[conf::WIDTH/conf::CZ][conf::HEIGHT/conf::CZ];
int fx=0;
int fy=0;

//simulation stats
//int herbivoreBirths= 0;
//int carnivoreBirths= 0;
//int kills= 0;
//int deaths= 0;

vector<Agent> agents;

void changeSize(int w, int h) {

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,conf::WWIDTH,conf::WHEIGHT,0,0,1);
}

void RenderString(float x, float y, void *font, const char* string, float r, float g, float b)
{  
  glColor3f(r,g,b); 
  glRasterPos2f(x, y);
  int len = (int) strlen(string);
  for (int i = 0; i < len; i++)
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
}

void drawCircle(float x, float y, float r){
	float n;
	for (int k=0;k<17;k++){
		n = k*(M_PI/8);
		glVertex3f(x+r*sin(n),y+r*cos(n),0);
	}
}

void renderScene() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	//draw food	
	if(drawFood){
		glBegin(GL_QUADS);
		for(int i=0;i<FW;i++){
			for(int j=0;j<FH;j++){
				float f= 0.5*food[i][j]/conf::FOODMAX;
				glColor3f(0.9-f,0.9-f,1.0-f);
				glVertex3f(i*conf::CZ,j*conf::CZ,0);
				glVertex3f(i*conf::CZ+conf::CZ,j*conf::CZ,0);
				glVertex3f(i*conf::CZ+conf::CZ,j*conf::CZ+conf::CZ,0);
				glVertex3f(i*conf::CZ,j*conf::CZ+conf::CZ,0);
			}
		}
		glEnd();
	}

	float n;
	float r= conf::BOTRADIUS;
	float rp= conf::BOTRADIUS+2;
	//draw every agent
	for(int i=0;i<agents.size();i++){
	
		//handle selected agent
		if(agents[i].selectflag>0){
			
			//draw selection
			glBegin(GL_POLYGON); 
			glColor3f(1,1,0);
			drawCircle(agents[i].pos.x, agents[i].pos.y, conf::BOTRADIUS+5);
			glEnd();
			
			glPushMatrix();
			glTranslatef(agents[i].pos.x-80,agents[i].pos.y+20,0);
			//draw inputs, outputs
			float col;
			float yy=15;
			float xx=15;
			float ss=16;
			glBegin(GL_QUADS);
			for(int j=0;j<INPUTSIZE;j++){
				col= agents[i].in[j];
				glColor3f(col,col,col);	
				glVertex3f(0+ss*j, 0, 0.0f);
				glVertex3f(xx+ss*j, 0, 0.0f);
				glVertex3f(xx+ss*j, yy, 0.0f);
				glVertex3f(0+ss*j, yy, 0.0f);
			}
			yy+=5;
			for(int j=0;j<OUTPUTSIZE;j++){
				col= agents[i].out[j];
				glColor3f(col,col,col);	
				glVertex3f(0+ss*j, yy, 0.0f);
				glVertex3f(xx+ss*j, yy, 0.0f);
				glVertex3f(xx+ss*j, yy+ss, 0.0f);
				glVertex3f(0+ss*j, yy+ss, 0.0f);
			}
			yy+=ss*2;

			//draw brain. Eventually move this to brain class?
			float offx=0;
			ss=8;
			for(int j=0;j<BRAINSIZE;j++){
				col= agents[i].brain.boxes[j].out;
				glColor3f(col,col,col);	
				glVertex3f(offx+0+ss*j, yy, 0.0f);
				glVertex3f(offx+xx+ss*j, yy, 0.0f);
				glVertex3f(offx+xx+ss*j, yy+ss, 0.0f);
				glVertex3f(offx+ss*j, yy+ss, 0.0f);
				if ((j+1)%30==0) {yy+=ss;offx-=ss*30;}
			}

			glEnd();
			glPopMatrix();
		}

		/*
		//draw giving/receiving
		if(agents[i].dfood!=0){
			glBegin(GL_POLYGON);
			float mag=cap(abs(agents[i].dfood)/conf::FOODTRANSFER/3);
			if(agents[i].dfood>0) glColor3f(0,mag,0); //draw boost as green outline
			else glColor3f(mag,0,0);
			for (int k=0;k<17;k++){
				n = k*(M_PI/8);
				glVertex3f(agents[i].pos.x+rp*sin(n),agents[i].pos.y+rp*cos(n),0);
				n = (k+1)*(M_PI/8);
				glVertex3f(agents[i].pos.x+rp*sin(n),agents[i].pos.y+rp*cos(n),0);
			}
			glEnd();
		}
		*/

		//draw indicator of this agent... used for various events
		if(agents[i].indicator>0){
			glBegin(GL_POLYGON);
				glColor3f(agents[i].ir,agents[i].ig,agents[i].ib);
				drawCircle(agents[i].pos.x, agents[i].pos.y, conf::BOTRADIUS+((int)agents[i].indicator));
			glEnd();
			agents[i].indicator-=1;
		}

		//viewcone of this agent
		glBegin(GL_LINES);
			//and view cones
			glColor3f(0.5,0.5,0.5);
			for(int j=-2;j<3;j++){
				if(j==0)continue;
				glVertex3f(agents[i].pos.x,agents[i].pos.y,0);
				glVertex3f(agents[i].pos.x+(conf::BOTRADIUS*4)*cos(agents[i].angle+j*M_PI/8),agents[i].pos.y+(conf::BOTRADIUS*4)*sin(agents[i].angle+j*M_PI/8),0);
			}
			//and eye to the back
			glVertex3f(agents[i].pos.x,agents[i].pos.y,0);
			glVertex3f(agents[i].pos.x+(conf::BOTRADIUS*1.5)*cos(agents[i].angle+M_PI+3*M_PI/16),agents[i].pos.y+(conf::BOTRADIUS*1.5)*sin(agents[i].angle+M_PI+3*M_PI/16),0);
			glVertex3f(agents[i].pos.x,agents[i].pos.y,0);
			glVertex3f(agents[i].pos.x+(conf::BOTRADIUS*1.5)*cos(agents[i].angle+M_PI-3*M_PI/16),agents[i].pos.y+(conf::BOTRADIUS*1.5)*sin(agents[i].angle+M_PI-3*M_PI/16),0);
		glEnd();

		glBegin(GL_POLYGON); //body
			glColor3f(agents[i].red,agents[i].gre,agents[i].blu);
			drawCircle(agents[i].pos.x, agents[i].pos.y, conf::BOTRADIUS);
		glEnd();

		glBegin(GL_LINES); 
			//outline
			if(agents[i].boost) glColor3f(0.8,0,0); //draw boost as green outline
			else glColor3f(0,0,0);

			for (int k=0;k<17;k++)
			{
				n = k*(M_PI/8);
				glVertex3f(agents[i].pos.x+r*sin(n),agents[i].pos.y+r*cos(n),0);
				n = (k+1)*(M_PI/8);
				glVertex3f(agents[i].pos.x+r*sin(n),agents[i].pos.y+r*cos(n),0);
			}
			//and spike
			glColor3f(0.5,0,0);
			glVertex3f(agents[i].pos.x,agents[i].pos.y,0);
			glVertex3f(agents[i].pos.x+(3*r*agents[i].spikeLength)*cos(agents[i].angle),agents[i].pos.y+(3*r*agents[i].spikeLength)*sin(agents[i].angle),0);
		glEnd();

		//and health
		int xo=18;
		int yo=-15;
		glBegin(GL_QUADS); 
			//black background
			glColor3f(0,0,0);
			glVertex3f(agents[i].pos.x+xo,agents[i].pos.y+yo,0);
			glVertex3f(agents[i].pos.x+xo+5,agents[i].pos.y+yo,0);
			glVertex3f(agents[i].pos.x+xo+5,agents[i].pos.y+yo+40,0);
			glVertex3f(agents[i].pos.x+xo,agents[i].pos.y+yo+40,0);
			
			//health
			glColor3f(0,0.8,0);
			glVertex3f(agents[i].pos.x+xo,agents[i].pos.y+yo+20*(2-agents[i].health),0);
			glVertex3f(agents[i].pos.x+xo+5,agents[i].pos.y+yo+20*(2-agents[i].health),0);
			glVertex3f(agents[i].pos.x+xo+5,agents[i].pos.y+yo+40,0);
			glVertex3f(agents[i].pos.x+xo,agents[i].pos.y+yo+40,0);
			
			//if this is a hybrid, we want to put a marker down
			if(agents[i].hybrid){
				glColor3f(0,0,0.8);
				glVertex3f(agents[i].pos.x+xo+6,agents[i].pos.y+yo,0);
				glVertex3f(agents[i].pos.x+xo+12,agents[i].pos.y+yo,0);
				glVertex3f(agents[i].pos.x+xo+12,agents[i].pos.y+yo+10,0);
				glVertex3f(agents[i].pos.x+xo+6,agents[i].pos.y+yo+10,0);	
			}
			
			glColor3f(1-agents[i].herbivore,agents[i].herbivore,0);
			glVertex3f(agents[i].pos.x+xo+6,agents[i].pos.y+yo+12,0);
			glVertex3f(agents[i].pos.x+xo+12,agents[i].pos.y+yo+12,0);
			glVertex3f(agents[i].pos.x+xo+12,agents[i].pos.y+yo+22,0);
			glVertex3f(agents[i].pos.x+xo+6,agents[i].pos.y+yo+22,0);	

			//how much sound is this bot making?
			glColor3f(agents[i].soundmul,agents[i].soundmul,agents[i].soundmul);
			glVertex3f(agents[i].pos.x+xo+6,agents[i].pos.y+yo+24,0);
			glVertex3f(agents[i].pos.x+xo+12,agents[i].pos.y+yo+24,0);
			glVertex3f(agents[i].pos.x+xo+12,agents[i].pos.y+yo+34,0);
			glVertex3f(agents[i].pos.x+xo+6,agents[i].pos.y+yo+34,0);	

			//draw giving/receiving
			if(agents[i].dfood!=0){
				
				float mag=cap(abs(agents[i].dfood)/conf::FOODTRANSFER/3);
				if(agents[i].dfood>0) glColor3f(0,mag,0); //draw boost as green outline
				else glColor3f(mag,0,0);
				glVertex3f(agents[i].pos.x+xo+6,agents[i].pos.y+yo+36,0);
				glVertex3f(agents[i].pos.x+xo+12,agents[i].pos.y+yo+36,0);
				glVertex3f(agents[i].pos.x+xo+12,agents[i].pos.y+yo+46,0);
				glVertex3f(agents[i].pos.x+xo+6,agents[i].pos.y+yo+46,0);		
			}
		

		glEnd();

		//print stats
		//generation count
		sprintf(buf2, "%i", agents[i].gencount);
		RenderString(agents[i].pos.x-conf::BOTRADIUS*1.5, agents[i].pos.y+conf::BOTRADIUS*1.8, GLUT_BITMAP_TIMES_ROMAN_24, buf2, 0.0f, 0.0f, 0.0f);
		//age
		sprintf(buf2, "%i", agents[i].age);
		RenderString(agents[i].pos.x-conf::BOTRADIUS*1.5, agents[i].pos.y+conf::BOTRADIUS*1.8+12, GLUT_BITMAP_TIMES_ROMAN_24, buf2, 0.0f, 0.0f, 0.0f);
		
		//health		
		sprintf(buf2, "%.2f", agents[i].health);
		RenderString(agents[i].pos.x-conf::BOTRADIUS*1.5, agents[i].pos.y+conf::BOTRADIUS*1.8+24, GLUT_BITMAP_TIMES_ROMAN_24, buf2, 0.0f, 0.0f, 0.0f);
			
		//repcounter
		sprintf(buf2, "%.2f", agents[i].repcounter);
		RenderString(agents[i].pos.x-conf::BOTRADIUS*1.5, agents[i].pos.y+conf::BOTRADIUS*1.8+36, GLUT_BITMAP_TIMES_ROMAN_24, buf2, 0.0f, 0.0f, 0.0f);
	}

	glPopMatrix();
	glutSwapBuffers();
}


//calculate inputs for every agent
void setInputs() {
	
	//P1 R1 G1 B1 FOOD P2 R2 G2 B2 SOUND SMELL HEALTH P3 R3 G3 B3 CLOCK1 CLOCK 2 HEARING     BLOOD_SENSOR
	//0   1  2  3  4   5   6  7 8   9     10     11   12 13 14 15 16       17      18           19

	float PI8=M_PI/8/2; //pi/8/2
	float PI38= 3*PI8; //3pi/8/2
	for(int i=0;i<agents.size();i++){
		Agent* a= &agents[i];

		//HEALTH
		a->in[11]= cap(a->health/2); //divide by 2 since health is in [0,2]

		//FOOD
		int cx= (int) a->pos.x/conf::CZ;
		int cy= (int) a->pos.y/conf::CZ;
		a->in[4]= food[cx][cy]/conf::FOODMAX;

		//SOUND SMELL EYES
		float p1,r1,g1,b1,p2,r2,g2,b2,p3,r3,g3,b3;
		p1=0;r1=0;g1=0;b1=0;p2=0;r2=0;g2=0;b2=0;p3=0;r3=0;g3=0;b3=0;
		float soaccum=0;
		float smaccum=0;
		float hearaccum=0;
		
		//BLOOD ESTIMATOR
		float blood= 0;

		for(int j=0;j<agents.size();j++){
			if(i==j) continue;
			Agent* a2= &agents[j];

			if(a->pos.x<a2->pos.x-conf::DIST || a->pos.x>a2->pos.x+conf::DIST 
				|| a->pos.y>a2->pos.y+conf::DIST || a->pos.y<a2->pos.y-conf::DIST) continue;

			float d= (a->pos-a2->pos).length();
			
			if(d<conf::DIST){

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
				if(leyeangle<-M_PI) leyeangle+= 2*M_PI;
				if(reyeangle>M_PI) reyeangle-= 2*M_PI;
				if(backangle>M_PI) backangle-= 2*M_PI;
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

				if(diff1<PI38){
					//we see this agent with left eye. Accumulate info
					float mul1= conf::EYE_SENSITIVITY*((PI38-diff1)/PI38)*((conf::DIST-d)/conf::DIST);
					//float mul1= 100*((conf::DIST-d)/conf::DIST);
					p1 += mul1*(d/conf::DIST);
					r1 += mul1*a2->red;
					g1 += mul1*a2->gre;
					b1 += mul1*a2->blu;
				}

				if(diff2<PI38){
					//we see this agent with left eye. Accumulate info
					float mul2= conf::EYE_SENSITIVITY*((PI38-diff2)/PI38)*((conf::DIST-d)/conf::DIST);
					//float mul2= 100*((conf::DIST-d)/conf::DIST);
					p2 += mul2*(d/conf::DIST);
					r2 += mul2*a2->red;
					g2 += mul2*a2->gre;
					b2 += mul2*a2->blu;
				}
				
				if(diff3<PI38){
					//we see this agent with back eye. Accumulate info
					float mul3= conf::EYE_SENSITIVITY*((PI38-diff3)/PI38)*((conf::DIST-d)/conf::DIST);
					//float mul2= 100*((conf::DIST-d)/conf::DIST);
					p3 += mul3*(d/conf::DIST);
					r3 += mul3*a2->red;
					g3 += mul3*a2->gre;
					b3 += mul3*a2->blu;
				}

				if(diff4<PI38){
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

void brainsTick(){
	//takes in[] to out[] for every agent

	#pragma omp parallel for
	for(int i=0;i<agents.size();i++){
		agents[i].brain.tick(agents[i].in, agents[i].out);
	}
}

void processOutputs(){
	//assign meaning
	//LEFT RIGHT R G B SPIKE BOOST SOUND_MULTIPLIER GIVING
	// 0    1    2 3 4   5     6         7             8
	for(int i=0;i<agents.size();i++){
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
		else if(a->spikeLength>g)
			a->spikeLength= g; //its easy to retract spike, just hard to put it up
	}

	//move bots
	//#pragma omp parallel for
	for(int i=0;i<agents.size();i++){
		Agent* a= &agents[i];

		Vector2f v(conf::BOTRADIUS/2, 0);
		v.rotate(a->angle + M_PI/2);
		
		Vector2f w1p= a->pos+ v; //wheel positions
		Vector2f w2p= a->pos- v;

		float BW1= conf::BOTSPEED*a->w1;
		float BW2= conf::BOTSPEED*a->w2;
		if(a->boost){BW1=BW1*conf::BOOSTSIZEMULT;}
		if(a->boost){BW2=BW2*conf::BOOSTSIZEMULT;}

		//move bots
		Vector2f vv= w2p- a->pos;
		vv.rotate(-BW1);
		a->pos= w2p-vv;
		a->angle -= BW1;
		if(a->angle<-M_PI) a->angle= M_PI - (-M_PI-a->angle);
		vv= a->pos - w1p;
		vv.rotate(BW2);
		a->pos= w1p+vv;
		a->angle += BW2;
		if(a->angle>M_PI) a->angle= -M_PI + (a->angle-M_PI);

		//wrap around the map
		if(a->pos.x<0) a->pos.x= conf::WIDTH+a->pos.x;
		if(a->pos.x>=conf::WIDTH) a->pos.x= a->pos.x-conf::WIDTH;
		if(a->pos.y<0) a->pos.y= conf::HEIGHT+a->pos.y;
		if(a->pos.y>=conf::HEIGHT) a->pos.y= a->pos.y-conf::HEIGHT;
	}
	
	//process food intake for herbivors
	for(int i=0;i<agents.size();i++){
		
		int cx= (int) agents[i].pos.x/conf::CZ;
		int cy= (int) agents[i].pos.y/conf::CZ;
		float f= food[cx][cy];
		if(f>0 && agents[i].health<2){
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
	for(int i=0;i<agents.size();i++){agents[i].dfood=0;}
	for(int i=0;i<agents.size();i++){
		if(agents[i].give>0.5){
			for(int j=0;j<agents.size();j++){
				float d= (agents[i].pos-agents[j].pos).length();
				if(d<conf::FOOD_SHARING_DISTANCE){
					//initiate transfer
					if(agents[j].health<2) agents[j].health += conf::FOODTRANSFER;
					agents[i].health -= conf::FOODTRANSFER;
					agents[j].dfood += conf::FOODTRANSFER; //only for drawing
					agents[i].dfood -= conf::FOODTRANSFER;
				}
			}
		}
	}

	//process spike dynamics for carnivors
	if(modcounter%2==0){ //we dont need to do this TOO often. can save efficiency here since this is n^2 op in #agents
		for(int i=0;i<agents.size();i++){
			for(int j=0;j<agents.size();j++){
				if(i==j || agents[i].spikeLength<0.2 || agents[i].w1<0.3 || agents[i].w2<0.3) continue;
				float d= (agents[i].pos-agents[j].pos).length();

				if(d<2*conf::BOTRADIUS){
					//these two are in collision and agent i has extended spike and is going decent fast!
					Vector2f v(1,0);
					v.rotate(agents[i].angle);
					float diff= v.angle_between(agents[j].pos-agents[i].pos);
					if(fabs(diff)<M_PI/8){
						//bot i is also properly aligned!!! that's a hit
						float mult=1;
						if(agents[i].boost) mult= conf::BOOSTSIZEMULT;
						float DMG= conf::SPIKEMULT*agents[i].spikeLength*max(fabs(agents[i].w1),fabs(agents[i].w2))*conf::BOOSTSIZEMULT;
						
						agents[j].health-= DMG; 

						if(agents[i].health>2) agents[i].health=2; //cap health at 2
						agents[i].spikeLength= 0; //retract spike back down
						
						agents[i].initEvent(40*DMG,1,1,0); //yellow event means bot has spiked other bot. nice!

						Vector2f v2(1,0);
						v2.rotate(agents[j].angle);
						float adiff= v.angle_between(v2);
						if(fabs(adiff)<M_PI/2){
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

//MR is chance of mutations happening
//MR2 is the severity of the mutations
//kinda...
//ai is index of the agent we want to copy from. If we passed reference, we could invalide it 
//while making more babies, because push_back to agents can make the vector reallocate somewhere else
void reproduce(int ai, float MR, float MR2){ 
	if(randf(0,1)<0.04) MR= MR*randf(1, 10);
	if(randf(0,1)<0.04) MR2= MR2*randf(1, 10);

	agents[ai].initEvent(30,0,0.8,0); //green event means agent reproduced.
	for(int i=0;i<conf::BABIES;i++){
		Agent a2;
		
		//spawn the baby somewhere closeby behind agent
		//we want to spawn behind so that agents dont accidentally eat their young right away
		Vector2f fb(conf::BOTRADIUS,0);
		fb.rotate(-a2.angle);
		a2.pos= agents[ai].pos + fb + Vector2f(randf(-conf::BOTRADIUS*2,conf::BOTRADIUS*2), randf(-conf::BOTRADIUS*2,conf::BOTRADIUS*2)); 
		if(a2.pos.x<0) a2.pos.x= conf::WIDTH+a2.pos.x;
		if(a2.pos.x>=conf::WIDTH) a2.pos.x= a2.pos.x-conf::WIDTH;
		if(a2.pos.y<0) a2.pos.y= conf::HEIGHT+a2.pos.y;
		if(a2.pos.y>=conf::HEIGHT) a2.pos.y= a2.pos.y-conf::HEIGHT;

		a2.gencount= agents[ai].gencount+1;
		a2.repcounter= a2.herbivore*randf(conf::REPRATEH-0.1,conf::REPRATEH+0.1) + (1-a2.herbivore)*randf(conf::REPRATEC-0.1,conf::REPRATEC+0.1);

		//noisy attribute passing
		a2.MUTRATE1= agents[ai].MUTRATE1;
		a2.MUTRATE2= agents[ai].MUTRATE2;
		if(randf(0,1)<0.2) a2.MUTRATE1= randn(agents[ai].MUTRATE1, conf::METAMUTRATE1);
		if(randf(0,1)<0.2) a2.MUTRATE2= randn(agents[ai].MUTRATE2, conf::METAMUTRATE2);
		if(agents[ai].MUTRATE1<0.001) agents[ai].MUTRATE1= 0.001;
		if(agents[ai].MUTRATE2<0.02) agents[ai].MUTRATE2= 0.02;
		a2.herbivore= cap(randn(agents[ai].herbivore, MR2*4));
		if(randf(0,1)<MR*5) a2.clockf1= randn(a2.clockf1, MR2);
		if(a2.clockf1<2) a2.clockf1= 2;
		if(randf(0,1)<MR*5) a2.clockf2= randn(a2.clockf2, MR2);
		if(a2.clockf2<2) a2.clockf2= 2;

		//mutate brain here
		a2.brain= agents[ai].brain;
		
		for(int j=0;j<BRAINSIZE;j++){
			
			if(randf(0,1)<MR*3) {
				a2.brain.boxes[j].bias+= randn(0, MR2);
				a2.mutations.push_back("bias jiggled\n");
			}

			if(false && randf(0,1)<MR*3) {
				a2.brain.boxes[j].kp+= randn(0, MR2);
				if(a2.brain.boxes[j].kp<0.01) a2.brain.boxes[j].kp=0.01;
				if(a2.brain.boxes[j].kp>1) a2.brain.boxes[j].kp=1;
				a2.mutations.push_back("kp jiggled\n");
			}

			if(randf(0,1)<MR*3) {
				int rc= randi(0, CONNS);
				a2.brain.boxes[j].w[rc]+= randn(0, MR2);
				if(a2.brain.boxes[j].w[rc]<0.01) a2.brain.boxes[j].w[rc]= 0.01;
				a2.mutations.push_back("weight jiggled\n");
			}
			
			//more unlikely changes here
			if(randf(0,1)<MR) {
				int rc= randi(0, CONNS);
				int ri= randi(0,BRAINSIZE);
				a2.brain.boxes[j].id[rc]= ri;
				a2.mutations.push_back("connectivity changed\n");
			}

			if(randf(0,1)<MR) {
				int rc= randi(0, CONNS);
				a2.brain.boxes[j].notted[rc]= !a2.brain.boxes[j].notted[rc];
				a2.mutations.push_back("notted was flipped\n");
			}

			if(randf(0,1)<MR) {
				a2.brain.boxes[j].type= 1-a2.brain.boxes[j].type; 
				a2.mutations.push_back("type of a box was changed\n");
			}
		}
	
		a2.id= idcounter;
		idcounter++;
		agents.push_back(a2);

		//record this
		//FILE* fp = fopen("log.txt", "a");
		//fprintf(fp, "%i %i %i\n", 1, agents[ai].id, a2.id); //1 marks the event: child is born
		//fclose(fp);
	}
}

//find two successful agents, cross them over and add them
void addNewByCrossover(){

	//find two success cases
	int i1= randi(0, agents.size());
	int i2= randi(0, agents.size());
	for(int i=0;i<agents.size();i++){
		if(agents[i].age > agents[i1].age && randf(0,1)<0.1){
			i1= i;
		}
		if(agents[i].age > agents[i2].age && randf(0,1)<0.1 && i!=i1){
			i2= i;
		}		
	}
	
	Agent* a1= &agents[i1];
	Agent* a2= &agents[i2];
	Agent anew;
	anew.hybrid=true; //set this non-default flag
	anew.gencount= a1->gencount;
	if(a2->gencount<anew.gencount) anew.gencount= a2->gencount;

	//agent heredity attributes
	anew.clockf1= randf(0,1)<0.5 ? a1->clockf1 : a2->clockf1;
	anew.clockf2= randf(0,1)<0.5 ? a1->clockf2 : a2->clockf2;
	anew.herbivore= randf(0,1)<0.5 ? a1->herbivore : a2->herbivore;
	anew.MUTRATE1= randf(0,1)<0.5 ? a1->MUTRATE1 : a2->MUTRATE1;
	anew.MUTRATE2= randf(0,1)<0.5 ? a1->MUTRATE2 : a2->MUTRATE2;

	//cross brains
	anew.brain= a1->brain;
	for(int i=0;i<anew.brain.boxes.size(); i++){
		anew.brain.boxes[i].bias= randf(0,1)<0.5 ? a1->brain.boxes[i].bias : a2->brain.boxes[i].bias;
		anew.brain.boxes[i].kp= randf(0,1)<0.5 ? a1->brain.boxes[i].kp : a2->brain.boxes[i].kp;
		anew.brain.boxes[i].type= randf(0,1)<0.5 ? a1->brain.boxes[i].type : a2->brain.boxes[i].type;

		for(int j=0;j<anew.brain.boxes[i].id.size();j++){
			anew.brain.boxes[i].id[j] = randf(0,1)<0.5 ? a1->brain.boxes[i].id[j] : a2->brain.boxes[i].id[j];
			anew.brain.boxes[i].notted[j] = randf(0,1)<0.5 ? a1->brain.boxes[i].notted[j] : a2->brain.boxes[i].notted[j];
			anew.brain.boxes[i].w[j] = randf(0,1)<0.5 ? a1->brain.boxes[i].w[j] : a2->brain.boxes[i].w[j];
		}
	}

	//maybe do mutation here? I dont know. So far its only crossover
	anew.id= idcounter; idcounter++;
	agents.push_back(anew);
}

void addRandomBots(int num){
	for(int i=0;i<num;i++){
		Agent a;
		a.id= idcounter;
		idcounter++;
		agents.push_back(a);
	}
}

void writeReport(){
	//save all kinds of nice data stuff
	int numherb=0;
	int numcarn=0;
	int topcarn=0;
	int topherb=0;
	for(int i=0;i<agents.size();i++){
		if(agents[i].herbivore>0.5) numherb++;
		else numcarn++;

		if(agents[i].herbivore>0.5 && agents[i].gencount>topherb) topherb= agents[i].gencount;
		if(agents[i].herbivore<0.5 && agents[i].gencount>topcarn) topcarn= agents[i].gencount;
	}

	FILE* fp = fopen("report.txt", "a");
	fprintf(fp, "%i %i %i %i\n", numherb, numcarn, topcarn, topherb);
	fclose(fp);
}

void update() {
	modcounter++;
	
	//Process periodic events
	//Age goes up!
	if(modcounter%100==0){
		for(int i=0;i<agents.size();i++){agents[i].age+= 1;} //agents age...
	}
	if(modcounter%1000==0) writeReport();
	if(modcounter>=10000) {
		modcounter=0;
		epoch++;
	}
	if(modcounter%conf::FOODADDFREQ==0){
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
	for(int i=0;i<agents.size();i++){
		float baseloss= 0.0002 + 0.0001*(abs(agents[i].w1) + abs(agents[i].w2))/2;
		if(agents[i].w1<0.1 && agents[i].w2<0.1) baseloss=0.0001; //hibernation :p
		baseloss += 0.00005*agents[i].soundmul; //shouting costs energy. just a tiny bit

		if(agents[i].boost){
			//boost carries its price, and it's pretty heavy!
			agents[i].health -= baseloss*conf::BOOSTSIZEMULT*2;
		} else {
			agents[i].health -= baseloss;
		}
	}

	//remove dead agents.
	//first distribute foods
	for(int i=0;i<agents.size();i++){
		if(agents[i].health<=0){
			//distribute its food. It will be erased soon
			//first figure out how many are around, to distribute this evenly
			int numaround=0;
			for(int j=0;j<agents.size();j++){
				if(agents[j].health>0){
					float d= (agents[i].pos-agents[j].pos).length();
					if(d<conf::FOOD_DISTRIBUTION_RADIUS){
						numaround++;
					}
				}
			}
			if(numaround>0){
				//distribute its food evenly
				for(int j=0;j<agents.size();j++){
					if(agents[j].health>0 && agents[j].herbivore<0.7){
						float d= (agents[i].pos-agents[j].pos).length();
						if(d<conf::FOOD_DISTRIBUTION_RADIUS){
							agents[j].health += 3*(1-agents[j].herbivore)*(1-agents[j].herbivore)/numaround;
							agents[j].repcounter -= 2*(1-agents[j].herbivore)*(1-agents[j].herbivore)/numaround; //good job, can use spare parts to make copies
							if(agents[j].health>2) agents[j].health=2; //cap it!
							agents[j].initEvent(30,1,1,1); //white means they ate! nice
						}
					}
				}
			}

		}
	}
	vector<Agent>::iterator iter= agents.begin();
	while(iter != agents.end()){
		if(iter->health <=0){
			iter= agents.erase(iter);
		} else {
			++iter;
		}
	}
	
	//handle reproduction
	for(int i=0;i<agents.size();i++){
		if(agents[i].repcounter<0 && agents[i].health>0.65){ //agent is healthy and is ready to reproduce
			//agents[i].health= 0.8; //the agent is left vulnerable and weak, a bit
			reproduce(i, agents[i].MUTRATE1, agents[i].MUTRATE2); //this adds conf::BABIES new agents to agents[]
			agents[i].repcounter= agents[i].herbivore*randf(conf::REPRATEH-0.1,conf::REPRATEH+0.1) + (1-agents[i].herbivore)*randf(conf::REPRATEC-0.1,conf::REPRATEC+0.1);
		}
	}
	
	//environment tick
	for(int x=0;x<FW;x++){
		for(int y=0;y<FH;y++){
			food[x][y]+= conf::FOODGROWTH; //food grows
			if(food[x][y]>conf::FOODMAX)food[x][y]=conf::FOODMAX; //cap at conf::FOODMAX
		}
	}

	//add new agents, if environment isn't closed
	if(conf::CLOSED==0){ 
		//make sure environment is always populated with at least NUMBOTS bots
		if(agents.size()<conf::NUMBOTS
){
			//add new agent
			addRandomBots(1);
		}
		if(modcounter%200==0){
			if(randf(0,1)<0.5)
				addRandomBots(1); //every now and then add random bots in
			else
				addNewByCrossover(); //or by crossover
		}
	}

	//show FPS
	int currentTime = glutGet( GLUT_ELAPSED_TIME );
	frames++;
	if((currentTime - lastUpdate) >= 1000){
		int numherb=0;
		int numcarn=0;
		for(int i=0;i<agents.size();i++){
			if(agents[i].herbivore>0.5) numherb++;
			else numcarn++;
		}
		sprintf( buf, "FPS: %d NumAgents: %d Carnivors: %d Herbivors: %d Epoch: %d", frames, agents.size(), numcarn, numherb, epoch );
		glutSetWindowTitle( buf );
		frames = 0;
		lastUpdate = currentTime;
	}
	if(skipdraw<=0 && draw){
		clock_t endwait;
		float mult=-0.005*(skipdraw-1); //ugly, ah well
		endwait = clock () + mult * CLOCKS_PER_SEC ;
		while (clock() < endwait) {}
	}
}

void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27) 
		exit(0);
	else if (key=='r') {
		agents.clear();
		addRandomBots(conf::NUMBOTS
);
		printf("Agents reset\n");
	} else if (key=='p') {
		//pause
		paused= !paused;
	} else if (key=='d') {
		//drawing
		draw= !draw;
	} else if(key==43){
		//+
		skipdraw++;

	} else if(key==45){
		//-
		skipdraw--;
	} else if(key=='f'){
		drawFood=!drawFood;
	} else if(key=='c'){
		conf::CLOSED= 1-conf::CLOSED;
		printf("Environemt closed now= %i\n",conf::CLOSED==0?0:1);
	} else {
		printf("Unknown key pressed: %i\n", key);
	}
	
}

void processMouse(int button, int state, int x, int y) {
	if(state==0){
		float mind=1e10;
		float mini=-1;
		float d;
		for(int i=0;i<agents.size();i++){
			d= pow(x-agents[i].pos.x,2)+pow(y-agents[i].pos.y,2);
			if (d<mind){
				mind=d;
				mini=i;
			}
		}
		//toggle selection of this agent
		for(int i=0;i<agents.size();i++) agents[i].selectflag=false;
		agents[mini].selectflag= true;
		agents[mini].printSelf();	
	}
}

void processMouseActiveMotion(int x, int y) {
}

void handleIdle(){
	if(!paused) update();
	if(draw){
		if(skipdraw>0){if(modcounter%skipdraw==0) renderScene();} //increase fps by skipping drawing
		else renderScene(); //we will decrease fps by waiting using clocks
	}
}
int main(int argc, char **argv) {
	srand(time(0));

	//start the report file. null it up if it exists
	FILE* fp = fopen("report.txt", "w");
	fclose(fp);

	addRandomBots(conf::NUMBOTS
);
	//for(int k=0;k<500;k++) reproduce(0, 100*agents[0].MUTRATE1, 100*agents[0].MUTRATE2);

	//inititalize food layer
	if(conf::WIDTH%conf::CZ!=0 || conf::HEIGHT%conf::CZ!=0) printf("CAREFUL! The cell size variable conf::CZ should divide evenly into  both conf::WIDTH and conf::HEIGHT! It doesn't right now!");
	printf("p= pause, d= toggle drawing (for faster computation), f= draw food too, += faster, -= slower");
	for(int x=0;x<FW;x++){
		for(int y=0;y<FH;y++){
			food[x][y]= 0;
		}
	}

	//GLUT SETUP
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(30,30);
	glutInitWindowSize(conf::WWIDTH,conf::WHEIGHT);
	glutCreateWindow("TEST");
	glClearColor(0.9f, 0.9f, 1.0f, 0.0f);
	glutDisplayFunc(renderScene);
	glutIdleFunc(handleIdle);
	glutReshapeFunc(changeSize);
	
	glutKeyboardFunc(processNormalKeys);
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseActiveMotion);

	glutMainLoop();
    return 0;
}


