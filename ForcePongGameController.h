#ifndef _FORCEPONGGAMECONTROLLER_
#define _FORCEPONGGAMECONTROLLER_

#include "Dlg.h"
#include "DlgButton.h"
#include "Display.h"
#include "Mouse.h"
#include "Keyboard.h"

#include "ForcePongShipEditor.h"

#include "ForcePongDuel.h"
#include "ForcePongSurvivor.h"
#include "ForcePongTimeTrial.h"

#define GAMESTATE_PAUSE					0
#define GAMESTATE_DUEL_MAKEGAME			1
#define GAMESTATE_DUEL_PLAY				2
#define GAMESTATE_SURVIVOR_MAKEGAME		3
#define GAMESTATE_SURVIVOR_PLAY			4
#define GAMESTATE_TIMETRIAL_MAKEGAME	5
#define GAMESTATE_TIMETRIAL_PLAY		6

#define MAX_MENUS					MENU_TEXTURE_MAX
#define MAX_MENU_MAIN_BUTTONS		4
#define MAX_MENU_MAIN_PLAY_BUTTONS	3


class ForcePongGameController{
private:
	Display *menuDisplay;
	Mouse *menuMouse;
	Keyboard *menuKeyboard;

	Map	*fluidMap;
	Object **objPtr;


	int gameState;
	ForcePongShipEditor shipEditor;
	Dlg menus[MAX_MENUS];
	DlgButton *mainMenuButtons[MAX_MENU_MAIN_BUTTONS];
	DlgButton *mainMenuPlayButtons[MAX_MENU_MAIN_PLAY_BUTTONS];

	VectorT creatorPosition;

	ForcePongSurvivor	forcePongSurvivor;
	ForcePongDuel		forcePongDuel;
	ForcePongTimeTrial	forcePongTimeTrial;

public:
	int botType[MAX_BOTS];
	int nBots;
	int nShips;
	bool menuOn;

	ForcePongGameController(){
	}
	
	ForcePongGameController(Display *menuDisplay,Mouse *menuMouse,Keyboard *menuKeyboard,
							Map *fluidMap, Object **objPtr){

		this->menuMouse=menuMouse;
		this->menuKeyboard=menuKeyboard;
		this->menuDisplay=menuDisplay;
		this->fluidMap=fluidMap;
		this->objPtr=objPtr;

		for(int a=0;a<MAX_MENU_MAIN_BUTTONS;++a)		mainMenuButtons[a]=NULL;
		for(int a=0;a<MAX_MENU_MAIN_PLAY_BUTTONS;++a)	mainMenuPlayButtons[a]=NULL;

		creatorPosition=VectorT(600,50);
		//////////////////Setup menu Dlgs
		//Main menu
		menus[MENU_MAIN]=Dlg(	VectorT(0,0,0),
								VectorT(menuDisplay->windowWidth,menuDisplay->windowHeight,0),
								VectorT(1,1,1,.5),
								menuTextures[MENU_MAIN].texID,
								NULL,
								true,
								true);
		mainMenuButtons[0]=new DlgButton(	VectorT(890,630,0),
											VectorT(menuDisplay->windowWidth-880,33,0),
											VectorT(1,0,0,.25),
											-1,
											NULL,
											true,
											true,
											menuMouse);
		mainMenuButtons[1]=new DlgButton(	VectorT(890,663,0),
											VectorT(menuDisplay->windowWidth-880,33,0),
											VectorT(1,0,0,.25),
											-1,
											NULL,
											true,
											true,
											menuMouse);
		mainMenuButtons[2]=new DlgButton(	VectorT(890,696,0),
											VectorT(menuDisplay->windowWidth-880,33,0),
											VectorT(1,0,0,.25),
											-1,
											NULL,
											true,
											true,
											menuMouse);
		mainMenuButtons[3]=new DlgButton(	VectorT(890,729,0),
											VectorT(menuDisplay->windowWidth-880,33,0),
											VectorT(1,0,0,.25),
											-1,
											NULL,
											true,
											true,
											menuMouse);

		for(int a=0;a<MAX_MENU_MAIN_BUTTONS;++a) menus[MENU_MAIN].add(mainMenuButtons[a]);
		
		//Play menu
		menus[MENU_MAIN_PLAY]=Dlg(	VectorT(75,75,0),			//Play button
									VectorT(300,150,0),
									VectorT(1,1,1,.5),
									menuTextures[MENU_MAIN_PLAY].texID,
									"Play",
									false,
									false);
		mainMenuPlayButtons[0]=new DlgButton(VectorT(85,75,0),//Survivor mode button
											VectorT(285,50,0),
											VectorT(0,0,1,.25),
											-1,
											NULL,
											true,
											true,
											menuMouse);
		mainMenuPlayButtons[1]=new DlgButton(VectorT(85,125,0),//Duel mode button
											VectorT(285,50,0),
											VectorT(0,1,0,.25),
											-1,
											NULL,
											true,
											true,
											menuMouse);
		mainMenuPlayButtons[2]=new DlgButton(VectorT(85,175,0),//Timetrial mode button
											VectorT(285,50,0),
											VectorT(1,0,0,.25),
											-1,
											NULL,
											true,
											true,
											menuMouse);

		for(int a=0;a<MAX_MENU_MAIN_PLAY_BUTTONS;++a) menus[MENU_MAIN_PLAY].add(mainMenuPlayButtons[a]);


		menus[MENU_MAIN_ABOUT]=Dlg(	VectorT(75,225,0),
									VectorT(300,400,0),
									VectorT(1,1,1,.5),
									menuTextures[MENU_MAIN_ABOUT].texID,
									"About",
									false,
									false);
		
		menus[MENU_MAIN_OPTIONS]=Dlg(	VectorT(0,0,0),
										VectorT(200,100,0),
										VectorT(1,1,1,.5),
										menuTextures[MENU_MAIN_OPTIONS].texID,
										NULL,
										false,
										false);

		menuOn=true;

		///////////Setup ship editor
		shipEditor=ForcePongShipEditor(VectorT(400,100,0),VectorT(600,600),VectorT(.25,.25,.25,.75),-1,"Ship Editor",false,false,menuMouse,menuDisplay,&globalModels[MODEL_SHIP_USER]);

		//Setup other modes
		forcePongSurvivor=ForcePongSurvivor(menuDisplay,menuKeyboard,fluidMap,objPtr);
		forcePongTimeTrial=ForcePongTimeTrial(menuDisplay,menuKeyboard,fluidMap,objPtr);

		///////Setup demo game
		nShips=3;
		nBots=min(nShips,MAX_BOTS);
		int botTypes[]={TYPE_BOT_LASER,TYPE_BOT_MISSILE,TYPE_BOT_MISSILE};
		forcePongDuel=ForcePongDuel(menuDisplay,fluidMap,objPtr);
		forcePongDuel.makeGame(nShips,nBots,botTypes,true);
		observerMode=true;
		gameState=GAMESTATE_DUEL_PLAY;//Demo mode

		//Setup user targets
		userTarget=userShip;
		((ShipFluidObject*)(objPtr[userShip]))->setTarget(((ShipFluidObject*)(objPtr[userTarget])));
		
	}

	~ForcePongGameController(){
	}

	void cleanup(){
		//Cleanup controller objects
		for(int a=0;a<MAX_MENUS;++a){
			menus[a].cleanup();
		}
		for(int a=0;a<MAX_MENU_MAIN_BUTTONS;++a){
			if(mainMenuButtons[a]!=NULL){
				mainMenuButtons[a]->cleanup();
				delete mainMenuButtons[a];
			}
		}
		for(int a=0;a<MAX_MENU_MAIN_PLAY_BUTTONS;++a){
			if(mainMenuPlayButtons[a]!=NULL){
				mainMenuPlayButtons[a]->cleanup();
				delete mainMenuPlayButtons[a];
			}
		}
		shipEditor.cleanup();

		forcePongDuel.cleanup();
		forcePongSurvivor.cleanup();
		forcePongTimeTrial.cleanup();
	}

	
	void display(){
		if(menuOn){
			glPushAttrib(GL_ENABLE_BIT);
			glEnable(GL_BLEND);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			shipEditor.display();
			for(int a=0;a<MAX_MENUS;++a) menus[a].display();
			
			//Specific GUI stuff
			//About menu
			if(menus[MENU_MAIN_ABOUT].getActive()&&menus[MENU_MAIN_ABOUT].getVisible()){
				VectorT aboutMenuPosition=menus[MENU_MAIN_ABOUT].getPosition();
				int xOffset=menuDisplay->windowScaleX*20;
				int yOffset=menuDisplay->windowScaleY*20;
				glColor4f(1,1,1,.75);
				menuDisplay->displayString("Contact: yluo1@umd.edu\n\tYuancheng Luo\n\nMusic: dj micro: (transa) supernova\n[original]\n\nXkorgine productions, 2007 v1.6",
					aboutMenuPosition.x+1*xOffset,aboutMenuPosition.y+2*yOffset,.1,NULL);
			}
			//Game pane
			if(menus[MENU_MAIN_PLAY].getActive()){
				glColor4f(1,1,1,.75);
				menuDisplay->displayString("Controls:\
										   \nw - move up\
										   \na - move left\
										   \ns - move down\
										   \nd - move right\
										   \n\nCursor - change ship orientation\
										   \nLeft click - fire current weapon\
										   \nRight click - fire last weapon\
										   \nMiddle drag - push liquid\
										   \nq e r f 1 2 3 4 - select weapon\
										   \n\nTab - select next target\
										   \nEsc - Toggle Menu\
										   \n0 9 - Toggle Dynamic Lighting",
					300*menuDisplay->windowScaleX,300*menuDisplay->windowScaleY,.1,NULL);
			}
			
			//Survivor
			if(mainMenuPlayButtons[0]->getActive()&&mainMenuPlayButtons[0]->getMouseOver()){
				glColor4f(1,1,1,.75);
				menuDisplay->displayString("Clear the level by killing all enemies\nGain better weapons, more hp, shield, and power each level\nTougher enemies spawn as you play\n\n4 Level cycles\n1st and 2nd levels are melee fights\n3rd level is a FFA\n4th level is a team match",
					400*menuDisplay->windowScaleX,100*menuDisplay->windowScaleY,.1,NULL);
			}
			//Duel
			if(mainMenuPlayButtons[1]->getActive()&&mainMenuPlayButtons[1]->getMouseOver()){
				glColor4f(1,1,1,.75);
				menuDisplay->displayString("Fight in a 3 player endless FFA.\nYou only get TWO weapons so use tactics to win",
					400*menuDisplay->windowScaleX,150*menuDisplay->windowScaleY,.1,NULL);
			}
			//Time trial
			if(mainMenuPlayButtons[2]->getActive()&&mainMenuPlayButtons[2]->getMouseOver()){
				glColor4f(1,1,1,.75);
				menuDisplay->displayString("Destroy enemy ships before the timer runs to 0\nA time dilation ring surrounds your ship and slows down all hostiles\nDestroying ships add to your timer and expand the dilation ring\nLeveling is reversed (ALL ships respawns with higher stats)",
					400*menuDisplay->windowScaleX,200*menuDisplay->windowScaleY,.1,NULL);
			}


			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

			glPopAttrib();
		}
		switch(gameState){
			case GAMESTATE_PAUSE:
				break;
			case GAMESTATE_DUEL_PLAY:
				forcePongDuel.display();
				break;
			case GAMESTATE_SURVIVOR_PLAY:
				forcePongSurvivor.display();
				break;
			case GAMESTATE_TIMETRIAL_PLAY:
				forcePongTimeTrial.display();
				break;
		};
	}

	int update(){
		//Return gamestate
		
		if(menuOn){
			shipEditor.update();

			for(int a=0;a<MAX_MENUS;++a) menus[a].update();
			if(mainMenuButtons[0]->getActive()&&mainMenuButtons[0]->getMouseClicked()){//Play hit
				menus[MENU_MAIN_PLAY].setActive(!menus[MENU_MAIN_PLAY].getActive());
				menus[MENU_MAIN_PLAY].setVisible(!menus[MENU_MAIN_PLAY].getVisible());
			}else if(mainMenuPlayButtons[0]->getActive()&&mainMenuPlayButtons[0]->getMouseClicked()){
				//Survivor mode
				gameState=GAMESTATE_SURVIVOR_MAKEGAME;
			}else if(mainMenuPlayButtons[1]->getActive()&&mainMenuPlayButtons[1]->getMouseClicked()){
				//Duel mode
				for(int a=0;a<MAX_BOTS;++a) botType[a]=rand()%2;
				gameState=GAMESTATE_DUEL_MAKEGAME;
			}else if(mainMenuPlayButtons[2]->getActive()&&mainMenuPlayButtons[2]->getMouseClicked()){
				//Timetrial mode
				for(int a=0;a<MAX_BOTS;++a) botType[a]=rand()%2;
				gameState=GAMESTATE_TIMETRIAL_MAKEGAME;
			}else if(mainMenuButtons[1]->getActive()&&mainMenuButtons[1]->getMouseClicked()){//Options hit
				shipEditor.setActive(!shipEditor.getActive());
				shipEditor.setVisible(!shipEditor.getVisible());
			}else if(mainMenuButtons[2]->getActive()&&mainMenuButtons[2]->getMouseClicked()){//About hit
				menus[MENU_MAIN_ABOUT].setActive(!menus[MENU_MAIN_ABOUT].getActive());
				menus[MENU_MAIN_ABOUT].setVisible(!menus[MENU_MAIN_ABOUT].getVisible());
			}else if(mainMenuButtons[3]->getActive()&&mainMenuButtons[3]->getMouseClicked()){//Exit hit
				exit(0);
			}
		}

		switch(gameState){
			case GAMESTATE_PAUSE:
				
				break;
			case GAMESTATE_DUEL_MAKEGAME:
				//Make duel game
				nShips=3;
				nBots=min(nShips,MAX_BOTS);
				for(int a=0;a<nBots;++a) botType[a]=a%2;
				forcePongDuel.makeGame(nShips,nShips,botType,false);
				observerMode=false;
				menuOn=false;
				gameState=GAMESTATE_DUEL_PLAY;
				break;
			case GAMESTATE_DUEL_PLAY:
				forcePongDuel.update();
				break;

			case GAMESTATE_SURVIVOR_MAKEGAME:
				//Make game and set mode to play
				forcePongSurvivor.makeGame();
				gameState=GAMESTATE_SURVIVOR_PLAY;
				menuOn=false;
				observerMode=false;
				break;
			case GAMESTATE_SURVIVOR_PLAY:
				forcePongSurvivor.update();
				break;

			case GAMESTATE_TIMETRIAL_MAKEGAME:
				//Make timetrial game
				nShips=3;
				nBots=min(nShips,MAX_BOTS);
				for(int a=0;a<nBots;++a) botType[a]=rand()%2;
				forcePongTimeTrial.makeGame(nShips,nShips,botType,false);
				observerMode=false;
				menuOn=false;
				gameState=GAMESTATE_TIMETRIAL_PLAY;
				break;
			case GAMESTATE_TIMETRIAL_PLAY:
				forcePongTimeTrial.update();
				break;
		};
		return gameState;
	}

	void resize(VectorT windowDims,VectorT oldDims){

		shipEditor.resize(windowDims,oldDims);
		creatorPosition.x*=windowDims.x/oldDims.x;
		creatorPosition.y*=windowDims.y/oldDims.y;
	
		for(int a=0;a<MAX_MENUS;++a){
			menus[a].resize(windowDims,oldDims);
		}

		for(int a=0;a<MAX_MENU_MAIN_PLAY_BUTTONS;++a){
			if(mainMenuPlayButtons[a]!=NULL) mainMenuPlayButtons[a]->resize(windowDims,oldDims);
		}
	
		for(int a=0;a<MAX_MENU_MAIN_BUTTONS;++a){
			if(mainMenuButtons[a]!=NULL) mainMenuButtons[a]->resize(windowDims,oldDims);
		}

		forcePongSurvivor.resize(windowDims,oldDims);
		
	}


	void setGameState(int gameState){
		this->gameState=gameState;
	}

	int getGameState(){
		return gameState;
	}


};

#endif