#ifndef _SHIPFLUIDOBJECT_
#define _SHIPFLUIDOBJECT_

#include <Math.h>

#include "Constants.h"
#include "ConstantsIO.h"
#include "Conversion.h"
#include "VectorT.h"
#include "Display.h"
#include "Model.h"

#include "ForcePongConstants.h"
#include "FluidObject.h"
#include "WeaponObject.h"

/*
Must be dynamic
*/
class ShipFluidObject:public FluidObject{
protected:

	bool accelerating;
	bool raccelerating;
	bool sideLAccelerating;
	bool sideRAccelerating;
	bool sideUAccelerating;
	bool sideDAccelerating;

	int lives;

	bool impulseReady;

	int maxSpawnTime;//0=alive, >0=remaining spawn time
	float maxVelocity;
	float maxPower;
	float powerRegenRate;
	int maxLasers,maxMissiles;
	float maxHitPoints,lastHitPoints;
	float maxImpulse;
	float maxShield;

	float spawnTime;
	float power;
	float impulse;
	float shield;
	float hpFlash;

	VectorT impulseOrigin;

	FluidObject *shipTarget;
	VectorT shipTargetVectorT;
	WeaponObject *activeWeapon;
	WeaponObject *lastWeaponType;

public:
	WeaponObject **laserWeapons,**missileWeapons;
	int score;

	//Constructors

	ShipFluidObject(){
	}

	ShipFluidObject(int objID,int teamID, VectorT color,VectorT position,
		float radius,float mass,
		int maxSpawnTime,
		float maxVelocity,float maxPower,float powerRegenRate,
		int maxLasers,int maxMissiles,
		float maxImpulse,
		float maxHitPoints,
		float maxShield,
		int lives,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr, Model *model)
		:FluidObject(objID,teamID,TYPE_OBJ_SHIP,color,position,VectorT(0,0,0),radius,mass,maxHitPoints,
		mapPtr,firstElementPtr,currentElementPtr){
		//Make ship with default weapons

		this->maxSpawnTime=maxSpawnTime;
		this->maxVelocity=maxVelocity;
		this->maxPower=power=maxPower;
		this->powerRegenRate=powerRegenRate;
		this->maxLasers=maxLasers;
		this->maxMissiles=maxMissiles;
		this->maxImpulse=impulse=maxImpulse;
		this->maxHitPoints=hitPoints=maxHitPoints;
		this->maxShield=shield=maxShield;
		this->model=model;
		this->lives=lives;
		
		shipTarget=this;
		shipTargetVectorT=NULL;
		spawnTime=0;
		hpFlash=0;
		impulse=0;

		accelerating=false;
		raccelerating=false;

		sideLAccelerating=false;
		sideRAccelerating=false;
		sideUAccelerating=false;
		sideDAccelerating=false;


		impulseReady=true;

		score=0;

		//Initialize Weapons
		initializeAllWeapons();
		activeWeapon=laserWeapons[0];
		lastWeaponType=laserWeapons[0];
		respawn();
	}

	ShipFluidObject(int objID,int teamID, VectorT color,VectorT position,
		float radius,float mass,
		int maxSpawnTime,
		float maxVelocity,float maxPower,float powerRegenRate,
		int maxRandomLasers,int maxRandomMissiles,bool supersOn,
		float maxImpulse,
		float maxHitPoints,
		float maxShield,
		int lives,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr, Model *model)
		:FluidObject(objID,teamID,TYPE_OBJ_SHIP,color,position,VectorT(0,0,0),radius,mass,maxHitPoints,
		mapPtr,firstElementPtr,currentElementPtr){
		//Make ship with default weapons

		this->maxSpawnTime=maxSpawnTime;
		this->maxVelocity=maxVelocity;
		this->maxPower=power=maxPower;
		this->powerRegenRate=powerRegenRate;
		this->maxLasers=maxRandomLasers;
		this->maxMissiles=maxRandomMissiles;
		this->maxImpulse=impulse=maxImpulse;
		this->maxHitPoints=hitPoints=maxHitPoints;
		this->maxShield=shield=maxShield;
		this->model=model;
		this->lives=lives;
		
		shipTarget=this;
		shipTargetVectorT=NULL;
		spawnTime=0;
		hpFlash=0;
		impulse=0;

		accelerating=false;
		raccelerating=false;

		sideLAccelerating=false;
		sideRAccelerating=false;
		sideUAccelerating=false;
		sideDAccelerating=false;

		impulseReady=true;

		score=0;

		//Initialize Weapons
		int weaponTypes=3;
		laserWeapons=new WeaponObject*[maxLasers];
		missileWeapons=new WeaponObject*[maxMissiles];
		if(supersOn) weaponTypes=4;

		for(int a=0;a<maxLasers;++a){
			initializeLaser(rand()%weaponTypes,laserWeapons,a);
			if(activeWeapon!=NULL) lastWeaponType=activeWeapon; 
			activeWeapon=laserWeapons[a];
		}

		for(int a=0;a<maxMissiles;++a){
			initializeMissile(rand()%weaponTypes,missileWeapons,a);
			if(activeWeapon!=NULL) lastWeaponType=activeWeapon; 
			activeWeapon=missileWeapons[a];
		}

		respawn();
	}


	ShipFluidObject(int objID,int teamID, VectorT color,VectorT position,
		float radius,float mass,
		int maxSpawnTime,
		float maxVelocity,float maxPower,float powerRegenRate,
		int maxLasers,int maxMissiles,
		WeaponObject *laserWeaponObjects, WeaponObject *missileWeaponObjects,
		float maxImpulse,
		float maxHitPoints,
		float maxShield,
		int lives,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr, Model *model)
		:FluidObject(objID,teamID,TYPE_OBJ_SHIP,color,position,VectorT(0,0,0),radius,mass,maxHitPoints,
		mapPtr,firstElementPtr,currentElementPtr){
		//Make ship with specific weapons

		this->maxSpawnTime=maxSpawnTime;
		this->maxVelocity=maxVelocity;
		this->maxPower=power=maxPower;
		this->powerRegenRate=powerRegenRate;
		this->maxLasers=maxLasers;
		this->maxMissiles=maxMissiles;
		this->maxImpulse=impulse=maxImpulse;
		this->maxHitPoints=hitPoints=maxHitPoints;
		this->maxShield=shield=maxShield;
		this->model=model;
		this->lives=lives;
		
		shipTarget=this;
		shipTargetVectorT=NULL;
		spawnTime=0;
		hpFlash=0;
		impulse=0;

		accelerating=false;
		raccelerating=false;
		impulseReady=true;

		score=0;

		laserWeapons=new WeaponObject*[maxLasers];
		for(int a=0;a<maxLasers;++a){ 
			laserWeapons[a]=&laserWeaponObjects[a];
		}

		missileWeapons=new WeaponObject*[maxMissiles];
		for(int a=0;a<maxMissiles;++a){ 
			missileWeapons[a]=&missileWeaponObjects[a];
		}
		activeWeapon=NULL;
		lastWeaponType=NULL;

		respawn();
	}


	 ~ShipFluidObject(){
		for(int a=0;a<maxLasers;++a) {
			if(laserWeapons[a]!=NULL) delete laserWeapons[a];
		}
		delete []laserWeapons;
		for(int a=0;a<maxMissiles;++a) {
			if(missileWeapons[a]!=NULL) delete missileWeapons[a];
		}
		delete []missileWeapons;

	}

	/*
	Arguments:
		WeaponObject *weapon:	pointer to weapon object
		int weaponType:			TYPE_OBJ_LASER, TYPE_OBJ_MISSILE	
	*/
	void addWeapon(WeaponObject *weapon, int weaponType){
		if(weaponType==TYPE_OBJ_LASER){
			WeaponObject **tmp=new WeaponObject*[maxLasers+1];
			for(int a=0;a<maxLasers;++a) tmp[a]=laserWeapons[a];
			tmp[maxLasers++]=weapon;
			delete [] laserWeapons;
			laserWeapons=tmp;
		}else if(TYPE_OBJ_MISSILE){
			WeaponObject **tmp=new WeaponObject*[maxMissiles+1];
			for(int a=0;a<maxMissiles;++a) tmp[a]=missileWeapons[a];
			tmp[maxMissiles++]=weapon;
			delete [] missileWeapons;
			missileWeapons=tmp;
		}
	}

	 void initializeAllWeapons(){
		
		initializeAllLasers();
		initializeAllMissiles();

		setActiveWeapon(findWeaponType(TYPE_OBJ_LASER,TYPE_LASER));
	}
	
	 void initializeAllLasers(){

		laserWeapons=new WeaponObject*[maxLasers];
		for(int a=0;a<maxLasers;++a){ 
			initializeLaser(a,laserWeapons,a);
		}
	}

	void initializeLaser(int id,WeaponObject **weapon,int index){
		int a=id;
 		switch(a%4){
			case 0:
			weapon[index]=new WeaponObject(a,TYPE_LASER,teamID,3,color,VectorT(rand()%(defaultDisplay.windowWidth),rand()%(defaultDisplay.windowHeight),0),
											radius,1,
											.06,.1,.1,3.0,
											&power,&maxPower,&powerRegenRate,&(this->position),&directionTheta,&shipTargetVectorT,
											mapPtr,firstElementPtr,currentElementPtr);
			break;
			case 1:
			weapon[index]=new WeaponObject(a,TYPE_LASER_TRACK,teamID,3,color,VectorT(rand()%(defaultDisplay.windowWidth),rand()%(defaultDisplay.windowHeight),0),
										radius,1,
										.2,.15,.05,4.0,
										&power,&maxPower,&powerRegenRate,&(this->position),&directionTheta,&shipTargetVectorT,
										mapPtr,firstElementPtr,currentElementPtr);				
			break;
			case 2:
			weapon[index]=new WeaponObject(a,TYPE_LASER_TELEPORT,teamID,3,color,VectorT(rand()%(defaultDisplay.windowWidth),rand()%(defaultDisplay.windowHeight),0),
										radius,1,
										.4,.1875,.004,2.0,
										&power,&maxPower,&powerRegenRate,&(this->position),&directionTheta,&shipTargetVectorT,
										mapPtr,firstElementPtr,currentElementPtr);				
			break;
			case 3:
			weapon[index]=new WeaponObject(a,TYPE_LASER_GENESIS,teamID,3,color,VectorT(rand()%(defaultDisplay.windowWidth),rand()%(defaultDisplay.windowHeight),0),
										radius,1,
										1.0f,.0015,.0006,1.5,
										&power,&maxPower,&powerRegenRate,&(this->position),&directionTheta,&shipTargetVectorT,
										mapPtr,firstElementPtr,currentElementPtr);				
			break;
		};
	
	 }

	 void initializeAllMissiles(){

		missileWeapons=new WeaponObject*[maxMissiles];

		for(int a=0;a<maxMissiles;++a){ 
			initializeMissile(a,missileWeapons,a);
		}
	}

	void initializeMissile(int id,WeaponObject **weapon,int index){
		int a=id;
		switch(a%4){
			case 0:
			weapon[index]=new WeaponObject(a,TYPE_MISSILE,teamID,3,color,VectorT(rand()%(defaultDisplay.windowWidth),rand()%(defaultDisplay.windowHeight),0),
											radius,1,
											.12,.125,.05,1.75,
											&power,&maxPower,&powerRegenRate,&(this->position),&directionTheta,&shipTargetVectorT,
											mapPtr,firstElementPtr,currentElementPtr);
			break;
			case 1:
			weapon[index]=new WeaponObject(a,TYPE_MISSILE_TORQUE,teamID,3,color,VectorT(rand()%(defaultDisplay.windowWidth),rand()%(defaultDisplay.windowHeight),0),
											radius,1,
											.3,.23,.06,6,
											&power,&maxPower,&powerRegenRate,&(this->position),&directionTheta,&shipTargetVectorT,
											mapPtr,firstElementPtr,currentElementPtr);
			break;
			case 2:
			weapon[index]=new WeaponObject(a,TYPE_MISSILE_HOME,teamID,3,color,VectorT(rand()%(defaultDisplay.windowWidth),rand()%(defaultDisplay.windowHeight),0),
											radius,1,
											.35,.275,.0075,12,
											&power,&maxPower,&powerRegenRate,&(this->position),&directionTheta,&shipTargetVectorT,
											mapPtr,firstElementPtr,currentElementPtr);
			break;
			case 3:
			weapon[index]=new WeaponObject(a,TYPE_MISSILE_CHAOS,teamID,3,color,VectorT(rand()%(defaultDisplay.windowWidth),rand()%(defaultDisplay.windowHeight),0),
											radius,1,
											1.0,.375,.0008,1.0f,
											&power,&maxPower,&powerRegenRate,&(this->position),&directionTheta,&shipTargetVectorT,
											mapPtr,firstElementPtr,currentElementPtr);
			break;
		};
	}
	 ////////////////Display functions

	 void display(){
		if(isActive){//Is alive
			displayHighlights();
			displayShip();
			displayLife();
			displayWeapons();
		}
	}

	void displayShip(){
		//Draw Ship 
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_BLEND);

		glColor3fv((GLfloat*)&color);
		glPushMatrix();

		if(model==NULL){
			glTranslatef(position.x,position.y,position.z);
			glRotatef(directionTheta,0,0,1);
			glTranslatef(-position.x,-position.y,-position.z);
		
			//Ship body
			glBegin(GL_TRIANGLE_FAN);
			glVertex2f(position.x,position.y);
			for(int a=0;a<=15;++a)
			glVertex2f(	position.x+defaultSinValues[(SIN_TABLE_SIZE/4+SIN_TABLE_SIZE/15*a)%SIN_TABLE_SIZE]*radius,
						position.y+defaultSinValues[SIN_TABLE_SIZE/15*a]*radius);
			glVertex2f(	position.x+defaultSinValues[SIN_TABLE_SIZE/4]*radius,
						position.y+defaultSinValues[0]*radius);
			glEnd();

			//Ship pointer
			glColor3fv((GLfloat*)&(color*.5));
			glBegin(GL_LINES);
			glVertex3f(position.x,position.y,position.z);
			glVertex3f(position.x+radius*2,position.y,position.z);
			glEnd();
		}else{
			glTranslatef(position.x,position.y,position.z);
			glScalef(radius,radius,1);
			glRotatef(directionTheta,0,0,1);
			model->display();
		}
		glPopMatrix();

		glPopAttrib();
	}


	void displayWeapons(){
		//Draw Weapons
		for(int a=0;a<maxLasers;++a) laserWeapons[a]->display();
		for(int a=0;a<maxMissiles;++a) missileWeapons[a]->display();
	}

	void displayLife(){	
		//Draw HP/Shields

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		VectorT hpColor=valueToRedGreen(hitPoints)*.5;
		VectorT shieldColor=valueToRedGreen(shield);

		//HP
		if(userShip==objID) glColor3f(hpColor.x,hpColor.y,hpColor.z);
		else glColor4f(hpColor.x,hpColor.y,hpColor.z,minValue(hpFlash*1.5,1.0));
		glBegin(GL_LINE_LOOP);
		for(int a=0;a<=15;++a)
		glVertex2f(	position.x+defaultSinValues[(SIN_TABLE_SIZE/4+SIN_TABLE_SIZE/15*a)%SIN_TABLE_SIZE]*radius*.8,
					position.y+defaultSinValues[SIN_TABLE_SIZE/15*a]*radius*.8);
		glEnd();

		glColor4f(hpColor.x,hpColor.y,hpColor.z,hpFlash);
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(position.x,position.y);
		for(int a=0;a<=15;++a)
		glVertex2f(	position.x+defaultSinValues[(SIN_TABLE_SIZE/4+SIN_TABLE_SIZE/15*a)%SIN_TABLE_SIZE]*radius*.8,
					position.y+defaultSinValues[SIN_TABLE_SIZE/15*a]*radius*.8);
		glVertex2f(	position.x+defaultSinValues[SIN_TABLE_SIZE/4]*radius*.8,
					position.y+defaultSinValues[0]*radius*.8);
		glEnd();

		//Shield
		if(userShip==objID) glColor3f(shieldColor.x,shieldColor.y,shieldColor.z);
		else glColor4f(shieldColor.x,shieldColor.y,shieldColor.z,minValue(hpFlash*1.5,1.0));
		glBegin(GL_LINE_LOOP);
		for(int a=0;a<=15;++a)
		glVertex2f(	position.x+defaultSinValues[(SIN_TABLE_SIZE/4+SIN_TABLE_SIZE/15*a)%SIN_TABLE_SIZE]*radius*1.0,
					position.y+defaultSinValues[SIN_TABLE_SIZE/15*a]*radius*1.0);
		glEnd();

		glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	}

	void displayShipTargetLock(){
		if(shipTarget!=NULL){

			glPushAttrib(GL_ENABLE_BIT);
			glDisable(GL_BLEND);
			//Display locks on target
			displayTargetLock(shipTargetVectorT,shipTarget->radius*1.6f,VectorT(1,1,1,.05));
			displayTargetLock(shipTargetVectorT,shipTarget->radius*1.7f,VectorT(0,0,0,.05));
			glEnable(GL_BLEND);

			//Display connecting dot line between target
			VectorT position=this->position;
			VectorT position2=position;

			glColor4f(color.x*.15,color.y*.15,color.z*.15,.5);
			VectorT ray=shipTargetVectorT-position;
			VectorT ray2=ray;
			VectorT rayOrtho=(ray.cross(VectorT(0,0,1))).normalize();

			VectorT u,v,w,z;
			for(int a=0;a<40;a+=2){
				u=position+ray/40.0*a;
				v=position+ray/40.0*(a+1);
				glBegin(GL_LINES);
				glVertex2f(u.x,u.y);
				glVertex2f(v.x,v.y);
				glEnd();
			}

			glColor4f(color.x*.15+.85,color.y*.15+.85,color.z*.15+.85,.5);
			position+=rayOrtho*2;
			position2-=rayOrtho*2;
			ray=shipTargetVectorT+rayOrtho*2-position;
			ray2=shipTargetVectorT-rayOrtho*2-position2;

			for(int a=1;a<40;a+=2){
				u=position+ray/40.0*a;
				v=position+ray/40.0*(a+1);
				w=position2+ray2/40.0*a;
				z=position2+ray2/40.0*(a+1);

				glBegin(GL_LINES);
				glVertex2f(u.x,u.y);
				glVertex2f(v.x,v.y);
				glVertex2f(w.x,w.y);
				glVertex2f(z.x,z.y);
				glEnd();
			}

			glPopAttrib();
		}
	}

	void displayShipHeading(){
		if(isActive)	glColor4f(color.x,color.y,color.z,1);
		else			glColor4f(color.x,color.y,color.z,(float)(spawnTime)/maxSpawnTime);
		
		//Direction vector
		float raySize=radius*3;
		VectorT ray=direction*raySize;
		for(int a=0;a<20;a+=2){
			VectorT u=position+ray/20*a;
			VectorT v=position+ray/20*(a+1);
			glBegin(GL_LINES);
			glVertex2f(u.x,u.y);
			glVertex2f(v.x,v.y);
			glEnd();
		}
	
	}

	void displayStats(){
		const VectorT barDims=VectorT(100,10,0);
		VectorT statLoc=VectorT(20,defaultDisplay.windowHeight-100,0);
		const char statNames[][100]={"HP","Power","Shield","Impulse","Weapon","W Lvl."};
		float statValues[]={hitPoints,
							power,
							shield,
							impulse,
							(activeWeapon==NULL?0:activeWeapon->getWeaponReload()),
							(activeWeapon==NULL?0:activeWeapon->getWeaponLevel())};
		float statPercent[]={
			hitPoints/maxHitPoints,
			power/maxPower,
			shield/maxShield,
			impulse/maxImpulse,
			(activeWeapon==NULL?0:activeWeapon->getWeaponReload()),
			(activeWeapon==NULL?0:activeWeapon->getWeaponLevel())};
		for(int a=0;a<6;++a){
			glColor3fv((GLfloat*)&valueToRedGreen(statPercent[a]));
			glBegin(GL_QUADS);
			glVertex2f(50+statLoc.x,statLoc.y+a*(1.2f*barDims.y));
			glVertex2f(50+statLoc.x+barDims.x*statValues[a],statLoc.y+a*(1.2f*barDims.y));
			glVertex2f(50+statLoc.x+barDims.x*statValues[a],statLoc.y+barDims.y+a*(1.2f*barDims.y));
			glVertex2f(50+statLoc.x,statLoc.y+barDims.y+a*(1.2f*barDims.y));
			glEnd();

			defaultDisplay.displayString(statNames[a],statLoc.x,statLoc.y+(a+1)*(1.2f*barDims.y),FONT_SIZE,NULL);
		}	
	}

	void displayWeaponIcons(){
		//Show available weapons icons
		int wNumber=0;

		char c3[10];
		VectorT v;
		for(int a=0;a<maxLasers;++a,++wNumber){ 
			v=valueToRedGreen(laserWeapons[a]->getWeaponReload());
			glColor4fv((GLfloat*)&v);
			sprintf_s(c3,10,"%c-%d ",userWeaponKeys[laserWeapons[a]->type/SUB_WEAPON_MULT],laserWeapons[a]->highestWeaponLevel);
			defaultDisplay.displayString(c3,20+wNumber*30,defaultDisplay.windowHeight-120,.085,NULL);
		}
		for(int a=0;a<maxMissiles;++a,++wNumber){
			v=valueToRedGreen(missileWeapons[a]->getWeaponReload());
			glColor4fv((GLfloat*)&v);
			sprintf_s(c3,10,"%c-%d ",userWeaponKeys[missileWeapons[a]->type/SUB_WEAPON_MULT],missileWeapons[a]->highestWeaponLevel);
			defaultDisplay.displayString(c3,20+wNumber*30,defaultDisplay.windowHeight-120,.085,NULL);
		}

		
		
	}

	 void update(){

		//Update targeting vector
		shipTargetVectorT=shipTarget->position;

		//Update ship death and respawn
		if(!isActive){//Dead
			if(spawnTime>maxSpawnTime*.8){//Death animation
				VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);
				VectorT impulseColor=color+(-color+1)*((maxSpawnTime-spawnTime)/(maxSpawnTime*.1))*.9;
				float rad=fluidRadius*8*((maxSpawnTime-spawnTime)/(maxSpawnTime*.2));
				mapPtr->addImpulseToBuffer(impulsePosition,impulseColor,1.0,
					rad*.66,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);

				if(CHAOTIC_MODE) mapPtr->addImpulseToBuffer(impulsePosition,VectorT(0,0,0),1.0,
					rad,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_EXPLOSION);
	
			}
			if(spawnTime==0){//Revive
				if(lives<=0){
					(*currentElementPtr)=NULL;
					delete this;
					return;
				}
				respawn();
			}else --spawnTime;
		}

		//Update active ship
		if(isActive){//Alive

			processAcceleration();
			processCrossAcceleration();

			//Update kinetic physics
			force.z=force.w=0;
			acceleration=(force/mass);
			velocityRelative+=acceleration*(dt);
			if(velocityRelative.normSquared()<10) velocityRelative=0;

			float velRelMag=velocityRelative.normSquared();
			float percentVelRelative=minValue(velRelMag/(velocity.norm()*1.25+velRelMag),.25);

			totalVelocity=(velocity*1.25*(1-percentVelRelative)+velocityRelative*percentVelRelative)*(DEFAULT_SHIP_MASS/(mass));
			totalVelocity.z=totalVelocity.w=0;
			processBoundary();
			position+=totalVelocity*dt*globalScale;
			position.z=position.w=0;
			force=(velocityRelative)*mass*-1;//drag
			direction=VectorT(cos(directionTheta/RADIAN),sin(directionTheta/RADIAN),0);

			//Regen Impulse
			if(impulse<maxImpulse){
				if(CHAOTIC_MODE) impulse+=.002;
				else impulse+=.001;
			}else{
				impulse=maxImpulse;
				impulseReady=true;
			}

			//Regen Shield
			if(shield<maxShield) shield+=.002;
			else shield=maxShield;

			//Update weapons
			for(int a=0;a<maxLasers;++a) laserWeapons[a]->update();
			for(int a=0;a<maxMissiles;++a) missileWeapons[a]->update();

			//Update shields and hpFlash
			if(lastHitPoints>hitPoints){
				//Shield takes 10x damage
				float dmg=lastHitPoints-hitPoints;
				float f=minValue((lastHitPoints-hitPoints)*.5f*10,shield);
				shield-=f;
				hitPoints+=f/10;
				hpFlash+=(1.0-hpFlash)*.666f;
			}
			hpFlash*=.995;

			//Regenerate power
			if(power+powerRegenRate<maxPower) power+=powerRegenRate;
			else power=maxPower;

			//Check death status
			if(hitPoints<=0){
				spawnTime=maxSpawnTime;
				if(firstElementPtr[lastDmgSource]!=NULL){
					((ShipFluidObject*)(firstElementPtr[lastDmgSource]))->score++;
				}
				--lives;
				isActive=false;
			}

			//Update last values
			lastHitPoints=hitPoints;
		}

	}


	void respawn(){
		hitPoints=maxHitPoints;
		shield=maxShield;
		power=maxPower;
		impulse=maxImpulse;
		position=VectorT(radius+rand()%(defaultDisplay.windowWidth-(int)radius*2),
						radius+rand()%(defaultDisplay.windowHeight-(int)radius*2),0,0);
		velocityRelative=0;
		totalVelocity=0;
		force=0;
		//if(activeWeapon!=NULL) activeWeapon->refresh();
		//for(int a=0;a<maxLasers;++a) laserWeapons[a]->refresh();
		//for(int a=0;a<maxMissiles;++a) missileWeapons[a]->refresh();
		isActive=true;
	}

	void accelerate(){
		accelerating=true;
	}

	void raccelerate(){
		raccelerating=true;
	}

	void sideStepLeft(){
		sideLAccelerating=true;
	}

	void sideStepRight(){
		sideRAccelerating=true;
	}

	void sideStepUp(){
		sideUAccelerating=true;
	}

	void sideStepDown(){
		sideDAccelerating=true;
	}

	//Accelerating dependent on orientation of ship
	void processAcceleration(){

		bool moving=false;
		VectorT color;
		VectorT origColor;
		float p;

		if(accelerating){
			if((p=power-powerRegenRate*.1)>=0){
				power=p;
				color=(direction+1)/2;
				moving=true;
			}
			accelerating=false;
		}else if(raccelerating){
			if((p=power-powerRegenRate*.1)>=0){
				power=p;
				color=(-direction+1)/2;
				moving=true;
			}
			raccelerating=false;
		}

		if(moving){
			color.y=1.0-color.y;
			origColor=(-VectorT(direction.x,direction.y,0.0)+1)/2;

			VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);
			mapPtr->addImpulseToBuffer(impulsePosition,origColor,.2,fluidRadius*1.5,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
			mapPtr->addImpulseToBuffer(impulsePosition,color,.2,fluidRadius*1.5,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
		}
	}

	//Accelerating independent from ship
	//Up down left right
	void processCrossAcceleration(){
		bool moving=false;
		VectorT color;
		VectorT origColor;
		float p;
		
		if(sideLAccelerating){
			if((p=power-powerRegenRate*.1)>=0){
				power=p;
				color=(VectorT(-1,0)+1)/2;
				moving=true;
			}
			sideLAccelerating=false;
		}else if(sideRAccelerating){
			if((p=power-powerRegenRate*.1)>=0){
				power=p;
				color=(VectorT(1,0)+1)/2;
				moving=true;
			}
			sideRAccelerating=false;
		}

		if(sideUAccelerating){
			if((p=power-powerRegenRate*.1)>=0){
				power=p;
				if(moving) color=(color+(VectorT(0,1)+1)/2)/2;
				else color=(VectorT(0,1)+1)/2;
				moving=true;
			}
			sideUAccelerating=false;
		}else if(sideDAccelerating){
			if((p=power-powerRegenRate*.1)>=0){
				power=p;
				if(moving) color=(color+(VectorT(0,-1)+1)/2)/2;
				else color=(VectorT(0,-1)+1)/2;
				moving=true;
			}
			sideDAccelerating=false;
		}

		if(moving){
			origColor=(-VectorT(direction.x,direction.y,0.0)+1)/2;

			VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);
			mapPtr->addImpulseToBuffer(impulsePosition,origColor,.2,fluidRadius*1.5,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
			mapPtr->addImpulseToBuffer(impulsePosition,color,1,fluidRadius*1.5,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
		}

	}


	bool weaponReady(int weaponType,int ammoType){
		if(weaponType==TYPE_OBJ_LASER){
			for(int a=0;a<maxLasers;++a){
				if(laserWeapons[a]->type==ammoType){
					if(laserWeapons[a]->weaponReady){
						return true;
					}
				}
			}
		}else if(weaponType==TYPE_OBJ_MISSILE){
			for(int a=0;a<maxMissiles;++a){
				if(missileWeapons[a]->type==ammoType){
					if(missileWeapons[a]->weaponReady){
						return true;
					}
				}
			}
		}
		return false;
	}

	//Fire all lasers
	bool fireLasers(){
		bool weaponFired=false;
		if(isActive){
			float p;
			for(int a=0;a<maxLasers;++a){
				if((p=power-laserWeapons[a]->power)>=0&&laserWeapons[a]->fire()){
					power=p;
					weaponFired=true;
				}
			}
		}
		return weaponFired;
	}

	//Fire all missiles
	bool fireMissiles(){
		bool weaponFired=false;
		if(isActive){
			float p;
			for(int a=0;a<maxMissiles;++a){
				if((p=power-missileWeapons[a]->power)>=0&&missileWeapons[a]->fire()){
					power=p;
					weaponFired=true;
				}
			}
		}
		return weaponFired;
	}

	bool fireType(int ammoType){
		bool weaponFired=false;
		if(isActive){
			float p;

			for(int a=0;a<maxLasers;++a){//Find the laser
				if(laserWeapons[a]->type==ammoType){
					if(activeWeapon!=laserWeapons[a]) lastWeaponType=activeWeapon;
					activeWeapon=laserWeapons[a];
					if((p=power-laserWeapons[a]->power)>=0&&laserWeapons[a]->fire()){
						power=p;
						weaponFired=true;

					}
				}
			}

			for(int a=0;a<maxMissiles;++a){//Find the missile
				if(missileWeapons[a]->type==ammoType){
					if(activeWeapon!=missileWeapons[a]) lastWeaponType=activeWeapon;
					activeWeapon=missileWeapons[a];
					if((p=power-missileWeapons[a]->power)>=0&&missileWeapons[a]->fire()){
						power=p;
						weaponFired=true;
					}
				}
			}
		}

		return weaponFired;
	}

	bool fireActiveWeapon(){
		bool weaponFired=false;
		if(isActive){
			if(activeWeapon!=NULL){
				float p;
				if((p=power-activeWeapon->power)>=0&&activeWeapon->fire()){
					power=p;
					weaponFired=true;
				}
			}
		}
		return weaponFired;
	}

	bool fireLastWeapons(){
		bool weaponFired=false;
		if(isActive){
			if(lastWeaponType!=NULL){
				float p;
				if((p=power-lastWeaponType->power)>=0&&lastWeaponType->fire()){
					power=p;
					weaponFired=true;
				}
			}
		}
		return weaponFired;
	}


	bool fireActiveWeaponType(){
		if(activeWeapon!=NULL)
			return fireType(activeWeapon->type);
		
		return false;
	}

	void processUserImpulse(){
		if(isActive&&defaultMouse.mouseState[1]){
			
			if(impulseReady){
				float impulseFactor;

				if(CHAOTIC_MODE) impulseFactor=.035;
				else impulseFactor=.05;

				if(impulseReady){
					if(impulse>0){
						VectorT mousePosition=defaultMouse.position;
						if(mousePosition.x>=0&&mousePosition.y>=0&&mousePosition.x<defaultDisplay.windowWidth&&mousePosition.y<defaultDisplay.windowHeight){

							VectorT color=defaultMouse.distanceMoved;
							float d=color.norm();
							if(d!=0){
								color=(color/d+1.0)/2.0;
								color.y=1.0-color.y;
								VectorT relativeDiff(mousePosition.x-defaultMouse.clickOrigin[1].x,mousePosition.y-defaultMouse.clickOrigin[1].y,1);
								
								
								VectorT pos((position.x+relativeDiff.x)/defaultDisplay.windowWidth,1-(position.y+relativeDiff.y)/defaultDisplay.windowHeight,1.0);
								mapPtr->addImpulseToBuffer(pos,
									color,1.0,25,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
								mapPtr->addImpulseToBuffer(pos,
									color,1.0,25,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);

								impulse-=.04;
							}
						}
					}else{
						impulse=0;
						impulseReady=false;
					}
				}else{
					impulseReady=false;
				}
			}
		}else{
			impulseReady=false;
		}
	}

	
	WeaponObject* findWeaponType(int weaponType,int ammoType){
		switch(weaponType){
			case TYPE_OBJ_LASER:
			for(int a=0;a<maxLasers;++a){
				if(laserWeapons[a]->type==ammoType){
					return laserWeapons[a];
				}
			}
			break;
			case TYPE_OBJ_MISSILE:
			for(int a=0;a<maxMissiles;++a){
				if(missileWeapons[a]->type==ammoType){
					return missileWeapons[a];
				}
			}
			break;
		};
		return NULL;
	}

	void setActiveWeapon(WeaponObject *weapon){
		if(activeWeapon!=weapon) lastWeaponType=activeWeapon;
		activeWeapon=weapon;
	}

	void setTarget(FluidObject *shipTarget){
		this->shipTarget=shipTarget;
	}

	FluidObject* getTarget(){
		return shipTarget;
	}

	VectorT *getShipTargetVectorTPtr(){
		return &shipTargetVectorT;
	}

	float getPower(){
		return power;
	}

	float *getPowerPtr(){
		return &power;
	}

	float *getPowerRegenPtr(){
		return &powerRegenRate;
	}

	float getMaxPower(){
		return maxPower;
	}

	float *getMaxPowerPtr(){
		return &maxPower;
	}

	float getMaxSpeed(){
		return MAX_FLUID_SPEED*globalScale;
	}

	int getMaxLasers(){
		return maxLasers;
	}

	int getMaxMissiles(){
		return maxMissiles;
	}

	float getMaxHp(){
		return maxHitPoints;
	}


	int getLives(){
		return lives;
	}

	//Stat modifiers
	void addMaxHp(float value){
		maxHitPoints+=value;
	}

	void addMaxShields(float value){
		maxShield+=value;
	}
	
	void addMaxPower(float value){
		maxPower+=value;
	}

	void addMaxImpulse(float value){
		maxImpulse+=value;
	}

	void addMaxVelocity(float value){
		maxVelocity+=value;
	}

	void addMaxPowerRegen(float value){
		powerRegenRate+=value;
	}

	void addMaxSpawnTime(float value){
		maxSpawnTime+=value;
	}

	void addShipPowerRegenRate(float value){
		powerRegenRate+=value;
	}

	void addLife(int value){
		lives+=value;
	}

	void resizeToGlobalScales(float newScale, float oldScale){
		radius=radius/oldScale*newScale;
		for(int a=0;a<maxLasers;++a) laserWeapons[a]->radius=laserWeapons[a]->radius/oldScale*newScale;
		for(int a=0;a<maxMissiles;++a) missileWeapons[a]->radius=missileWeapons[a]->radius/oldScale*newScale;
	}

};

#endif