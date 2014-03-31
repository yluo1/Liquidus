#ifndef _FORCEPONGTIMETRIAL_
#define _FORCEPONGTIMETRIAL_

#include "Display.h"
#include "Keyboard.h"
#include "Object.h"
#include "Map.h"

#include "ForcePongConstants.h"
#include "ForcePongBot.h"
#include "ForcePongHighscore.h"
#include "ShipFluidObject.h"

class ForcePongTimeTrial{
private:

	typedef struct{
		float timer;

		float hpIncrement;
		float shieldIncrement;
		float powerIncrement;
		float impulseIncrement;
		float powerRegenIncrement;
		float rateOfFireMult;
		bool isActive;
	}ShipStats;

	Display *timeTrialDisplay;
	Keyboard *timeTrialKeyboard;

	Map	*fluidMap;
	Object **objPtr;

	//Time dilations
	float dilationRings;
	int dilationCycle;
	float dilationRingSizeMult;

	int lastScore;

	ShipFluidObject *userShipObj;
	ForcePongBot *botController[MAX_BOTS];

	ShipStats *shipStats;

	ForcePongHighscore highScores;

	bool lose;

	float timer;

public:
	//////Functions
	ForcePongTimeTrial(){
	}

	ForcePongTimeTrial(Display *timeTrialDisplay,Keyboard *timeTrialKeyboard,Map *fluidMap,Object **objPtr){
		this->timeTrialDisplay=timeTrialDisplay;
		this->fluidMap=fluidMap;
		this->objPtr=objPtr;

		for(int a=0;a<MAX_BOTS;++a) botController[a]=NULL;
		shipStats=NULL;

		highScores=ForcePongHighscore(timeTrialDisplay,timeTrialKeyboard);
	}

	~ForcePongTimeTrial(){
		
	}

	void makeGame(int nShips,int nBots,int botTypes[],bool demo){
		
		userShip=0;
		shipCount=nShips;
		globalScale=.7;
		dilationRings=1;
		dilationRingSizeMult=10;
		lastScore=0;
		timer=60;
		lose=false;

		//Clear memory
		for(int a=0;a<objCount;++a){
			if(objPtr[a]!=NULL){
				delete objPtr[a];
				objPtr[a]=NULL;	
			}
		}

		//Clear ship stats
		if(shipStats!=NULL) delete [] shipStats;

		//Setup bot controllers
		for(int a=0;a<shipCount;++a){
			if(botController[a]!=NULL){
				delete botController[a];
				botController[a]=NULL;
			}
		}
		for(int a=0;a<shipCount;++a) botController[a]=NULL;

		//Setup ship stats
		shipStats=new ShipStats[shipCount];
		for(int a=0;a<shipCount;++a){
			shipStats[a].timer=rand()%rand()%20+25;
			shipStats[a].hpIncrement=.15;
			shipStats[a].shieldIncrement=.2;
			shipStats[a].powerIncrement=.15;
			shipStats[a].powerRegenIncrement=.001;
			shipStats[a].impulseIncrement=.15;
			shipStats[a].rateOfFireMult=1.1;
			shipStats[a].isActive=true;
		}

		//Setup ships
		VectorT shipColors[]={VectorT(0,0,1),VectorT(0,1,0),VectorT(1,0,0),VectorT(1,1,0),VectorT(0,1,1),VectorT(1,0,1),VectorT(1,1,1),VectorT(0,0,0)};
		Model *sModel=NULL;
		for(int a=0,b=0;a<shipCount;++a){
			if(a!=userShip) sModel=&globalModels[MODEL_SHIP_DEFAULT];
			else sModel=&globalModels[MODEL_SHIP_USER];

			objPtr[a]=new ShipFluidObject(	a,a,(a<MAX_WEAPON_TYPES?shipColors[a]:randomColor()),VectorT(rand()%(defaultDisplay.windowWidth),rand()%(defaultDisplay.windowHeight),0),
											DEFAULT_SHIP_RADIUS,DEFAULT_SHIP_MASS,
											250,
											1,1,.0065,
											4,4,
											1,
											1,
											1,
											9999,
											fluidMap,objPtr,&objPtr[a],sModel);
		
			if(demo||a!=userShip)
				botController[a]=new ForcePongBot(a,botTypes[(b++)%nBots],.75,(ShipFluidObject*)objPtr[a],objPtr,&botController[a]);
		}
		userShipObj=(ShipFluidObject*)objPtr[userShip];

		//Set target to self
		userTarget=userShip;
		userShipObj->setTarget(((ShipFluidObject*)(objPtr[userTarget])));

		//Initial display set to ink
		displayMode=3;
	}

	void display(){
		//Score
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor3fv((GLfloat*)&(userShipObj->color));
		timeTrialDisplay->displayString("Score:",defaultDisplay.windowWidth-100,75,FONT_SIZE,NULL);
		for(int a=0;a<shipCount;++a){
			string s=numberToString(((ShipFluidObject*)objPtr[a])->score);
			glColor3fv((GLfloat*)&(((ShipFluidObject*)objPtr[a])->color));
			timeTrialDisplay->displayString(s,defaultDisplay.windowWidth-100+a*20,100,FONT_SIZE,NULL);
		}


		//Display timer
		glColor3fv((GLfloat*)&(userShipObj->color));
		timeTrialDisplay->displayString(numberToString((int)timer),timeTrialDisplay->windowWidth/2-10,75,FONT_SIZE*3,NULL);
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		//Display ship timers
		for(int a=0;a<shipCount;++a){
			if(a!=userShip){
				VectorT shipPos=objPtr[a]->position;
				float rad=objPtr[a]->radius;
				glColor3fv((GLfloat*)&((FluidObject*)objPtr[a])->color);
				timeTrialDisplay->displayString(numberToString((int)shipStats[a].timer),shipPos.x-rad,shipPos.y-rad*2,.2,NULL);
			}
		}

		//Draw Dilation rings
		glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,timeTextures[TIME_DILATION].texID);
		
		int dilationCeil=ceil(dilationRings);
		VectorT userColor=userShipObj->color;
		VectorT reverseColor=(-userColor+1)*.75+userColor*.25;
		for(int a=0;a<dilationCeil;++a){
			VectorT interColor=userColor*((float)(dilationCeil-a)/dilationCeil)+reverseColor*(1-(float)(dilationCeil-a)/dilationCeil);
			glColor4f(interColor.x,interColor.y,interColor.z,minValue(1,dilationRings-a));	
			float newRad=userShipObj->radius*(a+1)*dilationRingSizeMult;
			VectorT position=userShipObj->position;
			glPushMatrix();
			glTranslatef(position.x,position.y,0);
			if(a%2==0){
				glRotatef(dilationCycle*(a+1),0,0,1);
			}else{
				glRotatef(-dilationCycle*(a+1),0,0,1);
			}
			glBegin(GL_QUADS);
			glTexCoord2d(1,1); glVertex2f(newRad,newRad);
			glTexCoord2d(1,0); glVertex2f(newRad,-newRad);
			glTexCoord2d(0,0); glVertex2f(-newRad,-newRad);
			glTexCoord2d(0,1); glVertex2f(-newRad,newRad);
			glEnd();

			glPopMatrix();
		}
		glPopAttrib();

		if(lose){
			glColor4f(userShipObj->color.x,userShipObj->color.y,userShipObj->color.z,1);
			
			highScores.display();
		}
	
	}

	void update(){
		/*Rules
		When timer hits 0, you lose
		All enemies have timers set on decay. (if timer runs out, you don't gain time)
		If you kill an enemy, you gain their time

		Infinite lives
		Score based your total score when timer runs dry

		Time dilation effects:
		Your time dilation ring slows down enemies to max of 25%
		You gain rings by killing enemies
		Rings decay overtime but are baselined at 1
		Your ring level slows time loss

		Leveling:
		Destroying enemies makes them tougher on respawn.
		Getting destroyed forces your respawn time wait but raises your stats
		
		
		*/
		
		//Update bots
		for(int a=0;a<shipCount;++a){
			if(botController[a]!=NULL) botController[a]->update();
		}

		//Update User Targeting
		if(objPtr[userTarget]!=NULL&&((FluidObject*)objPtr[userTarget])->isActive&&((FluidObject*)objPtr[userTarget])->objType==TYPE_OBJ_SHIP){
			userShipObj->setTarget(((FluidObject*)(objPtr[userTarget])));
		}else{//If current target is dead or voided, search for new one
			int a;
			FluidObject *t;
			for(a=0;a<shipCount;++a){
				if(objPtr[(userTarget+a)%shipCount]!=NULL){
					t=(FluidObject*)objPtr[(userTarget+a)%shipCount];
					if(t->isActive&&t->objType==TYPE_OBJ_SHIP){
						userTarget=(userTarget+a)%shipCount;
						break;
					}
				}
			}
			if(a==shipCount){
				userTarget=userShip;
			}
			userShipObj->setTarget(((FluidObject*)(objPtr[userTarget])));
		}

		/////////////Update Time Dilation
		//decay dilation ring
		dilationRings=maxValue(1,dilationRings-.00075*dilationRings);
		
		//Increase ring after kill
		int upScore=userShipObj->score;
		dilationRings+=upScore-lastScore;
		lastScore=upScore;
		VectorT userOrigin=userShipObj->position;

		//increase ring cycle
		float userShipRadius=userShipObj->radius;
		int userShipTeam=userShipObj->teamID;
		dilationCycle=(dilationCycle+2)%360;
		int dilationCeil=ceil(dilationRings);

		for(int a=0;a<objCount;++a){
			FluidObject *fObj=(FluidObject*)objPtr[a];
			if(fObj!=NULL&&fObj->teamID!=userShipTeam){
				int b;
				for(b=0;b<dilationCeil;++b){
					float newRad=userShipRadius*(b+1)*dilationRingSizeMult;
					float otherRad=fObj->radius;
					if((fObj->position-userOrigin).normSquared()<newRad*newRad+otherRad*otherRad){
						fObj->dt=deltaTime*(.25+(1-minValue(1,dilationRings-b))*.75);
						break;
					}
				}
				if(b==dilationCeil){
					fObj->dt+=(deltaTime-fObj->dt)*.1;
				}
			}
		}

		//Check on ship deaths
		for(int a=0;a<shipCount;++a){
			bool isActive=((FluidObject*)objPtr[a])->isActive;
			if(shipStats[a].isActive-isActive==1){//Ship died so level it
				ShipFluidObject *cShip=((ShipFluidObject*)objPtr[a]);
				cShip->addMaxHp(shipStats[a].hpIncrement);
				cShip->addMaxShields(shipStats[a].shieldIncrement);
				cShip->addMaxPower(shipStats[a].powerIncrement);
				cShip->addMaxPowerRegen(shipStats[a].powerRegenIncrement);
				for(int b=0;b<userShipObj->getMaxLasers();++b){
					cShip->laserWeapons[b]->multRateOfFire(shipStats[a].rateOfFireMult);
				}
				for(int b=0;b<userShipObj->getMaxMissiles();++b){
					cShip->missileWeapons[b]->multRateOfFire(shipStats[a].rateOfFireMult);
				}

				//Add to timer
				if(cShip->getLastDmgSource()==userShip){
					if(a!=userShip) timer+=shipStats[a].timer;
				}
				shipStats[a].timer=rand()%20+25;

			}
			shipStats[a].isActive=isActive;
		}
	
		//Lose condition
		if(!lose&&timer==0){
			highScores.readScores("timeTrialHighscores");
			highScores.insertScore(userShipObj->score);
			observerMode=true;
			lose=true;
		}

		//Enter highscore
		if(lose){
			highScores.enterScore();
		}

		//Update ship timers
		for(int a=0;a<shipCount;++a){
			shipStats[a].timer=maxValue(shipStats[a].timer-((FluidObject*)objPtr[a])->dt,0);
		}
		//Update timer
		timer=maxValue(timer-.02,0);
		
	}

	void cleanup(){
		for(int a=0;a<MAX_BOTS;++a){
			if(botController[a]!=NULL){
				delete botController[a];
				botController[a]=NULL;
			}
		}

		delete [] shipStats;
	}

};

#endif