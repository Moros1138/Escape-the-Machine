#include "Game.h"

constexpr float thirtyFramesPerSecond = 1.0f / 30.f;

Game::Game()
{
    sAppName = "Escape the Machine";
}

Game::~Game()
{
    delete mainMenu;
    delete pauseMenu;
    delete ending;
    delete timeAttack;
    delete starMap;
    delete player;
    delete levels;
    delete sb;

    for (auto& obj : vObjects)
        delete obj;

    vObjects.clear();
}

bool Game::OnUserCreate()
{
    ImageAssets::get().LoadSprites();

    timer = 0.0f;
    fixedTimeSimulated = 0.0f;
    globalDeltaTime = 0.0f;

    olc::GamePad::init();

    playerControl = true;

    state = MAIN_MENU;

    mainMenu    = new MainMenu();
    pauseMenu   = new PauseMenu();
    ending      = new Ending();
    timeAttack  = new TimeAttack();
    starMap     = new StarMap();
    player      = new Player(olc::vf2d(2.0f, 17.0f), olc::BLUE);
    levels      = new Level();
    sb          = new Scoreboard();
    escapeNet   = new EscapeNet();

    starMap->Create();
    escapeNet->InitSession();

    return true;
}

bool Game::OnUserUpdate(float fElapsedTime)
{
    fElapsedTime = std::clamp(fElapsedTime, 0.0f, thirtyFramesPerSecond);
    globalDeltaTime = fElapsedTime;

    if (gamepad == nullptr || !gamepad->stillConnected)
        gamepad = olc::GamePad::selectWithAnyButton();

    if (state != ENDING)
        starMap->Update();

    switch (state)
    {    
    case MAIN_MENU: mainMenu->Update();  break;
    case GAME:      Update();            break;
    case ENDING:    ending->Update();    break;
    }    

    if (state != ENDING)
        starMap->Draw();


    return true;
}

bool Game::OnUserFixedUpdate()
{
    bool OutOfBoundary =
        player->position.x < 0.0f || player->position.x > ScreenWidth() / 16 ||
        player->position.y < 0.0f || player->position.y > ScreenHeight() / 16;

    if (OutOfBoundary || mainMenu->gameStart)
    {
        levels->Load(vObjects, mainMenu->strMode, mainMenu->gameStart);

        if (mainMenu->gameStart)
            mainMenu->gameStart = false;
    }

    player->Behaviour();

    for (auto& obj : vObjects)
    {
        if (player->position.x + 0.5f > obj->position.x && player->position.y + 0.5f > obj->position.y &&
            player->position.x + 0.5f < obj->position.x + 1.0f && player->position.y + 0.5f < obj->position.y + 1.0f)
            obj->Behaviour();
    }
    if (levels->GetTile(player->position.x + 0.5f, player->position.y + 0.5f) == 'C')
    {
        if (mode == TIME_ATTACK)
        {
            // hack to trigger once
            if(timeAttack->timeRunning)
            {
                std::cout << "END RACE\n";
                escapeNet->StartPause();
            }
            timeAttack->timeRunning = false;
        }
            
        playerControl = false;
        if (player->size.x <= 0.0f &&
            player->size.y <= 0.0f)
        {
            vObjects.clear();
            state = ENDING;
            ending->Init();
        }
    }

    return fixedTimeSimulated < timer;
}

void Game::Update()
{
    if ((GetKey(olc::ESCAPE).bPressed || GetGamePadButton(olc::GPButtons::START).bPressed) && !pauseMenu->bIsOn)
    {
        pauseMenu->bIsOn = true;
        if(timeAttack->timeRunning)
        {
            std::cout << "PAUSE START\n";
            escapeNet->StartPause();
        }
    }
        

    if (pauseMenu->bIsOn)
        pauseMenu->Update(levels, timeAttack, player->initPosition);
    else
    {
        if (mode == TIME_ATTACK && timeAttack->timeRunning)
        {
            timeAttack->Update();
        }

        if (levels->bHasFloorSwap)
            levels->Update();

        while (OnUserFixedUpdate())
        {
            fixedTimeSimulated += 1.0f / 60.0f;
        }
    } 

    Clear(olc::Pixel(0, 0, 30));

    starMap->Draw();

    levels->Draw();

    for (auto& obj : vObjects)
        obj->Draw();

    player->Draw();

    if (pauseMenu->bIsOn)
        pauseMenu->Print();

    if (mode == TIME_ATTACK)
        timeAttack->PrintTime();
}

void Game::DrawStringDecalXAligned(const std::string& sText, const olc::vi2d& offset, const olc::Pixel& col, const olc::vf2d& scale)
{
    DrawStringDecal(olc::vi2d(ScreenWidth() / 2 - sText.size() / 2 * (scale.x * 8) + offset.x, offset.y), sText, col, scale);
}

void Game::Restart()
{
    if (state == MAIN_MENU || state == ENDING ||
        mode == SURVIVAL || pauseMenu->bIsOn)
    {
        levels->nID = 1;
        levels->strID = "1";
        player->position = { 2.0f, 17.0f };
        player->initPosition = player->position;
        
        if (levels->colorFloorLockLevel)
        {
            levels->colorFloorLockLevel = false;
            ColorFloor::sClearedFloorCount = 0;
        }
        if (levels->branched)
            levels->branched = false;

        if (state == MAIN_MENU || state == ENDING)
            mainMenu->Reset();
    }

    player->position = player->initPosition;
    if (state != ENDING)
        levels->Load(vObjects, mainMenu->strMode, false);
    player->size = { 16.0f, 16.0f };

    if (state == MAIN_MENU && mode == TIME_ATTACK)
        timeAttack->timeRunning = true;
    
    if (levels->colorFloorLockLevel)
        ColorFloor::sClearedFloorCount = ColorFloor::sStartingClearedFloorCount;
    player->color = olc::BLUE;
    playerControl = true;
}

olc::HWButton Game::GetGamePadButton(olc::GPButtons b)
{
    if (gamepad != nullptr && gamepad->stillConnected)
            return gamepad->getButton(b);
    return olc::HWButton();
}

float Game::GetGamepadAxis(olc::GPAxes a)
{
    if (gamepad != nullptr && gamepad->stillConnected)
    {
        if (std::abs(gamepad->getAxis(a)) > 0.3f)
            return gamepad->getAxis(a);
    }
    return 0.0f;
}

bool Game::PressUp()
{
    return GetKey(olc::W).bPressed || GetKey(olc::UP).bPressed || GetGamePadButton(olc::GPButtons::DPAD_U).bPressed;
}

bool Game::PressDown()
{
    return GetKey(olc::S).bPressed || GetKey(olc::DOWN).bPressed || GetGamePadButton(olc::GPButtons::DPAD_D).bPressed;
}

bool Game::PressLeft()
{
    return GetKey(olc::A).bPressed || GetKey(olc::LEFT).bPressed || GetGamePadButton(olc::GPButtons::DPAD_L).bPressed;
}

bool Game::PressRight()
{
    return GetKey(olc::D).bPressed || GetKey(olc::RIGHT).bPressed || GetGamePadButton(olc::GPButtons::DPAD_R).bPressed;
}

bool Game::PressConfirmButton()
{
    return game->GetKey(olc::ENTER).bPressed || game->GetGamePadButton(olc::GPButtons::FACE_D).bPressed;
}
