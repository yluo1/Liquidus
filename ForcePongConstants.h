#ifndef _FORCEPONGCONSTANTS_
#define _FORCEPONGCONSTANTS_

#include "Constants.h"
#include "Texture.h"
#include "VectorT.h"
#include "Object.h"
#include "Model.h"


//Definitions

//If defined, there will be one 256x256*3*sizeof(float) byte texture readback per frame
#define LARGETEXTURE_READS

//Gameplay defs
#define DEFAULT_DT			.01f
#define SIN_TABLE_SIZE		1000

//Weapon and ammo defs
#define MAX_WEAPON_TYPES		8
#define SUB_WEAPON_MULT			3

#define TYPE_LASER				0
#define TYPE_LASERII			1
#define TYPE_LASERIII			2
#define TYPE_LASER_TRACK		3
#define TYPE_LASER_TRACKII		4
#define TYPE_LASER_TRACKIII		5
#define TYPE_LASER_TELEPORT		6
#define TYPE_LASER_TELEPORTII	7
#define TYPE_LASER_TELEPORTIII	8
#define TYPE_LASER_GENESIS		9
#define TYPE_LASER_GENESISII	10
#define TYPE_LASER_GENESISIII	11
#define TYPE_MISSILE			12
#define TYPE_MISSILEII			13
#define TYPE_MISSILEIII			14
#define TYPE_MISSILE_TORQUE		15
#define TYPE_MISSILE_TORQUEII	16
#define TYPE_MISSILE_TORQUEIII	17
#define TYPE_MISSILE_HOME		18
#define TYPE_MISSILE_HOMEII		19
#define TYPE_MISSILE_HOMEIII	20
#define TYPE_MISSILE_CHAOS		21
#define TYPE_MISSILE_CHAOSII	22
#define TYPE_MISSILE_CHAOSIII	23

#define TYPE_IMPULSE_DIRECTIONAL	0
#define TYPE_IMPULSE_EXPLOSION		1

#define TYPE_OBJ_SHIP		0
#define TYPE_OBJ_WEAPON		1
#define TYPE_OBJ_LASER		2
#define TYPE_OBJ_MISSILE	3

#define TYPE_BOT_MISSILE	0
#define TYPE_BOT_LASER		1

//Other constants
#define CHAOTIC_MODE		true
#define MAX_IMPULSES		1000
#define JACOBI_ITERATIONS	25
#define DIFFUSION_ITERATIONS	0
#define MAX_FLUID_SPEED		1000

//Texture constants
#define MENU_TEXTURE_MAX			6
#define LIGHT_TEXTURE_MAX			1
#define AMMO_TEXTURE_MAX			16
#define TIME_TEXTURE_MAX			1

//Texture index refs
#define MENU_MAIN					0
#define MENU_MAIN_PLAY				1
#define MENU_MAIN_ABOUT				2
#define MENU_MAIN_OPTIONS			3

#define LIGHT_CORONA				0

#define AMMO_LASERII				0
#define AMMO_LASERIII				1
#define AMMO_GENESIS_LASERII		2
#define AMMO_GENESIS_LASERIII		3
#define AMMO_TRACKING_LASERII		4
#define AMMO_TRACKING_LASERIII		5
#define AMMO_TRACKING_TELEPORTII	6
#define AMMO_TRACKING_TELEPORTIII	7
#define AMMO_MISSILEII				8
#define AMMO_MISSILEIII				9
#define AMMO_TORQUE_MISSILEII		10	
#define AMMO_TORQUE_MISSILEIII		11	
#define AMMO_HOME_MISSILEII			12
#define AMMO_HOME_MISSILEIII		13
#define AMMO_CHAOSII				14
#define AMMO_CHAOSIII				15

#define TIME_DILATION				0

#define MAX_GAME_MODELS				2
#define MAX_BOTS					10

#define MODEL_SHIP_USER				0
#define MODEL_SHIP_DEFAULT			1

#define	DEFAULT_SHIP_MASS			10.0
#define DEFAULT_SHIP_RADIUS			13.0

//Extern globals
extern bool enableVorticity;
extern bool enableDynamicLight;


extern int objCount;
extern int particleCount;
extern int shipCount;
extern int userTarget;
extern int userShip;
extern bool observerMode;
extern bool singleFBO;

extern int displayMode;

extern float *defaultSinValues;
extern char defaultShipModelPath[];
extern char userShipModelPath[];
extern Model globalModels[MAX_GAME_MODELS];

//Game externs
extern float deltaTime;
extern float dxy;
extern float viscosity;

extern float globalScale;
extern float globalUnitSizeScale;
extern char userWeaponKeys[MAX_WEAPON_TYPES];

extern float velocityFrame[256*256*3];

//Texture externs
extern Texture *menuTextures;
extern Texture *lightTextures;
extern Texture *ammoTextures;
extern Texture *timeTextures;

//Types
typedef void (*ImpulseFunction)(VectorT position,VectorT color, float strength,float radius,int frameBufferID,int textureUnitID);

typedef struct{
	VectorT position[MAX_IMPULSES];
	VectorT color[MAX_IMPULSES];
	float strength[MAX_IMPULSES];
	float radius[MAX_IMPULSES];
	int frameBufferID[MAX_IMPULSES];
	int textureUnitID[MAX_IMPULSES];
	int impulseType[MAX_IMPULSES];
	bool implosion[MAX_IMPULSES];
	int count;
	ImpulseFunction impulseFunction[MAX_IMPULSES];
}ImpulseBuffer;

//Funtions
void loadForcePongTextures();
void loadForcePongModels();
void cleanupForcePongTextures();
void cleanupForcePongModels();
int findNextOpenObject(Object **firstObjectElement, int totalObjs);



#endif