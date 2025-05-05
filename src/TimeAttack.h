#ifndef TIMEATTACK_H
#define TIMEATTACK_H

#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <cstdint>
#include <array>

struct TimeScoreData
{
	std::string name;
	std::string buffer;
};

class TimeAttack
{
private:
	std::string mInputName;
	bool mCanType;
	int m_nStr;
	std::stringstream mTimeScoreBuf;
	std::stringstream mCurrentTimeScoreBuf;
public:
	unsigned int mCurrentMinutes;
	unsigned int mCurrentSeconds;
	unsigned int mCurrentMiliSeconds;
	bool refreshLB;
	bool timeRunning;
	enum ScoreList {NORMAL, ENCORE} scoreList;
public:
	TimeAttack();
	void Update();
	
	void Start();
	void PrintTime();

	void ClearTimeBuffer();

	void PrintLeaderboard(ScoreList sl, const std::string& name = "", int minutes = -1, int seconds = -1, int miliseconds = -1);
	void Reset();
private:
	TimeScoreData GetTime(std::tuple<std::string, uint32_t, uint32_t, uint32_t>& item);
	std::string GetTimer();
};
#endif