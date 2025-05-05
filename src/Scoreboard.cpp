#include "Game.h"
#include "Scoreboard.h"

Scoreboard::Scoreboard()
{
}

void Scoreboard::RefreshScores()
{
    RefreshNormalScores();
    RefreshEncoreScores();
}

void Scoreboard::RefreshNormalScores()
{
    std::vector<LeaderboardEntry> leaderboard = game->escapeNet->GetLeaderboard("normal", 0, 10, "time", true);
    vNormalScores.clear();
    for(auto entry: leaderboard)
    {
        int miliseconds = entry.time % 1000;
        int seconds = (entry.time / 1000) % 60;
        int minutes = entry.time / 60000;
        vNormalScores.push_back({entry.name, minutes, seconds, miliseconds});
    }
}

void Scoreboard::RefreshEncoreScores()
{
    std::vector<LeaderboardEntry> leaderboard = game->escapeNet->GetLeaderboard("encore", 0, 10, "time", true);
    vEncoreScores.clear();
    for(auto entry: leaderboard)
    {
        int miliseconds = entry.time % 1000;
        int seconds = (entry.time / 1000) % 60;
        int minutes = entry.time / 60000;
        vEncoreScores.push_back({entry.name, minutes, seconds, miliseconds});
    }
}
