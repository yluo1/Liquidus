#ifndef __FORCEPONGLEVELS__
#define __FORCEPONGLEVELS__

#include <iostream>
#include <fstream>

#include "Display.h"
#include "Keyboard.h"
#include "Object.h"
#include "Map.h"

#include "ForcePongConstants.h"
#include "ForcePongBot.h"
#include "ForcePongHighscore.h"
#include "ShipFluidObject.h"
#include "WeaponObject.h"

#define LEVELUPTIME	200
#define SURVIVOR_TEXT_TIME 300

/*
Gameplay rules:
Win condition: High Score
Lose condition: Lose all lives
Level Condition: Destroy all ships
Leech: Every ship destroyed recovers some hp/power.
Levelup: Increase difficulty, change play types
Initials: Laser and missile only
*/

class ForcePongSurvivor{
private:
	typedef struct Level{
		int level;//Level number
		//Ship modifiers
		float maxShipHp;
		float maxShipShields;
		float maxShipPower;
		float maxShipImpulse;
		float maxShipPowerRegenRate;
		float maxShipVelocity;

		//Weapon modifiers (Multipliers)
		float weaponRateOfFireMult;//Higher is better
		float weaponDmgMult;//Higher is better
		float weaponPowerMult;//Lower is better

		//Weapon array
		int maxWeaponLevels[MAX_WEAPON_TYPES];	//Possible enemy weapons and levels
		int maxWeapons;			//Possible weapon randoms

		//Quantity modifiers
		int maxConcurrentEnemies;
		int maxEnemies;
	};

	typedef struct UserGrowth{
		float velocityMult;
		float powerMult;
		float powerRegenMult;
		float impulseMult;
		float hpMult;
		float shieldMult;
		float weaponRateOfFireMult;
	};


	Display *survivorDisplay;
	Keyboard *survivorKeyboard;
	Map	*fluidMap;
	Object **objPtr;
	ForcePongBot *botController[MAX_BOTS];
	
	StringBuffer statusBuffer;
	ForcePongHighscore highScores;

	//Game Variables
	Level gameLevel;
	UserGrowth userGrowth;
	bool lose;

	bool levelup;
	float timeToLevel;

	//Objects
	ShipFluidObject *userShipObj;		//Pointer to user ship

	/////////////Functions
	void makeLevel(){
		//Make the level based on gameLevel and mapSize info
		shipCount=gameLevel.maxConcurrentEnemies+1;

		//Clear all objects except user ship
		for(int a=0;a<objCount;++a){
			if(a!=userShip&&objPtr[a]!=NULL){
				delete objPtr[a];
				objPtr[a]=NULL;
			}
		}

		//Clear bots
		for(int a=0;a<MAX_BOTS;++a){
			if(botController[a]!=NULL){
				delete botController[a];
				botController[a]=NULL;
			}
		}

		//////////////Init Enemy ships
		int *enemyTeams=new int[gameLevel.maxConcurrentEnemies];
		VectorT *enemyColors=new VectorT[gameLevel.maxConcurrentEnemies];
		int levelType=gameLevel.level%4;
		if(levelType==2){//ffa
			for(int a=0;a<gameLevel.maxConcurrentEnemies;++a){
				enemyTeams[a]=a+1;
				enemyColors[a]=randomColor();
			}
		}else if(levelType==3){//team battle
			for(int a=0;a<gameLevel.maxConcurrentEnemies;++a){
				if(a<gameLevel.maxConcurrentEnemies/2){
					enemyTeams[a]=0;
					enemyColors[a]=userShipObj->color;
				}else{
					enemyTeams[a]=1;
					enemyColors[a]=-userShipObj->color+1;
				}
			}
		}else{//melee
			for(int a=0;a<gameLevel.maxConcurrentEnemies;++a){
				enemyTeams[a]=1;
				enemyColors[a]=-userShipObj->color+1;
			}
		}


		int *enemyLives=new int[gameLevel.maxConcurrentEnemies];
		memset(enemyLives,0,sizeof(int)*gameLevel.maxConcurrentEnemies);
		for(int a=0;a<gameLevel.maxEnemies;++a) enemyLives[a%gameLevel.maxConcurrentEnemies]++;

		for(int a=0;a<gameLevel.maxConcurrentEnemies;++a){	
			int nextOpenIndex=userShip+1+a;
			float randSizeMult=1;
			randSizeMult+=rand()%1000/1000.0f*(gameLevel.level%4)*.5-.25;
			objPtr[nextOpenIndex]=new ShipFluidObject(nextOpenIndex,enemyTeams[a],enemyColors[a],VectorT(rand()%(defaultDisplay.windowWidth),rand()%(defaultDisplay.windowHeight),0),
												DEFAULT_SHIP_RADIUS*randSizeMult,DEFAULT_SHIP_MASS*randSizeMult,
												250,
												gameLevel.maxShipVelocity,gameLevel.maxShipPower,gameLevel.maxShipPowerRegenRate,
												0,0,
												NULL,NULL,
												gameLevel.maxShipImpulse,
												gameLevel.maxShipHp*randSizeMult,
												gameLevel.maxShipShields*randSizeMult,
												enemyLives[a],
												fluidMap,objPtr,&objPtr[nextOpenIndex],NULL);
			

			int botType=((rand()%2)==0?TYPE_BOT_LASER:TYPE_BOT_MISSILE);

			//Roll Weapons
			bool weaponTaken[MAX_WEAPON_TYPES];
			for(int b=0;b<MAX_WEAPON_TYPES;++b) weaponTaken[b]=false;
			
			for(int tWeap=0;tWeap<gameLevel.maxWeapons;++tWeap){
				for(int b=0,c=rand()%(MAX_WEAPON_TYPES/2);b<(MAX_WEAPON_TYPES/2);++b){
					int d;
					if(botType==TYPE_BOT_LASER)			d=(c+b)%(MAX_WEAPON_TYPES/2);
					else if(botType==TYPE_BOT_MISSILE)  d=((c+b)%(MAX_WEAPON_TYPES/2))+(MAX_WEAPON_TYPES/2);

					if(!weaponTaken[d]&&gameLevel.maxWeaponLevels[d]>0){//Weapon not taken so add
						weaponTaken[d]=true;
						//Add weapon
						int weaponType;
						switch(d){
							case 0: weaponType=TYPE_LASER;				break;
							case 1: weaponType=TYPE_LASER_TRACK;		break;
							case 2: weaponType=TYPE_LASER_TELEPORT;		break;
							case 3: weaponType=TYPE_LASER_GENESIS;		break;
							case 4: weaponType=TYPE_MISSILE;			break;
							case 5: weaponType=TYPE_MISSILE_TORQUE;		break;
							case 6: weaponType=TYPE_MISSILE_HOME;		break;
							case 7: weaponType=TYPE_MISSILE_CHAOS;		break;
						}
						makeAndAddWeapon((ShipFluidObject*)objPtr[nextOpenIndex],nextOpenIndex,(botType==TYPE_BOT_LASER?TYPE_OBJ_LASER:TYPE_OBJ_MISSILE),weaponType,gameLevel.maxWeaponLevels[d],enemyTeams[a],randSizeMult,1.0/randSizeMult);
						break;
					}
				}
			}
			botController[a]=new ForcePongBot(nextOpenIndex,botType,(rand()%1000)/1000.0f*.25+.5,(ShipFluidObject*)objPtr[nextOpenIndex],objPtr,&botController[a]);
		}
		delete [] enemyColors;
		delete [] enemyTeams;
		delete [] enemyLives;
	}

	void levelUp(){
		/*
			Difficulty scaling:

			Even level:		+max HP/Shields
			Odd Level:		+max Power/Impulse
			
			2x level:		+random weapon level
			3x level:		+max enemy weapon
			4x level:		+power regen
			5x level:		+rate of fire,+life
			6x level:		+max enemy total
			7x level:		+max concurrent enemies
			8x level:		+mapsize

			4 Level cycles:
			1-2th level: Melee
			3rd level: FFA
			4th level: Teams

			//note: All weapons accessible by you is accessible by enemies

		*/

		/////////////Difficulty increases
		//Set user multipliers
		userGrowth.hpMult=userGrowth.shieldMult=userGrowth.impulseMult=userGrowth.powerMult=gameLevel.maxConcurrentEnemies*.5;

		//Game mode string text
		int gameCycle=gameLevel.level%4;
		if(gameCycle==2) 		statusBuffer.addString("Free For All",SURVIVOR_TEXT_TIME,VectorT(),.125,-userShipObj->color+1);
		else if(gameCycle==3) 	statusBuffer.addString("Team Battle",SURVIVOR_TEXT_TIME,VectorT(),.125,userShipObj->color+1);
		else					statusBuffer.addString("Melee",SURVIVOR_TEXT_TIME,VectorT(),.125,userShipObj->color+1);


		//Basic stat raise
		if(gameLevel.level%2==0){
			//Even level
			statusBuffer.addString("+max HP/Shields",SURVIVOR_TEXT_TIME,VectorT(),.1,userShipObj->color);
			gameLevel.maxShipHp+=.05;
			userShipObj->addMaxHp(.05*userGrowth.hpMult);
			gameLevel.maxShipShields+=.1;
			userShipObj->addMaxShields(.1*userGrowth.shieldMult);
		
		}else{
			//Odd levels
			statusBuffer.addString("+max Power/Impulse",SURVIVOR_TEXT_TIME,VectorT(),.1,userShipObj->color);
			gameLevel.maxShipPower+=.05;
			userShipObj->addMaxPower(.05*userGrowth.powerMult);
			gameLevel.maxShipImpulse+=.05;
			userShipObj->addMaxImpulse(.05*userGrowth.impulseMult);
		}
		
		//Weapon level raise
		if(gameLevel.level%2==0){//Every 2x level, increase max waepon level
			statusBuffer.addString("+weapon",SURVIVOR_TEXT_TIME,VectorT(),.1,userShipObj->color);
			int randwBase=rand()%MAX_WEAPON_TYPES;
			for(int a=0;a<MAX_WEAPON_TYPES;++a){
				int wNumber=(randwBase+a)%MAX_WEAPON_TYPES;
				if(gameLevel.maxWeaponLevels[wNumber]<SUB_WEAPON_MULT){
					//Increase weapon read level
					++gameLevel.maxWeaponLevels[wNumber];
					//Increase user ship weapon level
					WeaponObject *wObj;
					if(wNumber<4){
						wObj=userShipObj->findWeaponType(TYPE_OBJ_LASER,wNumber*SUB_WEAPON_MULT);
						if(wObj!=NULL){
							//Raise existing weapon level
							wObj->highestWeaponLevel+=1;
						}else{
							//Add new weapon
							makeAndAddWeapon(userShipObj,userShip,TYPE_OBJ_LASER,wNumber*SUB_WEAPON_MULT,1,userShipObj->teamID,1,1);
						}
					}else if(wNumber<MAX_WEAPON_TYPES){
						wObj=userShipObj->findWeaponType(TYPE_OBJ_MISSILE,wNumber*SUB_WEAPON_MULT);	
						if(wObj!=NULL){
							//Raise existing weapon level
							wObj->highestWeaponLevel+=1;
						}else{
							//Add new weapon
							makeAndAddWeapon(userShipObj,userShip,TYPE_OBJ_MISSILE,wNumber*SUB_WEAPON_MULT,1,userShipObj->teamID,1,1);
						}
					}
					break;
				}
			}
		}
	
		//level max holding weapons
		if(gameLevel.level%3==0){
			statusBuffer.addString("+max enemy weapons",SURVIVOR_TEXT_TIME,VectorT(),.1,userShipObj->color);
			gameLevel.maxWeapons++;
		}
		
		//level ship power regen
		if(gameLevel.level%4==0){
			statusBuffer.addString("+max power regen",SURVIVOR_TEXT_TIME,VectorT(),.1,userShipObj->color);
			gameLevel.maxShipPowerRegenRate+=.002;
			userShipObj->addShipPowerRegenRate(.002*userGrowth.powerRegenMult);
		}

		//level rate of fire
		//level life
		if(gameLevel.level%5==0){	
			gameLevel.weaponRateOfFireMult+=.15;
			for(int a=0;a<userShipObj->getMaxLasers();++a){
				userShipObj->laserWeapons[a]->multRateOfFire(gameLevel.weaponRateOfFireMult*userGrowth.weaponRateOfFireMult);
			}
			for(int a=0;a<userShipObj->getMaxMissiles();++a){
				userShipObj->missileWeapons[a]->multRateOfFire(gameLevel.weaponRateOfFireMult*userGrowth.weaponRateOfFireMult);
			}
			userShipObj->addLife(1);
		}
		
		//levle total enemies per level
		if(gameLevel.level%6==0){
			statusBuffer.addString("+total enemies",SURVIVOR_TEXT_TIME,VectorT(),.1,userShipObj->color);
			gameLevel.maxEnemies++;	
		}
		
		//level total concurrent enemies
		if(gameLevel.level%7==0){
			statusBuffer.addString("+concurrent enemies",SURVIVOR_TEXT_TIME,VectorT(),.1,userShipObj->color);
			gameLevel.maxConcurrentEnemies++;
		}

		//enlarge mapsize
		if(gameLevel.level%8==0){
			userShipObj->resizeToGlobalScales(globalScale*.85,globalScale);
			globalScale*=.95;
			statusBuffer.addString("+mapsize",SURVIVOR_TEXT_TIME,VectorT(),.1,userShipObj->color);
		}

		//recover ship hp
		userShipObj->addHp((userShipObj->getMaxHp()-userShipObj->getHp())*.25);

	}

	WeaponObject* makeAndAddWeapon(ShipFluidObject *ship,int objID,int weaponClassType,int weaponType,int weaponLevel,int team,float weaponDmgBaseMult, float weaponRateBaseMult){
		
		WeaponObject *cWeapon;
		
		if(weaponClassType==TYPE_OBJ_LASER){
		
			switch(weaponType/SUB_WEAPON_MULT){
				case 0:
				cWeapon=new WeaponObject(0,TYPE_LASER,team,weaponLevel,ship->color,ship->position,
											ship->radius,1,
											.06*gameLevel.weaponPowerMult,.3*gameLevel.weaponDmgMult*weaponDmgBaseMult,.08*gameLevel.weaponRateOfFireMult*weaponRateBaseMult,3.0,
											ship->getPowerPtr(),ship->getMaxPowerPtr(),ship->getPowerRegenPtr(),&ship->position,&ship->directionTheta,ship->getShipTargetVectorTPtr(),
											fluidMap,objPtr,&objPtr[objID]);
				break;
				case 1:
				cWeapon=new WeaponObject(0,TYPE_LASER_TRACK,team,weaponLevel,ship->color,ship->position,
											ship->radius,1,
											.2*gameLevel.weaponPowerMult,.15*gameLevel.weaponDmgMult*weaponDmgBaseMult,.025*gameLevel.weaponRateOfFireMult*weaponRateBaseMult,4.0,
											ship->getPowerPtr(),ship->getMaxPowerPtr(),ship->getPowerRegenPtr(),&ship->position,&ship->directionTheta,ship->getShipTargetVectorTPtr(),
											fluidMap,objPtr,&objPtr[objID]);			
				break;
				case 2:
				cWeapon=new WeaponObject(0,TYPE_LASER_TELEPORT,team,weaponLevel,ship->color,ship->position,
											ship->radius,1,
											.4*gameLevel.weaponPowerMult,.1875*gameLevel.weaponDmgMult*weaponDmgBaseMult,.004*gameLevel.weaponRateOfFireMult*weaponRateBaseMult,2.0,
											ship->getPowerPtr(),ship->getMaxPowerPtr(),ship->getPowerRegenPtr(),&ship->position,&ship->directionTheta,ship->getShipTargetVectorTPtr(),
											fluidMap,objPtr,&objPtr[objID]);			
				break;
				case 3:
				cWeapon=new WeaponObject(0,TYPE_LASER_GENESIS,team,weaponLevel,ship->color,ship->position,
											ship->radius,1,
											1.0f*gameLevel.weaponPowerMult,.0015*gameLevel.weaponDmgMult*weaponDmgBaseMult,.0006*gameLevel.weaponRateOfFireMult*weaponRateBaseMult,1.5,
											ship->getPowerPtr(),ship->getMaxPowerPtr(),ship->getPowerRegenPtr(),&ship->position,&ship->directionTheta,ship->getShipTargetVectorTPtr(),
											fluidMap,objPtr,&objPtr[objID]);				
			};
		}else if(weaponClassType==TYPE_OBJ_MISSILE){
			switch(weaponType/SUB_WEAPON_MULT){
				case 4:
				cWeapon=new WeaponObject(0,TYPE_MISSILE,team,weaponLevel,ship->color,ship->position,
											ship->radius,1,
											.16*gameLevel.weaponPowerMult,.3*gameLevel.weaponDmgMult*weaponDmgBaseMult,.02*gameLevel.weaponRateOfFireMult*weaponRateBaseMult,1.75,
											ship->getPowerPtr(),ship->getMaxPowerPtr(),ship->getPowerRegenPtr(),&ship->position,&ship->directionTheta,ship->getShipTargetVectorTPtr(),
											fluidMap,objPtr,&objPtr[objID]);
				break;
				case 5:
				cWeapon=new WeaponObject(0,TYPE_MISSILE_TORQUE,team,weaponLevel,ship->color,ship->position,
											ship->radius,1,
											.3*gameLevel.weaponPowerMult,.17*gameLevel.weaponDmgMult*weaponDmgBaseMult,.06*gameLevel.weaponRateOfFireMult*weaponRateBaseMult,6,
											ship->getPowerPtr(),ship->getMaxPowerPtr(),ship->getPowerRegenPtr(),&ship->position,&ship->directionTheta,ship->getShipTargetVectorTPtr(),
											fluidMap,objPtr,&objPtr[objID]);
				break;
				case 6:
				cWeapon=new WeaponObject(0,TYPE_MISSILE_HOME,team,weaponLevel,ship->color,ship->position,
											ship->radius,1,
											.4*gameLevel.weaponPowerMult,.275*gameLevel.weaponDmgMult*weaponDmgBaseMult,.0017*gameLevel.weaponRateOfFireMult*weaponRateBaseMult,12,
											ship->getPowerPtr(),ship->getMaxPowerPtr(),ship->getPowerRegenPtr(),&ship->position,&ship->directionTheta,ship->getShipTargetVectorTPtr(),
											fluidMap,objPtr,&objPtr[objID]);
				break;
				case 7:
				cWeapon=new WeaponObject(0,TYPE_MISSILE_CHAOS,team,weaponLevel,ship->color,ship->position,
											ship->radius,1,
											1.0*gameLevel.weaponPowerMult,.375*gameLevel.weaponDmgMult*weaponDmgBaseMult,.0008*gameLevel.weaponRateOfFireMult*weaponRateBaseMult,1.0f,
											ship->getPowerPtr(),ship->getMaxPowerPtr(),ship->getPowerRegenPtr(),&ship->position,&ship->directionTheta,ship->getShipTargetVectorTPtr(),
											fluidMap,objPtr,&objPtr[objID]);
				break;
			};
		}
		
		
		ship->addWeapon(cWeapon,weaponClassType);
		return cWeapon;
	}

public:
	
	/////////////Functions
	ForcePongSurvivor(){

	}

	ForcePongSurvivor(Display *survivorDisplay,Keyboard *survivorKeyboard,Map *fluidMap,Object **objPtr){
		this->survivorDisplay=survivorDisplay;
		this->survivorKeyboard=survivorKeyboard;
		this->fluidMap=fluidMap;
		this->objPtr=objPtr;

		for(int a=0;a<MAX_BOTS;++a) botController[a]=NULL;
		
		userShipObj=NULL;

		statusBuffer.setLock(true,survivorDisplay->windowWidth/2-20,
								(survivorDisplay->windowHeight/2+50));
		highScores=ForcePongHighscore(survivorDisplay,survivorKeyboard);

	}

	~ForcePongSurvivor(){
	}

	void makeGame(){
		//Init stuff
		globalScale=.7;
		lose=false;

		//Clear objs from game
		for(int a=0;a<objCount;++a){
			if(objPtr[a]!=NULL){
				delete objPtr[a];
				objPtr[a]=NULL;	
			}
		}

		//Initialize user growth mults
		userGrowth.velocityMult=1.0;
		userGrowth.powerMult=1.0;
		userGrowth.powerRegenMult=1.5;
		userGrowth.impulseMult=1.0;
		userGrowth.hpMult=10.0;
		userGrowth.shieldMult=15.0;
		userGrowth.weaponRateOfFireMult=1.5;

		//Initialize base level info
		gameLevel.level=1;
		gameLevel.maxShipHp=.05;
		gameLevel.maxShipShields=.05;
		gameLevel.maxShipPower=.4;
		gameLevel.maxShipImpulse=.5;
		gameLevel.maxShipPowerRegenRate=.002;
		gameLevel.maxShipVelocity=1.0;
		gameLevel.weaponRateOfFireMult=.5;
		gameLevel.weaponDmgMult=.25;
		gameLevel.weaponPowerMult=1.0;

		//Initialize basic weapons to laser and missile
		for(int a=0;a<MAX_WEAPON_TYPES;++a) gameLevel.maxWeaponLevels[a]=0;
		gameLevel.maxWeaponLevels[0]=1;
		gameLevel.maxWeaponLevels[4]=1;
		gameLevel.maxWeapons=1;

		gameLevel.maxConcurrentEnemies=2;
		gameLevel.maxEnemies=2;

		//Seed level 1
		levelup=true;
		timeToLevel=LEVELUPTIME;

		//Make User ship and weapons
		userGrowth.velocityMult=1.0;
		userGrowth.powerMult=1.1;
		userGrowth.powerRegenMult=1.0;
		userGrowth.impulseMult=1.0;
		userGrowth.hpMult=3.0;
		userGrowth.shieldMult=5.0;

		userShip=0;
		shipCount=gameLevel.maxConcurrentEnemies;
		Model *sModel=&globalModels[MODEL_SHIP_DEFAULT];
		objPtr[userShip]=userShipObj=new ShipFluidObject(userShip,0,generatePartialRandomColor(),VectorT(rand()%(defaultDisplay.windowWidth),rand()%(defaultDisplay.windowHeight),0),
										DEFAULT_SHIP_RADIUS,DEFAULT_SHIP_MASS,
										250,
										gameLevel.maxShipVelocity*userGrowth.velocityMult,gameLevel.maxShipPower*userGrowth.powerMult,gameLevel.maxShipPowerRegenRate*userGrowth.powerRegenMult,
										0,0,
										NULL,NULL,
										gameLevel.maxShipImpulse*userGrowth.impulseMult,
										.5,
										.5,
										7,
										fluidMap,objPtr,&objPtr[userShip],sModel);

		WeaponObject *tmpWeapon=makeAndAddWeapon(userShipObj,userShip,TYPE_OBJ_LASER,TYPE_LASER,1,0,1,1);
		WeaponObject *tmpWeapon2=makeAndAddWeapon(userShipObj,userShip,TYPE_OBJ_MISSILE,TYPE_MISSILE,1,0,1,1);
		userShipObj->setActiveWeapon(tmpWeapon2);
		userShipObj->setActiveWeapon(tmpWeapon);
		//Set target to self
		userTarget=userShip;
		userShipObj->setTarget(((ShipFluidObject*)(objPtr[userTarget])));

	}

	VectorT generatePartialRandomColor(){
		float p1=rand()%1000/1000.0f*.25+.75;
		float p2=rand()%1000/1000.0f*.25+.25;
		float p3=rand()%1000/1000.0f*.25;
		int p=rand()%6;
		VectorT c;
		if(p==0)		return VectorT(p1,p2,p3);
		else if(p==1)	return VectorT(p1,p3,p2);
		else if(p==2)	return VectorT(p2,p1,p3);
		else if(p==3)	return VectorT(p2,p3,p1);
		else if(p==4)	return VectorT(p3,p1,p2);
		else 			return VectorT(p3,p2,p1);
	}
	
	void update(){
		
		//Check for levelup
		FluidObject *f;
		int s=0;
		for(;s<shipCount;++s){
			f=(FluidObject*)objPtr[s];
			if(f!=NULL&&f->objType==TYPE_OBJ_SHIP){
				if(f->teamID!=0&&((ShipFluidObject*)f)->getLives()>0)
				break;
			}
		}
		if(!levelup&&s==shipCount){
			levelup=true;
			gameLevel.level++;
		}

		//Update time for levelup
		if(levelup){
			if(timeToLevel==LEVELUPTIME){
				levelUp();
			}
			if(timeToLevel<=0){
				levelup=false;
				timeToLevel=LEVELUPTIME;
				//Make level
				makeLevel();
			}else --timeToLevel;
		}

		//Check user loss status
		if(!lose&&userShipObj->getLives()<=0){
			//Lost
			highScores.readScores("survivorHighscores");
			highScores.insertScore(gameLevel.level);
			userShipObj->addLife(INT_MAX);
			observerMode=true;
			lose=true;
		}
	
		//Enter highscore
		if(lose){
			highScores.enterScore();
		}

		//Update string buffer
		statusBuffer.update();

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
	

		//Update bots
		for(int a=0;a<shipCount;++a){
			if(botController[a]!=NULL){
				botController[a]->update();
			}
		}

	}

	void display(){
		//Draw level info
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		string lvlN=numberToString(gameLevel.level);
		string lvl="Lvl ";
		lvl.append(lvlN);
		survivorDisplay->displayString(lvl,900*survivorDisplay->windowScaleX,75,.1,NULL);

		if(!observerMode){
			string livesN=numberToString(userShipObj->getLives());
			string lives="Lives ";
			lives.append(livesN);
			survivorDisplay->displayString(lives,900*survivorDisplay->windowScaleX,100,.1,NULL);
		}

		if(levelup){
			string s="Level ";
			s.append(lvlN);
			glColor4f(userShipObj->color.x,userShipObj->color.y,userShipObj->color.z,(float)(timeToLevel)/LEVELUPTIME);
			survivorDisplay->displayString(s,survivorDisplay->windowWidth/2-100,survivorDisplay->windowHeight/2,.5,NULL);
		}
		statusBuffer.show();

		if(lose){
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(userShipObj->color.x,userShipObj->color.y,userShipObj->color.z,1);	
			highScores.display();
		}


	}

	void resize(VectorT windowDims,VectorT oldDims){
			statusBuffer.setLock(true,windowDims.x/2-20,
								windowDims.y/2+50);
	}

	void cleanup(){
		for(int a=0;a<MAX_BOTS;++a){
			if(botController[a]!=NULL){
				delete botController[a];
				botController[a]=NULL;
			}
		}

		statusBuffer.cleanup();

	}

	
};

#endif