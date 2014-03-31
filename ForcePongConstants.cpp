#include "ForcePongConstants.h"

bool enableVorticity=true;
bool enableDynamicLight=true;

int objCount=300;
int particleCount=333;
int shipCount=0;
bool observerMode=true;

float deltaTime=DEFAULT_DT;
float dxy=1.0f;
float viscosity=.5;

int userShip=0;
int userTarget;
int displayMode;

float globalScale=.7;

float *defaultSinValues=initDefaultSinValues(SIN_TABLE_SIZE);

char defaultShipModelPath[]="models/defaultModel.model";
char userShipModelPath[]="models/userModel.model";
char userWeaponKeys[MAX_WEAPON_TYPES]={'q','e','r','f','1','2','3','4'};
Texture *menuTextures=NULL;
Texture *lightTextures=NULL;
Texture *ammoTextures=NULL;
Texture *timeTextures=NULL;

bool singleFBO=true;

float velocityFrame[256*256*3];

Model globalModels[MAX_GAME_MODELS];

void loadForcePongTextures(){
	menuTextures=new Texture[MENU_TEXTURE_MAX];

	menuTextures[MENU_MAIN].loadTGA("Textures/mainPanel.tga",GL_RGB);
	menuTextures[MENU_MAIN_PLAY].loadTGA("Textures/mainPanelPlay.tga",GL_RGB);
	menuTextures[MENU_MAIN_ABOUT].loadTGA("Textures/mainPanelAbout.tga",GL_RGB);

	lightTextures=new Texture[LIGHT_TEXTURE_MAX];
	lightTextures[LIGHT_CORONA].loadTGA("Textures/lightSphere.tga",GL_RGB);
	
	ammoTextures=new Texture[AMMO_TEXTURE_MAX];
	ammoTextures[AMMO_LASERII].loadTGA("Textures/laserAmmo0.tga",GL_RGB);
	ammoTextures[AMMO_LASERIII].loadTGA("Textures/laserAmmo1.tga",GL_RGB);
	ammoTextures[AMMO_GENESIS_LASERII].loadTGA("Textures/laserAmmo2.tga",GL_RGB);
	ammoTextures[AMMO_GENESIS_LASERIII].loadTGA("Textures/laserAmmo7.tga",GL_RGB);
	ammoTextures[AMMO_TRACKING_LASERII].loadTGA("Textures/laserAmmo3.tga",GL_RGB);
	ammoTextures[AMMO_TRACKING_LASERIII].loadTGA("Textures/laserAmmo4.tga",GL_RGB);
	ammoTextures[AMMO_TRACKING_TELEPORTII].loadTGA("Textures/laserAmmo5.tga",GL_RGB);
	ammoTextures[AMMO_TRACKING_TELEPORTIII].loadTGA("Textures/laserAmmo6.tga",GL_RGB);
	ammoTextures[AMMO_MISSILEII].loadTGA("Textures/missileAmmo0.tga",GL_RGB);
	ammoTextures[AMMO_MISSILEIII].loadTGA("Textures/missileAmmo1.tga",GL_RGB);
	ammoTextures[AMMO_TORQUE_MISSILEII].loadTGA("Textures/missileAmmo2.tga",GL_RGB);
	ammoTextures[AMMO_TORQUE_MISSILEIII].loadTGA("Textures/missileAmmo3.tga",GL_RGB);
	ammoTextures[AMMO_HOME_MISSILEII].loadTGA("Textures/missileAmmo4.tga",GL_RGB);
	ammoTextures[AMMO_HOME_MISSILEIII].loadTGA("Textures/missileAmmo5.tga",GL_RGB);
	ammoTextures[AMMO_CHAOSII].loadTGA("Textures/missileAmmo6.tga",GL_RGB);
	ammoTextures[AMMO_CHAOSIII].loadTGA("Textures/missileAmmo7.tga",GL_RGB);

	timeTextures=new Texture[TIME_TEXTURE_MAX];
	timeTextures[TIME_DILATION].loadTGA("Textures/dilation.tga",GL_RGB);

}


void loadForcePongModels(){
	globalModels[MODEL_SHIP_DEFAULT].readModel(defaultShipModelPath);
	globalModels[MODEL_SHIP_USER].readModel(userShipModelPath);
}

void cleanupForcePongModels(){
	globalModels[MODEL_SHIP_DEFAULT].cleanup();
	globalModels[MODEL_SHIP_USER].cleanup();
}
void cleanupForcePongTextures(){
	delete [] menuTextures;
	delete [] lightTextures;
	delete [] ammoTextures;
	delete [] timeTextures;
}

int findNextOpenObject(Object **firstObjectElement, int totalObjs){
	for(int a=shipCount;a<totalObjs;++a){//Find element to add
		if(firstObjectElement[a]==NULL) return a;
	}
	return -1;
}