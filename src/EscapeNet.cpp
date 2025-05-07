#include <fstream>

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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LeaderboardEntry, name, mode, time)

extern "C"
{
    std::string g_raceMode = "";
    std::string g_raceName = "";
    int g_raceTime = 0;
    std::vector<LeaderboardEntry> g_leaderboard;
    std::vector<LeaderboardEntry> g_filteredLeaderboard;

    int escapeNet__initSession()
    { 
        using nlohmann::json;

        std::ifstream f("races.json");
        if(!f.is_open())
        {
            for(int i = 0; i < 10; i++)
            {
                g_leaderboard.push_back({ "ALEXIO", "normal", 5999999 });
                g_leaderboard.push_back({ "ALEXIO", "encore", 5999999 });
            }

            // --- Save to JSON file ---
            json json_output;
            json_output["leaderboard"] = g_leaderboard; // Serialize the vector

            std::ofstream o("races.json");
            if (o.is_open()) {
                o << json_output.dump(4); // Pretty print with 4-space indent
                o.close();
                std::cout << "Leaderboard saved to leaderboard.json\n";
            } else {
                std::cerr << "Failed to open file for writing\n";
                return 1;
            }

            f.open("races.json");
        }

        json json_input;
        f >> json_input; // Parse JSON
        f.close();
    
        // Deserialize the vector
        g_leaderboard = json_input["leaderboard"].get<std::vector<LeaderboardEntry>>();
        
        for(auto entry : g_leaderboard)
        {
            std::cout << entry.mode << " " << entry.name << " " << entry.time << "\n";
        }
        
        std::cout << "escapeNet__initSession is not implemented on this platform, artificially succeeding.\n";
        return 1;
    }
    
    int escapeNet__setRacerName(const char* str)
    {
        std::cout << "escapeNet__setRacerName is not implemented on this platform, artificially succeeding.\n";
        g_raceName = str;
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
        using nlohmann::json;

        g_raceMode = raceMode;
        g_raceTime = raceTime;

        g_leaderboard.push_back({
            g_raceName,
            g_raceMode,
            g_raceTime,
        });

        // --- Save to JSON file ---
        json json_output;
        json_output["leaderboard"] = g_leaderboard; // Serialize the vector

        std::ofstream o("races.json");
        if (o.is_open()) {
            o << json_output.dump(4); // Pretty print with 4-space indent
            o.close();
            std::cout << "Leaderboard saved to leaderboard.json\n";
        } else {
            std::cerr << "Failed to open file for writing\n";
            return 1;
        }

        std::cout << "escapeNet__finishRace is not implemented on this platform, artificially succeeding.\n";
        return 1;
    }
    
    int escapeNet__getLeaderboard(const char *mode, const char *sortBy, int offset, int limit, int ascending)
    {
        // Work on a copy to filter and sort, then assign back
        g_filteredLeaderboard = g_leaderboard;
        
        // filter mode
        if(mode && std::strlen(mode) > 0)
        {
            g_filteredLeaderboard.erase(
                std::remove_if(
                    g_filteredLeaderboard.begin(),
                    g_filteredLeaderboard.end(),
                    [mode](const LeaderboardEntry& entry) {
                        return entry.mode != mode;
                    }
                ),
                g_filteredLeaderboard.end()
            );
        }

        // Sort based on sortBy parameter
        if (sortBy) {
            if (std::strcmp(sortBy, "name") == 0) {
                std::sort(g_filteredLeaderboard.begin(), g_filteredLeaderboard.end(),
                    [ascending](const LeaderboardEntry& a, const LeaderboardEntry& b) {
                        return ascending ? a.name < b.name : a.name > b.name;
                    });
            } else if (std::strcmp(sortBy, "time") == 0) {
                std::sort(g_filteredLeaderboard.begin(), g_filteredLeaderboard.end(),
                    [ascending](const LeaderboardEntry& a, const LeaderboardEntry& b) {
                        return ascending ? a.time < b.time : a.time > b.time;
                    });
            } else if (std::strcmp(sortBy, "mode") == 0) {
                std::sort(g_filteredLeaderboard.begin(), g_filteredLeaderboard.end(),
                    [ascending](const LeaderboardEntry& a, const LeaderboardEntry& b) {
                        return ascending ? a.mode < b.mode : a.mode > b.mode;
                    });
            }
        }

        // Apply offset and limit
        if (offset < 0) offset = 0;
        if (limit < 0) limit = g_filteredLeaderboard.size();

        // Ensure offset doesn't exceed vector size
        if (offset >= static_cast<int>(g_filteredLeaderboard.size())) {
            g_filteredLeaderboard.clear();
            return 0;
        }

        // Calculate end iterator for slicing
        auto start = g_filteredLeaderboard.begin() + offset;
        auto end = (offset + limit <= static_cast<int>(g_filteredLeaderboard.size())) 
            ? start + limit 
            : g_filteredLeaderboard.end();

        // Resize to keep only the desired range
        g_filteredLeaderboard.erase(end, g_filteredLeaderboard.end());
        if (start != g_filteredLeaderboard.begin()) {
            g_filteredLeaderboard.erase(g_filteredLeaderboard.begin(), start);
        }

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
        leaderboard = g_filteredLeaderboard;
    #endif
    }

    return leaderboard;
}
