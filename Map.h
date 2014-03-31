#ifndef _MAP_
#define _MAP_

#include <stdlib.h>
#include "GL/glew.h"
#include "GL/glut.h"

#include "ConstantsIO.h"
#include "Display.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Texture.h"
#include "ShaderProgram.h"

#include "ForcePongConstants.h"
#include "ForcePongShaders.h"



class Map{
private:

	//Fluid Constants
	float dt;
	float dxy;
	float viscosity;


	//Texture IDs
	Texture velocityGrid;
	Texture divergenceGrid;
	Texture pressureGrid;
	Texture vorticityGrid;
	Texture inkGrid;
	Texture dynamicLightGrid;

	//Shader ids
	ShaderProgram *jacobiShader;
	ShaderProgram *advectionShader;
	ShaderProgram *boundaryShader;
	ShaderProgram *impulseShader;
	ShaderProgram *explosionShader;
	ShaderProgram *divergenceShader;
	ShaderProgram *gradientShader;
	ShaderProgram *vorticityShader;
	ShaderProgram *displayShader;
	ShaderProgram *addVorticityShader;
	ShaderProgram *dynamicLightShader;
	
	//Shader object locations
	GLint advectTexVelocityLoc,advectTexAdvectLoc,advectDtLoc,advectTexScaled,advectDxy,advectDissipation;
	GLint boundTexLoc,boundTexScaled,boundScale,boundOffset;
	GLint jacobiTexLoc,jacobiTexLoc2,jacobiTexScaled,jacobiAlpha,jacobiRBeta;
	GLint impulseOrigin, impulseRadius,impulseTexLoc,impulseTexScaled,impulseColor,impulseStrength;
	GLint explosionOrigin, explosionRadius,explosionTexLoc,explosionTexScaled,explosionStrength,explosionImplosion;
	GLint divergenceTexLoc,divergenceTexScaled, divergenceDxy;
	GLint gradientTexPressureLoc,gradientTexVelocityLoc,gradientTexScaled, gradientDxy;
	GLint vorticityTexVelocityLoc,vorticityDxy,vorticityTexScaled;
	GLint addVorticityTexVelocityLoc,addVorticityTexVorticityLoc,addVorticityTexScaled,addVorticityScale,addVorticityDxy,addVorticityDtLoc;
	GLint dynamicLightInkTexLoc,dynamicLightVelTexLoc;
	GLint displayTexLoc;
	

	//Impulse Buffer
	ImpulseBuffer impulseDirectionalBuffer;
	ImpulseBuffer impulseExplosionBuffer;

	int lastPixDatSize;
	////////////////////Member Functions

	void calculate();
	void renderBorder(VectorT bottomLeft,VectorT bottomRight,VectorT topLeft,VectorT topRight);
	void renderMap();
	void renderVectorTMap();

public:
	//Framebuffer ids
	GLuint velocityFrameBuffer,divergenceFrameBuffer,pressureFrameBuffer,inkFrameBuffer,vorticityFrameBuffer,dynamicLightFrameBuffer;
	GLuint frameBufferWidth,frameBufferHeight;
	GLfloat *pixDat;

	Map();
	Map(float dt,float dxy,float viscosity,
		int frameBufferWidth,int frameBufferHeight);
	~Map();

	void addImpulseToBuffer(VectorT position,VectorT color, float strength,float radius,int frameBufferID,int textureUnitID,int impulseType);
	void addImpulseToBuffer(VectorT position,VectorT color, float strength,float radius,int frameBufferID,int textureUnitID,int impulseType,bool implosion);
	void applyImpulse();
	void enableTextures();
	void disableTextures();
	void renderImpulse(int textureUnitID,float radius,VectorT color,float xNorm,float yNorm,float strength,int impulseType,bool implosion);
	void show(int mode);
	void update();

	void cleanup();

};
#endif