#include "Game.h"
#include "Ending.h"

#include <time.h>

Ending::Ending()
{
    srand(time(NULL));
    mMountains =  new SpriteObject("Mountains");
    mGrass     =  new SpriteObject("Grass");
    mVictory   =  new SpriteObject("Victory");
    mEncore    =  new SpriteObject("Encore");
    mSurvive   =  new SpriteObject("Survival");
	mLpoGuy    =  new SpriteObject("LPOguy");
	mTimer = 0.0f;
	mEncoreDialogTextID = 0;
	mTimeScoreName = "";
	mCanType = true;
	mCurseWordFound = false;

	mClouds = new SpriteObject* [12];
    for (int i = 0; i < 12; i++)
    {
        mClouds[i] = new SpriteObject("Cloud");
        mClouds[i]->position = { (float)(i * 2) * 32.0f, (float)(rand() % 4) * 16.0f };
    }
}

Ending::~Ending()
{
	delete mMountains;
	delete mGrass;
	delete mVictory;
	delete mEncore;
	delete mSurvive;
	delete mLpoGuy;

	for (int i = 0; i < 12; i++)
		delete mClouds[i];

	delete[] mClouds;
}

void Ending::Init()
{
	mCanType = true;
	mTimeScoreName = "";
}

void Ending::Update()
{
	if (game->mode != TIME_ATTACK)
	{
		game->player->position = { 10.0f, 13.0f };

		game->player->size.x += 10 * game->globalDeltaTime;
		game->player->size.y += 10 * game->globalDeltaTime;

		if (game->player->size.x >= 16.0f &&
			game->player->size.y >= 16.0f)
		{
			game->player->size = { 16.0f, 16.0f };

			if (mTimer <= 1.0f)
				mTimer += game->globalDeltaTime;
		}

		if (mTimer > 1.0f)
		{
			if (game->content == NORMAL)
			{
				if (game->PressConfirmButton())
				{
					mTimer = 0.0f;
					game->Restart();
					game->state = MAIN_MENU;
				}
			}
			else if (game->content == ENCORE)
			{
				if (game->PressConfirmButton())
				{					
					if (mEncoreDialogTextID <= 13)
						mEncoreDialogTextID++;
					else
					{
						mTimer = 0.0f;
						game->Restart();
						game->state = MAIN_MENU;
						mLpoGuy->sprSheetOffset = { 0, 0 };
						mEncoreDialogTextID = 0;
					}
				}
				if (game->GetKey(olc::ESCAPE).bPressed || game->GetGamePadButton(olc::GPButtons::START).bPressed)
					mEncoreDialogTextID = 14;
			}
		}

		game->Clear(olc::CYAN);

		for (int i = 0; i < 12; i++)
			mClouds[i]->DrawFullSprite();

		mMountains->DrawFullSprite(olc::vi2d(-10, -40));

		game->FillRectDecal(olc::vi2d(0, 210), olc::vi2d(480, 110), olc::Pixel(0, 128, 255));

		for (int i = 0; i < 15; i++)
			mGrass->DrawFullSprite(olc::vi2d(i * 32, 222.0f));

		game->player->Draw();
		
		if (mTimer > 1.0f)
		{
			if (game->mode == SURVIVAL)
				mSurvive->DrawFullSprite(olc::vi2d(110, 48));
			
			if (game->content == NORMAL)
			{
				mVictory->DrawFullSprite(olc::vi2d(120, 120));
				game->DrawStringDecalXAligned("Press Enter/A button to go back to main menu!", olc::vi2d(0, 180));
			}
			else
			{
				if (game->mode == SURVIVAL)
					mEncore->DrawFullSprite(olc::vi2d(140, 16));

				EncoreDialog();
			}
		}

		if (game->content == ENCORE)
			mLpoGuy->Draw(olc::vf2d(16.0f, 12.0f) * 16, olc::WHITE, { 32, 32 });
	}
	else
	{
		game->starMap->Update();

		if (mCanType)
		{	
			if (mTimeScoreName.size() < 10)
			{
				for (int i = 1; i < 47; i++)
				{
					if (game->GetKey((olc::Key)i).bPressed)
					{
						if (i >= 27 && i <= 36)
							mTimeScoreName += (i + 21);
						else if (i >= 37 && i <= 46)
							mTimeScoreName += (i + 11);
						else
							mTimeScoreName += (i + 64);
					}
				}				
			}			

			if (game->GetKey(olc::BACK).bPressed && mTimeScoreName.size() > 0)
				mTimeScoreName.resize(mTimeScoreName.size() - 1);

			if (game->GetKey(olc::ENTER).bPressed)
			{
				mCurseWordFound = !game->escapeNet->SetName(mTimeScoreName);
				if(!mCurseWordFound)
				{
					game->escapeNet->EndPause();
					mCanType = false;

					uint32_t& minutes = game->timeAttack->mCurrentMinutes;
					uint32_t& seconds = game->timeAttack->mCurrentSeconds;
					uint32_t& miliseconds = game->timeAttack->mCurrentMiliSeconds;
					
					//Update Leaberboard
					game->escapeNet->FinishRace();
					game->sb->RefreshScores();				
				}
			}			
		}
		else
		{
			if (game->GetKey(olc::ENTER).bPressed)
			{
				game->Restart();
				game->state = MAIN_MENU;
			}
		}

		game->Clear(olc::Pixel(0, 0, 30));

		game->starMap->Draw();

		if (mCanType)
		{
			game->timeAttack->PrintTime();
			game->DrawStringDecalXAligned("Enter your name:", olc::vi2d(0, 104), olc::WHITE, {2.0f, 2.0f});
			game->FillRectDecal(olc::vi2d(game->ScreenWidth() / 2 - 5 * 16 - 5, 144), olc::vi2d(10 * 16 + 10, 16 + 10), olc::BLUE);
			game->DrawStringDecal(olc::vi2d(game->ScreenWidth() / 2 - 5 * 16, 144 + 5), mTimeScoreName,  olc::WHITE, { 2.0f, 2.0f });
			game->DrawStringDecalXAligned("Press Enter to finish name typing", olc::vi2d(0, 288));
		}
		else
		{
			std::string& name = mTimeScoreName;
			uint32_t& minutes = game->timeAttack->mCurrentMinutes;
			uint32_t& seconds = game->timeAttack->mCurrentSeconds;
			uint32_t& miliseconds = game->timeAttack->mCurrentMiliSeconds;

			game->timeAttack->PrintLeaderboard((TimeAttack::ScoreList)game->content, name, minutes, seconds, miliseconds);
			game->DrawStringDecalXAligned("Press enter to go to main menu", olc::vi2d(0, 286));
		}

		if (mCurseWordFound)
		{
			game->DrawStringDecalXAligned("FORBIDDEN WORD!!!", olc::vi2d(0, 208), olc::WHITE, { 2.0f, 2.0f });
			game->DrawStringDecalXAligned("PLEASE CHOOSE ANOTHER NAME", olc::vi2d(0, 226), olc::WHITE, { 2.0f, 2.0f });
		}
	}
    
}

void Ending::EncoreDialog()
{
	if (mEncoreDialogTextID < 14)
	{
		game->FillRectDecal(olc::vi2d(22, 243), olc::vi2d(435, 60), olc::BLUE);
		game->FillRectDecal(olc::vi2d(22, 243), olc::vi2d(435, 1));
		game->FillRectDecal(olc::vi2d(22, 243), olc::vi2d(1, 60));
		game->FillRectDecal(olc::vi2d(22, 303), olc::vi2d(435, 1));
		game->FillRectDecal(olc::vi2d(457, 243), olc::vi2d(1, 60));
		game->DrawStringDecal(olc::vi2d(game->ScreenWidth() / 2 - 23 * 8, 307), "Enter/A button:Continue        ESC/Start:Skip");
	}
	else if (mEncoreDialogTextID == 14)
		game->DrawStringDecalXAligned("Press Enter/A button to go back to main menu!", olc::vi2d(0, 307));

	if (mEncoreDialogTextID == 0)
	{
		game->DrawStringDecal(olc::vi2d(29, 250), "Save the Worlds guy:");
		game->DrawStringDecal(olc::vi2d(29, 260), "Hello!");
	}
	else if (mEncoreDialogTextID == 1)
	{
		game->DrawStringDecal(olc::vi2d(27, 250), "Escape the Machine guy:");
		game->DrawStringDecal(olc::vi2d(27, 260), "Hi.");
	}
	else if (mEncoreDialogTextID == 2)
	{
		game->DrawStringDecal(olc::vi2d(27, 250), "Save the Worlds guy:");
		game->DrawStringDecal(olc::vi2d(27, 260), "How's it going?");
	}
	else if (mEncoreDialogTextID == 3)
	{
		game->DrawStringDecal(olc::vi2d(27, 250), "Escape the Machine guy:");
		game->DrawStringDecal(olc::vi2d(27, 260), "Oh, you know. Escaping the machine and all.");
		game->DrawStringDecal(olc::vi2d(27, 270), "You?");
	}
	else if (mEncoreDialogTextID == 4)
	{
		game->DrawStringDecal(olc::vi2d(27, 250), "Save the Worlds guy:");
		game->DrawStringDecal(olc::vi2d(27, 260), "Eh, not much. Saving the world and stuff.");
	}
	else if (mEncoreDialogTextID == 5)
	{
		game->DrawStringDecal(olc::vi2d(27, 250), "Escape the Machine guy:");
		game->DrawStringDecal(olc::vi2d(27, 260), "Oh cool. Wish i could do that.");
	}
	else if (mEncoreDialogTextID == 6)
	{
		game->DrawStringDecal(olc::vi2d(27, 250), "Save the Worlds guy:");
		game->DrawStringDecal(olc::vi2d(27, 260), "Would you believe all i had to do is to collect");
		game->DrawStringDecal(olc::vi2d(27, 270), "certain coins just to have the graphics of the");
		game->DrawStringDecal(olc::vi2d(27, 280), "world reappear?");
	}
	else if (mEncoreDialogTextID == 7)
	{
		game->DrawStringDecal(olc::vi2d(27, 250), "Escape the Machine guy:");
		game->DrawStringDecal(olc::vi2d(27, 260), "Wow! That easy?");
	}
	else if (mEncoreDialogTextID == 8)
	{
		game->DrawStringDecal(olc::vi2d(27, 250), "Save the Worlds guy:");
		game->DrawStringDecal(olc::vi2d(27, 260), "Apparently.");
	}
	else if (mEncoreDialogTextID == 9)
	{
		game->DrawStringDecal(olc::vi2d(27, 250), "Escape the Machine guy:");
		game->DrawStringDecal(olc::vi2d(27, 260), "Huh. I oughta give it a try.");
	}
	else if (mEncoreDialogTextID == 10)
	{
		game->DrawStringDecal(olc::vi2d(27, 250), "Save the Worlds guy:");
		game->DrawStringDecal(olc::vi2d(27, 260), "........");
	}
	else if (mEncoreDialogTextID == 11)
	{
		game->DrawStringDecal(olc::vi2d(27, 250), "Save the Worlds guy:");
		game->DrawStringDecal(olc::vi2d(27, 260), "So the programmer still can't draw graphics");
		game->DrawStringDecal(olc::vi2d(27, 270), "for your game?");
	}
	else if (mEncoreDialogTextID == 12)
	{
		game->DrawStringDecal(olc::vi2d(27, 250), "Escape the Machine guy:");
		game->DrawStringDecal(olc::vi2d(27, 260), "Nope.");
	}
	else if (mEncoreDialogTextID == 13)
	{
		game->DrawStringDecal(olc::vi2d(27, 250), "Save the Worlds guy:");
		game->DrawStringDecal(olc::vi2d(27, 260), "Figures. Talk about lazy dev, am i right?");
	}
	
}
