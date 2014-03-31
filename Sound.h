#ifndef __SOUND__
#define __SOUND__

#include <iostream>
#include <vector>

#include "AL/al.h"
#include "AL/alut.h"
#include "vorbis/vorbisfile.h"
#include "vorbis/codec.h"

#include "VectorT.h"

#define BUFFER_SIZE   32768     // 32 KB buffers

using std::vector;

class Sound{
private:
	ALuint bufferID;            // The OpenAL sound buffer ID
	ALuint sourceID;            // The OpenAL sound source
	ALenum format;              // The sound data format
	ALsizei freq;               // The frequency of the sound data

	vector<char> bufferData; // The sound buffer data from file
public:
	Sound(){
	}

	~Sound(){
	}

	void loadWav(char *fileName){
		// Create sound buffer and source
		alGenBuffers(1, &bufferID);
		alGenSources(1, &sourceID);
	}

	void loadOgg(char *fileName){
		// Create sound buffer and source
		alGenBuffers(1, &bufferID);
		alGenSources(1, &sourceID);

		int endian = 0;             // 0 for Little-Endian, 1 for Big-Endian
		int bitStream;
		long bytes;
		char ar[BUFFER_SIZE];    // Local fixed size array
		FILE *f;

		// Open for binary reading
		if(fopen_s(&f,fileName, "rb")){
			fprintf(stderr,"fopen_s failed to open file\n");
			getchar();
		}

		vorbis_info *pInfo=NULL;
		OggVorbis_File oggFile;

		ov_open_callbacks(f, &oggFile, NULL, 0, OV_CALLBACKS_DEFAULT);

		// Get some information about the OGG file
		pInfo = ov_info(&oggFile, -1);
		if(pInfo==NULL){
			fprintf(stderr,"Bad OGG file \n");
		}

		// Check the number of channels... always use 16-bit samples
		if (pInfo->channels == 1)
			format = AL_FORMAT_MONO16;
		else
			format = AL_FORMAT_STEREO16;
		// end if

		// The frequency of the sampling rate
		freq = pInfo->rate;

		do {
			// Read up to a buffer's worth of decoded sound data
			bytes = ov_read(&oggFile, ar, BUFFER_SIZE, endian, 2, 1, &bitStream);
			// Append to end of buffer
			bufferData.insert(bufferData.end(), ar, ar + bytes);
		} while (bytes > 0);
		
		ov_clear(&oggFile);

		// Upload sound data to buffer
		alBufferData(bufferID, format, &bufferData[0], static_cast < ALsizei > (bufferData.size()), freq);
	}

	void setAndAttachSourcePosition(VectorT position,bool loop){ 
		//Set source position
		alSource3f(sourceID, AL_POSITION, position.x, position.y, position.z);

		// Attach sound buffer to source
		alSourcei(sourceID, AL_BUFFER, bufferID);

		alSourcei(sourceID,AL_LOOPING,loop);
	}
	
	void playSound(){
		alSourcePlay(sourceID);
	}

	void cleanup(){
		alDeleteBuffers(1, &bufferID);
		alDeleteSources(1, &sourceID);
		bufferData.clear();
	}

	static void kill(){
		// Clean up the OpenAL library
		alutExit();
	}

	static void setListenerPosition(VectorT position){ 
		alListener3f(AL_POSITION, position.x,position.y,position.z);
	}

	static void init(int argc, char** argv){
		alutInit(&argc, argv);
	}
};

#endif