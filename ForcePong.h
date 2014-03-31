#ifndef _FORCEPONG_
#define _FORCEPONG_

#include <time.h>

#include "Constants.h"
#include "ConstantsIO.h"
#include "Conversion.h"
#include "Display.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Object.h"
#include "Sound.h"
#include "StringBuffer.h"
#include "ParticleObject.h"

#include "ForcePongConstants.h"
#include "ForcePongBot.h"
#include "ForcePongGameController.h"
#include "ShipFluidObject.h"
#include "Map.h"

class ForcePong{
private:

	////////////////Objects
	Map fluidMap;
	
	StringBuffer dmgStringBuffer;

	Object **objs;
	ParticleObject **particleObjs;

	ForcePongGameController gameController;

	//Other resources
	Sound musicBG;

	////////////////Variables
	bool showProjectilesLocks;
	bool keyboardOnly;

	//Resources

	////////////////Functions
	void changeDisplayMode();
	void cleanGame();
	void updateMouseInputs();
	void updateKeyboardInputs();
	void collisionDetection();
	void loadTextures();
	void makeGameSurvivor(ShipFluidObject *ships,int nShips);
	void show();
	void showUserSpecifics();
	void showProjecileTargetLocks();
	void update();
	void updateGame();
	void updateGameInputs();

public:
	ForcePong();
	void init();
	void run();
	void resize(VectorT windowDims,VectorT oldDims);
	void cleanup();
};

#endif