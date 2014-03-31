#ifndef _KEYBOARD_
#define _KEYBOARD_



////////////////////////////////////////////////

#define GLUT_ESC_KEY		27
#define GLUT_BACKSPACE_KEY	8
#define GLUT_ENTER_KEY		13
#define GLUT_DELETE_KEY		127

class Keyboard{//start of Keyboard class
	public:
	bool ignoreCaps;

	bool normalKeyArray[256];//ascii key values 0=not pressed, 1=pressed
	bool specialKeyArray[256];//0=not pressed, 1=pressed

	Keyboard(){
		ignoreCaps=true;	
	}

	void update(int key,bool val){
		specialKeyArray[key]=val;
	}

	void update(unsigned char key,bool val){		
		normalKeyArray[key]=val;
		if(key>='a'&&key<='z') normalKeyArray[key+('A'-'a')]=val;
		else if(key>='A'&&key<='Z') normalKeyArray[key-('A'-'a')]=val;
	}

	void processDefaultKeys(){	
		if(normalKeyArray[27]) exit(0);//exit on esc
	}
};//end of Keyboard class


#endif