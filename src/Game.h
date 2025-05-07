#ifndef GAME_H
#define GAME_H

#include "Scoreboard.h"
#include "olcPixelGameEngine.h"
#include "olcPGEX_Gamepad.h"

#include "Ending.h"
#include "Level.h"
#include "Menu.h"
#include "Object.h"
#include "StarMap.h"
#include "TimeAttack.h"
#include "TitleCard.h"
#include "EscapeNet.h"

#include <vector>

#if !defined(__EMSCRIPTEN__) && !defined(__linux__)
	#include <Windows.h>
#endif

enum GameState
{
	MAIN_MENU,
	GAME,
	ENDING
};

enum GameContent
{
	NORMAL,
	ENCORE
};

enum GameMode
{
	MAIN,
	TIME_ATTACK,
	SURVIVAL
};

class Game : public olc::PixelGameEngine
{
public:
	GameState state;
	GameContent content;
	GameMode mode;

	olc::GamePad* gamepad = nullptr;
public:
	MainMenu* mainMenu;
	PauseMenu* pauseMenu;
	Ending* ending;
	TimeAttack* timeAttack;
	Scoreboard* sb;
	StarMap* starMap;
	Level* levels;
	Player* player;
	EscapeNet* escapeNet;
		
	std::vector<Object*> vObjects;
	bool playerControl;

	double fixedTimeSimulated;
	float timer;
	float globalDeltaTime;
	bool bUseGamepad = false;
public:
	Game();
	~Game();

	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
	bool OnUserFixedUpdate();

	void Update();

	void DrawStringDecalXAligned(const std::string& sText, const olc::vi2d& offset, const olc::Pixel& col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f });

	void Restart();
	
	bool IsGamePadReady();
	olc::HWButton GetGamePadButton(olc::GPButtons b);
	float GetGamepadAxis(olc::GPAxes a);
	
	bool PressUp();
	bool PressDown();
	bool PressLeft();
	bool PressRight();
	bool PressConfirmButton();
};

inline Game* game = new Game();

#endif