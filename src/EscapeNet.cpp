#include "EscapeNet.h"
#include "json.hpp"

#ifdef __EMSCRIPTEN__

EM_JS(int, escapeNet__initSession, (), {
    return Asyncify.handleSleep(function(wakeUp) {
        fetch('/session', { method: 'POST', credentials: 'same-origin' })
            .then((response) =>
            {
                return response.ok
                ? response.json().then((data) => JSON.stringify(data, null, 2))
                : Promise.reject(new Error("Unexpected response"));
            })
            .then((message) =>
            {
                console.log(message);
                wakeUp(1);
            })
            .catch((err) =>
            {
                if(err === "unwind") return;
                console.error(err);
                wakeUp(0);
            })
    });
});

EM_JS(int, escapeNet__setRacerName, (const char* str), {
    
    let racerName = UTF8ToString(str);
    
    return Asyncify.handleSleep(function(wakeUp) {
        fetch('/name', {
            method: 'POST',
            credentials: 'same-origin',
            headers: {
                "Content-Type": "application/json",
            },
            body: JSON.stringify({ userName: racerName })
        }).then((response) =>
        {
            return response.ok
            ? response.json().then((data) => JSON.stringify(data, null, 2))
            : Promise.reject(new Error("Unexpected response"));
        })
        .then((message) =>
        {
            wakeUp(1);
        })
        .catch((err) =>
        {
            if(err === "unwind") return;
            console.error(err.message);
            wakeUp(0);
        })    
    });
});

EM_JS(int, escapeNet__startRace, (), {
    
    return Asyncify.handleSleep(function(wakeUp) {
        fetch('/race', {
            method: 'POST',
            credentials: 'same-origin',
            headers: {
                "Content-Type": "application/json",
            },
            body: JSON.stringify({})
        }).then((response) =>
        {
            return response.ok
            ? response.json()
            : Promise.reject(new Error("Unexpected response"));
        })
        .then((message) =>
        {
            Module.escapeRaceId = message.raceId;
            wakeUp(1);
        })
        .catch((err) =>
        {
            if(err === "unwind") return;
            console.error(err.message);
            wakeUp(0);
        })    
    });
});

EM_JS(int, escapeNet__stopRace, (), {
    
    if(Module.escapeRaceId === undefined)
    {
        console.error("Trying to finish a race that never start!");
        return 0;
    }

    return Asyncify.handleSleep(function(wakeUp) {
        fetch('/race', {
            method: 'DELETE',
            credentials: 'same-origin',
            headers: {
                "Content-Type": "application/json",
            },
            body: JSON.stringify({ raceId: Module.escapeRaceId })
        }).then((response) =>
        {
            return response.ok
            ? response.json()
            : Promise.reject(new Error("Unexpected response"));
        })
        .then((message) =>
        {
            wakeUp(1);
        })
        .catch((err) =>
        {
            if(err === "unwind") return;
            console.error(err.message);
            wakeUp(0);
        })    
    });
});


EM_JS(int, escapeNet__finishRace, (const char* raceMode, int raceTime), {
    
    if(Module.escapeRaceId === undefined)
    {
        console.error("Trying to finish a race that never start!");
        return 0;
    }
    
    const raceData = {
        raceId: Module.escapeRaceId,
        raceTime: raceTime,
        raceMode: UTF8ToString(raceMode),
    };
    
    return Asyncify.handleSleep(function(wakeUp) {
        fetch('/race', {
            method: 'PATCH',
            credentials: 'same-origin',
            headers: {
                "Content-Type": "application/json",
            },
            body: JSON.stringify(raceData)
        }).then((response) =>
        {
            return response.ok
            ? response.json().then((data) => JSON.stringify(data, null, 2))
            : Promise.reject(new Error("Unexpected response"));
        })
        .then((message) =>
        {
            wakeUp(1);
        })
        .catch((err) =>
        {
            if(err === "unwind") return;
            console.error(err.message);
            wakeUp(0);
        })    
    });
});

EM_JS(int, escapeNet__startPause, (), {
    

    return Asyncify.handleSleep(function(wakeUp) {
        fetch('/pause', {
            method: 'POST',
            credentials: 'same-origin',
            headers: {
                "Content-Type": "application/json",
            },
            body: JSON.stringify({raceId: Module.escapeRaceId})
        }).then((response) =>
        {
            return response.ok
            ? response.json()
            : Promise.reject(new Error("Unexpected response"));
        })
        .then((message) =>
        {
            wakeUp(1);
        })
        .catch((err) =>
        {
            if(err === "unwind") return;
            console.error(err.message);
            wakeUp(0);
        })    
    });
});

EM_JS(int, escapeNet__endPause, (), {
    
    return Asyncify.handleSleep(function(wakeUp) {
        fetch('/pause', {
            method: 'PATCH',
            credentials: 'same-origin',
            headers: {
                "Content-Type": "application/json",
            },
            body: JSON.stringify({raceId: Module.escapeRaceId})
        }).then((response) =>
        {
            return response.ok
            ? response.json()
            : Promise.reject(new Error("Unexpected response"));
        })
        .then((message) =>
        {
            wakeUp(1);
        })
        .catch((err) =>
        {
            if(err === "unwind") return;
            console.error(err.message);
            wakeUp(0);
        })    
    });
});

EM_JS(int, escapeNet__getLeaderboard, (const char* mode, const char* sortBy, const int offset, const int limit, const int ascending), {

    return Asyncify.handleSleep(function(wakeUp) {
        // just some rudimentary input validation
        const params = {
            sort: ((ascending == 1) ? "asc" : "desc"),
            mode: encodeURIComponent(UTF8ToString(mode)),
            offset: offset,
            limit: limit,
            sortBy: "id"
        };

        if(['id', 'mode', 'time', 'created_at'].indexOf(UTF8ToString(sortBy)) !== -1)
        {
            params.sortBy = UTF8ToString(sortBy);
        }

        fetch(`/race?mode=${params.mode}&sortBy=${params.sortBy}&sort=${params.sort}&offset=${params.offset}&limit=${params.limit}`, {
            method: 'GET',
            credentials: 'same-origin',
        }).then((response) =>
        {
            return response.ok
            ? response.json()
            : Promise.reject(new Error("Unexpected response"));
        })
        .then((message) =>
        {
            // set aside the results, so we can use them in the next phase.
            Module._leaderboardResults = JSON.stringify(message.results);
            wakeUp(1);
        })
        .catch((err) =>
        {
            if(err === "unwind") return;
            console.error(err.message);
            wakeUp(0);
        });
    });
});

#else
extern "C"
{
    int escapeNet__initSession()
    { 
        std::cout << "escapeNet__initSession is not implemented on this platform, artificially succeeding.\n";
        return 1;
    }
    
    int escapeNet__setRacerName(const char* str)
    {
        std::cout << "escapeNet__setRacerName is not implemented on this platform, artificially succeeding.\n";
        return 1;
    }
    
    int escapeNet__startRace()
    {
        std::cout << "escapeNet__startRace is not implemented on this platform, artificially succeeding.\n";
        return 1;
    }
    
    int escapeNet__stopRace()
    {
        std::cout << "escapeNet__stopRace is not implemented on this platform, artificially succeeding.\n";
        return 1;
    }

    int escapeNet__finishRace(const char* raceMode, int raceTime)
    {
        std::cout << "escapeNet__finishRace is not implemented on this platform, artificially succeeding.\n";
        return 1;
    }
    
    int escapeNet__getLeaderboard(const char *map, const char *sortBy, int offset, int limit, int ascending)
    {
        std::cout << "escapeNet__getLeaderboard is not implemented on this platform, artificially succeeding.\n";
        return 1;
    }

    int escapeNet__startPause()
    {
        std::cout << "escapeNet__startPause is not implemented on this platform, artificially succeeding.\n";
        return 1;
    }

    int escapeNet__endPause()
    {
        std::cout << "escapeNet__endPause is not implemented on this platform, artificially succeeding.\n";
        return 1;
    }

}
#endif

EscapeNet::EscapeNet()
{
}

bool EscapeNet::InitSession()
{
    return (escapeNet__initSession() == 1);
}

bool EscapeNet::SetName(const std::string& name)
{
    m_name = name;
    return (escapeNet__setRacerName(m_name.c_str()) == 1);
}

bool EscapeNet::StartRace(const std::string& mode)
{
    m_mode = mode;
    m_tp1 = std::chrono::system_clock::now();
    m_tp2 = std::chrono::system_clock::now();
    m_pause_time = 0;

    return (escapeNet__startRace() == 1);
}

int EscapeNet::GetCurrentRaceTime()
{
    std::chrono::duration<double, std::milli> duration = std::chrono::system_clock::now() - m_tp1;
    return static_cast<int>(duration.count()) - m_pause_time;
}

std::pair<int, bool> EscapeNet::FinishRace()
{
    m_tp2 = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> duration = m_tp2 - m_tp1;
    m_time = static_cast<int>(duration.count());
    
    return { m_time - m_pause_time, (escapeNet__finishRace(m_mode.c_str(), m_time - m_pause_time) == 1)};
}

bool EscapeNet::StartPause()
{
    m_pause_tp1 = std::chrono::system_clock::now();
    m_pause_tp2 = std::chrono::system_clock::now();

    return (escapeNet__startPause() == 1);
}

bool EscapeNet::StopRace()
{
    return (escapeNet__stopRace() == 1);
}

bool EscapeNet::EndPause()
{
    m_pause_tp2 = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> duration = m_pause_tp2 - m_pause_tp1;
    m_pause_time += static_cast<int>(duration.count());

    return (escapeNet__endPause() == 1);
}

std::vector<LeaderboardEntry> EscapeNet::GetLeaderboard(const std::string& mode, const int offset, const int limit, const std::string& sortBy, bool ascending)
{
    std::vector<LeaderboardEntry> leaderboard;

    int result = escapeNet__getLeaderboard(mode.c_str(), sortBy.c_str(), offset, limit, (ascending) ? 1 : 0);
    
    if(result == 1)
    {
    #if defined(__EMSCRIPTEN__)

        char* leaderboardJsonString = (char*)EM_ASM_PTR({
                
            // get the number of bytes we need to allocate to fit the string
            let lengthBytes = lengthBytesUTF8(Module._leaderboardResults) + 1;

            // allocate enough memory to hold the string
            let stringOnWasmHeap = _malloc(lengthBytes);
            
            // copy the javascript string into the heap
            stringToUTF8(Module._leaderboardResults, stringOnWasmHeap, lengthBytes);
            
            // we're done with this, remove it!
            delete Module._leaderboardResults;
            
            // return the pointer
            return stringOnWasmHeap;
        });        
    
        using json = nlohmann::json;

        json leaderboardJson = json::parse(leaderboardJsonString);

        free(leaderboardJsonString);

        for(auto &el : leaderboardJson.items())
        {
            // std::string name;
            // std::string mode;
            // int time;
            leaderboard.push_back(LeaderboardEntry{
                el.value().at("name"),
                el.value().at("mode"),
                el.value().at("time"),
            });
        }
        
        std::cout << "get leaderboard successful\n";
    #else
        std::cout << "EscapeNet::GetLeaderboard is not implemented for this platform\n";
    #endif
    }

    return leaderboard;
}
