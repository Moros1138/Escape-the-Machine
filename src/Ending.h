#ifndef ENDING_H
#define ENDING_H

#include <memory>
#include <array>

#include "Object.h"

class Ending
{
private:	
	SpriteObject* mMountains = nullptr;
	SpriteObject* mGrass = nullptr;
	SpriteObject* mVictory = nullptr;
	SpriteObject* mSurvive = nullptr;
	SpriteObject* mEncore = nullptr;
	SpriteObject* mLpoGuy = nullptr;
	SpriteObject** mClouds;
	std::string mTimeScoreName;
	int mEncoreDialogTextID;
	float mTimer;
	bool mCanType;
	bool mCurseWordFound;

public:
	Ending();
	~Ending();

	void Init();
	void Update();
	

private:
	void EncoreDialog();
};

#endif