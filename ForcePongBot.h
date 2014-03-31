#ifndef _FORCEPONGBOT_
#define _FORCEPONGBOT_

#include "Constants.h"
#include "Object.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "ShipFluidObject.h"
	
#define MOVEMENT_STATE_NOTHING	0
#define MOVEMENT_STATE_TARGET	1
#define MOVEMENT_STATE_NEXTPT	2

class ForcePongBot{

private:

	ShipFluidObject *ship;
	FluidObject *shipTarget;
	Object **firstElementPtr;
	ForcePongBot **currentElementPtr;

	int objID;//Index in an object array

	//States
	int movementState;/*	0=point to target
							1=point to next point
						*/
	//Pathing
	VectorT nextPt;
	VectorT nextBasePt;
	float nextDist;//Min dist of next pt to current pt
	float nextRadial;//Dist from point to confirm

	//Offense
	VectorT targetDirection;
	float targetDist;
	int nextTargetWeapon;
	int nextMovingWeapon;

	int offDefCycle;
	int offDefCycleMax;//Clicks for next cycle
	float offenseDefense;//Percent of time dedicated to off vs def (0=off, 1=def)

	int shipWeaponTypes[4];

	int botType;
	bool nextWeaponSet;
	
public:
	ForcePongBot(int objID, int botType,float offenseDefense,ShipFluidObject *ship, Object **firstElementPtr,ForcePongBot **currentElementPtr){

		this->objID=objID;
		this->offenseDefense=offenseDefense;
		this->ship=ship;
		this->firstElementPtr=firstElementPtr;
		this->currentElementPtr=currentElementPtr;
		this->botType=botType;

		movementState=MOVEMENT_STATE_TARGET;
		offDefCycleMax=10;
		offDefCycle=0;
		nextBasePt=ship->position;
		nextRadial=100;
		nextDist=600;
		targetDirection=VectorT(1,0,0);
		targetDist=ship->radius;
		
		switch(botType){//Fill all remaining slots of shipWeaponTypes with the iterated weapon
			case TYPE_BOT_MISSILE:
				for(int a=0,b=0;a<24;++a){
					if(ship->findWeaponType(TYPE_OBJ_MISSILE,a)!=NULL){
						for(int c=b;c<4;++c) shipWeaponTypes[c]=a;
						++b;
					}
				}
			break;
			case TYPE_BOT_LASER:
				for(int a=0,b=0;a<24;++a){
					if(ship->findWeaponType(TYPE_OBJ_LASER,a)!=NULL){
						for(int c=b;c<4;++c) shipWeaponTypes[c]=a;
						++b;
					}
				}
			break;
		};
		nextTargetWeapon=shipWeaponTypes[0];
		nextMovingWeapon=shipWeaponTypes[1];


		nextWeaponSet=true;

		shipTarget=NULL;
	}

	~ForcePongBot(){
		
	}

	void update(){
		if(firstElementPtr[objID]==NULL||((FluidObject*)firstElementPtr[objID])->objType!=TYPE_OBJ_SHIP){//Host ship as been removed so remove this
			(*currentElementPtr)=NULL;
			delete this;
			return;
		}else if(ship->isActive){
			updateTarget();
			updateWeaponUsage();
			updatePathing();
			updateMovement();
			updateCycle();
		}
	}

	void updateCycle(){
		movementState=(offDefCycle<offDefCycleMax*offenseDefense?MOVEMENT_STATE_TARGET:MOVEMENT_STATE_NEXTPT);
		if(offDefCycle>=offDefCycleMax){
			offDefCycle=0;
		}
	}

	void updateWeaponUsage(){
		switch(botType){
			case TYPE_BOT_LASER:
			//If higher weapon is loaded, save power to fire
			switch(movementState){
				case MOVEMENT_STATE_TARGET:
					if(ship->direction.angle(targetDirection)<1.0/RADIAN){

						if(ship->fireType(nextTargetWeapon)){
							nextWeaponSet=false;
						}
						if(ship->getPower()>ship->getMaxPower()*.1&&!nextWeaponSet){
							nextWeaponSet=true;
							int n=rand()%100;
							if(n<85) nextTargetWeapon=shipWeaponTypes[0];//Laser
							else if(n<97) nextTargetWeapon=shipWeaponTypes[1];//Track
							else if(n<100) nextTargetWeapon=shipWeaponTypes[3];//Genesis

							if(!ship->weaponReady(TYPE_OBJ_LASER,nextTargetWeapon))
								nextTargetWeapon=shipWeaponTypes[0];//Default to laser					

							++offDefCycle;
						}

					}
					break;
				case MOVEMENT_STATE_NEXTPT:
					if(ship->fireType(nextMovingWeapon)){
						nextWeaponSet=false;
					}
					if(ship->getPower()>ship->getMaxPower()*.1&&!nextWeaponSet){
						nextWeaponSet=true;
						int n=rand()%100;
						if(n<75) nextMovingWeapon=shipWeaponTypes[1];//Track
						else if(n<100) nextMovingWeapon=shipWeaponTypes[2];//Teleport
						
						if(!ship->weaponReady(TYPE_OBJ_LASER,nextMovingWeapon))
							nextMovingWeapon=shipWeaponTypes[0];//Default to laser

						++offDefCycle;
					}
			
					break;
			};
			break;

			case TYPE_BOT_MISSILE:
				switch(movementState){
				case MOVEMENT_STATE_TARGET:
					if(ship->direction.angle(targetDirection)<1.0/RADIAN){

						if(ship->fireType(nextTargetWeapon)){
							nextWeaponSet=false;
						}
						if(ship->getPower()>ship->getMaxPower()*.1&&!nextWeaponSet){
							nextWeaponSet=true;
							int n=rand()%100;
		
							if(n<80) nextTargetWeapon=shipWeaponTypes[0];//Missile
							else if(n<95) nextTargetWeapon=shipWeaponTypes[1];//Torque
							else if(n<100) nextTargetWeapon=shipWeaponTypes[3];//Chaos

							if(!ship->weaponReady(TYPE_OBJ_MISSILE,nextTargetWeapon))
								nextTargetWeapon=shipWeaponTypes[0];//Default to missile

							++offDefCycle;
						}
					}
					break;
				case MOVEMENT_STATE_NEXTPT:
					if(ship->fireType(nextMovingWeapon)){
						nextWeaponSet=false;
					}
					if(ship->getPower()>ship->getMaxPower()*.1&&!nextWeaponSet){
						nextWeaponSet=true;
						int n=rand()%100;
						if(n<50) nextMovingWeapon=shipWeaponTypes[1];//Torque
						else if(n<100) nextMovingWeapon=shipWeaponTypes[2];//Homing
						
						if(!ship->weaponReady(TYPE_OBJ_MISSILE,nextMovingWeapon))
							nextMovingWeapon=shipWeaponTypes[0];//Default to missile

						++offDefCycle;
					}
			
					break;
			};
			break;
		}
	}

	void updateTarget(){
		int a;
		if(shipTarget==NULL||!shipTarget->isActive){
			for(a=1;a<shipCount;++a){
				int tmpTarget=(ship->objID+a)%shipCount;
				if(firstElementPtr[tmpTarget]!=NULL){
					if(((FluidObject*)firstElementPtr[tmpTarget])->isActive){
						if(((FluidObject*)firstElementPtr[tmpTarget])->objType==TYPE_OBJ_SHIP){
							if(((FluidObject*)firstElementPtr[tmpTarget])->teamID!=ship->teamID){
								shipTarget=(FluidObject*)firstElementPtr[tmpTarget];
								ship->setTarget(shipTarget);
								if(objID==userShip) userTarget=a;
								break;
							}
						}
					}
				}
			}
			if(a==shipCount){
				shipTarget=NULL;
				ship->setTarget((FluidObject*)currentElementPtr);
			}
		}

	}

	void updatePathing(){
	
		nextPt=nextBasePt;
		float distSq=(ship->position-nextPt).normSquared();

		if(distSq<=nextRadial*nextRadial){//Determine next point
			do{
				nextBasePt=VectorT(ship->radius+rand()%(int)(defaultDisplay.windowWidth-ship->radius*2),
								  ship->radius+rand()%(int)(defaultDisplay.windowHeight-ship->radius*2),0);
			}while((nextBasePt-ship->position).norm()>nextDist);
		}
	}
	
	void updateMovement(){

		switch(movementState){
			case MOVEMENT_STATE_TARGET:	
				if(shipTarget!=NULL){
		
					float t,t2;
					targetDist=(shipTarget->position-ship->position).norm();
					float ammoSpeed=MAX_FLUID_SPEED;
					switch(botType){
						case TYPE_BOT_LASER: ammoSpeed=ship->findWeaponType(TYPE_OBJ_LASER,nextTargetWeapon)->getAmmoSpeed();
						break;
						case TYPE_BOT_MISSILE:  ammoSpeed=ship->findWeaponType(TYPE_OBJ_MISSILE,nextTargetWeapon)->getAmmoSpeed();
						break;
					};
					t2=targetDist/ammoSpeed;
					
					float tLB=1,tUB=1000;
					t=1;
					
					VectorT v1=(shipTarget->position-ship->position).normalize()*ammoSpeed;
					VectorT v2=shipTarget->totalVelocity*deltaTime;

					
					for(int a=0;a<100;++a){//Recursivly estimate target distance
						targetDirection=(v1*t2+v2*t)/t/ammoSpeed;
						float w=targetDirection.norm();
						if(w>1){
							tLB=t;
							t=(tUB+t)/2;
						}else if(w<1){
							tUB=t;
							t=(tLB+t)/2;
						}
						if(fabs(w-1.0)<=.00001) break;
					}

					targetDirection=targetDirection.normalize();	

					if(!targetDirection.clockwiseTestZ(ship->direction)) ship->turnLeft();
					else ship->turnRight();
				}
				break;
			case MOVEMENT_STATE_NEXTPT:

				targetDirection=nextPt-ship->position;
				if(!targetDirection.clockwiseTestZ(ship->direction)) ship->turnLeft();
				else ship->turnRight();
					
				ship->accelerate();

				break;
		};
	}

};

#endif