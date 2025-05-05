#include "Game.h"
#include "TimeAttack.h"

TimeAttack::TimeAttack()
{
    mInputName = "";
    mCanType = true;
    refreshLB = true;
    timeRunning = false;
    m_nStr = 0;

    mCurrentMiliSeconds = 0;
    mCurrentSeconds = 0;
    mCurrentMinutes = 0;

    scoreList = ScoreList::NORMAL;
}

void TimeAttack::Update()
{     
    int duration = game->escapeNet->GetCurrentRaceTime();
    
    mCurrentMiliSeconds = duration % 1000;
    mCurrentSeconds     = (duration / 1000) % 60;
    mCurrentMinutes     = (duration / 60000);
}

void TimeAttack::Start()
{
    std::cout << "START\n";
    game->escapeNet->StartRace(game->mainMenu->strMode);
}

void TimeAttack::PrintTime()
{
   if (game->state != GameState::ENDING)
   {
       game->DrawStringDecal(olc::vi2d(0, 0), GetTimer(),
           olc::WHITE, { 2.0f, 2.0f });
   }
   else
   {       
       game->DrawStringDecal(olc::vi2d(game->ScreenWidth() / 2 - (GetTimer().size() / 2) * 24, 24), GetTimer(),
           olc::WHITE, { 3.0f, 3.0f });
   }    
}

void TimeAttack::ClearTimeBuffer()
{
    mTimeScoreBuf.str(std::string());
}

void TimeAttack::PrintLeaderboard(ScoreList sl, const std::string& name, int minutes, int seconds , int miliseconds)
{
    std::string mode = (sl == ScoreList::NORMAL ? "NORMAL" : "ENCORE");
    auto& items = (sl == ScoreList::NORMAL ? game->sb->vNormalScores : game->sb->vEncoreScores); 

    std::vector<olc::Pixel> listColors;

    for (int i = 0; i < items.size(); i++)
        listColors.push_back(olc::WHITE);

    game->DrawStringDecal(olc::vi2d(182, 5), mode, olc::WHITE, { 3.0f, 3.0f });
    for (int i = 0; i < items.size(); i++)
    {
        if (name == std::get<0>(items[i]) &&
            minutes == std::get<1>(items[i]) &&
            seconds == std::get<2>(items[i]) &&
            miliseconds == std::get<3>(items[i]))
        {
            listColors[i] = olc::YELLOW;
        }

        game->DrawStringDecal(olc::vi2d(144, i * 24 + 48), GetTime(items[i]).name, listColors[i]);
        game->DrawStringDecal(olc::vi2d(288, i * 24 + 48), GetTime(items[i]).buffer, listColors[i]);
        
    }
}

void TimeAttack::Reset()
{
    std::cout << "RESET\n";
    Start();
}

TimeScoreData TimeAttack::GetTime(std::tuple<std::string, uint32_t, uint32_t, uint32_t>& item)
{
    // name             item.get<0>
    // minutes          item.get<1>
    // seconds          item.get<2>
    // milliseconds     item.get<3>
    
    mTimeScoreBuf.str(std::string());

    mTimeScoreBuf << std::setfill('0') << std::setw(2) << std::get<1>(item) << ":";
    mTimeScoreBuf << std::setfill('0') << std::setw(2) << std::get<2>(item) << ".";
    mTimeScoreBuf << std::setfill('0') << std::setw(3) << std::get<3>(item);

    return { std::get<0>(item), mTimeScoreBuf.str() };
}

std::string TimeAttack::GetTimer()
{
    mCurrentTimeScoreBuf.str(std::string());

    mCurrentTimeScoreBuf << std::setfill('0') << std::setw(2) << mCurrentMinutes << ":";
    mCurrentTimeScoreBuf << std::setfill('0') << std::setw(2) << mCurrentSeconds << ".";
    mCurrentTimeScoreBuf << std::setfill('0') << std::setw(3) << mCurrentMiliSeconds;

    return mCurrentTimeScoreBuf.str();
}
