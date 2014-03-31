#ifndef _SHADERPROGRAM_
#define _SHADERPROGRAM_

#include "Shader.h"
#include <vector>

using namespace std;

class ShaderProgram{
private:
	GLuint vertexShaderID;
	GLuint fragmentShaderID;

public:
	GLuint shaderProgramID;

	ShaderProgram(GLuint vertexShaderID,GLuint fragmentShaderID){

		this->vertexShaderID=vertexShaderID;
		this->fragmentShaderID=fragmentShaderID;

		shaderProgramID=glCreateProgram();
		
		if(vertexShaderID!=-1) glAttachShader(shaderProgramID,vertexShaderID);
		if(fragmentShaderID!=-1) glAttachShader(shaderProgramID,fragmentShaderID);

		glLinkProgram(shaderProgramID);
	}

	ShaderProgram(char *vertexShaderFileName,char *fragmentShaderFileName){
		Shader* vertexShader=NULL;
		Shader* fragmentShader=NULL;

		vertexShaderID=-1;
		fragmentShaderID=-1;

		shaderProgramID=glCreateProgram();
		
		int linked;

		if(vertexShaderFileName!=NULL){
			vertexShader=new Shader(GL_VERTEX_SHADER,vertexShaderFileName);
			vertexShaderID=vertexShader->shaderID;
			glAttachShader(shaderProgramID,vertexShader->shaderID);
			delete vertexShader;

		}
		if(fragmentShaderFileName!=NULL){
			fragmentShader=new Shader(GL_FRAGMENT_SHADER,fragmentShaderFileName);
			fragmentShaderID=fragmentShader->shaderID;
			glAttachShader(shaderProgramID,fragmentShader->shaderID);
			delete fragmentShader;
		}

		glLinkProgram(shaderProgramID);
		glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &linked);
		if(!linked)	printf("Shader Linking error\n");
		int info=0;
		glGetProgramiv(shaderProgramID,GL_INFO_LOG_LENGTH,&info);
		if(info>1){
			char *infoLog=new char[info];
			glGetProgramInfoLog(shaderProgramID,info,&info,infoLog);
			printf("Shader Program Error\n%s\n",infoLog);
			delete [] infoLog;
		}

	}

	ShaderProgram(char *vertexShaderSource,char *fragmentShaderSource,int vertexShaderSourceLen, int fragmentShaderSourceLen){
		Shader* vertexShader=NULL;
		Shader* fragmentShader=NULL;

		vertexShaderID=-1;
		fragmentShaderID=-1;

		shaderProgramID=glCreateProgram();
		
		int linked;

		if(vertexShaderSource!=NULL){
			vertexShader=new Shader(GL_VERTEX_SHADER,vertexShaderSource,vertexShaderSourceLen);
			vertexShaderID=vertexShader->shaderID;
			glAttachShader(shaderProgramID,vertexShader->shaderID);
			delete vertexShader;

		}
		if(fragmentShaderSource!=NULL){
			fragmentShader=new Shader(GL_FRAGMENT_SHADER,fragmentShaderSource,fragmentShaderSourceLen);
			fragmentShaderID=fragmentShader->shaderID;
			glAttachShader(shaderProgramID,fragmentShader->shaderID);
			delete fragmentShader;
		}

		glLinkProgram(shaderProgramID);
		glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &linked);
		if(!linked)	printf("Shader Linking error\n");
		int info=0;
		glGetProgramiv(shaderProgramID,GL_INFO_LOG_LENGTH,&info);
		if(info>1){
			char *infoLog=new char[info];
			glGetProgramInfoLog(shaderProgramID,info,&info,infoLog);
			printf("Shader Program Error\n%s\n",infoLog);
			delete [] infoLog;
		}

	}

	~ShaderProgram(){
		if(vertexShaderID!=-1){
			glDetachShader(shaderProgramID,vertexShaderID);
			glDeleteShader(vertexShaderID); 
		}
		if(fragmentShaderID!=-1){
			glDetachShader(shaderProgramID,fragmentShaderID);
			glDeleteShader(fragmentShaderID);
		}
		
		glDeleteProgram(shaderProgramID);
	}
	void setActive(){
		glUseProgram(shaderProgramID);
	}

	void setInactive(){
		glUseProgram(0);
	}
};

#endif