#ifndef ESCAPENET_H
#define ESCAPENET_H

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#endif

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

struct LeaderboardEntry {
    std::string name;
    std::string mode;
    int time;
};

class EscapeNet
{
public:
    EscapeNet();

    bool InitSession();
    bool SetName(const std::string& name);

    bool StartRace(const std::string& mode);
    bool StopRace();
    std::pair<int, bool> FinishRace();

    bool StartPause();
    bool EndPause();

    void IncrementCounter(const std::string& mode);

    int GetCurrentRaceTime();
    std::vector<LeaderboardEntry> GetLeaderboard(const std::string& mode, const int offset = 0, const int limit = 100, const std::string& sortBy = "time", bool ascending = true);

private:
    std::chrono::time_point<std::chrono::system_clock> m_tp1;
    std::chrono::time_point<std::chrono::system_clock> m_tp2;
    std::chrono::time_point<std::chrono::system_clock> m_pause_tp1;
    std::chrono::time_point<std::chrono::system_clock> m_pause_tp2;

    std::string m_name;
    std::string m_mode;
    int m_time;
    int m_pause_time;
    
    std::string raceId;
};

#endif
