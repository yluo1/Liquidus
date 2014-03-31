#ifndef _SHADER_
#define _SHADER_

#include "GL/glew.h"
#include <iostream>
#include <vector>
#include <string.h>
#include <fstream>

using namespace std;

class Shader{
private:
public:
	GLuint shaderID;
	GLenum shaderType;

	Shader(GLenum shaderType,char *fileName){
		this->shaderType=shaderType;
		shaderID=glCreateShader(shaderType);

		ifstream fi(fileName,ios::in|ios::binary|ios::ate);
		int size=fi.tellg();
		char *sourceText=new char[size+1];
		sourceText[size]='\0';
		fi.seekg (0, ios::beg);
		fi.read (sourceText, size);
		fi.close();

		const char *sourcePointer=sourceText;
		glShaderSource(shaderID, 1, &sourcePointer,NULL);
		delete []sourceText;
		glCompileShader(shaderID);

		//Debugging
		int info;
		glGetShaderiv(shaderID,GL_INFO_LOG_LENGTH,&info);
		if(info>1){
			char *infoLog=new char[info];
			glGetShaderInfoLog(shaderID,info,&info,infoLog);
			printf("Shader Error\n%s\n",infoLog);
			delete [] infoLog;
		}
	}

	Shader(GLenum shaderType,char *file,int fileLen){
		this->shaderType=shaderType;
		shaderID=glCreateShader(shaderType);

		char *sourceText=new char[fileLen+1];
		memcpy(sourceText,file,fileLen);
		sourceText[fileLen]='\0';

		const char *sourcePointer=sourceText;

		glShaderSource(shaderID, 1, &sourcePointer,NULL);
		delete []sourceText;
		glCompileShader(shaderID);

		//Debugging
		int info;
		glGetShaderiv(shaderID,GL_INFO_LOG_LENGTH,&info);
		if(info>1){
			char *infoLog=new char[info];
			glGetShaderInfoLog(shaderID,info,&info,infoLog);
			printf("Shader Error\n%s\n",infoLog);
			delete [] infoLog;
		}

	}
};
#endif