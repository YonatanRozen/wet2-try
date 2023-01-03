//
// Created by talro on 21/12/2022.
//
#ifndef WET2_PLAYER_H
#define WET2_PLAYER_H

#include "wet2util.h"
#include "Team.h"

class Player {
private:
    int playerId;
    int gamesPlayed;
    int ability;
    int cards;
    bool goalKeeper;
    Team* originalTeam;
    permutation_t spirit;
    permutation_t partialSpirit;
    Player* next;

public:
    Player(int playerId, int gamesPlayed, int ability, int cards,  bool goalKeeper, Team* team, const permutation_t& spirit) :
            playerId(playerId), gamesPlayed(gamesPlayed), ability(ability), cards(cards), goalKeeper(goalKeeper),
            originalTeam(team), spirit(spirit), partialSpirit(permutation_t::neutral()), next(NULL){
    }
    bool isGoalKeeper() const{return goalKeeper;}
    int GetAbility() const{return ability;}
    int GetPlayerId() {return playerId;}
    int GetGamesPlayed() {return gamesPlayed;}
    int GetCards() {return cards;}
    void SetGamesPlayed(int val) {this->gamesPlayed = val;}
    void SetCards(int val) {this->cards = val;}
    void SetTeam(Team* other) {this->originalTeam = other;}
    void SetNext(Player* other) {this->next = other;}
    Player* GetNext() {return next;}
    Team* GetTeam() {return originalTeam;}
    const permutation_t& GetSpirit() {return spirit;}
    const permutation_t& GetPartialSpirit() {return partialSpirit;}
    void multiplySpirit(const permutation_t& permutation){
        partialSpirit = permutation*(partialSpirit);
    }
};


#endif //WET2_PLAYER_H
