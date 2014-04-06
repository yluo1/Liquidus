#include "Map.h"

Map::Map(){
}

Map::Map(float dt,float dxy,float viscosity,int frameBufferWidth,int frameBufferHeight){
	this->dt=dt;
	this->dxy=dxy;
	this->viscosity=viscosity;

	this->frameBufferWidth=frameBufferWidth;
	this->frameBufferHeight=frameBufferHeight;
	pixDat=NULL;

	int internalFormat=GL_RGBA_FLOAT32_ATI;

	if(frameBufferWidth<0||frameBufferHeight<0){
		velocityGrid.loadTGA("Textures/initialFluidVel.tga",internalFormat);
		this->frameBufferWidth=velocityGrid.width;
		this->frameBufferHeight=velocityGrid.height;
	}else velocityGrid.makeBlankTexture(frameBufferWidth,frameBufferHeight,internalFormat,GL_RGBA,GL_FLOAT);

	
	pressureGrid.makeBlankTexture(velocityGrid.width,velocityGrid.height,internalFormat,GL_RGBA,GL_FLOAT);
	divergenceGrid.makeBlankTexture(velocityGrid.width,velocityGrid.height,internalFormat,GL_RGBA,GL_FLOAT);
	//inkGrid.makeBlankTexture(velocityGrid.width,velocityGrid.height,internalFormat,GL_RGBA,GL_FLOAT);
	inkGrid.loadTGA("Textures/initialFluidInk.tga",internalFormat);
	vorticityGrid.makeBlankTexture(velocityGrid.width,velocityGrid.height,internalFormat,GL_RGBA,GL_FLOAT);
	dynamicLightGrid.makeBlankTexture(velocityGrid.width,velocityGrid.height,internalFormat,GL_RGBA,GL_FLOAT);

	//Setup shaders
	advectionShader=new		ShaderProgram(NULL,advectionFragment,0,(int)strlen(advectionFragment));
	boundaryShader=new		ShaderProgram(NULL,boundaryFragment,0,(int)strlen(boundaryFragment));
	jacobiShader=new		ShaderProgram(NULL,jacobiFragment,0,(int)strlen(jacobiFragment));
	impulseShader=new		ShaderProgram(NULL,impulseFragment,0,(int)strlen(impulseFragment));
	explosionShader=new		ShaderProgram(NULL,explosionFragment,0,(int)strlen(explosionFragment));
	divergenceShader=new	ShaderProgram(NULL,divergenceFragment,0,(int)strlen(divergenceFragment));
	gradientShader=new		ShaderProgram(NULL,gradientFragment,0,(int)strlen(gradientFragment));
	displayShader=new		ShaderProgram(NULL,displayFragment,0,(int)strlen(displayFragment));
	vorticityShader=new		ShaderProgram(NULL,vorticityFragment,0,(int)strlen(vorticityFragment));
	addVorticityShader=new	ShaderProgram(NULL,addVorticityFragment,0,(int)strlen(addVorticityFragment));
	dynamicLightShader=new  ShaderProgram(NULL,dynamicLightFragment,0,(int)strlen(dynamicLightFragment));

	//Find uniforms
	advectDtLoc=glGetUniformLocation(advectionShader->shaderProgramID,"deltaTime");
	advectTexVelocityLoc=glGetUniformLocation(advectionShader->shaderProgramID,"texVelocity");
	advectTexAdvectLoc=glGetUniformLocation(advectionShader->shaderProgramID,"texAdvect");
	advectTexScaled=glGetUniformLocation(advectionShader->shaderProgramID,"texScaled");
	advectDxy=glGetUniformLocation(advectionShader->shaderProgramID,"dxy");
	advectDissipation=glGetUniformLocation(advectionShader->shaderProgramID,"dissipation");

	boundTexLoc=glGetUniformLocation(boundaryShader->shaderProgramID,"tex");
	boundTexScaled=glGetUniformLocation(boundaryShader->shaderProgramID,"texScaled");
	boundScale=glGetUniformLocation(boundaryShader->shaderProgramID,"scale");
	boundOffset=glGetUniformLocation(boundaryShader->shaderProgramID,"offset");

	jacobiTexLoc=glGetUniformLocation(jacobiShader->shaderProgramID,"tex");
	jacobiTexLoc2=glGetUniformLocation(jacobiShader->shaderProgramID,"tex2");
	jacobiTexScaled=glGetUniformLocation(jacobiShader->shaderProgramID,"texScaled");
	jacobiAlpha=glGetUniformLocation(jacobiShader->shaderProgramID,"alpha");
	jacobiRBeta=glGetUniformLocation(jacobiShader->shaderProgramID,"rBeta");
	
	impulseOrigin=glGetUniformLocation(impulseShader->shaderProgramID,"origin");
	impulseRadius=glGetUniformLocation(impulseShader->shaderProgramID,"radius");
	impulseTexLoc=glGetUniformLocation(impulseShader->shaderProgramID,"tex");
	impulseTexScaled=glGetUniformLocation(impulseShader->shaderProgramID,"texScaled");
	impulseColor=glGetUniformLocation(impulseShader->shaderProgramID,"impulseColor");
	impulseStrength=glGetUniformLocation(impulseShader->shaderProgramID,"impulseStrength");

	explosionOrigin=glGetUniformLocation(explosionShader->shaderProgramID,"origin");
	explosionRadius=glGetUniformLocation(explosionShader->shaderProgramID,"radius");
	explosionTexLoc=glGetUniformLocation(explosionShader->shaderProgramID,"tex");
	explosionTexScaled=glGetUniformLocation(explosionShader->shaderProgramID,"texScaled");
	explosionStrength=glGetUniformLocation(explosionShader->shaderProgramID,"impulseStrength");
	explosionImplosion=glGetUniformLocation(explosionShader->shaderProgramID,"implosion");

	divergenceTexLoc=glGetUniformLocation(divergenceShader->shaderProgramID,"tex");
	divergenceTexScaled=glGetUniformLocation(divergenceShader->shaderProgramID,"texScaled");
	divergenceDxy=glGetUniformLocation(divergenceShader->shaderProgramID,"dxy");

	gradientTexPressureLoc=glGetUniformLocation(gradientShader->shaderProgramID,"texPressure");
	gradientTexVelocityLoc=glGetUniformLocation(gradientShader->shaderProgramID,"texVelocity");
	gradientTexScaled=glGetUniformLocation(gradientShader->shaderProgramID,"texScaled");
	gradientDxy=glGetUniformLocation(gradientShader->shaderProgramID,"dxy");

	vorticityTexVelocityLoc=glGetUniformLocation(vorticityShader->shaderProgramID,"texVelocity");
	vorticityDxy=glGetUniformLocation(vorticityShader->shaderProgramID,"dxy");
	vorticityTexScaled=glGetUniformLocation(vorticityShader->shaderProgramID,"texScaled");

	addVorticityTexVelocityLoc=		glGetUniformLocation(addVorticityShader->shaderProgramID,"texVelocity");
	addVorticityTexVorticityLoc=	glGetUniformLocation(addVorticityShader->shaderProgramID,"texVorticity");
	addVorticityTexScaled=			glGetUniformLocation(addVorticityShader->shaderProgramID,"texScaled");
	addVorticityScale=				glGetUniformLocation(addVorticityShader->shaderProgramID,"vorticityScale");
	addVorticityDxy=				glGetUniformLocation(addVorticityShader->shaderProgramID,"dxy");
	addVorticityDtLoc=				glGetUniformLocation(addVorticityShader->shaderProgramID,"deltaTime");

	dynamicLightInkTexLoc=glGetUniformLocation(dynamicLightShader->shaderProgramID,"inkTex");
	dynamicLightVelTexLoc=glGetUniformLocation(dynamicLightShader->shaderProgramID,"velTex");

	displayTexLoc=glGetUniformLocation(displayShader->shaderProgramID,"tex");
	
	//Set and pass texture units
	enableTextures();

	advectionShader->setActive();
	glUniform1f(advectDxy,dxy);
	glUniform1f(advectDtLoc,dt);
	glUniform1f(advectTexScaled,1.0/velocityGrid.width);
	glUniform1i(advectTexVelocityLoc,0);
	glUniform1i(advectTexAdvectLoc,0);
	glUniform1f(advectDissipation,0);
	advectionShader->setInactive();

	boundaryShader->setActive();
	glUniform1i(boundTexLoc,0);
	glUniform1f(boundTexScaled,1.0/velocityGrid.width);
	glUniform1f(boundScale,-1);
	boundaryShader->setInactive();

	jacobiShader->setActive();
	glUniform1f(jacobiTexScaled,1.0/velocityGrid.width);
	glUniform1i(jacobiTexLoc,0);
	glUniform1i(jacobiTexLoc2,0);
	float alpha=dxy*dxy/(viscosity*dt);
	glUniform1f(jacobiAlpha,alpha);
	glUniform1f(jacobiRBeta,1.0f/(4.0+alpha));
	jacobiShader->setInactive();

	impulseShader->setActive();
	glUniform1f(impulseTexScaled,1.0/velocityGrid.width);
	impulseShader->setInactive();

	explosionShader->setActive();
	glUniform1f(explosionTexScaled,1.0/velocityGrid.width);
	glUniform1f(explosionImplosion,false);
	explosionShader->setInactive();

	divergenceShader->setActive();
	glUniform1f(divergenceDxy,dxy);
	glUniform1f(divergenceTexScaled,1.0/velocityGrid.width);
	glUniform1i(divergenceTexLoc,0);
	divergenceShader->setInactive();

	gradientShader->setActive();
	glUniform1f(gradientDxy,dxy);
	glUniform1f(gradientTexScaled,1.0/velocityGrid.width);
	glUniform1i(gradientTexPressureLoc,2);
	glUniform1i(gradientTexVelocityLoc,0);
	gradientShader->setInactive();

	vorticityShader->setActive();
	glUniform1f(vorticityDxy,dxy);
	glUniform1f(vorticityTexScaled,1.0/velocityGrid.width);
	glUniform1i(vorticityTexVelocityLoc,0);
	vorticityShader->setInactive();

	addVorticityShader->setActive();
	glUniform1f(addVorticityDxy,dxy);
	glUniform1f(addVorticityTexScaled,1.0/velocityGrid.width);
	glUniform1f(addVorticityScale,10.0);
	glUniform1f(addVorticityDtLoc,dt);
	glUniform1i(addVorticityTexVelocityLoc,0);
	glUniform1i(addVorticityTexVorticityLoc,4);
	addVorticityShader->setInactive();

	dynamicLightShader->setActive();
	glUniform1i(dynamicLightInkTexLoc,3);
	glUniform1i(dynamicLightVelTexLoc,0);
	dynamicLightShader->setInactive();

	//Setup framebuffers
	GLint maxBuffers;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxBuffers);
//maxBuffers = 3;

	if(maxBuffers>=4){
		singleFBO=true;
		//Use one FBO with multiple textures bind to 4 color attachments
		glGenFramebuffersEXT(1,&velocityFrameBuffer);

		divergenceFrameBuffer=pressureFrameBuffer=inkFrameBuffer=velocityFrameBuffer;
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,velocityFrameBuffer);

		//Bind textures to color attachments 0-3
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
								  GL_COLOR_ATTACHMENT0_EXT,
								  GL_TEXTURE_2D,velocityGrid.texID,0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
								  GL_COLOR_ATTACHMENT1_EXT,
								  GL_TEXTURE_2D,divergenceGrid.texID,0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
								  GL_COLOR_ATTACHMENT2_EXT,
								  GL_TEXTURE_2D,pressureGrid.texID,0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
								  GL_COLOR_ATTACHMENT3_EXT,
								  GL_TEXTURE_2D,inkGrid.texID,0);

		//Make FBO#2 for vorticity framebuffer
		glGenFramebuffersEXT(1,&vorticityFrameBuffer);
		dynamicLightFrameBuffer=vorticityFrameBuffer;

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,vorticityFrameBuffer);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
							  GL_COLOR_ATTACHMENT0_EXT,
							  GL_TEXTURE_2D,vorticityGrid.texID,0);

		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
							  GL_COLOR_ATTACHMENT1_EXT,
							  GL_TEXTURE_2D,dynamicLightGrid.texID,0);
		


	}else{
		//Use 4 FBO using color attachment 1
		singleFBO=false;
		printf("Your GPU does not support 4 or more color attachments. Will attempt to use multiple FBOs\n");
	
		glGenFramebuffersEXT(1,&velocityFrameBuffer);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,velocityFrameBuffer);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
								  GL_COLOR_ATTACHMENT0_EXT, 
								  GL_TEXTURE_2D,velocityGrid.texID,0);

		glGenFramebuffersEXT(1,&divergenceFrameBuffer);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,divergenceFrameBuffer);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
								  GL_COLOR_ATTACHMENT0_EXT, 
								  GL_TEXTURE_2D,divergenceGrid.texID,0);

		glGenFramebuffersEXT(1,&pressureFrameBuffer);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,pressureFrameBuffer);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
								  GL_COLOR_ATTACHMENT0_EXT, 
								  GL_TEXTURE_2D,pressureGrid.texID,0);

		glGenFramebuffersEXT(1,&inkFrameBuffer);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,inkFrameBuffer);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
								  GL_COLOR_ATTACHMENT0_EXT, 
								  GL_TEXTURE_2D,inkGrid.texID,0);

		glGenFramebuffersEXT(1,&vorticityFrameBuffer);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,vorticityFrameBuffer);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
								  GL_COLOR_ATTACHMENT0_EXT, 
								  GL_TEXTURE_2D,vorticityGrid.texID,0);

		glGenFramebuffersEXT(1,&dynamicLightFrameBuffer);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,dynamicLightFrameBuffer);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
							  GL_COLOR_ATTACHMENT0_EXT,
							  GL_TEXTURE_2D,dynamicLightGrid.texID,0);


	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);


	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status!=GL_FRAMEBUFFER_COMPLETE_EXT){
		printf("FBO Error\n");
		if(status==GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT) printf("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT error\n");
		if(status==GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT) printf("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT error\n");
		//if(status==GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT) printf("GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT error\n");
		if(status==GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT) printf("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT error\n");
		if(status==GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT) printf("GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT error\n");
		if(status==GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT) printf("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT error\n");
		if(status==GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT) printf("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT error\n");
		if(status==GL_FRAMEBUFFER_UNSUPPORTED_EXT) printf("GL_FRAMEBUFFER_UNSUPPORTED_EXT error\n");
		getchar();
		exit(0);
	}

	lastPixDatSize=0;
	impulseDirectionalBuffer.count=0;
	impulseExplosionBuffer.count=0;
}

Map::~Map(){

}

void Map::cleanup(){
	delete jacobiShader;
	delete advectionShader;
	delete boundaryShader;
	delete impulseShader;
	delete explosionShader;
	delete divergenceShader;
	delete gradientShader;
	delete displayShader;
	delete vorticityShader;
	delete addVorticityShader;
	delete dynamicLightShader;

	if(singleFBO) glDeleteFramebuffersEXT(1, &velocityFrameBuffer);
	else{
		glDeleteFramebuffersEXT(1, &velocityFrameBuffer);
		glDeleteFramebuffersEXT(1, &divergenceFrameBuffer);
		glDeleteFramebuffersEXT(1, &pressureFrameBuffer);
		glDeleteFramebuffersEXT(1, &inkFrameBuffer);
		glDeleteFramebuffersEXT(1, &vorticityFrameBuffer);
		glDeleteFramebuffersEXT(1, &dynamicLightFrameBuffer);
	}

	glDeleteTextures(1,&velocityGrid.texID);
	glDeleteTextures(1,&divergenceGrid.texID);
	glDeleteTextures(1,&pressureGrid.texID);
	glDeleteTextures(1,&inkGrid.texID);
	glDeleteTextures(1,&vorticityGrid.texID);
	glDeleteTextures(1,&dynamicLightGrid.texID);

	if(pixDat!=NULL) delete []pixDat;
}

void Map::addImpulseToBuffer(VectorT position,VectorT color, float strength,float radius,int frameBufferID,int textureUnitID,int impulseType){
	if(position.x>1.0||position.x<0||position.y>1.0||position.y<0) return;
	if(impulseType==TYPE_IMPULSE_DIRECTIONAL){
		int a=impulseDirectionalBuffer.count;
		if(a<MAX_IMPULSES){
			impulseDirectionalBuffer.position[a]=position;
			impulseDirectionalBuffer.color[a]=color;
			impulseDirectionalBuffer.strength[a]=strength;
			impulseDirectionalBuffer.radius[a]=radius;
			impulseDirectionalBuffer.frameBufferID[a]=frameBufferID;
			impulseDirectionalBuffer.textureUnitID[a]=textureUnitID;
			impulseDirectionalBuffer.impulseType[a]=impulseType;
			impulseDirectionalBuffer.implosion[a]=false;
			impulseDirectionalBuffer.count++;
		}
	}else if(impulseType==TYPE_IMPULSE_EXPLOSION){
		int a=impulseExplosionBuffer.count;
		if(a<MAX_IMPULSES){
			impulseExplosionBuffer.position[a]=position;
			impulseExplosionBuffer.color[a]=color;
			impulseExplosionBuffer.strength[a]=strength;
			impulseExplosionBuffer.radius[a]=radius;
			impulseExplosionBuffer.frameBufferID[a]=frameBufferID;
			impulseExplosionBuffer.textureUnitID[a]=textureUnitID;
			impulseExplosionBuffer.impulseType[a]=impulseType;
			impulseExplosionBuffer.implosion[a]=false;
			impulseExplosionBuffer.count++;
		}
	}
}

void Map::addImpulseToBuffer(VectorT position,VectorT color, float strength,float radius,int frameBufferID,int textureUnitID,int impulseType,bool implosion){
	if(position.x>1.0||position.x<0||position.y>1.0||position.y<0) return;
	if(impulseType==TYPE_IMPULSE_DIRECTIONAL){
		int a=impulseDirectionalBuffer.count;
		if(a<MAX_IMPULSES){
			impulseDirectionalBuffer.position[a]=position;
			impulseDirectionalBuffer.color[a]=color;
			impulseDirectionalBuffer.strength[a]=strength;
			impulseDirectionalBuffer.radius[a]=radius;
			impulseDirectionalBuffer.frameBufferID[a]=frameBufferID;
			impulseDirectionalBuffer.textureUnitID[a]=textureUnitID;
			impulseDirectionalBuffer.impulseType[a]=impulseType;
			impulseDirectionalBuffer.implosion[a]=implosion;
			impulseDirectionalBuffer.count++;
		}
	}else if(impulseType==TYPE_IMPULSE_EXPLOSION){
		int a=impulseExplosionBuffer.count;
		if(a<MAX_IMPULSES){
			impulseExplosionBuffer.position[a]=position;
			impulseExplosionBuffer.color[a]=color;
			impulseExplosionBuffer.strength[a]=strength;
			impulseExplosionBuffer.radius[a]=radius;
			impulseExplosionBuffer.frameBufferID[a]=frameBufferID;
			impulseExplosionBuffer.textureUnitID[a]=textureUnitID;
			impulseExplosionBuffer.impulseType[a]=impulseType;
			impulseExplosionBuffer.implosion[a]=implosion;
			impulseExplosionBuffer.count++;
		}
	}
}


void Map::calculate(){
	glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT |GL_ENABLE_BIT);
	glDisable(GL_BLEND);
	glViewport(0,0,frameBufferWidth,frameBufferHeight);

	///////////////////////////////Velocity Framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,velocityFrameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	
	//Advect Velocity
	advectionShader->setActive();
	glUniform1i(advectTexAdvectLoc,0);
	renderMap();
	advectionShader->setInactive();

	////////////////////Vorticity Buffer
	if(enableVorticity){
		if(singleFBO){
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,vorticityFrameBuffer);
			glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT); 
		}else glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,vorticityFrameBuffer);
		vorticityShader->setActive();
		renderMap();
		vorticityShader->setInactive();

		//Apply vorticity confinement to Velocity
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,velocityFrameBuffer);
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		addVorticityShader->setActive();
		renderMap();
		addVorticityShader->setInactive();
	}


	//Diffuse Velocity
	jacobiShader->setActive();
	glUniform1i(jacobiTexLoc,0);
	glUniform1i(jacobiTexLoc2,0);
	float alpha=dxy*dxy/(viscosity*dt);
	glUniform1f(jacobiAlpha,alpha);
	glUniform1f(jacobiRBeta,1.0/(4.0+alpha));
	for(int a=0;a<DIFFUSION_ITERATIONS;++a) renderMap();
	jacobiShader->setInactive();


	//Boundary Border to velocity
	boundaryShader->setActive();
	glUniform1i(boundTexLoc,0);
	glUniform1i(boundScale,1);
	renderMap();
	boundaryShader->setInactive();

	
	///////////////////////////////Divergence Framebuffer
	if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
	else glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,divergenceFrameBuffer);

	divergenceShader->setActive();
	renderMap();
	divergenceShader->setInactive();

	///////////////////////////////Pressure Framebuffer
	if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT); 
	else glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,pressureFrameBuffer);

	
	jacobiShader->setActive();
	glUniform1i(jacobiTexLoc,2);
	glUniform1i(jacobiTexLoc2,1);
	glUniform1f(jacobiAlpha,-(dxy*dxy));
	glUniform1f(jacobiRBeta,1.0/4.0);
	for(int a=0;a<JACOBI_ITERATIONS;++a) renderMap();
	jacobiShader->setInactive();

	///////////////////////////////Boundary Pressure Framebuffer

	boundaryShader->setActive();
	glUniform1i(boundTexLoc,2);
	glUniform1i(boundScale,0);
	renderMap();
	boundaryShader->setInactive();

	///////////////////////////////Subtract gradient in Velocity Framebuffer
	if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	else glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,velocityFrameBuffer);
	
	gradientShader->setActive();
	renderMap();
	gradientShader->setInactive();

	////////////////////Ink Buffer
	if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT); 
	else glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,inkFrameBuffer);

	advectionShader->setActive();
	glUniform1i(advectTexAdvectLoc,3);
	renderMap();
	advectionShader->setInactive();

	///////////////////Dynamic lighting
	if(enableDynamicLight&&displayMode==5){
		if(singleFBO){
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,dynamicLightFrameBuffer);
			glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT); 
		}else glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,dynamicLightFrameBuffer);
		dynamicLightShader->setActive();
		renderMap();
		dynamicLightShader->setInactive();
	}

	///////////////////////////////Normal Framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
	if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	glPopAttrib();

}

void Map::enableTextures(){
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, velocityGrid.texID);
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, divergenceGrid.texID);
	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, pressureGrid.texID);
	glActiveTexture(GL_TEXTURE3);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, inkGrid.texID);

	glActiveTexture(GL_TEXTURE4);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, vorticityGrid.texID);
	glActiveTexture(GL_TEXTURE5);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, dynamicLightGrid.texID);

}

void Map::disableTextures(){
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE3);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE4);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE5);
	glDisable(GL_TEXTURE_2D);

}

void Map::renderBorder(VectorT bottomLeft,VectorT bottomRight,VectorT topLeft,VectorT topRight){

	glUniform2f(boundOffset,0,-1);//Bottom Bar
	glBegin(GL_LINES);
	glTexCoord2d(bottomLeft.x/defaultDisplay.windowWidth,1-bottomLeft.y/defaultDisplay.windowHeight);
	glVertex2f(bottomLeft.x,bottomLeft.y);
	glTexCoord2d(bottomRight.x/defaultDisplay.windowWidth,1-bottomRight.y/defaultDisplay.windowHeight);
	glVertex2f(bottomRight.x,bottomRight.y);
	glEnd();

	glUniform2f(boundOffset,-1,0);//Right bar
	glBegin(GL_LINES);
	glTexCoord2d(bottomRight.x/defaultDisplay.windowWidth,1-bottomRight.y/defaultDisplay.windowHeight);
	glVertex2f(bottomRight.x,bottomRight.y);
	glTexCoord2d(topRight.x/defaultDisplay.windowWidth,1-topRight.y/defaultDisplay.windowHeight);
	glVertex2f(topRight.x,topRight.y);
	glEnd();

	glUniform2f(boundOffset,0,1);//Top bar
	glBegin(GL_LINES);
	glTexCoord2d(topRight.x/defaultDisplay.windowWidth,1-topRight.y/defaultDisplay.windowHeight);
	glVertex2f(topRight.x,topRight.y);
	glTexCoord2d(topLeft.x/defaultDisplay.windowWidth,1-topLeft.y/defaultDisplay.windowHeight);
	glVertex2f(topLeft.x,topLeft.y);
	glEnd();

	glUniform2f(boundOffset,1,0);//Left Bar
	glBegin(GL_LINES);
	glTexCoord2d(topLeft.x/defaultDisplay.windowWidth,1-topLeft.y/defaultDisplay.windowHeight);
	glVertex2f(topLeft.x,topLeft.y);
	glTexCoord2d(bottomLeft.x/defaultDisplay.windowWidth,1-bottomLeft.y/defaultDisplay.windowHeight);
	glVertex2f(bottomLeft.x,bottomLeft.y);
	glEnd();

}
void Map::renderMap(){

	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2d(0,1);
		glVertex2f(0,0);//Top Left

		glTexCoord2d(0,0);
		glVertex2f(0,defaultDisplay.windowHeight);//top  Left
		
		glTexCoord2d(1,1);
		glVertex2f(defaultDisplay.windowWidth,0);//bottom Right
		
		glTexCoord2d(1,0);
		glVertex2f(defaultDisplay.windowWidth,defaultDisplay.windowHeight);//top Right
	glEnd();
	

}

void Map::renderImpulse(int textureUnitID,float radius,VectorT color,float xNorm,float yNorm,float strength,int impulseType,bool implosion){

	VectorT position=VectorT(xNorm*defaultDisplay.windowWidth,(1.0-yNorm)*defaultDisplay.windowHeight,0);
	switch(impulseType){
		case TYPE_IMPULSE_DIRECTIONAL: 

			radius*=defaultDisplay.windowWidth/frameBufferWidth;
			glColor4f(color.x,color.y,color.z,strength);
			glBegin(GL_TRIANGLE_FAN);//Ship body
			glVertex2f(position.x,position.y);
			for(int a=0;a<20;++a)
				glVertex2f(	position.x+defaultSinValues[(SIN_TABLE_SIZE/4+SIN_TABLE_SIZE/20*a)%SIN_TABLE_SIZE]*radius,
							position.y+defaultSinValues[SIN_TABLE_SIZE/20*a]*radius);
			
			glVertex2f(	position.x+defaultSinValues[SIN_TABLE_SIZE/4]*radius,
						position.y+defaultSinValues[0]*radius);
			glEnd();
			
			break;
		case TYPE_IMPULSE_EXPLOSION: 
			explosionShader->setActive();
			glUniform1f(explosionRadius,radius);
			glUniform2f(explosionOrigin,xNorm,yNorm);
			glUniform1i(explosionTexLoc,textureUnitID);
			glUniform1f(explosionStrength,strength);
			glUniform1i(explosionImplosion,implosion);
			renderMap();
			explosionShader->setInactive();
			break;
	};
}


void Map::renderVectorTMap(){

	if(lastPixDatSize!=defaultDisplay.windowWidth*defaultDisplay.windowHeight*3){
		if(pixDat!=NULL) delete [] pixDat;
		lastPixDatSize=defaultDisplay.windowWidth*defaultDisplay.windowHeight*3;
		pixDat=new GLfloat[lastPixDatSize];
	}
	glReadPixels(0,0,defaultDisplay.windowWidth,defaultDisplay.windowHeight,GL_RGB,GL_FLOAT,pixDat);

	glColor3f(1,0,0);
	for(int a=0;a<defaultDisplay.windowWidth;a+=16){//x
		for(int b=0;b<defaultDisplay.windowHeight;b+=16){//y
			glBegin(GL_LINES);
			glVertex2f(a,defaultDisplay.windowHeight-1-b);
			glVertex2f(a+(pixDat[3*(b*defaultDisplay.windowWidth+a)+0]-.5)*20.0,
					defaultDisplay.windowHeight-1-b+(pixDat[3*(b*defaultDisplay.windowWidth+a)+1]-.5)*20.0);
			glEnd();
		}
	}

}

void Map::update(){
	applyImpulse();

	enableTextures();
	calculate();

	//Reset buffers
	impulseDirectionalBuffer.count=0;
	impulseExplosionBuffer.count=0;
}

void Map::show(int mode){
	
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_BLEND);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	displayShader->setActive();
	glUniform1i(displayTexLoc,mode);
	renderMap();
	displayShader->setInactive();
	glPopAttrib();
}
void Map::applyImpulse(){
	glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT |GL_ENABLE_BIT);
	glDisable(GL_BLEND);
	glViewport(0,0,frameBufferWidth,frameBufferHeight);
	if(singleFBO) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,velocityFrameBuffer);
	int lastFrameBufferID=-1;

	//Apply directional impulses
	disableTextures();
	for(int a=0;a<impulseDirectionalBuffer.count;++a){
		if(singleFBO){
			int b=impulseDirectionalBuffer.textureUnitID[a];
			if(b==0){
				glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
			}else if(b==1){
				glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
			}else if(b==2){
				glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);
			}else if(b==3){
				glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT);
			}else if(b==4){
				if(impulseDirectionalBuffer.frameBufferID[a]!=lastFrameBufferID){
					lastFrameBufferID=impulseDirectionalBuffer.frameBufferID[a];
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,impulseDirectionalBuffer.frameBufferID[a]);
					glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
				}
			}else if(b==5){
				if(impulseDirectionalBuffer.frameBufferID[a]!=lastFrameBufferID){
					lastFrameBufferID=impulseDirectionalBuffer.frameBufferID[a];
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,impulseDirectionalBuffer.frameBufferID[a]);
					glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
				}
			}
		}else{
			if(impulseDirectionalBuffer.frameBufferID[a]!=lastFrameBufferID){
				lastFrameBufferID=impulseDirectionalBuffer.frameBufferID[a];
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,impulseDirectionalBuffer.frameBufferID[a]);
			}
		}

		renderImpulse(impulseDirectionalBuffer.textureUnitID[a],
			impulseDirectionalBuffer.radius[a],
			impulseDirectionalBuffer.color[a],
			impulseDirectionalBuffer.position[a].x,impulseDirectionalBuffer.position[a].y,
			impulseDirectionalBuffer.strength[a],
			impulseDirectionalBuffer.impulseType[a],
			impulseDirectionalBuffer.implosion[a]);
	}

	//Apply explosive impulse
	enableTextures();
	for(int a=0;a<impulseExplosionBuffer.count;++a){
		if(singleFBO){
			int b=impulseExplosionBuffer.textureUnitID[a];
			if(b==0){
				glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
			}else if(b==1){
				glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
			}else if(b==2){
				glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);
			}else if(b==3){
				glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT);
			}else if(b==4){
				if(impulseDirectionalBuffer.frameBufferID[a]!=lastFrameBufferID){
					lastFrameBufferID=impulseDirectionalBuffer.frameBufferID[a];
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,impulseDirectionalBuffer.frameBufferID[a]);
					glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
				}
			}else if(b==5){
				if(impulseDirectionalBuffer.frameBufferID[a]!=lastFrameBufferID){
					lastFrameBufferID=impulseDirectionalBuffer.frameBufferID[a];
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,impulseDirectionalBuffer.frameBufferID[a]);
					glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
				}
			}
		}else{
			if(impulseExplosionBuffer.frameBufferID[a]!=lastFrameBufferID){
				lastFrameBufferID=impulseExplosionBuffer.frameBufferID[a];
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,impulseExplosionBuffer.frameBufferID[a]);
			}
		}
		renderImpulse(impulseExplosionBuffer.textureUnitID[a],
			impulseExplosionBuffer.radius[a],
			impulseExplosionBuffer.color[a],
			impulseExplosionBuffer.position[a].x,impulseExplosionBuffer.position[a].y,
			impulseExplosionBuffer.strength[a],
			impulseExplosionBuffer.impulseType[a],
			impulseExplosionBuffer.implosion[a]);
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	
	glPopAttrib();
}
