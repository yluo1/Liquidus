#ifndef _TEXTURE_
#define _TEXTURE_

#include <stdio.h>
#include <string.h>
#include "GL/glew.h"
#include "GL/glut.h"

#include "Conversion.h"


class Texture{//class start
private:
	GLubyte	*imageData;		// Image Data (Up To 32 Bits)
	GLuint	bpp;			// Image Color Depth In Bits Per Pixel.
public:

	GLuint	width;			// Image Width
	GLuint	height;			// Image Height
	GLuint	texID;			// Texture ID Used To Select A Texture

	Texture(){
		imageData=NULL;
	}


	void loadTGA(char *fileName,GLint internalFormat){
		if(imageData!=NULL) delete[] imageData; 

		GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
		GLubyte		TGAcompare[12];								// Used To Compare TGA Header
		GLubyte		header[6];									// First 6 Useful Bytes From The Header
		GLuint		bytesPerPixel;								// Holds Number Of Bytes Per Pixel Used In The TGA File
		GLuint		imageSize;									// Used To Store The Image Size When Setting Aside Ram
		GLuint		temp;										// Temporary Variable
		GLuint		type=GL_RGBA;								// Set The Default GL Mode To RBGA (32 BPP)


		FILE *file;
		fopen_s(&file,fileName, "rb");						// Open The TGA File
		
		if(	file==NULL ||										// Does File Even Exist?
			fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	// Are There 12 Bytes To Read?
			memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0				||	// Does The Header Match What We Want?
			fread(header,1,sizeof(header),file)!=sizeof(header))				// If So Read Next 6 Header Bytes
		{
			fclose(file);										// If Anything Failed, Close The File
			printf("bad read\n");
			return;										// Return False
		}

		//////////////////MUST BE POWER OF 2!!!!! POS
		 width  = header[1] * 256 + header[0];			// Determine The TGA Width	(highbyte*256+lowbyte)
		 height = header[3] * 256 + header[2];			// Determine The TGA Height	(highbyte*256+lowbyte)
	    
 		if(	 width	<=0	||								// Is The Width Less Than Or Equal To Zero
			 height	<=0	||								// Is The Height Less Than Or Equal To Zero
			(header[4]!=24 && header[4]!=32))	{				// Is The TGA 24 or 32 Bit
			fclose(file);										// If Anything Failed, Close The File
				printf("bad read\n");
			return;										// Return False
		}

		 bpp	= header[4];							// Grab The TGA's Bits Per Pixel (24 or 32)
		bytesPerPixel	=  bpp/8;						// Divide By 8 To Get The Bytes Per Pixel
		imageSize		=  width* height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

		 imageData=(GLubyte *)malloc(imageSize);		// Reserve Memory To Hold The TGA Data

		if(	 imageData==NULL ||							// Does The Storage Memory Exist?
			fread( imageData, 1, imageSize, file)!=imageSize) {	// Does The Image Size Match The Memory Reserved?
			if( imageData!=NULL)						// Was Image Data Loaded
				free( imageData);						// If So, Release The Image Data

			fclose(file);										// Close The File
			printf("bad read\n");
			return;										// Return False
		}

		for(GLuint i=0; i<int(imageSize); i+=bytesPerPixel)	{	// Loop Through The Image Data
																// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
			temp= imageData[i];							// Temporarily Store The Value At Image Data 'i'
			 imageData[i] =  imageData[i + 2];	// Set The 1st Byte To The Value Of The 3rd Byte
			 imageData[i + 2] = temp;					// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
		}	

		fclose (file);											// Close The File
		if (bpp==24)		 {							// Was The TGA 24 Bits
			type=GL_RGB;										// If So Set The 'type' To GL_RGB
		}
		
		glEnable(GL_TEXTURE_2D);
		// Build A Texture From The Data	
		glGenTextures(1, &texID);					// Generate OpenGL texture IDs	


			   // select our current texture
		glBindTexture( GL_TEXTURE_2D, texID );

		// select modulate to mix texture with color for shading
		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// build our texture mipmaps
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, type, GL_UNSIGNED_BYTE, imageData);

		free(imageData);
		imageData=NULL;

	}

	void makeBlankTexture(GLint width,GLint height,GLint internalFormat,GLenum format, GLenum type ){
		glGenTextures(1, &texID);		// Generate OpenGL texture IDs		
		glBindTexture(GL_TEXTURE_2D,texID);

		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		this->width=width;
		this->height=height;
		
		glTexImage2D(GL_TEXTURE_2D, 0,internalFormat,width,height,0,format,type,0);

	}

	void makeNoiseTexture(GLint width,GLint height,GLint internalFormat,GLenum format, GLenum type ){
		glGenTextures(1, &texID);		// Generate OpenGL texture IDs		
		glBindTexture(GL_TEXTURE_2D,texID);

		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		this->width=width;
		this->height=height;
		float *imageDataFloat=new float[width*height*4];
		for(int a=0;a<width*height*4;++a) imageDataFloat[a]=rand()%10000/10000.0f;
		glTexImage2D(GL_TEXTURE_2D, 0,internalFormat,width,height,0,format,type,imageDataFloat);
		delete []imageDataFloat;
	}

	void makeTexture(void *data, GLuint width,GLuint height,GLint internalFormat,GLenum format, GLenum type){
		glGenTextures(1, &texID);		// Generate OpenGL texture IDs		
		glBindTexture(GL_TEXTURE_2D,texID);

		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		this->width=width;
		this->height=height;

		glTexImage2D(GL_TEXTURE_2D, 0,internalFormat,width,height,0,format,type,data);
	}

	void makeColorPalette(GLuint width, GLuint height,GLint internalFormat,GLenum format, GLenum type ){
		glGenTextures(1, &texID);		// Generate OpenGL texture IDs		
		glBindTexture(GL_TEXTURE_2D,texID);

		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		this->width=width;
		this->height=height;
		float *imageDataFloat=new float[width*height*4];

		VectorT hsv;
		VectorT rgb;
		hsv.y=.5;
		hsv.z=1.0;
		for(unsigned int a=0;a<height;++a){
			hsv.y=(float)a/height;
			for(unsigned int b=0;b<width;++b){
				hsv.x=360.0/(width)*b;
				rgb=HSVToRGB(hsv);
				imageDataFloat[4*(a*width+b)+0]=rgb.x;
				imageDataFloat[4*(a*width+b)+1]=rgb.y;
				imageDataFloat[4*(a*width+b)+2]=rgb.z;
				imageDataFloat[4*(a*width+b)+3]=1.0f;

			}
		}
		glTexImage2D(GL_TEXTURE_2D, 0,internalFormat,width,height,0,format,type,imageDataFloat);
		delete []imageDataFloat;
	}

};//class end


#endif