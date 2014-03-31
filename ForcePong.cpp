#include "ForcePong.h"

ForcePong::ForcePong(){

}

void ForcePong::init(){
	#ifdef DISPLAYINFO
		printf("Loading Textures & models\n");
	#endif
	//Load textures
	loadForcePongTextures();
	
	//Load models
	loadForcePongModels();

	#ifdef DISPLAYINFO
		printf("Loading Music\n");
	#endif

	//Load Music
	musicBG.loadOgg("Music/music1.ogg");
	musicBG.setListenerPosition(VectorT(0,0,0));
	musicBG.setAndAttachSourcePosition(VectorT(0,0,0),true);
	musicBG.playSound();
	

	//Setup 2D world
	defaultDisplay.initModelCamera2D();
	srand(time(NULL));

	//Setup obj list ptr
	objs=new Object*[objCount];
	for(int a=0;a<objCount;++a) objs[a]=NULL;

	//Setup particle list ptr
	particleObjs=new ParticleObject*[particleCount];
	for(int a=0;a<particleCount;++a) particleObjs[a]=NULL;

	#ifdef DISPLAYINFO
		printf("Setting map\n");
	#endif
	//Setup Fluid map
	fluidMap=Map(deltaTime,dxy,viscosity,-1,-1);

	#ifdef DISPLAYINFO
		printf("Setting controller\n");
	#endif
	//Setup game controller
	gameController=ForcePongGameController(&defaultDisplay,&defaultMouse,&defaultKeyboard,&fluidMap,objs);

	//Setup opengl env constants
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);
	glLineWidth(2);
	glPointSize(10);

	//Set projectile locks
	showProjectilesLocks=false;

	keyboardOnly=false;

	for(int a=0;a<particleCount;++a) 
		particleObjs[a]=new ParticleObject( VectorT(rand()%1024,rand()%768,0,0),VectorT(),
											VectorT(.5,.5),
											VectorT(1,1,1,1),-1,
											1,(rand()%1000)/1000.0f*.01,
											true);

	#ifdef DISPLAYINFO
		printf("Done\n");
	#endif
}

void ForcePong::cleanup(){
	cleanGame();

	gameController.cleanup();

	cleanupForcePongTextures();
	cleanupForcePongModels();

	dmgStringBuffer.cleanup();
	fluidMap.cleanup();
	musicBG.cleanup();

}

void ForcePong::cleanGame(){
	//Remove all objects and bots controllers from memory
	if(objs!=NULL){
		for(int a=0;a<objCount;++a){
			if(objs[a]!=NULL){
				delete objs[a];
				objs[a]=NULL;
			}
		}
		delete [] objs;
		objs=NULL;
	}

	if(particleObjs!=NULL){
		for(int a=0;a<particleCount;++a){
			if(particleObjs[a]!=NULL){
				delete particleObjs[a];
				particleObjs[a]=NULL;
			}
		}
		delete [] particleObjs;
		particleObjs=NULL;
	}

}

void ForcePong::run(){

	//////////////Show
	show();

	//////////////Change Display modes
	changeDisplayMode();	

	//////////////Update
	update();

	//////////////Set last values
	defaultMouse.lastPosition=defaultMouse.position;

}

void ForcePong::show(){
	glClearColor(0,0,0,1.0);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	//Show map
	fluidMap.show(displayMode);

	//Draw all non-map objects
	//Setup display context
	fluidMap.disableTextures();
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	//Show objs
	for(int a=0;a<objCount;++a){
		if(objs[a]!=NULL) objs[a]->display();
	}

	//Show Particles
	for(int a=0;a<particleCount;++a){
		if(particleObjs[a]!=NULL) particleObjs[a]->display();
	}

	//Show dmgStringBuffer
	dmgStringBuffer.show();

	//Highlight projectiles
	if(showProjectilesLocks) showProjecileTargetLocks();
	
	//Show user specific info
	showUserSpecifics();

	//Show GUI/Menu stuff
	glActiveTexture(GL_TEXTURE0);
	gameController.display();

	glDisable(GL_BLEND);

}

void ForcePong::showProjecileTargetLocks(){
	FluidObject *targetObj;
	for(int a=0;a<objCount;++a){
		if(objs[a]!=NULL){
			targetObj=((FluidObject*)objs[a]);
			VectorT c=targetObj->color;
			c.w=1.0;
			targetObj->displayTargetLock(targetObj->position,7.5,c);
		}
	}
}


void ForcePong::showUserSpecifics(){

	//Target lock
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	((ShipFluidObject*)(objs[userShip]))->displayShipHeading();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	((ShipFluidObject*)(objs[userShip]))->displayShipTargetLock();

	//Stats
	((ShipFluidObject*)(objs[userShip]))->displayStats();
	//Available weapons
	((ShipFluidObject*)objs[userShip])->displayWeaponIcons();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	VectorT sColor=((ShipFluidObject*)objs[userShip])->color;
	float mx=defaultMouse.position.x,my=defaultMouse.position.y;

	glColor4f(sColor.x,sColor.y,sColor.z,.5);
	//Cursor extensions extension 
	glBegin(GL_QUADS);
	//Vert
	glVertex2f(defaultDisplay.windowWidth,my+1);
	glVertex2f(defaultDisplay.windowWidth,my-1);
	glVertex2f(0,my-1);
	glVertex2f(0,my+1);
	//Honriz
	glVertex2f(mx+1,defaultDisplay.windowHeight);
	glVertex2f(mx+1,0);
	glVertex2f(mx-1,0);
	glVertex2f(mx-1,defaultDisplay.windowHeight);
	glEnd();

	//Cursor Crosshairs
	sColor=-sColor+1;
	glColor3fv((GLfloat*)&sColor);
	glBegin(GL_QUADS);
	glVertex2f(mx+3,my+10);
	glVertex2f(mx+3,my-10);
	glVertex2f(mx-3,my-10);
	glVertex2f(mx-3,my+10);

	glVertex2f(mx+10,my+3);
	glVertex2f(mx+10,my-3);
	glVertex2f(mx-10,my-3);
	glVertex2f(mx-10,my+3);
	glEnd();

	//Ship line
	glColor4f(sColor.x,sColor.y,sColor.z,.5);
	VectorT shipPos=((ShipFluidObject*)objs[userShip])->position;
	glBegin(GL_LINES);
	glVertex2f(mx,my);
	glVertex2f(shipPos.x,shipPos.y);
	glEnd();

	//Ship Circle
	float raySize=((ShipFluidObject*)objs[userShip])->radius*3;
	for(int a=0;a<30;a+=2){
		glBegin(GL_LINES);
		glVertex2f(	shipPos.x+defaultSinValues[(SIN_TABLE_SIZE/4+SIN_TABLE_SIZE/30*a)%SIN_TABLE_SIZE]*raySize,
					shipPos.y+defaultSinValues[SIN_TABLE_SIZE/30*a]*raySize);
		glVertex2f(	shipPos.x+defaultSinValues[(SIN_TABLE_SIZE/4+SIN_TABLE_SIZE/30*(a+1))%SIN_TABLE_SIZE]*raySize,
					shipPos.y+defaultSinValues[SIN_TABLE_SIZE/30*(a+1)]*raySize);
		glEnd();
	}

}

void ForcePong::update(){
	//Game controller states determine function calls
	//Update game controller
	switch(gameController.update()){
		case GAMESTATE_PAUSE:
			break;
		//Game Modes
		case GAMESTATE_DUEL_PLAY:
			updateGame();
			break;

		case GAMESTATE_SURVIVOR_PLAY:
			updateGame();

			break;
		case GAMESTATE_TIMETRIAL_PLAY:
			updateGame();
			break;
	}
	
	
	if(defaultKeyboard.normalKeyArray[GLUT_ESC_KEY]){
		if(gameController.menuOn) gameController.menuOn=false;
		else gameController.menuOn=true;

		defaultKeyboard.normalKeyArray[GLUT_ESC_KEY]=false;
	}
}

void ForcePong::updateGameInputs(){

	//Observer mode disables all game inputs
	if(!observerMode){
		//Update user keyboard input
		updateKeyboardInputs();

		//Update user mouse inputs
		updateMouseInputs();
	}
}
void ForcePong::updateGame(){
	//Update all fluid map, objects, AI, collisions, and user inputs
	//All game modes should call this function

	//Update game inputs
	updateGameInputs();

	//Update dmgStringBuffer
	dmgStringBuffer.update();

	//Update fluidmap
	fluidMap.update();

	//Update fluid in objects
	//Set viewport to map
	glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0,0,fluidMap.frameBufferWidth,fluidMap.frameBufferHeight);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fluidMap.velocityFrameBuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);

	#ifdef LARGETEXTURE_READS

		//Single read(Slower on AGP cards)
		glReadPixels(0,0,256,256,GL_RGB,GL_FLOAT,velocityFrame);

		for(int a=0;a<objCount;++a){//Set velocity of all objects to fluid velocity
			if(objs[a]!=NULL&&((FluidObject*)objs[a])->fluidAffected){
				int col=(objs[a]->position.x*(float)(fluidMap.frameBufferWidth)/defaultDisplay.windowWidth);
				int row=(fluidMap.frameBufferHeight-objs[a]->position.y*(float)(fluidMap.frameBufferHeight)/defaultDisplay.windowHeight);

				objs[a]->velocity=VectorT((velocityFrame[3*(256*row+col)]*2-1.0)*1000,-(velocityFrame[3*(256*row+col)+1]*2-1.0)*1000,0);
			}
		}
	#else

		float ptDat[3];
		for(int a=0;a<objCount;++a){//Set velocity of all objects to fluid velocity
			if(objs[a]!=NULL&&((FluidObject*)objs[a])->fluidAffected){
				int col=(objs[a]->position.x*(float)(fluidMap.frameBufferWidth)/defaultDisplay.windowWidth);
				int row=(fluidMap.frameBufferHeight-objs[a]->position.y*(float)(fluidMap.frameBufferHeight)/defaultDisplay.windowHeight);
				glReadPixels(col,row,
							1,1,GL_RGB,GL_FLOAT,&ptDat);

				objs[a]->velocity=VectorT((ptDat[0]*2-1.0)*1000,-(ptDat[1]*2-1.0)*1000,0);
			}
		}
	
	#endif
	//Reset viewport
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glPopAttrib();
	
	//Update objects
	for(int a=0;a<objCount;++a){
		if(objs[a]!=NULL){
			objs[a]->update();
		}
	}

	//Update particles
	for(int a=0;a<particleCount;++a){
		if(particleObjs[a]!=NULL){
			particleObjs[a]->update();
		}
	}

	//Update Collision Detection
	collisionDetection();
}


void ForcePong::collisionDetection(){
	//Update damage and physics
	//Glancing/Radial collision
	for(int a=0;a<objCount;++a){
		if(objs[a]!=NULL&&((FluidObject*)objs[a])->isActive){
			for(int b=a+1;b<objCount;++b){
				//Collision detection
				if(objs[b]!=NULL&&((FluidObject*)objs[b])->isActive){
					if(((FluidObject*)objs[a])->teamID!=((FluidObject*)objs[b])->teamID){//Different team
						float sumRad=objs[a]->radius+objs[b]->radius;
						VectorT norm=(objs[a]->position-objs[b]->position);
						float d=norm.norm();
						norm.normalizeSelf();
						//Collision Response
						if(d<sumRad){
							VectorT v1=((FluidObject*)objs[a])->totalVelocity;
							VectorT v2=((FluidObject*)objs[b])->totalVelocity;
							float m1=((FluidObject*)objs[a])->mass;
							float m2=((FluidObject*)objs[b])->mass;
							float dmg1=0;
							float dmg2=0;
			
							if(!(m1==0||m2==0)){//No massless collision response
								//Fix collision positions
								VectorT pos=((FluidObject*)objs[a])->position+norm*(sumRad-d)*.5f;
								VectorT pos2=((FluidObject*)objs[b])->position-norm*(sumRad-d)*.5f;
								if(pos.x>objs[a]->radius&&pos.x<defaultDisplay.windowWidth-objs[a]->radius&&pos.y>objs[a]->radius&&pos.y<defaultDisplay.windowHeight-objs[a]->radius)
									((FluidObject*)objs[a])->position=pos;			
								if(pos2.x>objs[b]->radius&&pos2.x<defaultDisplay.windowWidth-objs[b]->radius&&pos2.y>objs[b]->radius&&pos2.y<defaultDisplay.windowHeight-objs[b]->radius)
									((FluidObject*)objs[b])->position=pos2;	

								float a1=v1.dot(norm);
								float a2=v2.dot(norm);
								
								float p;
								if(m1!=0&&m2!=0) p=fabs(2.0f*(a1-a2))/(m1+m2);
								else p=0.0;
								
								VectorT v1f=(v1+norm*m2*p)*.5;
								VectorT v2f=(v2-norm*m1*p)*.5;
								((FluidObject*)objs[a])->velocityRelative=v1f;
								((FluidObject*)objs[b])->velocityRelative=v2f;

								//Apply Damage, default to collision
								dmg1=v1f.norm()*deltaTime*.012;
								dmg2=v2f.norm()*deltaTime*.012;
							}

							//Apply Damage
							int t1=((FluidObject*)objs[a])->objType;
							int t2=((FluidObject*)objs[b])->objType;
							switch(t1){
								case TYPE_OBJ_LASER:
									dmg2+=((LaserObject*)objs[a])->damage;
									break;
								case TYPE_OBJ_MISSILE: 
									dmg2+=((MissileObject*)objs[a])->damage;
									break;
							};

							switch(t2){
								case TYPE_OBJ_LASER:
									dmg1+=((LaserObject*)objs[b])->damage;
									break;
								case TYPE_OBJ_MISSILE: 
									dmg1+=((MissileObject*)objs[b])->damage;
									break;
							};
		
							((FluidObject*)objs[a])->applyDamage(dmg1,t2,((FluidObject*)objs[b])->teamID);
							((FluidObject*)objs[b])->applyDamage(dmg2,t1,((FluidObject*)objs[a])->teamID);
					
							//Add damage text
							float textSize;
							if(((FluidObject*)objs[a])->objType==TYPE_OBJ_SHIP){
								textSize=minValue(maxValue(dmg1*2,.15),1);
								dmgStringBuffer.addString(numberToString((int)(dmg1*1000)),100,objs[a]->position,textSize,((FluidObject*)objs[b])->color);
							}
							if(((FluidObject*)objs[b])->objType==TYPE_OBJ_SHIP){
								textSize=minValue(maxValue(dmg2*2,.15),1);
								dmgStringBuffer.addString(numberToString((int)(dmg2*1000)),100,objs[b]->position,textSize,((FluidObject*)objs[a])->color);
							}
						}
					}
				}
			}
		}
	}
}

void ForcePong::updateMouseInputs(){
	ShipFluidObject *uShip=((ShipFluidObject*)(objs[userShip]));

	//Impulse modifier
	uShip->processUserImpulse();

	if(!keyboardOnly){
		//Direction control
		VectorT targetDirection=defaultMouse.position-uShip->position;
		if(!targetDirection.clockwiseTestZ(uShip->direction)) uShip->turnLeft();
		else uShip->turnRight();

		//Weapon Control
		if(defaultMouse.mouseState[0]){
			//uShip->fireActiveWeapon();
			uShip->fireActiveWeaponType();
		}

		if(defaultMouse.mouseState[2]) 
			uShip->fireLastWeapons();
	}
}


void ForcePong::updateKeyboardInputs(){
	ShipFluidObject *uShip=((ShipFluidObject*)(objs[userShip]));

	if(keyboardOnly){
		if(defaultKeyboard.normalKeyArray['a'])
			uShip->turnLeft();
		if(defaultKeyboard.normalKeyArray['d'])
			uShip->turnRight();
		if(defaultKeyboard.normalKeyArray['w'])
			uShip->accelerate();
		if(defaultKeyboard.normalKeyArray['s'])
			uShip->raccelerate();

		if(defaultKeyboard.normalKeyArray['u'])
			uShip->fireType(TYPE_MISSILE);
		if(defaultKeyboard.normalKeyArray['i'])
			uShip->fireType(TYPE_MISSILE_TORQUE);
		if(defaultKeyboard.normalKeyArray['o'])
			uShip->fireType(TYPE_MISSILE_HOME);
		if(defaultKeyboard.normalKeyArray['p'])
			uShip->fireType(TYPE_MISSILE_CHAOS);

		if(defaultKeyboard.normalKeyArray['j'])
			uShip->fireType(TYPE_LASER);
		if(defaultKeyboard.normalKeyArray['k'])
			uShip->fireType(TYPE_LASER_TRACK);
		if(defaultKeyboard.normalKeyArray['l'])
			uShip->fireType(TYPE_LASER_TELEPORT);
		if(defaultKeyboard.normalKeyArray[';'])
			uShip->fireType(TYPE_LASER_GENESIS);

	}else{
		if(defaultKeyboard.normalKeyArray['a'])
			uShip->sideStepLeft();
		if(defaultKeyboard.normalKeyArray['d'])
			uShip->sideStepRight();
		if(defaultKeyboard.normalKeyArray['w'])
			uShip->sideStepUp();
		if(defaultKeyboard.normalKeyArray['s'])
			uShip->sideStepDown();
		
		WeaponObject *weapon;
		if(defaultKeyboard.normalKeyArray[userWeaponKeys[4]]){
			weapon=uShip->findWeaponType(TYPE_OBJ_MISSILE,TYPE_MISSILE);
			if(weapon!=NULL) uShip->setActiveWeapon(weapon);	
		}

		if(defaultKeyboard.normalKeyArray[userWeaponKeys[5]]){
			weapon=uShip->findWeaponType(TYPE_OBJ_MISSILE,TYPE_MISSILE_TORQUE);
			if(weapon!=NULL) uShip->setActiveWeapon(weapon);	
		}

		if(defaultKeyboard.normalKeyArray[userWeaponKeys[6]]){
			weapon=uShip->findWeaponType(TYPE_OBJ_MISSILE,TYPE_MISSILE_HOME);
			if(weapon!=NULL) uShip->setActiveWeapon(weapon);	
		}

		if(defaultKeyboard.normalKeyArray[userWeaponKeys[7]]){
			weapon=uShip->findWeaponType(TYPE_OBJ_MISSILE,TYPE_MISSILE_CHAOS);
			if(weapon!=NULL) uShip->setActiveWeapon(weapon);	
		}

		if(defaultKeyboard.normalKeyArray[userWeaponKeys[0]]){
			weapon=uShip->findWeaponType(TYPE_OBJ_LASER,TYPE_LASER);
			if(weapon!=NULL) uShip->setActiveWeapon(weapon);	
		}

		if(defaultKeyboard.normalKeyArray[userWeaponKeys[1]]){
			weapon=uShip->findWeaponType(TYPE_OBJ_LASER,TYPE_LASER_TRACK);
			if(weapon!=NULL) uShip->setActiveWeapon(weapon);	
		}

		if(defaultKeyboard.normalKeyArray[userWeaponKeys[2]]){
			weapon=uShip->findWeaponType(TYPE_OBJ_LASER,TYPE_LASER_TELEPORT);
			if(weapon!=NULL) uShip->setActiveWeapon(weapon);	
		}

		if(defaultKeyboard.normalKeyArray[userWeaponKeys[3]]){
			weapon=uShip->findWeaponType(TYPE_OBJ_LASER,TYPE_LASER_GENESIS);
			if(weapon!=NULL) uShip->setActiveWeapon(weapon);		
		}
	}
	if(defaultKeyboard.normalKeyArray['-'])
		uShip->fireMissiles();
	if(defaultKeyboard.normalKeyArray['='])
		uShip->fireLasers();

	
	if(defaultKeyboard.normalKeyArray['\t']){//Cycle through user targets
		int a;
		FluidObject *t;
		for(a=0;a<shipCount;++a){
			if(objs[userTarget=(userTarget+1)%shipCount]!=NULL){
				t=(FluidObject*)objs[userTarget];
				if(t->isActive&&t->objType==TYPE_OBJ_SHIP) break;
			}
		}
		if(a==shipCount){
			userTarget=userShip;
		}

		defaultKeyboard.normalKeyArray['\t']=false;
	}

	if(defaultKeyboard.normalKeyArray['t']){//Toogle enemy projectile displays
		showProjectilesLocks=!showProjectilesLocks;
		defaultKeyboard.normalKeyArray['t']=false;
	}
				
}
void ForcePong::changeDisplayMode(){
	
	if(defaultKeyboard.normalKeyArray['0']){
		displayMode=3;
	}else if(defaultKeyboard.normalKeyArray['9']){
		displayMode=5;
	}

	if(defaultKeyboard.specialKeyArray[GLUT_KEY_F5]){
		displayMode=0;
	}else if(defaultKeyboard.specialKeyArray[GLUT_KEY_F6]){
		displayMode=1;
	}else if(defaultKeyboard.specialKeyArray[GLUT_KEY_F7]){
		displayMode=2;
	}else if(defaultKeyboard.specialKeyArray[GLUT_KEY_F8]){
		displayMode=4;
	}
	
}

void ForcePong::resize(VectorT windowDims,VectorT oldDims){

	for(int a=0;a<objCount;++a){
		if(objs[a]!=NULL) objs[a]->resize(windowDims,oldDims);
	}

	gameController.resize(windowDims,oldDims);
}

