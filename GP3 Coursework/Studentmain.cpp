#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#define GLX_GLXEXT_LEGACY //Must be declared so that our local glxext.h is picked up, rather than the system one


//#include <windows.h>
#include "GameConstants.h"
#include "windowOGL.h"
#include "cWNDManager.h"
#include "cColours.h"
#include "cShapes.h"
#include "cPyramid.h"
#include "cCube.h"
#include "cSphere.h"
#include "cMaterial.h"
#include "cLight.h"
#include "cStarfield.h"
#include "cFontMgr.h"
#include "cCamera.h"
#include "cInputMgr.h"
#include "cSoundMgr.h"
#include "cModelLoader.h"
#include "cModel.h"
#include "cPlayer.h"
#include "cEnemy.h"
#include "cLaser.h"
#include "tardisWarsGame.h"

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR cmdLine,
                   int cmdShow)
{

    //Set our window settings
    const int windowWidth = 1024;
    const int windowHeight = 768;
    const int windowBPP = 16;



    //This is our window
	static cWNDManager* pgmWNDMgr = cWNDManager::getInstance();

	// This is the input manager
	static cInputMgr* theInputMgr = cInputMgr::getInstance();

	// This is the Font manager
	static cFontMgr* theFontMgr = cFontMgr::getInstance();

	// This is the sound manager
	static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();
	
	//The example OpenGL code
    windowOGL theOGLWnd;

    //Attach our example to our window
	pgmWNDMgr->attachOGLWnd(&theOGLWnd);

	// Attach the keyboard manager
	pgmWNDMgr->attachInputMgr(theInputMgr);
	
    // This is our Sun and Moon Variabkes with there respectable sizes.
	cSphere theSun(4, 40, 40);
	cSphere theDeathStarMoon(1, 20, 20);
    //Attempt to create the window
	if (!pgmWNDMgr->createWND(windowWidth, windowHeight, windowBPP))
    {
        //If it fails

        MessageBox(NULL, "Unable to create the OpenGL Window", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
        return 1;
    }

	if (!theOGLWnd.initOGL(windowWidth, windowHeight)) //Initialize our example
    {
        MessageBox(NULL, "Could not initialize the application", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
        return 1;
    }

	// Create Texture map For deathstar, Enemys, Lasers, Moon and Sun
	cTexture tardisTexture;
	tardisTexture.createTexture("Models/DeathStar.png");
	cTexture spaceShipTexture;
	spaceShipTexture.createTexture("Models/SpaceShip/mat_plan4.png");
	cTexture laserTexture;
	laserTexture.createTexture("Models/Laser.png");
	cTexture starTexture;
	starTexture.createTexture("Images/star.png");
	cTexture moonTexture;
	moonTexture.createTexture("Images/moon.png");
	cTexture sunTexture;
	sunTexture.createTexture("Images/sun.png");

	// the starfield
	cStarfield theStarField(starTexture.getTexture(), glm::vec3(50.0f, 50.0f, 50.0f));
	
    // Initializing our Sun and Moon and getting their textures and Positions in the Game world.
	theDeathStarMoon.initialise(moonTexture.getTexture(), glm::vec3(-5, 10, 20), glm::vec3(0, 0, 0));
	theSun.initialise(sunTexture.getTexture(), glm::vec3(10, 10, 20), glm::vec3(0, 0, 0));
	
	// Create Materials for lights
	cMaterial sunMaterial(lightColour4(0.0f, 0.0f, 0.0f, 1.0f), lightColour4(1.0f, 1.0f, 1.0f, 1.0f), lightColour4(1.0f, 1.0f, 1.0f, 1.0f), lightColour4(0, 0, 0, 1.0f), 5.0f);
	cMaterial moonMaterial(lightColour4(0.1f, 0.1f, 0.1f, 1.0f), lightColour4(1.0f, 1.0f, 1.0f, 1.0f), lightColour4(0.2f, 0.2f, 0.2f, 1.0f), lightColour4(0, 0, 0, 1.0f), 10.0f);
	// Create Light
	cLight sunLight(GL_LIGHT0, lightColour4(0, 0, 0, 1), lightColour4(1, 1, 1, 1), lightColour4(1, 1, 1, 1), glm::vec4(0, 0, 20, 1),
		glm::vec3(0.0, 0.0, 1.0), 0.0f, 180.0f, 1.0f, 0.0f, 0.0f);
	cLight lfLight(GL_LIGHT1, lightColour4(0, 0, 0, 1), lightColour4(1, 1, 1, 1), lightColour4(1, 1, 1, 1), glm::vec4(30, 0, 100, 1),
		glm::vec3(0.0, 0.0, 1.0), 0.0f, 180.0f, 1.0f, 0.0f, 0.0f);
	cLight rfLight(GL_LIGHT2, lightColour4(0, 0, 0, 1), lightColour4(1, 1, 1, 1), lightColour4(1, 1, 1, 1), glm::vec4(-30, 0, 100, 1),
		glm::vec3(0.0, 0.0, 1.0), 0.0f, 180.0f, 1.0f, 0.0f, 0.0f);
	cLight cbLight(GL_LIGHT3, lightColour4(0, 0, 0, 1), lightColour4(1, 1, 1, 1), lightColour4(1, 1, 1, 1), glm::vec4(0, 0, -100, 1),
		glm::vec3(0.0, 0.0, 1.0), 0.0f, 180.0f, 1.0f, 0.0f, 0.0f);
	//Define Ambient light for scene
	GLfloat g_Ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, g_Ambient);

	// load game fonts
	// Load Fonts
	LPCSTR gameFonts[3] = { "Fonts/digital-7.ttf", "Fonts/space age.ttf", "Fonts/Star_Jedi_Rounded.ttf" };

	theFontMgr->addFont("SevenSeg", gameFonts[0], 24);
	theFontMgr->addFont("Space", gameFonts[1], 24);
	theFontMgr->addFont("StarWars", gameFonts[2], 48);

	// load game sounds
	// Load Sound
	LPCSTR gameSounds[3] = { "Audio/imperial.wav", "Audio/blaster.wav", "Audio/explosion2.wav" };

	theSoundMgr->add("Theme", gameSounds[0]);
	theSoundMgr->add("Shot", gameSounds[1]);
	theSoundMgr->add("Explosion", gameSounds[2]);

	// Create a camera
	cCamera theCamera;
	theCamera.setTheCameraPos(glm::vec3(0.0f, 75.0f, 50.0f));
	theCamera.setTheCameraLookAt(glm::vec3(0.0f, 0.0f, 0.0f));
	theCamera.setTheCameraUpVector(glm::vec3(0.0f, 1.0f, 0.0f)); // pointing upwards in world space
	theCamera.setTheCameraAspectRatio(windowWidth, windowHeight);
	theCamera.setTheProjectionMatrix(45.0f, theCamera.getTheCameraAspectRatio(), 0.1f, 300.0f);
	theCamera.update();

	GLfloat cameraRotRadius = 80.0f;
	GLfloat cameraRotationAngle = 0.0f;
	// Second Camera With different position created.
	cCamera theCamera2;
	theCamera2.setTheCameraPos(glm::vec3(0.0f, 0.0f, 75.0f));
	theCamera2.setTheCameraLookAt(glm::vec3(0.0f, 0.0f, 0.0f));
	theCamera2.setTheCameraUpVector(glm::vec3(0.0f, 1.0f, 0.0f)); // pointing upwards in world space
	theCamera2.setTheCameraAspectRatio(windowWidth, windowHeight);
	theCamera2.setTheProjectionMatrix(45.0f, theCamera2.getTheCameraAspectRatio(), 0.1f, 600.0f);
	theCamera2.update();
	//Clear key buffers
	theInputMgr->clearBuffers(theInputMgr->KEYS_DOWN_BUFFER | theInputMgr->KEYS_PRESSED_BUFFER);
	

	// Player Model, Enemy Model and Laser Model
	cModelLoader tardisMdl;
	tardisMdl.loadModel("Models/DeathStar.obj", tardisTexture); // Player

	cModelLoader spaceShipMdl;
	spaceShipMdl.loadModel("Models/SpaceShip/planet4.obj", spaceShipTexture); // Enemy
	
	cModelLoader theLaser;
	theLaser.loadModel("Models/laser.obj", laserTexture);
	// Enemys are randomly spawned into the world with a specific scale.
	for (int loop = 0; loop < 20; loop++)
	{
		theEnemy.push_back(new cEnemy);
		theEnemy[loop]->randomise();
		theEnemy[loop]->setMdlDimensions(spaceShipMdl.getModelDimensions());
		theEnemy[loop]->setScale(glm::vec3(0.05f, 0.05f, 0.05f));
	}

	// The player is created and initialised with a specific scale and has the inpur and sound manager attached to it.
	cPlayer thePlayer;
	thePlayer.initialise(glm::vec3(0, 0, 0), 0.0f, glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0, 0, 0), 5.0f, true);
	thePlayer.setMdlDimensions(tardisMdl.getModelDimensions());
	thePlayer.attachInputMgr(theInputMgr);
	thePlayer.attachSoundMgr(theSoundMgr);

	float tCount = 0.0f;
	string outputMsg;
	//Loops theme sound so it is constantly playing
	theSoundMgr->getSnd("Theme")->playAudio(AL_LOOPING);

	std::vector<cLaser*> laserList;
	std::vector<cLaser*>::iterator index;



   //This is the mainloop, we render frames until isRunning returns false
	while (pgmWNDMgr->isWNDRunning())
	{
		pgmWNDMgr->processWNDEvents(); //Process any window events

		//We get the time that passed since the last frame
		float elapsedTime = pgmWNDMgr->getElapsedSeconds();

		// Lab code goes here
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		theOGLWnd.initOGL(windowWidth, windowHeight);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		// If statements for changing the camera by holding down the C button.
		if (GetAsyncKeyState(0x43))
		{
			glLoadMatrixf((GLfloat*)&theCamera2.getTheViewMatrix());

		}
		else
		{
			glLoadMatrixf((GLfloat*)&theCamera.getTheViewMatrix());
		}
		// If statements for turning the sound on and off by pressing S to turn off theme song and press A to turn it back on.
		if (theInputMgr->isKeyDown(0x53))
		{
			theSoundMgr->getSnd("Theme")->stopAudio();
		
		}

		else

			if (theInputMgr->isKeyDown(0x41))
			{
			theSoundMgr->getSnd("Theme")->playAudio(AL_LOOPING);
			}

		theStarField.render(0.0f);
		// Rendering the Sun with Light attahced
		theSun.prepare(0.0f);
		sunMaterial.useMaterial();
		sunLight.lightOn();
		theSun.render(theSun.getRotAngle());
		
		lfLight.lightOn();
		rfLight.lightOn();
		cbLight.lightOn();
		// Rendering the moon 
		theDeathStarMoon.prepare(0.0f);
		moonMaterial.useMaterial();
		theDeathStarMoon.render(theDeathStarMoon.getRotAngle());
		// Begining Enemy Iteration
		for (vector<cEnemy*>::iterator enemyIterator = theEnemy.begin(); enemyIterator != theEnemy.end(); ++enemyIterator)
		{
			if ((*enemyIterator)->isActive())
			{
				spaceShipMdl.renderMdl((*enemyIterator)->getPosition(), (*enemyIterator)->getRotation(), (*enemyIterator)->getScale());
				(*enemyIterator)->update(elapsedTime);
			}
		}
		// Rendering Player
		tardisMdl.renderMdl(thePlayer.getPosition(), thePlayer.getRotation(), thePlayer.getScale());
		thePlayer.update(elapsedTime);
		//Beginning Laser Iteration
		for (vector<cLaser*>::iterator laserIterartor = theTardisLasers.begin(); laserIterartor != theTardisLasers.end(); ++laserIterartor)
		{
			if ((*laserIterartor)->isActive())
			{
				theLaser.renderMdl((*laserIterartor)->getPosition(), (*laserIterartor)->getRotation(), (*laserIterartor)->getScale());
				(*laserIterartor)->update(elapsedTime);
			}
		}

		outputMsg = to_string(theEnemy.size()); // convert float to string

		
		//Rendering Font into top left and right hand corners of the screen
		glPushMatrix();
		theOGLWnd.setOrtho2D(windowWidth, windowHeight);
		theFontMgr->getFont("StarWars")->printText("Death Star", FTPoint(10, 35, 0.0f), colour3f(255.0f,255.0f,0.0f));
		theFontMgr->getFont("StarWars")->printText(outputMsg.c_str(), FTPoint(850, 35, 0.0f), colour3f(255.0f, 255.0f, 0.0f)); // uses c_str to convert string to LPCSTR
		glPopMatrix();

		pgmWNDMgr->swapBuffers();

		tCount += elapsedTime;
		
		
		//Clear key buffers
		theInputMgr->clearBuffers(theInputMgr->KEYS_DOWN_BUFFER | theInputMgr->KEYS_PRESSED_BUFFER);

	
		
	}

	theOGLWnd.shutdown(); //Free any resources
	pgmWNDMgr->destroyWND(); //Destroy the program window

    return 0; //Return success
}
