#ifndef __FORCEPONGHIGHSCORE__
#define __FORCEPONGHIGHSCORE__

#include <stdio.h>

#include "Display.h"
#include "Keyboard.h"


#define MAX_HIGHSCORE_ENTRIES		10

class ForcePongHighscore{
private:

	typedef struct{
		int scores[MAX_HIGHSCORE_ENTRIES];
		char names[MAX_HIGHSCORE_ENTRIES][4];
	}HighScore;	

	HighScore highScores;
	int scoreEntryIndex;
	int scoreCharacterIndex;
	bool enterHighScore;

	Keyboard *highScoreKeyboard;
	Display *highScoreDisplay;

	char *fileName;

	
public:
	ForcePongHighscore(){

	}

	ForcePongHighscore(Display *highScoreDisplay, Keyboard *highScoreKeyboard){
		this->highScoreDisplay=highScoreDisplay;
		this->highScoreKeyboard=highScoreKeyboard;
		for(int a=0;a<MAX_HIGHSCORE_ENTRIES;++a){
			highScores.scores[a]=(MAX_HIGHSCORE_ENTRIES-a)*2-1;
			highScores.names[a][0]='L';
			highScores.names[a][1]='A';
			highScores.names[a][2]='X';
			highScores.names[a][3]='\0';
		}
		fileName=NULL;
		enterHighScore=false;
	}

	
	void readScores(char *fileName){
		this->fileName=fileName;
		ifstream hScore;
		hScore.open(fileName, ios::in | ios::app | ios::binary);
		if(hScore.is_open()){
			hScore.read((char*)&highScores,sizeof(HighScore));
		}
		hScore.close();
	}


	void insertScore(int score){
		for(int a=0;a<MAX_HIGHSCORE_ENTRIES;++a){
			if(score>highScores.scores[a]){
				scoreEntryIndex=a;
				scoreCharacterIndex=0;
				enterHighScore=true;
				for(int b=MAX_HIGHSCORE_ENTRIES-1;b>a;--b){
					highScores.scores[b]=highScores.scores[b-1];
					memcpy(highScores.names[b],highScores.names[b-1],3);
				}
				highScores.scores[a]=score;
				for(int b=0;b<4;++b) highScores.names[a][b]='\0';
				break;
			}
			
		}
	}

	void display(){	
		if(enterHighScore){
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			string s="Enter Highscore";
			highScoreDisplay->displayString(s,highScoreDisplay->windowWidth/2-120,highScoreDisplay->windowHeight/2-100*highScoreDisplay->windowScaleY,.35,NULL);
			highScoreDisplay->displayString(highScores.names[scoreEntryIndex],highScoreDisplay->windowWidth/2,highScoreDisplay->windowHeight/2-50*highScoreDisplay->windowScaleY,.25,NULL);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		}else{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			string s="Highscore";
			highScoreDisplay->displayString(s,highScoreDisplay->windowWidth/2-20,highScoreDisplay->windowHeight/2-100*highScoreDisplay->windowScaleY,.15,NULL);
			for(int a=0;a<MAX_HIGHSCORE_ENTRIES;++a){
				highScoreDisplay->displayString(highScores.names[a],highScoreDisplay->windowWidth/2,highScoreDisplay->windowHeight/2-50*highScoreDisplay->windowScaleY+a*20,.1,NULL);
				highScoreDisplay->displayString(numberToString(highScores.scores[a]),highScoreDisplay->windowWidth/2+40,highScoreDisplay->windowHeight/2-50*highScoreDisplay->windowScaleY+a*20,.1,NULL);
			}
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		}
	}

	void enterScore(){
		//Enter highscore
		if(enterHighScore){
			int a;
			for(a='A';a<='Z';++a){ 
				if(highScoreKeyboard->normalKeyArray[a]) break;
			}
			if(a<='Z'){
				highScores.names[scoreEntryIndex][scoreCharacterIndex]=a;
				scoreCharacterIndex=minValue(scoreCharacterIndex+1,2);
				highScoreKeyboard->normalKeyArray[a]=false;
			}else if(highScoreKeyboard->normalKeyArray[GLUT_BACKSPACE_KEY]){
				highScores.names[scoreEntryIndex][scoreCharacterIndex]='\0';
				scoreCharacterIndex=maxValue(scoreCharacterIndex-1,0);
				highScoreKeyboard->normalKeyArray[GLUT_BACKSPACE_KEY]=false;
			}else if(highScoreKeyboard->normalKeyArray[GLUT_ENTER_KEY]){
				enterHighScore=false;
				//write to file
				ofstream hScore;
				hScore.open((fileName==NULL?"highscore":fileName), ios::out | ios::binary);
				if(hScore.is_open()){
					hScore.write((char*)&highScores,sizeof(HighScore));
				}
				hScore.close();
				highScoreKeyboard->normalKeyArray[GLUT_ENTER_KEY]=false;
			}

		}
	}

};
#endif