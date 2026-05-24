#include "header.h"
#include "Core.h"
#include "IMG.h"
#include "CFG.h"
#include "Text.h"
#include "SDL_mixer.h"
#include <fstream>

/* ******************************************** */

Map* CCore::oMap = new Map();
bool CCore::mouseLeftPressed = false;
bool CCore::mouseRightPressed = false;
int CCore::mouseX = 0;
int CCore::mouseY = 0;
bool CCore::quitGame = false;

bool CCore::movePressed = false;
bool CCore::keyMenuPressed = false;
bool CCore::keyS = false;
bool CCore::keyW = false;
bool CCore::keyA = false;
bool CCore::keyD = false;
bool CCore::keyShift = false;
bool CCore::keyAPressed = false;
bool CCore::keyDPressed = false;

//static bool AUTOPLAY_ENABLED = true;
//static int AUTOPLAY_FRAME = 0;

CCore::CCore(void) {
	this->quitGame = false;
	this->iFPS = 0;
	this->iNumOfFPS = 0;
	this->lFPSTime = 0;

	SDL_Init(
    SDL_INIT_VIDEO |
    SDL_INIT_TIMER |
    SDL_INIT_AUDIO |
    SDL_INIT_GAMECONTROLLER |
    SDL_INIT_JOYSTICK
	);

	SDL_GameController* controller = nullptr;
	std::ofstream log("/tmp/umario_startup.log");

	if (SDL_NumJoysticks() > 0) {
    if (SDL_IsGameController(0)) {
        controller = SDL_GameControllerOpen(0);
        
        log << "Opened as GameController" << std::endl;
    } else {
        SDL_JoystickOpen(0);  // fallback: raw joystick events
        log << "Opened as raw Joystick" << std::endl;
    }
	}

	log << "Joysticks found: "
		<< SDL_NumJoysticks()
		<< std::endl;

	for(int i=0;i<SDL_NumJoysticks();i++)
	{
		log << "Joystick "
			<< i
			<< ": "
			<< SDL_JoystickNameForIndex(i)
			<< std::endl;
	}
	
	window = SDL_CreateWindow("NihiraLuvsMario", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, CCFG::GAME_WIDTH, CCFG::GAME_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
	
	if(window == NULL) {
		quitGame = true;
	}

	rR = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(rR, CCFG::GAME_WIDTH, CCFG::GAME_HEIGHT);

	// ----- ICO
	std::string fileName = "files/images/ico.bmp";
	SDL_Surface* loadedSurface = SDL_LoadBMP(fileName.c_str());
	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 255, 0, 255));

	SDL_SetWindowIcon(window, loadedSurface);
	SDL_FreeSurface(loadedSurface);

	mainEvent = new SDL_Event();
	// ----- ICO
	
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	
	oMap = new Map(rR);
	CCFG::getMM()->setActiveOption(rR);
	CCFG::getSMBLOGO()->setIMG("super_mario_bros", rR);

	CCFG::getMusic()->PlayMusic();

	this->keyMenuPressed = this->movePressed = this->keyS = this->keyW = this->keyA = this->keyD = this->keyShift = false;

	this->keyAPressed = this->keyDPressed = this->firstDir = false;

	this->mouseX = this->mouseY = 0;

	CCFG::keyIDA = SDLK_a;
	CCFG::keyIDS = SDLK_s;
	CCFG::keyIDD = SDLK_d;
	CCFG::keyIDSpace = SDLK_SPACE;
	CCFG::keyIDShift = SDLK_LSHIFT;
}

CCore::~CCore(void) {
	delete oMap;
	delete mainEvent;
	SDL_DestroyRenderer(rR);
	SDL_DestroyWindow(window);
}

/* ******************************************** */

void CCore::mainLoop() {
	lFPSTime = SDL_GetTicks();

	while(!quitGame && mainEvent->type != SDL_QUIT) {
		
		frameTime = SDL_GetTicks();
		//SDL_PollEvent(mainEvent);
		
		while(SDL_PollEvent(mainEvent))
		{
			std::ofstream log("/tmp/eventtypes.log", std::ios::app);
			log << "Event type = " << mainEvent->type << std::endl;
			Input();
			MouseInput();
		}
		
		SDL_RenderClear(rR);

		CCFG::getMM()->setBackgroundColor(rR);
		SDL_RenderFillRect(rR, NULL);
		
		/*
		if (AUTOPLAY_ENABLED)
		{
			AUTOPLAY_FRAME++;
		}
		*/
		
		//Input();
		//MouseInput();
		Update();
		Draw();

		/*CCFG::getText()->Draw(rR, "FPS:" + std::to_string(iNumOfFPS), CCFG::GAME_WIDTH - CCFG::getText()->getTextWidth("FPS:" + std::to_string(iNumOfFPS), 8) - 8, 5, 8);

		if(SDL_GetTicks() - 1000 >= lFPSTime) {
			lFPSTime = SDL_GetTicks();
			iNumOfFPS = iFPS;
			iFPS = 0;
		}

		++iFPS;*/

		SDL_RenderPresent(rR);
		
		if(SDL_GetTicks() - frameTime < MIN_FRAME_TIME) {
			SDL_Delay(MIN_FRAME_TIME - (SDL_GetTicks () - frameTime));
		}
	}
}

void CCore::Input() {
	switch(CCFG::getMM()->getViewID()) {
		case 2: case 7:
			if(!oMap->getInEvent())
			{
				InputPlayer();
				/*
				if (AUTOPLAY_ENABLED)
				{
					InputPlayerAutoplay();
				}
				else
				{
					InputPlayer();
				}
				*/
			} 
			else
			{
				resetMove();
			}
			break;
		default:
			InputMenu();
			break;
	}
}

void CCore::InputMenu() {
	if(mainEvent->type == SDL_KEYDOWN) {
		CCFG::getMM()->setKey(mainEvent->key.keysym.sym);

		switch(mainEvent->key.keysym.sym) {
			case SDLK_s: case SDLK_DOWN:
				if(!keyMenuPressed) {
					CCFG::getMM()->keyPressed(2);
					keyMenuPressed = true;
				}
				break;
			case SDLK_w: case SDLK_UP:
				if(!keyMenuPressed) {
					CCFG::getMM()->keyPressed(0);
					keyMenuPressed = true;
				}
				break;
			case SDLK_KP_ENTER: case SDLK_RETURN:
				if(!keyMenuPressed) {
					CCFG::getMM()->enter();
					keyMenuPressed = true;
				}
				break;
			case SDLK_ESCAPE:
				if(!keyMenuPressed) {
					CCFG::getMM()->escape();
					keyMenuPressed = true;
				}
				break;
				
			case SDLK_F11: //Toggle F11 for Fullscreen or windowed mode
				{
					Uint32 flags = SDL_GetWindowFlags(window);
					if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
						SDL_SetWindowFullscreen(window, 0);
					else
						SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
				}
				break;
				
			case SDLK_LEFT: case SDLK_d:
				if(!keyMenuPressed) {
					CCFG::getMM()->keyPressed(3);
					keyMenuPressed = true;
				}
				break;
			case SDLK_RIGHT: case SDLK_a:
				if(!keyMenuPressed) {
					CCFG::getMM()->keyPressed(1);
					keyMenuPressed = true;
				}
				break;
		}
	}

	if(mainEvent->type == SDL_KEYUP) {
		switch(mainEvent->key.keysym.sym) {
			case SDLK_s: case SDLK_DOWN: case SDLK_w: case SDLK_UP: case SDLK_KP_ENTER: case SDLK_RETURN: case SDLK_ESCAPE: case SDLK_a: case SDLK_RIGHT: case SDLK_LEFT: case SDLK_d:
				keyMenuPressed = false;
				break;
			default:
				break;
		}
	}
	
	// In InputMenu(), after the SDL_KEYUP block:

	if(mainEvent->type == SDL_CONTROLLERBUTTONDOWN) {
		switch(mainEvent->cbutton.button) {
			case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
				if(!keyMenuPressed) {
					CCFG::getMM()->keyPressed(2);
					keyMenuPressed = true;
				}
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_UP:
				if(!keyMenuPressed) {
					CCFG::getMM()->keyPressed(0);
					keyMenuPressed = true;
				}
				break;
			case SDL_CONTROLLER_BUTTON_A:
			case SDL_CONTROLLER_BUTTON_START:
				if(!keyMenuPressed) {
					CCFG::getMM()->enter();
					keyMenuPressed = true;
				}
				break;
				
			case SDL_CONTROLLER_BUTTON_B:
				if(!keyMenuPressed) {
					CCFG::getMM()->escape();
					keyMenuPressed = true;
				}
				break;
		}
	}
	if(mainEvent->type == SDL_CONTROLLERBUTTONUP) {
		keyMenuPressed = false;
	}
	if(mainEvent->type == SDL_JOYBUTTONDOWN)
	{
		if(mainEvent->jbutton.button == 9)
		{
			if(!keyMenuPressed)
			{
				CCFG::getMM()->enter();
				keyMenuPressed = true;
			}
		}
	}

	if(mainEvent->type == SDL_JOYBUTTONUP)
	{
		if(mainEvent->jbutton.button == 9)
			keyMenuPressed = false;
	}
	
	if(mainEvent->type == SDL_JOYAXISMOTION)
	{
		if(!keyMenuPressed)
		{
			if(mainEvent->jaxis.axis == 1)
			{
				if(mainEvent->jaxis.value < -16000)
				{
					CCFG::getMM()->keyPressed(0); // UP
					keyMenuPressed = true;
				}
				else if(mainEvent->jaxis.value > 16000)
				{
					CCFG::getMM()->keyPressed(2); // DOWN
					keyMenuPressed = true;
				}
			}
		}
	}

	if(mainEvent->type == SDL_JOYAXISMOTION)
	{
		if(mainEvent->jaxis.axis == 1 &&
		   abs(mainEvent->jaxis.value) < 8000)
		{
			keyMenuPressed = false;
		}
	}

}

struct AutoInput {
    bool left  = false;
    bool right = false;
    bool jump  = false;
    bool run   = false;
};

/*
AutoInput GetAutoplayInput(Map* map, int frame) {
    AutoInput in;

    auto* player = map->getPlayer();

    // --- BASE BEHAVIOR ---
    in.right = true;     // Always move right
    in.run   = true;     // Always run

    // --- JUMP CONSTRAINTS ---
    if (!map->getUnderWater() &&
        player->getMove() &&
        frame % 120 == 0) {
        in.jump = true;  // periodic jump (safe default)
    }

    return in;
}
*/

void CCore::InputPlayer() {
	
	if(mainEvent->type == SDL_JOYAXISMOTION)
	{
		std::ofstream log("/tmp/umario_events.log", std::ios::app);

		log << "AXIS "
			<< (int)mainEvent->jaxis.axis
			<< " = "
			<< mainEvent->jaxis.value
			<< std::endl;
	}

	if(mainEvent->type == SDL_JOYBUTTONDOWN)
	{
		std::ofstream log("/tmp/umario_events.log", std::ios::app);

		log << "BUTTON DOWN "
			<< (int)mainEvent->jbutton.button
			<< std::endl;
	}
	
	if(mainEvent->type == SDL_WINDOWEVENT) {
		switch(mainEvent->window.event) {
			case SDL_WINDOWEVENT_FOCUS_LOST:
				CCFG::getMM()->resetActiveOptionID(CCFG::getMM()->ePasue);
				CCFG::getMM()->setViewID(CCFG::getMM()->ePasue);
				CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPASUE);
				CCFG::getMusic()->PauseMusic();
				break;
		}
	}
	
	if(mainEvent->type == SDL_JOYAXISMOTION)
	{
		std::ofstream log("/tmp/joydebug.log", std::ios::app);

		log << "AXIS "
			<< (int)mainEvent->jaxis.axis
			<< " = "
			<< mainEvent->jaxis.value
			<< std::endl;
	}
	
	
	if(mainEvent->type == SDL_JOYBUTTONDOWN)
	{
		std::ofstream log("/tmp/joydebug.log", std::ios::app);

		log << "BUTTON DOWN "
			<< (int)mainEvent->jbutton.button
			<< std::endl;
	}
	
	if(mainEvent->type == SDL_JOYBUTTONDOWN)
	{
		switch(mainEvent->jbutton.button)
		{
			case 0:
				if(!CCFG::keySpace)
				{
					oMap->getPlayer()->jump();
					CCFG::keySpace = true;
				}
				break;

			case 1:
				if(!keyShift)
				{
					oMap->getPlayer()->startRun();
					keyShift = true;
				}
				break;
				
			case 2:   // Y / X button — alternate jump
				if(!CCFG::keySpace)
				{
					oMap->getPlayer()->jump();
					CCFG::keySpace = true;
				}
				break;
			case 9:   // START button
				if(!keyMenuPressed)
				{
					CCFG::getMM()->resetActiveOptionID(CCFG::getMM()->ePasue);
					CCFG::getMM()->setViewID(CCFG::getMM()->ePasue);
					CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPASUE);
					CCFG::getMusic()->PauseMusic();
					keyMenuPressed = true;
				}
				break;
		}
	}

	if(mainEvent->type == SDL_JOYBUTTONUP)
	{
		std::ofstream log("/tmp/joydebug.log", std::ios::app);

		log << "BUTTON UP "
			<< (int)mainEvent->jbutton.button
			<< std::endl;
	}
	
	if(mainEvent->type == SDL_JOYBUTTONUP)
	{
		switch(mainEvent->jbutton.button)
		{
			case 0:
				CCFG::keySpace = false;
				break;

			case 1:
				if(keyShift)
				{
					oMap->getPlayer()->resetRun();
					keyShift = false;
				}
				break;
			case 2:
				CCFG::keySpace = false;
				break;
			case 9:
				keyMenuPressed = false;
				break;
		}
	}
	 

	if(mainEvent->type == SDL_KEYUP) {
		if(mainEvent->key.keysym.sym == CCFG::keyIDD) {
				if(firstDir) {
					firstDir = false;
				}

				keyDPressed = false;
			}

			if(mainEvent->key.keysym.sym == CCFG::keyIDS) {
				oMap->getPlayer()->setSquat(false);
				keyS = false;
			}
		
			if(mainEvent->key.keysym.sym == CCFG::keyIDA) {
				if(!firstDir) {
					firstDir = true;
				}

				keyAPressed = false;
			}
		
			if(mainEvent->key.keysym.sym == CCFG::keyIDSpace) {
				CCFG::keySpace = false;
			}
		
			if(mainEvent->key.keysym.sym == CCFG::keyIDShift) {
				if(keyShift) {
					oMap->getPlayer()->resetRun();
					keyShift = false;
				}
			}
		switch(mainEvent->key.keysym.sym) {
			case SDLK_KP_ENTER: case SDLK_RETURN: case SDLK_ESCAPE:
				keyMenuPressed = false;
				break;
		}
	}
	
	if(mainEvent->type == SDL_CONTROLLERBUTTONDOWN) {
    switch(mainEvent->cbutton.button) {

        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            keyAPressed = true;
            if(!keyDPressed) firstDir = false;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            keyDPressed = true;
            if(!keyAPressed) firstDir = true;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            if(!keyS) {
                keyS = true;
                if(!oMap->getUnderWater() && !oMap->getPlayer()->getInLevelAnimation())
                    oMap->getPlayer()->setSquat(true);
            }
            break;

        case SDL_CONTROLLER_BUTTON_A:
            if(!CCFG::keySpace) {
                oMap->getPlayer()->jump();
                CCFG::keySpace = true;
            }
            break;

        case SDL_CONTROLLER_BUTTON_B:
            if(!keyShift) {
                oMap->getPlayer()->startRun();
                keyShift = true;
            }
            break;

        case SDL_CONTROLLER_BUTTON_START:
            if(!keyMenuPressed) {
                CCFG::getMM()->resetActiveOptionID(CCFG::getMM()->ePasue);
                CCFG::getMM()->setViewID(CCFG::getMM()->ePasue);
                CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPASUE);
                CCFG::getMusic()->PauseMusic();
                keyMenuPressed = true;
            }
            break;
    }
	}
	if(mainEvent->type == SDL_CONTROLLERBUTTONUP) {

    switch(mainEvent->cbutton.button) {

        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            if(!firstDir) {
                firstDir = true;
            }
            keyAPressed = false;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            if(firstDir) {
                firstDir = false;
            }
            keyDPressed = false;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            oMap->getPlayer()->setSquat(false);
            keyS = false;
            break;

        case SDL_CONTROLLER_BUTTON_A:
            CCFG::keySpace = false;
            break;

        case SDL_CONTROLLER_BUTTON_B:
            if(keyShift) {
                oMap->getPlayer()->resetRun();
                keyShift = false;
            }
            break;

        case SDL_CONTROLLER_BUTTON_START:
            keyMenuPressed = false;
            break;
    }
	}

	if(mainEvent->type == SDL_KEYDOWN) {
		if(mainEvent->key.keysym.sym == CCFG::keyIDD) {
			keyDPressed = true;
			if(!keyAPressed) {
				firstDir = true;
			}
		}

		if(mainEvent->key.keysym.sym == CCFG::keyIDS) {
			if(!keyS) {
				keyS = true;
				if(!oMap->getUnderWater() && !oMap->getPlayer()->getInLevelAnimation()) oMap->getPlayer()->setSquat(true);
			}
		}
		
		if(mainEvent->key.keysym.sym == CCFG::keyIDA) {
			keyAPressed = true;
			if(!keyDPressed) {
				firstDir = false;
			}
		}
		
		if(mainEvent->key.keysym.sym == CCFG::keyIDSpace) {
			if(!CCFG::keySpace) {
				oMap->getPlayer()->jump();
				CCFG::keySpace = true;
			}
		}
		
		if(mainEvent->key.keysym.sym == CCFG::keyIDShift) {
			if(!keyShift) {
				oMap->getPlayer()->startRun();
				keyShift = true;
			}
		}

		switch(mainEvent->key.keysym.sym) {
			case SDLK_KP_ENTER: case SDLK_RETURN:
				if(!keyMenuPressed) {
					CCFG::getMM()->enter();
					keyMenuPressed = true;
				}
			case SDLK_ESCAPE:
				if(!keyMenuPressed && CCFG::getMM()->getViewID() == CCFG::getMM()->eGame) {
					CCFG::getMM()->resetActiveOptionID(CCFG::getMM()->ePasue);
					CCFG::getMM()->setViewID(CCFG::getMM()->ePasue);
					CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPASUE);
					CCFG::getMusic()->PauseMusic();
					keyMenuPressed = true;
				}
				break;
				
			case SDLK_F11:
				{
					Uint32 flags = SDL_GetWindowFlags(window);
					if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
						SDL_SetWindowFullscreen(window, 0);
					else
						SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
				}
				break;
		}
	}
	
	

    switch(mainEvent->cbutton.button) {

        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            if(!firstDir) {
                firstDir = true;
            }
            keyAPressed = false;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            if(firstDir) {
                firstDir = false;
            }
            keyDPressed = false;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            oMap->getPlayer()->setSquat(false);
            keyS = false;
            break;

        case SDL_CONTROLLER_BUTTON_A:
            CCFG::keySpace = false;
            break;

        case SDL_CONTROLLER_BUTTON_B:
            if(keyShift) {
                oMap->getPlayer()->resetRun();
                keyShift = false;
            }
            break;

        case SDL_CONTROLLER_BUTTON_START:
            keyMenuPressed = false;
            break;
    }
	
	
	if(mainEvent->type == SDL_JOYAXISMOTION)
{
    if(mainEvent->jaxis.axis == 0)
    {
        if(mainEvent->jaxis.value < -16000)
        {
            keyAPressed = true;
            keyDPressed = false;
            firstDir = false;
        }
        else if(mainEvent->jaxis.value > 16000)
        {
            keyDPressed = true;
            keyAPressed = false;
            firstDir = true;
        }
        else
        {
            keyAPressed = false;
            keyDPressed = false;
        }
    }

    
    if(mainEvent->jaxis.axis == 1)
    {
        /*
        if(mainEvent->jaxis.value < -16000)
        {
            if(!CCFG::keySpace)
            {
                oMap->getPlayer()->jump();
                CCFG::keySpace = true;
            }
        }
        */
        
        if(mainEvent->jaxis.value > 16000)
        {
            if(!keyS)
            {
                keyS = true;

                if(!oMap->getUnderWater() &&
                   !oMap->getPlayer()->getInLevelAnimation())
                {
                    oMap->getPlayer()->setSquat(true);
                }
            }
        }
        else
        {
            oMap->getPlayer()->setSquat(false);
            keyS = false;
            CCFG::keySpace = false;
        }
    }
	}
	

	if(keyAPressed) {
		if(!oMap->getPlayer()->getMove() && firstDir == false && !oMap->getPlayer()->getChangeMoveDirection() && !oMap->getPlayer()->getSquat()) {
			oMap->getPlayer()->startMove();
			oMap->getPlayer()->setMoveDirection(false);
		} else if(!keyDPressed && oMap->getPlayer()->getMoveSpeed() > 0 && firstDir != oMap->getPlayer()->getMoveDirection()) {
			oMap->getPlayer()->setChangeMoveDirection();
		}
	}

	if(keyDPressed) {
		if(!oMap->getPlayer()->getMove() && firstDir == true && !oMap->getPlayer()->getChangeMoveDirection() && !oMap->getPlayer()->getSquat()) {
			oMap->getPlayer()->startMove();
			oMap->getPlayer()->setMoveDirection(true);
		} else if(!keyAPressed && oMap->getPlayer()->getMoveSpeed() > 0 && firstDir != oMap->getPlayer()->getMoveDirection()) {
			oMap->getPlayer()->setChangeMoveDirection();
		}
	}

	if(oMap->getPlayer()->getMove() && !keyAPressed && !keyDPressed) {
		oMap->getPlayer()->resetMove();
	}
}

/*
void CCore::InputPlayerAutoplay() {
    AutoInput in = GetAutoplayInput(oMap, AUTOPLAY_FRAME);

    // Simulate LEFT / RIGHT keys
    keyAPressed = in.left;
    keyDPressed = in.right;

    // Direction resolution (same logic as keyboard)
    if (keyDPressed && !keyAPressed) firstDir = true;
    if (keyAPressed && !keyDPressed) firstDir = false;

    // RUN
    if (in.run && !keyShift) {
        oMap->getPlayer()->startRun();
        keyShift = true;
    }
    if (!in.run && keyShift) {
        oMap->getPlayer()->resetRun();
        keyShift = false;
    }

    // JUMP (single pulse)
    if (in.jump && !CCFG::keySpace) {
        oMap->getPlayer()->jump();
        CCFG::keySpace = true;
    }
    if (!in.jump) {
        CCFG::keySpace = false;
    }

    // Movement resolution (copied safely from InputPlayer)
    if(keyAPressed) {
        if(!oMap->getPlayer()->getMove() && firstDir == false &&
           !oMap->getPlayer()->getChangeMoveDirection() &&
           !oMap->getPlayer()->getSquat()) {
            oMap->getPlayer()->startMove();
            oMap->getPlayer()->setMoveDirection(false);
        }
    }

    if(keyDPressed) {
        if(!oMap->getPlayer()->getMove() && firstDir == true &&
           !oMap->getPlayer()->getChangeMoveDirection() &&
           !oMap->getPlayer()->getSquat()) {
            oMap->getPlayer()->startMove();
            oMap->getPlayer()->setMoveDirection(true);
        }
    }

    if(oMap->getPlayer()->getMove() && !keyAPressed && !keyDPressed) {
        oMap->getPlayer()->resetMove();
    }
}
*/

void CCore::MouseInput() {
	switch(mainEvent->type) {
		case SDL_MOUSEBUTTONDOWN: {
			switch (mainEvent->button.button) {
				case SDL_BUTTON_LEFT:
					mouseLeftPressed = true;
					break;
				case SDL_BUTTON_RIGHT:
					mouseRightPressed = true;
					break;
			}
			break;
		}
		case SDL_MOUSEMOTION: {
			
			SDL_GetMouseState(&mouseX, &mouseY);
			//CCFG::getMM()->getConsole()->print("x:" + std::to_string(mouseX));
			//CCFG::getMM()->getConsole()->print("y:" + std::to_string(mouseY));
			break;
		}
		case SDL_MOUSEBUTTONUP: {
			switch (mainEvent->button.button) {
				case SDL_BUTTON_LEFT:
					mouseLeftPressed = false;
					break;
				case SDL_BUTTON_RIGHT:
					mouseRightPressed = false;
					break;
			}
			break;
		}
		case SDL_MOUSEWHEEL:
			if(mainEvent->wheel.timestamp > SDL_GetTicks() - 2) {
				//CCFG::getMM()->getLE()->mouseWheel(mainEvent->wheel.y);
			}
			break;
	}
}

void CCore::resetKeys() {
	movePressed = keyMenuPressed = keyS = keyW = keyA = keyD = CCFG::keySpace = keyShift = keyAPressed = keyDPressed = false;
}

void CCore::Update() {
	CCFG::getMM()->Update();
}


void CCore::Draw() {
	CCFG::getMM()->Draw(rR);
}

/* ******************************************** */

void CCore::resetMove() {
	this->keyAPressed = this->keyDPressed = false;
}

Map* CCore::getMap() {
	return oMap;
}
