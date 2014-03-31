#ifndef _FORCEPONGDUEL_
#define _FORCEPONGDUEL_

#include "Display.h"
#include "Object.h"
#include "Map.h"

#include "ForcePongConstants.h"
#include "ForcePongBot.h"
#include "ShipFluidObject.h"

class ForcePongDuel{
private:
	Display *duelDisplay;
	Map	*fluidMap;
	Object **objPtr;
	
	ShipFluidObject *userShipObj;
	ForcePongBot *botController[MAX_BOTS];
public:

	//////Functions
	ForcePongDuel(){
		for(int a=0;a<MAX_BOTS;++a) botController[a]=NULL;
	}

	ForcePongDuel(Display *duelDisplay,Map *fluidMap,Object **objPtr){
		this->duelDisplay=duelDisplay;
		this->fluidMap=fluidMap;
		this->objPtr=objPtr;

		for(int a=0;a<MAX_BOTS;++a) botController[a]=NULL;

	}

	~ForcePongDuel(){
		
	}

	void makeGame(int nShips,int nBots,int botTypes[],bool demo){
		
		userShip=0;
		shipCount=nShips;
		globalScale=.7;

		//Clear memory
		for(int a=0;a<objCount;++a){
			if(objPtr[a]!=NULL){
				delete objPtr[a];
				objPtr[a]=NULL;	
			}
		}

		//Setup bot controllers
		for(int a=0;a<shipCount;++a){
			if(botController[a]!=NULL){
				delete botController[a];
				botController[a]=NULL;
			}
		}
		for(int a=0;a<shipCount;++a) botController[a]=NULL;



		//Setup ships
		VectorT shipColors[]={VectorT(0,0,1),VectorT(0,1,0),VectorT(1,0,0),VectorT(1,1,0),VectorT(0,1,1),VectorT(1,0,1),VectorT(1,1,1),VectorT(0,0,0)};
		Model *sModel=NULL;
		for(int a=0,b=0;a<shipCount;++a){
			if(a!=userShip) sModel=&globalModels[MODEL_SHIP_DEFAULT];
			else sModel=&globalModels[MODEL_SHIP_USER];
			if(demo){//All weapons enabled
				objPtr[a]=new ShipFluidObject(	a,a,(a<MAX_WEAPON_TYPES?shipColors[a]:randomColor()),VectorT(rand()%(defaultDisplay.windowWidth),rand()%(defaultDisplay.windowHeight),0),
												13,10,
												250,
												1,1,.0065,
												4,4,
												1,
												1,
												1,
												9999,
												fluidMap,objPtr,&objPtr[a],sModel);
			}else{
				int ut;
				int maxMissiles=0,maxLasers=0;
				if(a==userShip){
					ut=rand()%3;
					if(ut==0) maxMissiles=2;
					else if(ut==1) maxLasers=2;
					else maxMissiles=maxLasers=1;
				}else{
					maxLasers=2*(botTypes[b%nBots]==TYPE_BOT_LASER);
					maxMissiles=2*(botTypes[b%nBots]==TYPE_BOT_MISSILE);
				}
				objPtr[a]=new ShipFluidObject(	a,a,randomColor(),VectorT(rand()%(defaultDisplay.windowWidth),rand()%(defaultDisplay.windowHeight),0),
									DEFAULT_SHIP_RADIUS,DEFAULT_SHIP_MASS,
									250,
									1,1,.0065,
									maxLasers,maxMissiles,true,
									1,
									1,
									1,
									9999,
									fluidMap,objPtr,&objPtr[a],sModel);
			}
		
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
		duelDisplay->displayString("Score:",defaultDisplay.windowWidth-100,75,FONT_SIZE,NULL);
		for(int a=0;a<shipCount;++a){
			string s=numberToString(((ShipFluidObject*)objPtr[a])->score);
			glColor3fv((GLfloat*)&(((ShipFluidObject*)objPtr[a])->color));
			duelDisplay->displayString(s,defaultDisplay.windowWidth-100+a*20,100,FONT_SIZE,NULL);
		}
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}

	void update(){
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

	}

	void cleanup(){
		for(int a=0;a<MAX_BOTS;++a){
			if(botController[a]!=NULL){
				delete botController[a];
				botController[a]=NULL;
			}
		}
	}
};

#endif