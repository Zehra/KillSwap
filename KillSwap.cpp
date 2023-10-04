// KillSwap.cpp

#include "bzfsAPI.h"
#include "../src/bzfs/bzfs.h"
#include "../src/bzfs/GameKeeper.h"

int SetPlayerDead(int playerID) {
    int status = 0;
    GameKeeper::Player *p = GameKeeper::Player::getPlayerByIndex(playerID);
    if (p == NULL) {
        status= -1;
    } else {
        if (p->player.isObserver()) {
            status= -2;
        } else {
            if (p->player.isAlive()) {
                status=1;
                p->player.setDead();
                p->player.setRestartOnBase(false);
            } else {
                status=2;
            }
        }
    }
    return status;
}

// Utility functions.
int checkRange(int min, int max, int amount) {
    int num = 0;
    if ((amount >= min) && (amount <= max)) {
        num = 1;
    } else if ((amount < min) || (amount > max)) {
        num = 0;
    } else {
        num = -1;
    }
    return num;
}

int checkPlayerSlot(int player) {
    return checkRange(0,199,player); // 199 because of array.
}

class KillSwap : public bz_Plugin
{
public:
  const char* Name(){return "KillSwap[0.1.0]";}
  int spawn = -1;
  float playerPos[4];

  void Init (const char* commandLine) {
    bz_RegisterCustomFlag("KS", "Kill Swap", "Switches your position with who you kill.", 0, eGoodFlag);
    Register(bz_ePlayerDieEvent);
    Register(bz_eGetPlayerSpawnPosEvent);
  }

  void Event(bz_EventData *eventData ){
      switch (eventData->eventType) {
          case bz_ePlayerDieEvent: {
          bz_PlayerDieEventData_V2* deathData = (bz_PlayerDieEventData_V2*)eventData;
          int killer = deathData->killerID;
          if (deathData->flagKilledWith == "KS") {
            if (checkPlayerSlot(killer) == 1) {
                playerPos[0]= deathData->state.pos[0];
                playerPos[1]= deathData->state.pos[1];
                playerPos[2]= deathData->state.pos[2];
                playerPos[3]= deathData->state.rotation;
                spawn = killer;
                SetPlayerDead(killer);
                playerAlive(killer);
            }
          }    
        }break;

        case bz_eGetPlayerSpawnPosEvent: {
          bz_GetPlayerSpawnPosEventData_V1* spawnPosData = (bz_GetPlayerSpawnPosEventData_V1*)eventData;
          int player = spawnPosData->playerID;
          if (player == spawn) {
              spawnPosData->handled = true;
              spawnPosData->pos[0]= playerPos[0];
              spawnPosData->pos[1]= playerPos[1];
              spawnPosData->pos[2]= (playerPos[2] + 0.5);
              spawnPosData->rot   = playerPos[3];
              spawn = -1;
          }
        }break;

        default:{ 
        }break;
      }
  }

  void Cleanup (void) {
    Flush();
  }
};

BZ_PLUGIN(KillSwap)
