// CornholeSchedule.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>

#pragma warning(disable:4996)

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef std::vector< uint32_t > PlayerChoiceVector;

class Rand
{
public:
    Rand(uint64_t seed = 0)
    {
        mCurrent = seed;
    };

    uint64_t get(void)
    {
        return ((mCurrent = mCurrent * 214013L + 2531011L) & 0x7fffffff);
    };

    // random number between 0.0 and 1.0
    float ranf(void)
    {
        uint64_t v = get() & 0x7FFF;
        return (float)v * (1.0f / 32767.0f);
    };

    float ranf(float low, float high)
    {
        return (high - low) * ranf() + low;
    }

    void setSeed(uint64_t seed)
    {
        mCurrent = seed;
    };

private:
    uint64_t mCurrent;
};

class RandPool
{
public:
    RandPool(void)
    {
    }
    void init(uint64_t size, uint64_t seed) // size of random number bool.
    {
        mRand.setSeed(seed); // init random number generator.
        mData = (uint64_t*)malloc(size_t(sizeof(uint64_t) * size));
        mSize = size;
        mTop = mSize;
        for (uint64_t i = 0; i < mSize; i++)
        {
            mData[i] = i;
        }
    }

    ~RandPool(void)
    {
        free(mData);
    };

    // pull a number from the random number pool, will never return the
    // same number twice until the 'deck' (pool) has been exhausted.
    // Will set the shuffled flag to true if the deck/pool was exhausted
    // on this call.
    uint64_t get(bool& shuffled)
    {
        if (mTop == 0) // deck exhausted, shuffle deck.
        {
            shuffled = true;
            mTop = mSize;
        }
        else
        {
            shuffled = false;
        }
        uint64_t entry = mRand.get() % mTop;
        mTop--;
        uint64_t ret = mData[entry]; // swap top of pool with entry
        mData[entry] = mData[mTop]; // returned
        mData[mTop] = ret;
        return ret;
    };

    float ranf(void)
    {
        return mRand.ranf();
    };

    uint64_t get(void)
    {
        return mRand.get();
    }

private:
    Rand mRand; // random number generator.
    uint64_t* mData; // random number bool.
    uint64_t mSize; // size of random number pool.
    uint64_t mTop; // current top of the random number pool.
};



#define MAX_PLAYERS 48
#define MAX_ROUNDS 8
#define MAX_GAMES 6

const char *names[MAX_PLAYERS] =
{
#if 1

"Ron Billings",
"Tagg Billings",

"Karla Bloom",
"Jay Richards",

"TJ Havens",
"Trafford Havens",

"Dave Halter",
"Christie Kabrich",

"Chad Thompson",
"Sara Wolf",

"Dennis McGowan",
"Wendy McGowan",

"Austin Webb",
"Courtney Webb",

"Roger	Pitt",
"Tammy Pitt",

"Clay Prater",
"Faith	Prater",

"Jim Ross",
"Leanne Ross",

"Jim Schmidt",
"Laurie Schmidt",

"Shane Wilson",
"Cindy Hughes",

"Diane Sanger",
"Mike Sanger",

"Larry Hanahan",
"Jim Hitt",

"Nathan Foli",
"Lisa Ross",

"Mike McCorkell",
"Westin McCorkell",

"Micah	Sanger",
"John Allessi",

"Forest Delano",
"Kirk DuPonce",

"Rodney Goodette",
"Glen Jacobson",

"Jim Mackey",
"Sean McDonald",

"Mitvalsky	Michael",
"Sean Smith",

"Steve Varner",
"Gabe Wolcott",

"Steve Hemshoot",
"Player 46",

"Player 47",
"Player 48"

#else
"A1",
"A2",

"B1",
"B2",

"C1",
"C2",

"D1",
"D2",

"E1",
"E2",

"F1",
"F2",

"G1",
"G2",

"H1",
"H2",

"I1",
"I2",

"J1",
"J2",

"K1",
"K2",

"L1",
"L2",

"M1",
"M2",

"N1",
"N2",

"O1",
"O2",

"P1",
"P2",

"Q1",
"Q2",

"R1",
"R2",

"S1",
"S2",

"T1",
"T2",

"U1",
"U2",

"V1",
"V2",

"W1",
"W2",

"Y1",
"Y2"
#endif
};

enum Game
{
    NONE,
    GAME1,
    GAME2,
    GAME3,
    GAME4
};

class Player
{
public:
    Player(void)
    {
    }

    void init(uint32_t id)
    {
        mPlayerId = id;
        if ( id&1 )
        {
            mPartnerId = id-1;
        }
        else
        {
            mPartnerId = id+1;
        }
        for (auto &i:mPlayCount)
        {
            i = 0;
        }
        for (auto &i : mPartnerCount)
        {
            i = 0;
        }
    }

    void setPlayGame(Game gameId)
    {
        mLastPlayed = gameId;
    }

    bool canPlayThisGame(Game gameId)
    {
        if ( mLastPlayed == Game::NONE )
        {
            return true;
        }
        bool ret = false;
        switch ( mLastPlayed )
        {
            case Game::GAME1:
                ret = gameId == Game::GAME3; // if we last played game one and this is game 3, then ok
                break;
            case Game::GAME2:
                ret = gameId == GAME4; // if we last played game2 and this is game 4, then ok
                break;
        }
        return ret;
    }

    void resetGame(void)
    {
        mLastPlayed = Game::NONE;
    }

    void addPartner(uint32_t partner)
    {
        mPartnerCount[partner]++;
    }

    void addOpponent(uint32_t opponent)
    {
        mPlayCount[opponent]++;
    }

    uint64_t getError(void) const
    {
        uint64_t ret = 0;
        for (uint32_t i = 0; i < MAX_PLAYERS; i++)
        {
            if (i != mPlayerId)
            {
                if (mPartnerCount[i] || mPlayCount[i])
                {
//                    ret+=mPlayCount[i];
                    ret+=mPartnerCount[i];
                }
            }
        }
        return ret;
    }

    void playerReport(FILE *fph) const
    {
        fprintf(fph,"\n");
        fprintf(fph,"PlayerReport for player: %s\n", names[mPlayerId]);
        fprintf(fph,"Partner,PartnerCount,OpponentCount\n");
        for (uint32_t i=0; i<MAX_PLAYERS; i++)
        {
            if ( i!=mPlayerId)
            {
                if ( mPartnerCount[i] || mPlayCount[i] )
                {
                    fprintf(fph,"%s,%d,%d\n", names[i], mPartnerCount[i], mPlayCount[i]);
                }
            }
        }
        fprintf(fph,"\n");
    }

    void playerReport(void) const
    {
        printf("\n");
        printf("PlayerReport for player: %s\n", names[mPlayerId]);
        printf("Partner,PartnerCount,OpponentCount\n");
        for (uint32_t i = 0; i < MAX_PLAYERS; i++)
        {
            if (i != mPlayerId)
            {
                if (mPartnerCount[i] || mPlayCount[i])
                {
                    printf("%-10s : %4d : %4d\n", names[i], mPartnerCount[i], mPlayCount[i]);
                }
            }
        }
        printf("\n");
    }

    Game        mLastPlayed{Game::NONE};
    uint32_t    mPlayerId{0};
    uint32_t    mPartnerId{0};
    uint32_t    mPlayCount[MAX_PLAYERS];        // Number of times we played this player
    uint32_t    mPartnerCount[MAX_PLAYERS];     // Number of times we partnered with this player
};

class SingleGame
{
public:
    void print(FILE *fph)
    {
        const char *player1 = names[mPlayer1];
        const char *player2 = names[mPlayer2];
        const char *player3 = names[mPlayer3];
        const char *player4 = names[mPlayer4];
        fprintf(fph,"\"'%s and %s' vs '%s and %s'\"", player1, player2,player3,player4);
        fflush(fph);
    }

    void print(void)
    {
        const char *player1 = names[mPlayer1];
        const char *player2 = names[mPlayer2];
        const char *player3 = names[mPlayer3];
        const char *player4 = names[mPlayer4];
        printf("'%s and %s' vs '%s and %s'\n", player1, player2, player3, player4);
    }

    uint32_t    mPlayer1{0};
    uint32_t    mPlayer2{0};
    uint32_t    mPlayer3{0};
    uint32_t    mPlayer4{0};
};

class CornholeLeague
{
public:
    CornholeLeague(uint64_t seed) : mSeed(seed)
    {
    }

    void saveSchedule(void)
    {
        FILE *fph = fopen("CornholeLeague.csv", "wb");
        if (fph == nullptr)
        {
            printf("Failed to open output file.\n");
            exit(1);
        }
        uint64_t index = 0;
        for (uint32_t i = 0; i < MAX_ROUNDS; i++)
        {
            fprintf(fph, "\n");
            fprintf(fph, "Week%d\n", i + 1);
            fprintf(fph, "\n");
            for (uint32_t j = 0; j < 4; j++)
            {
                fprintf(fph, "Board1:Game%d,Board2:Game%d,Board3:Game%d,Board4:Game%d,Board5:Game%d,Board6:Game%d\n",
                    j+1,j+1,j+1,
                    j+1,j+1,j+1);
                for (uint32_t i = 0; i < MAX_GAMES; i++)
                {
                    mAllGames[index].print(fph);
                    index++;
                    if ((i + 1) < MAX_GAMES)
                    {
                        fprintf(fph, ",");
                    }
                }
                fprintf(fph, "\n");
                fprintf(fph, "\n");
                fflush(fph);
            }
        }
#if 1
        for (uint32_t i = 0; i < MAX_PLAYERS; i++)
        {
            mPlayers[i].playerReport(fph);
        }
#endif
        fclose(fph);

    }

    void createSchedule(void)
    {
        mAllGames.clear();
        mRandPool.init(MAX_PLAYERS,mSeed);

        for (uint32_t i=0; i<MAX_PLAYERS; i++)
        {
            mPlayers[i].init(i);
        }
        for (uint32_t i=0; i<MAX_ROUNDS; i++)
        {
            // Reset the game state...
            for (auto &p:mPlayers)
            {
                p.resetGame();
            }
            for (uint32_t j=0; j<4; j++)
            {
                // First, find the pool of players to play this round...
                Game g = Game(j+1);
                selectPlayersForThisRound(i,g);
                for (uint32_t i=0; i<MAX_GAMES; i++)
                {
                    mAllGames.push_back(mGames[i]);
                }
            }
        }
    }

    uint32_t getPlayer(Game game)
    {
        uint32_t ret = 0;
        std::vector<uint32_t> canPlayList;
        for (uint32_t i=0; i<MAX_PLAYERS; i++)
        {
            uint32_t randomPlayer = i;
            if (mPlayers[randomPlayer].canPlayThisGame(game))
            {
                canPlayList.push_back(randomPlayer);
            }
        }
        assert(!canPlayList.empty());
        if ( !canPlayList.empty() )
        {
            uint32_t count = uint32_t(canPlayList.size());
            uint32_t choice = uint32_t(mRandPool.get())%count;
            ret = canPlayList[choice];
        }
        return ret;
    }

    // Find a partner who is not our primary partner and we
    // have partnered with the fewest times before and played the
    // fewest games before
    uint32_t findPartner(uint32_t partner,Game game)
    {
        uint32_t ret = 0xFFFFFFFF;

        uint32_t lowPartnerCount = 0xFFFFFFFF;
        uint32_t lowPlayCount = 0xFFFFFFFF;

        Player &prt = mPlayers[partner];

        std::vector<uint32_t> choices;

        for (uint32_t i=0; i<MAX_PLAYERS; i++)
        {
            Player &p = mPlayers[i];
            if ( i != partner && 
                 p.canPlayThisGame(game) )
            {
                if ( p.mPartnerCount[partner] <= lowPartnerCount )
                {
                    if ( p.mPartnerCount[partner] < lowPartnerCount)
                    {
                        choices.clear();
                        lowPartnerCount = p.mPartnerCount[partner];
                        lowPlayCount = 0xFFFFFFFF;
                    }
                    if ( p.mPlayCount[partner] <= lowPlayCount )
                    {
                        lowPlayCount = p.mPlayCount[partner];
                        choices.push_back(i);
                    }
                }
            }
        }
        assert(!choices.empty());
        if ( !choices.empty() )
        {
            uint32_t count = uint32_t(choices.size());
            uint32_t choice = uint32_t(mRandPool.get())%count;
            ret = choices[choice];
        }
        return ret;
    }

    // Find a partner who is not our primary partner and we
    // have partnered with the fewest times before and played the
    // fewest games before
    uint32_t findPartnerExcludeOpponents(uint32_t partner, 
                                        Game game,
                                        uint32_t op1,
                                        uint32_t op2)
    {
        uint32_t ret = 0;
        PlayerChoiceVector choices;

        uint32_t lowPlayCount = 0xFFFFFFFF;

        Player &prt = mPlayers[partner];

        for (uint32_t i = 0; i < MAX_PLAYERS; i++)
        {
            Player &p = mPlayers[i];
            if (i != partner &&
                i != op1 &&
                i != op2 &&
                p.canPlayThisGame(game))
            {
                // Trying to minimize the number of times
                // these three other players have played this
                // new candidate player..
                uint32_t playCount1 = p.mPlayCount[partner];
                uint32_t playCount2 = p.mPlayCount[op1];
                uint32_t playCount3 = p.mPlayCount[op2];
                uint32_t maxPlayCount = playCount1 + playCount2 + playCount3;
                if (maxPlayCount <= lowPlayCount)
                {
                    if ( maxPlayCount < lowPlayCount )
                    {
                        lowPlayCount = maxPlayCount;
                        choices.clear();
                    }
                    choices.push_back(i);
                }
            }
        }
        if ( choices.empty() )
        {
            assert(0);
        }
        else
        {
            ret = randomChoice(choices);
        }

        return ret;
    }


    void selectPlayersForThisRound(uint32_t currentRound,Game game)
    {
        bool forcePartner = false;
        if ( currentRound == 0  || currentRound == (MAX_ROUNDS-1))
        {
            if ( game == GAME1 || game == GAME2)
            {
                forcePartner = true;
            }
        }
        for (uint32_t gameCount=0; gameCount<MAX_GAMES; gameCount++)
        {
            static uint32_t gCount=0;
            gCount++;
            SingleGame &s = mGames[gameCount];
            s.mPlayer1 = getPlayer(game); // find a player that can play this round
            mPlayers[s.mPlayer1].setPlayGame(game); // note that we are playing this game
            if ( forcePartner )
            {
                s.mPlayer2 = mPlayers[s.mPlayer1].mPartnerId;
            }
            else
            {
                s.mPlayer2 = findPartner(s.mPlayer1,game);
            }
            mPlayers[s.mPlayer2].setPlayGame(game); // not that we are playing this game
            s.mPlayer3 = bestOpponent(s.mPlayer1,s.mPlayer2,game);
            mPlayers[s.mPlayer3].setPlayGame(game); // not that we are playing this game
            if ( forcePartner )
            {
                s.mPlayer4 = mPlayers[s.mPlayer3].mPartnerId;
            }
            else
            {
                s.mPlayer4 = findPartnerExcludeOpponents(s.mPlayer3,game,s.mPlayer1,s.mPlayer2);
            }
            mPlayers[s.mPlayer4].setPlayGame(game); // note that we are playing this game

            Player &p1 = mPlayers[s.mPlayer1];
            Player &p2 = mPlayers[s.mPlayer2];
            Player &p3 = mPlayers[s.mPlayer3];
            Player &p4 = mPlayers[s.mPlayer4];

            assert(p1.mPlayerId == s.mPlayer1 );
            assert(p2.mPlayerId == s.mPlayer2 );
            assert(p3.mPlayerId == s.mPlayer3 );
            assert(p4.mPlayerId == s.mPlayer4 );

            p1.addPartner(s.mPlayer2);
            p2.addPartner(s.mPlayer1);
            p3.addPartner(s.mPlayer4);
            p4.addPartner(s.mPlayer3);

            p1.addOpponent(s.mPlayer3);
            p1.addOpponent(s.mPlayer4);

            p2.addOpponent(s.mPlayer3);
            p2.addOpponent(s.mPlayer4);

            p3.addOpponent(s.mPlayer1);
            p3.addOpponent(s.mPlayer2);

            p4.addOpponent(s.mPlayer1);
            p4.addOpponent(s.mPlayer2);

        }

    }

    // Find the list of players we have played the fewest number of times

    uint32_t bestOpponent(uint32_t tp1,uint32_t tp2,Game game)
    {
        uint32_t ret = 0;

        PlayerChoiceVector choices;

        uint32_t playCountLow = 32;

        for (uint32_t i = 0; i < MAX_PLAYERS; i++)
        {
            Player &p = mPlayers[i];
            // We can't play against ourselves
            if (i != tp1 && i != tp2 )
            {
                // If this player eligible to play this round
                if (p.canPlayThisGame(game))
                {
                    uint32_t pcount1 = p.mPlayCount[tp1];
                    uint32_t pcount2 = p.mPlayCount[tp2];
                    uint32_t playCount = pcount1+pcount2;
                    if (playCount <= playCountLow)
                    {
                        if ( playCount < playCountLow )
                        {
                            playCountLow = playCount;
                            choices.clear();
                        }
                        choices.push_back(i);
                    }
                }
            }
        }
        if ( choices.empty())
        {
            assert(0);
        }
        else
        {
            ret = randomChoice(choices);
        }
        return ret;
    }

    uint32_t randomChoice(const PlayerChoiceVector &choices)
    {
        uint32_t ret = 0;

        if ( !choices.empty())
        {
            uint32_t choice = rand()%choices.size();
            ret = choices[choice];
        }
        return ret;
    }

    uint64_t getError(void) const
    {
        printf("========================================\n");
        uint64_t error = 0;
        for (uint32_t i = 0; i < MAX_PLAYERS; i++)
        {
            error+=mPlayers[i].getError();
            mPlayers[i].playerReport();
        }
        printf("========================================\n");
        printf("Total error: %lld\n", error);
        printf("========================================\n");

        return error;
    }

    Player      mPlayers[MAX_PLAYERS];
    uint64_t    mSeed{0};
    RandPool    mRandPool;
    SingleGame  mGames[MAX_GAMES];
    std::vector<SingleGame> mAllGames;
};


int main()
{
#if 1
    CornholeLeague cl(7);
    cl.createSchedule();
    cl.saveSchedule();

#else
    Rand r;
    uint64_t bestSeed=0;
    uint64_t leastError=0xFFFFFFFF;
    // Recreate the schedule using different random seeds
    // looking for a result with the last error..
    for (uint64_t i=0; i<4; i++)
    {
        uint64_t seed = r.get();
        CornholeLeague cl(seed);
        cl.createSchedule();
        uint64_t error = cl.getError();
        if ( error < leastError )
        {
            printf("Found a schedule with less error: Went from %lld to %lld\n",leastError, error);
            leastError = error;
            bestSeed = seed;
        }
    }
#endif
}

