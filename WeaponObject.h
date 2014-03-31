#ifndef _WEAPONOBJECT_
#define _WEAPONOBJECT_

#include "ConstantsIO.h"
#include "VectorT.h"
#include "Object.h"

#include "ForcePongConstants.h"
#include "ShipFluidObject.h"

//Ammo headers
#include "LaserObject.h"
#include "LaserIIObject.h"
#include "LaserIIIObject.h"
#include "TrackingLaserObject.h"
#include "TrackingLaserIIObject.h"
#include "TrackingLaserIIIObject.h"
#include "TeleportLaserObject.h"
#include "TeleportLaserIIObject.h"
#include "TeleportLaserIIIObject.h"
#include "GenesisLaserObject.h"
#include "GenesisLaserIIObject.h"
#include "GenesisLaserIIIObject.h"
#include "MissileObject.h"
#include "MissileIIObject.h"
#include "MissileIIIObject.h"
#include "TorqueMissileObject.h"
#include "TorqueMissileIIObject.h"
#include "TorqueMissileIIIObject.h"
#include "HomingMissileObject.h"
#include "HomingMissileIIObject.h"
#include "HomingMissileIIIObject.h"
#include "ChaosMissileObject.h"
#include "ChaosMissileIIObject.h"
#include "ChaosMissileIIIObject.h"

class WeaponObject:public FluidObject{
private:

	float damage;//Weapon damage
	float rateOfFire;//Rounds per click
	float baseRateOfFire;
	float clicksPerFire;//1.0/rateOfFire
	float baseClicksPerFire;
	float reload;//clicks till next fire
	bool alternateFire;
	float ammoLife;
	float estimatedAmmoSpeed;
	int lastAmmoIndex;

	Object **shipObjPtr;

	float *shipPower;
	float *shipMaxPower;
	float *shipPowerRegen;
	VectorT *shipTarget;//Target weapon is pointing at
	VectorT *shipPosition;
	float *shipDirectionTheta;
	
	float weaponLevel;

public:
	int type;//Type of weapon
	bool weaponReady;
	float power;//Power drainage on use
	float basePower;
	int highestWeaponLevel;

	WeaponObject(int objID,int type,int teamID,int highestWeaponLevel,VectorT color,VectorT position,float radius,float mass,
		float power,float damage,float rateOfFire,float ammoLife,
		float *shipPower,float *shipMaxPower,float *shipPowerRegen,VectorT *shipPosition,float *shipDirectionTheta,VectorT *shipTarget,
		Map *mapPtr,Object **firstElementPtr,Object **shipObjPtr)
		:FluidObject(objID,teamID,3,color,position,VectorT(0,0,0),radius,mass,hitPoints,
		mapPtr,firstElementPtr,shipObjPtr){

		this->type=type;
		this->power=basePower=minValue(power,*shipMaxPower);
		this->damage=damage;
		this->rateOfFire=baseRateOfFire=rateOfFire;
		this->shipObjPtr=shipObjPtr;

		this->shipPower=shipPower;
		this->shipMaxPower=shipMaxPower;
		this->shipTarget=shipTarget;
		this->shipPowerRegen=shipPowerRegen;
		this->ammoLife=ammoLife;
		this->shipPosition=shipPosition;
		this->shipDirectionTheta=shipDirectionTheta;
		this->highestWeaponLevel=highestWeaponLevel;

		clicksPerFire=1.0/rateOfFire;
		baseClicksPerFire=clicksPerFire;
		reload=clicksPerFire;
		estimatedAmmoSpeed=MAX_FLUID_SPEED;

		lastAmmoIndex=0;
		weaponLevel=0;
		alternateFire=true;
		weaponReady=true;
	}

	~WeaponObject(){
		
	}

	void display(){
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_BLEND);

		glColor3f(color.x/2,color.y/2,color.z/2);

		glPushMatrix();

		glTranslatef(position.x,position.y,position.z);
		glRotatef(directionTheta,0,0,1);

		glBegin(GL_TRIANGLES);
		glVertex3f(0,.25*radius,0);
		glVertex3f(0,-.25*radius,0);
		glVertex3f(.7071*radius,0,0);
		glEnd();
		glPopMatrix();

		glPopAttrib();

	}

	void update(){

		if(reload<clicksPerFire){
			weaponReady=false;
			++reload;
		}
		if(reload>=clicksPerFire){
			reload=clicksPerFire;
			weaponReady=true;
		}
	
		directionTheta=*shipDirectionTheta;
		direction=VectorT(cos(directionTheta/RADIAN),sin(directionTheta/RADIAN),0).normalize();
		position=*shipPosition;

		//Weapon specific updates
		switch(type+(int)weaponLevel){
			case TYPE_LASER_GENESIS:
				if(reload*4<clicksPerFire) *shipPower=-.2;
				break;
			case TYPE_LASER_GENESISII:
				if(reload*4<clicksPerFire) *shipPower=-.2;
				break;
			case TYPE_LASER_GENESISIII:
				if(reload*4<clicksPerFire) *shipPower=-.2;
				break;
			case TYPE_MISSILE_CHAOS:
				if(reload*4<clicksPerFire) *shipPower=-.2;
				break;
			case TYPE_MISSILE_CHAOSII:
				if(reload*4<clicksPerFire) *shipPower=-.2;
				break;
			case TYPE_MISSILE_CHAOSIII:
				if(reload*4<clicksPerFire) *shipPower=-.2;
				break;
		};
	}

	/*
	Return false if failed to fire, true otherwise
	*/
	bool fire(){
		if(weaponReady){
			int lastWeaponLevel=(int)weaponLevel;
			int validRefs=0;
			int a=findNextOpenObject(firstElementPtr,objCount);
			if(a!=-1){
				VectorT initialVelocity=direction;
				int type=this->type+(int)weaponLevel;
				switch(type){
					case TYPE_LASER:
						weaponLevel+=.008;
						power=minValue(basePower,*shipMaxPower);
						clicksPerFire=baseClicksPerFire;
						firstElementPtr[a]=new LaserObject(a,teamID,type,color,position,initialVelocity*500,
							 radius/3,damage,ammoLife,damage,
							 (alternateFire=!alternateFire),
							 mapPtr,firstElementPtr,&firstElementPtr[a]);	 
					break;
					case TYPE_LASERII:
						weaponLevel+=.008;
						power=minValue(basePower*1.2,*shipMaxPower);
						clicksPerFire=baseClicksPerFire;
						for(int b=0;b<3;++b){
							a=findNextOpenObject(firstElementPtr,objCount);
							if(a!=-1){
								firstElementPtr[a]=new LaserIIObject(a,teamID,type,color,position,initialVelocity*500,
									 radius/2,damage,ammoLife,damage*2,
									 2-b,
									 mapPtr,firstElementPtr,&firstElementPtr[a]);
							}
						}
					break;
					case TYPE_LASERIII:
						weaponLevel+=.5;
						power=minValue(basePower,*shipMaxPower);
						clicksPerFire=baseClicksPerFire*20;
						if(firstElementPtr[lastAmmoIndex]==NULL||((LaserIIIObject*)firstElementPtr[lastAmmoIndex])->getType()!=type){
							firstElementPtr[a]=new LaserIIIObject(a,teamID,type,color,position,initialVelocity*500,
								 radius,damage,ammoLife*5,damage,
								 shipObjPtr,shipPower,shipPosition,*shipPowerRegen,
								 mapPtr,firstElementPtr,&firstElementPtr[a]);	 
						}else a=lastAmmoIndex;
					break;
					case TYPE_LASER_TRACK:
						weaponLevel+=.01;
						power=minValue(basePower,*shipMaxPower);
						clicksPerFire=baseClicksPerFire;
						firstElementPtr[a]=new TrackingLaserObject(a,teamID,type,color,position,initialVelocity*500,
							 radius/3,1.0,damage,ammoLife,damage*5,
							 shipTarget,
							 mapPtr,firstElementPtr,&firstElementPtr[a]);
					break;
					case TYPE_LASER_TRACKII:
						weaponLevel+=.33;
						power=minValue(basePower*3,*shipMaxPower);
						clicksPerFire=baseClicksPerFire*20;
						for(int b=0;b<3;++b){
							int a=findNextOpenObject(firstElementPtr,objCount);
							if(a!=-1){
								firstElementPtr[a]=new TrackingLaserIIObject(a,teamID,type,color,position,initialVelocity*500,
									 radius*3,1.0,damage*.33,ammoLife*3,damage*10,
									 shipObjPtr,shipPosition,shipDirectionTheta,shipTarget,b,
									 mapPtr,firstElementPtr,&firstElementPtr[a]);
							}
						}
					break;
					case TYPE_LASER_TRACKIII:
						weaponLevel+=.5;
						power=minValue(basePower,*shipMaxPower);
						clicksPerFire=baseClicksPerFire*30;
						for(int b=0;b<24;++b){
							int a=findNextOpenObject(firstElementPtr,objCount);
							if(a!=-1){
								firstElementPtr[a]=new TrackingLaserIIIObject(a,teamID,type,color,position,initialVelocity*500,
									 radius*2,1.0,damage*.666,ammoLife*4,damage*8,
									 shipObjPtr,shipPosition,*shipTarget,*shipDirectionTheta,
									 mapPtr,firstElementPtr,&firstElementPtr[a]);
							}
						}
					break;
					case TYPE_LASER_TELEPORT:
						weaponLevel+=.15;
						power=minValue(basePower,*shipMaxPower);
						clicksPerFire=baseClicksPerFire;
						firstElementPtr[a]=new TeleportLaserObject(a,teamID,type,color,position,initialVelocity*250,
							 radius,damage,ammoLife,damage*50,
							 shipObjPtr,shipPosition,0,
							 mapPtr,firstElementPtr,&firstElementPtr[a]);
					break;
					case TYPE_LASER_TELEPORTII:
						weaponLevel+=.3;
						power=minValue(basePower*2,*shipMaxPower);
						clicksPerFire=baseClicksPerFire*3;
						for(int b=0;b<12;++b){
							a=findNextOpenObject(firstElementPtr,objCount);
							if(a!=-1){
								firstElementPtr[a]=new TeleportLaserIIObject(a,teamID,type,color*.5,position,initialVelocity*250,
										 radius,damage,ammoLife,damage*1000,
										 shipObjPtr,shipPosition,0,b,
										 mapPtr,firstElementPtr,&firstElementPtr[a]);
							}
						}
					break;
					case TYPE_LASER_TELEPORTIII:
						weaponLevel+=.4;
						power=minValue(basePower,*shipMaxPower);
						clicksPerFire=baseClicksPerFire*8.5;
						firstElementPtr[a]=new TeleportLaserIIIObject(a,teamID,type,color,*shipTarget,initialVelocity*250,
							 radius*15,damage,ammoLife*10,damage*5,
							 shipObjPtr,shipTarget,
							 mapPtr,firstElementPtr,&firstElementPtr[a]);
					break;
					case TYPE_LASER_GENESIS:
						power=minValue(basePower,*shipMaxPower);
						weaponLevel+=.2;
						clicksPerFire=baseClicksPerFire;
						firstElementPtr[a]=new GenesisLaserObject(a,teamID,type,color,position,initialVelocity*500*(radius/DEFAULT_SHIP_RADIUS),
							radius*2,damage,ammoLife,damage,
							shipObjPtr,shipPosition,1750.0f*globalScale,0,
							mapPtr,firstElementPtr,&firstElementPtr[a]);
							
					break;
					case TYPE_LASER_GENESISII:
						power=minValue(basePower,*shipMaxPower);
						weaponLevel+=.2;
						clicksPerFire=baseClicksPerFire*1.5;
						firstElementPtr[a]=new GenesisLaserIIObject(a,teamID,type,color,position,initialVelocity*500*(radius/DEFAULT_SHIP_RADIUS),
												radius*2,damage,ammoLife*3.75,damage,
												shipObjPtr,shipPosition,shipDirectionTheta,0,300*globalScale,
												mapPtr,firstElementPtr,&firstElementPtr[a]);
					break;
					case TYPE_LASER_GENESISIII:
						power=minValue(basePower,*shipMaxPower);
						weaponLevel+=1;
						clicksPerFire=baseClicksPerFire*5;
						firstElementPtr[a]=new GenesisLaserIIIObject(a,teamID,type,color,position,initialVelocity*500*(radius/DEFAULT_SHIP_RADIUS),
												radius*10,damage,ammoLife*3.5,damage,
												shipObjPtr,shipPosition,shipDirectionTheta,0,600*globalScale,
												mapPtr,firstElementPtr,&firstElementPtr[a]);
					break;
					case TYPE_MISSILE:
						weaponLevel+=.01;
						power=minValue(basePower,*shipMaxPower);
						clicksPerFire=baseClicksPerFire;
						firstElementPtr[a]=new MissileObject(a,teamID,type,color,position,initialVelocity*250,
							radius,1.25,damage,ammoLife,damage,
							mapPtr,firstElementPtr,&firstElementPtr[a]);
					break;
					case TYPE_MISSILEII:
						weaponLevel+=.01;
						power=minValue(basePower*.9,*shipMaxPower);
						clicksPerFire=baseClicksPerFire;
						for(int b=2;b>=0;--b){
							a=findNextOpenObject(firstElementPtr,objCount);
							if(a!=-1){
								firstElementPtr[a]=new MissileIIObject(a,teamID,type,color,position,initialVelocity*250,
									radius,1.25,damage,ammoLife*.8,damage,
									b,
									mapPtr,firstElementPtr,&firstElementPtr[a]);
							}
						}
					break;
					case TYPE_MISSILEIII:
						weaponLevel+=.4;
						power=minValue(basePower*1.2,*shipMaxPower);
						clicksPerFire=baseClicksPerFire*55;
						for(int b=0;b<25;++b){
							a=findNextOpenObject(firstElementPtr,objCount);
							if(a!=-1){
								firstElementPtr[a]=new MissileIIIObject(a,teamID,type,color,position,initialVelocity*500,
									radius,1.25,damage,ammoLife*.5,damage*4,
									shipTarget,25,b,5,
									mapPtr,firstElementPtr,&firstElementPtr[a]);
							}
						}
					break;
					case TYPE_MISSILE_TORQUE:
						weaponLevel+=.02;
						power=minValue(basePower,*shipMaxPower);
						clicksPerFire=baseClicksPerFire;
						firstElementPtr[a]=new TorqueMissileObject(a,teamID,type,color,position,initialVelocity*250,
							radius*1.125,1.3,damage,ammoLife*3,damage,
							mapPtr,firstElementPtr,&firstElementPtr[a]);
					break;
					case TYPE_MISSILE_TORQUEII:
						weaponLevel+=.2;
						power=minValue(basePower*1.5,*shipMaxPower);
						clicksPerFire=baseClicksPerFire*6;
						int refTrack[20];
						for(int b=0;b<20;++b){
							a=findNextOpenObject(firstElementPtr,objCount);
							if(a!=-1){
								refTrack[validRefs++]=a;
								firstElementPtr[a]=new TorqueMissileIIObject(a,teamID,type,color,position,initialVelocity*300,
									radius*.75,1.3,damage*2,ammoLife,damage,
									b,20,
									NULL,NULL,
									mapPtr,firstElementPtr,&firstElementPtr[a]);
							}
						}
						for(int b=0;b<validRefs;++b){
							((TorqueMissileIIObject*)firstElementPtr[refTrack[b]])->setNext((TorqueMissileIIObject*)firstElementPtr[refTrack[(b+1)%validRefs]]);
							((TorqueMissileIIObject*)firstElementPtr[refTrack[b]])->setLast((TorqueMissileIIObject*)firstElementPtr[refTrack[(b-1)+(b==0)*validRefs]]);
						}
					break;
					case TYPE_MISSILE_TORQUEIII:
						weaponLevel+=1;
						power=minValue(basePower*2.0,*shipMaxPower);
						clicksPerFire=baseClicksPerFire*12;
						for(int b=0;b<3;++b){
							a=findNextOpenObject(firstElementPtr,objCount);
							if(a!=-1){
								firstElementPtr[a]=new TorqueMissileIIIObject(a,teamID,type,color,
									position,initialVelocity*250,
									radius*2,1.3,damage*.5,ammoLife,damage*12,
									b,3,
									shipObjPtr,shipPosition,shipDirectionTheta,
									mapPtr,firstElementPtr,&firstElementPtr[a]);
							}
						}
					break;
					case TYPE_MISSILE_HOME:
						weaponLevel+=.02;
						power=minValue(basePower,*shipMaxPower);
						clicksPerFire=baseClicksPerFire;
						firstElementPtr[a]=new HomingMissileObject(a,teamID,type,color,position,initialVelocity*250,
							radius*1.25,2.33,damage,ammoLife,damage,
							shipTarget,
							mapPtr,firstElementPtr,&firstElementPtr[a]);
					break;
					case TYPE_MISSILE_HOMEII:
						weaponLevel+=.1;
						power=minValue(basePower*2.0,*shipMaxPower);
						clicksPerFire=baseClicksPerFire*3;
						for(int b=0;b<4;++b){
							a=findNextOpenObject(firstElementPtr,objCount);
							if(a!=-1){
								firstElementPtr[a]=new HomingMissileIIObject(a,teamID,type,color,position,initialVelocity*250,
									radius*1.25,2.33,damage*.66,ammoLife,damage,
									shipTarget,b,4,
									mapPtr,firstElementPtr,&firstElementPtr[a]);
							}
						}
					break;
					case TYPE_MISSILE_HOMEIII:
						weaponLevel+=.5;
						power=minValue(basePower,*shipMaxPower);
						clicksPerFire=baseClicksPerFire*3;
						firstElementPtr[a]=new HomingMissileIIIObject(a,teamID,type,color,position,initialVelocity*250,
							radius*1.5,2.33,damage*.66,ammoLife*7,damage*7.5,
							shipTarget,
							mapPtr,firstElementPtr,&firstElementPtr[a]);
			
					break;
					case TYPE_MISSILE_CHAOS:
						weaponLevel+=.2;
						power=minValue(basePower,*shipMaxPower);
						clicksPerFire=baseClicksPerFire;
						firstElementPtr[a]=new ChaosMissileObject(a,teamID,type,color,position,initialVelocity*1000,
							radius/globalScale*.8,3.3,damage,ammoLife,damage*1.5,
							0,6,2,
							mapPtr,firstElementPtr,&firstElementPtr[a]);
					break;
					case TYPE_MISSILE_CHAOSII:
						weaponLevel+=.2;
						power=minValue(basePower,*shipMaxPower);
						clicksPerFire=baseClicksPerFire*2;
						firstElementPtr[a]=new ChaosMissileIIObject(a,teamID,type,color,position,initialVelocity*250,
							radius*3
							,3.3,damage*.25,ammoLife*7,damage*10,
							100,0,8,
							mapPtr,firstElementPtr,&firstElementPtr[a]);
					break;
					case TYPE_MISSILE_CHAOSIII:
						weaponLevel+=1.0;
						power=minValue(basePower,*shipMaxPower);
						clicksPerFire=baseClicksPerFire*3;
						firstElementPtr[a]=new ChaosMissileIIIObject(a,teamID,type,color,position,initialVelocity*250,
							radius*3
							,3.3,.0001,ammoLife*8,99999,
							120,0,6,
							mapPtr,firstElementPtr,&firstElementPtr[a]);
					break;
				};
				reload=0;
				lastAmmoIndex=a;
				if(a==-1){//Usually means too many objects on screen
					estimatedAmmoSpeed=1;
				}else{
					estimatedAmmoSpeed=((FluidObject*)firstElementPtr[a])->getMaxSpeed();
				}
				if(weaponLevel>=highestWeaponLevel){
					weaponLevel=0;
					reload=baseClicksPerFire;
				}

				//Recover Weapon 
				if(fabs(weaponLevel-lastWeaponLevel)>=1) reload=clicksPerFire;

				return true;
			}

		}
		return false;
	}

	void refresh(){
		reload=clicksPerFire;
		weaponReady=true;
	}

	float getAmmoSpeed(){
		return estimatedAmmoSpeed;
	}

	float getWeaponLevel(){
		return weaponLevel;
	}

	float getWeaponReload(){
		return fabs(reload/clicksPerFire);
	}

	void multRateOfFire(float value){
		rateOfFire=baseRateOfFire*value;
		baseClicksPerFire=1.0f/rateOfFire;
		clicksPerFire/=value;
	}

};

#endif