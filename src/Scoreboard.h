#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "EscapeNet.h"
#include <cstdint>
#include <iostream>
#include <vector>
#include <map>

class Scoreboard
{
public:
    Scoreboard();
    
    void RefreshScores();
    void RefreshNormalScores();
    void RefreshEncoreScores();

private:
    std::string sBasePath;

public:
    std::vector<std::tuple<std::string, uint32_t, uint32_t, uint32_t>> vNormalScores;
    std::vector<std::tuple<std::string, uint32_t, uint32_t, uint32_t>> vEncoreScores;
};

#endif
