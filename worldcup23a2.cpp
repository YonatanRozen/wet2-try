#include "worldcup23a2.h"

static const int INITIAL_SIZE = 128;

bool CompareById(const Team& team1, const Team& team2){
    return team1.GetId() < team2.GetId();
}

bool CompareByAbility(const Team& team1, const Team& team2){
    if (team1.GetAbility() < team2.GetAbility()){
        return true;
    }else if (team1.GetAbility() == team2.GetAbility() &&
    team1.GetId() < team2.GetId()){
        return true;
    }
    return false;
}

void minimizePath(Player* player){
    if (player->GetNext() == NULL || player->GetNext()->GetNext() == NULL){
        return;
    }
    minimizePath(player->GetNext());
    player->SetGamesPlayed(player->GetGamesPlayed() + player->GetNext()->GetGamesPlayed());
    player->multiplySpirit(player->GetNext()->GetPartialSpirit());
    player->SetNext(player->GetNext()->GetNext());
}

world_cup_t::world_cup_t() : teamsById(nullptr), teamsByAbility(nullptr),  playersHash(NULL), numOfTeams(0) {}



world_cup_t::~world_cup_t()
{
    delete playersHash;
    if (teamsById != NULL)
        teamsById->hollowTree();
    delete teamsById;
    delete teamsByAbility;
}

StatusType world_cup_t::add_team(int teamId)
{
    Team tempTeam = Team(teamId);
    if (teamId <= 0){
        return StatusType::INVALID_INPUT;
    }else if (this->teamsById != NULL && this->teamsById->find(tempTeam, &CompareById) != nullptr){
        return StatusType::FAILURE;
    }
    try{
        Team* newTeam = new Team(teamId);
        if (this->teamsById == NULL){
            this->teamsById = new AVL_Rank<Team>();
        }
        if (this->teamsByAbility == NULL){
            this->teamsByAbility = new AVL_Rank<Team>();
        }
        this->teamsById->insert(newTeam, &CompareById);
        this->teamsByAbility->insert(newTeam, &CompareByAbility);
        numOfTeams++;
    }catch(const std::bad_alloc&){
        return StatusType::ALLOCATION_ERROR;
    }
	return StatusType::SUCCESS;
}

StatusType world_cup_t::remove_team(int teamId)
{
    Team tempTeam = Team(teamId);
	if (teamId <= 0){
        return StatusType::INVALID_INPUT;
    }else if (this->teamsById == nullptr ||
    this->teamsById->find(tempTeam, &CompareById) == nullptr){
        return StatusType::FAILURE;
    }
    try{
        Team* team = this->teamsById->find(tempTeam, &CompareById);
        team->SetActive(false);
        this->teamsById->remove(*team, &CompareById);
        this->teamsByAbility->remove(*team, &CompareByAbility);
        if (team->GetLeader())
            team->GetLeader()->SetTeam(NULL);
        delete team;
        numOfTeams--;
    }catch(...){
        return StatusType::ALLOCATION_ERROR;
    }
	return StatusType::SUCCESS;
}

StatusType world_cup_t::add_player(int playerId, int teamId,
                                   const permutation_t &spirit, int gamesPlayed,
                                   int ability, int cards, bool goalKeeper)
{
    Team temp = Team(teamId);
    if(playerId <= 0 || teamId <= 0 || !spirit.isvalid() || gamesPlayed < 0 || cards < 0)
        return StatusType::INVALID_INPUT;
    if((playersHash != NULL && playersHash->get(playerId) != NULL) ||
    (teamsById != NULL && teamsById->find(temp, &CompareById) == NULL))
        return StatusType::FAILURE;
    try{
        Team* team = teamsById->find(temp, CompareById);
        Player* newPlayer;
        if (playersHash == NULL)
            playersHash = new HashTable<int, Player*>(INITIAL_SIZE);
        if (team->GetNumOfPlayers() == 0){
            newPlayer = new Player(playerId, gamesPlayed, ability, cards, goalKeeper, team, spirit);
            playersHash->insert(playerId, newPlayer);
            team->SetLeader(newPlayer);
        }else{
            newPlayer = new Player(playerId, gamesPlayed, ability, cards, goalKeeper, NULL, spirit);
            playersHash->insert(playerId, newPlayer);
            newPlayer->SetNext(team->GetLeader());
            newPlayer->multiplySpirit(team->GetTeamSpirit());
            newPlayer->SetGamesPlayed(gamesPlayed - team->GetLeader()->GetGamesPlayed());
        }
        teamsByAbility->remove(*team, &CompareByAbility);
        team->multiplyTeamSpirit(spirit);
        team->SetNumOfPlayers(team->GetNumOfPlayers()+1);
        team->SetNumGoalKeepers(team->GetNumGoalkeepers() + goalKeeper);
        team->SetAbility(team->GetAbility() + ability);
        team->UpdateStrength();
        teamsByAbility->insert(team, &CompareByAbility);

    }catch(const std::bad_alloc&){
        return StatusType::ALLOCATION_ERROR;
    }
	return StatusType::SUCCESS;
}

output_t<int> world_cup_t::play_match(int teamId1, int teamId2)
{
    if (teamId1 <= 0 || teamId2 <= 0 || teamId1 == teamId2){
        return StatusType::INVALID_INPUT;
    }
    if (teamsById == NULL)
        return StatusType::FAILURE;
    Team team1cpy = Team(teamId1);
    Team team2cpy = Team(teamId2);
    Team* team1 = teamsById->find(team1cpy, &CompareById);
    Team* team2 = teamsById->find(team2cpy, &CompareById);
    if (team1 == NULL || team2 == NULL)
        return StatusType::FAILURE;
    else if (team1->GetNumGoalkeepers() == 0 || team2->GetNumGoalkeepers() == 0)
        return StatusType::FAILURE;
    int winner;
    int team1Stats = team1->GetAbility() + team1->GetPoints();
    int team2Stats = team2->GetAbility() + team2->GetPoints();
    if (team1Stats > team2Stats){
        team1->SetPoints(team1->GetPoints() + 3);
        winner = 1;
    }else if (team1Stats == team2Stats){
        if (team1->GetStrength() > team2->GetStrength()){
            team1->SetPoints(team1->GetPoints() + 3);
            winner = 2;
        }else if (team1->GetStrength() == team2->GetStrength()){
            team1->SetPoints(team1->GetPoints() + 1);
            team2->SetPoints(team2->GetPoints() + 1);
            winner = 0;
        }else{
            team2->SetPoints(team2->GetPoints() + 3);
            winner = 4;
        }
    }else{
        team2->SetPoints(team2->GetPoints() + 3);
        winner = 3;
    }
    team1->SetGamesPlayed(team1->GetGamesPlayed() + 1);
    team2->SetGamesPlayed(team2->GetGamesPlayed() + 1);
    team1->GetLeader()->SetGamesPlayed(team1->GetLeader()->GetGamesPlayed()+1);
    team2->GetLeader()->SetGamesPlayed(team2->GetLeader()->GetGamesPlayed()+1);
	return output_t<int>{winner};
}

output_t<int> world_cup_t::num_played_games_for_player(int playerId)
{
    if (playerId <= 0){
        return output_t<int>{StatusType::INVALID_INPUT};
    }
    else if (playersHash == NULL || playersHash->get(playerId) == NULL){
        return output_t<int>{StatusType::FAILURE};
    }
	Player* player = playersHash->get(playerId);
    minimizePath(player);
    int sum = 0;
    while (player != NULL){
        sum += player->GetGamesPlayed();
        player = player->GetNext();
    }
    return sum;
}

StatusType world_cup_t::add_player_cards(int playerId, int cards)
{
    if (playerId <= 0 || cards < 0){
        return StatusType::INVALID_INPUT;
    }
    else if (playersHash == NULL || playersHash->get(playerId) == NULL){
        return StatusType::FAILURE;
    }
    Player* player = playersHash->get(playerId);
    Player* temp = player;
    while (temp->GetNext() != NULL){
        temp = temp->GetNext();
    }
    Team* team = temp->GetTeam();
    if (team == NULL){
        return StatusType::FAILURE;
    }
    player->SetCards(player->GetCards() + cards);
    return StatusType::SUCCESS;
}

output_t<int> world_cup_t::get_player_cards(int playerId)
{
	if (playerId <= 0){
        return StatusType::INVALID_INPUT;
    }
    else if (playersHash == NULL || playersHash->get(playerId) == NULL){
        return StatusType::FAILURE;
    }
    Player* player = playersHash->get(playerId);
	return output_t<int>{player->GetCards()};
}

output_t<int> world_cup_t::get_team_points(int teamId)
{
    Team temp = Team(teamId);
	if (teamId <= 0){
        return StatusType::INVALID_INPUT;
    }else if (teamsById == NULL || teamsById->find(temp, &CompareById) == NULL){
        return StatusType::FAILURE;
    }
    Team* team = teamsById->find(temp, &CompareById);
	return output_t<int>{team->GetPoints()};
}

output_t<int> world_cup_t::get_ith_pointless_ability(int i)
{
    if (teamsByAbility == NULL || i < 0 || i >= numOfTeams){
        return StatusType::FAILURE;
    }
    Team* ithTeam = teamsByAbility->getIthNode(i);
	return output_t<int>{ithTeam->GetId()};
}

output_t<permutation_t> world_cup_t::get_partial_spirit(int playerId)
{
    if (playerId <= 0){
        return StatusType::INVALID_INPUT;
    }else if (playersHash == NULL || playersHash->get(playerId) == NULL){
        return StatusType::FAILURE;
    }
    Player* player = playersHash->get(playerId);
    Player* temp = player;
    while (temp->GetNext() != NULL){
        temp = temp->GetNext();
    }
    Team* team = temp->GetTeam();
    if (team == NULL){
        return StatusType::FAILURE;
    }
    minimizePath(player);
    permutation_t p = player->GetSpirit();
    while (player != NULL){
        p = player->GetPartialSpirit()*p;
        player = player->GetNext();
    }
    return output_t<permutation_t>{p};

}

StatusType world_cup_t::buy_team(int buyerId, int boughtId)
{
	if (buyerId <= 0 || boughtId <= 0 || buyerId == boughtId){
        return StatusType::INVALID_INPUT;
    }
    Team temp1 = Team(buyerId);
    Team temp2 = Team(boughtId);
    if (teamsById == NULL || teamsById->find(temp1, &CompareById) == NULL ||
            teamsById->find(temp2, &CompareById) == NULL){
        return StatusType::FAILURE;
    }
    Team* buyer = teamsById->find(temp1, &CompareById);
    Team* bought = teamsById->find(temp2, &CompareById);
    int buyerSize = buyer->GetNumOfPlayers();
    int boughtSize = bought->GetNumOfPlayers();
    Player* boughtLeader = bought->GetLeader();
    Player* buyerLeader = buyer->GetLeader();
    if(buyerSize == 0){
        teamsById->remove(*buyer, &CompareById);
        teamsByAbility->remove(*buyer, &CompareByAbility);
        teamsById->remove(*bought, &CompareById);
        teamsByAbility->remove(*bought, &CompareByAbility);
        bought->SetId(buyer->GetId());
        bought->SetActive(true);
        teamsById->insert(bought, &CompareById);
        teamsByAbility->insert(bought, &CompareByAbility);
        delete buyer;
    }else if (boughtSize == 0){
        teamsById->remove(*bought, &CompareById);
        teamsByAbility->remove(*bought, &CompareByAbility);
        delete bought;
        buyer->SetActive(true);
    }
    else if (buyerSize >= boughtSize){
        teamsByAbility->remove(*buyer, &CompareByAbility);
        boughtLeader->SetNext(buyerLeader);
        boughtLeader->multiplySpirit(buyer->GetLeader()->GetPartialSpirit().inv()*buyer->GetTeamSpirit());
        boughtLeader->SetGamesPlayed(boughtLeader->GetGamesPlayed() - buyerLeader->GetGamesPlayed());
        boughtLeader->SetTeam(NULL);
        buyer->SetAbility(bought->GetAbility() + buyer->GetAbility());
        buyer->SetNumOfPlayers(bought->GetNumOfPlayers() + buyer->GetNumOfPlayers());
        buyer->SetNumGoalKeepers(bought->GetNumGoalkeepers() + buyer->GetNumGoalkeepers());
        buyer->SetPoints(buyer->GetPoints() + bought->GetPoints());
        buyer->SetActive(true);
        buyer->multiplyTeamSpirit(bought->GetTeamSpirit());
        buyer->UpdateStrength();
        teamsById->remove(*bought, &CompareById);
        teamsByAbility->remove(*bought, &CompareByAbility);
        teamsByAbility->insert(buyer, &CompareByAbility);
        delete bought;
    }else{
        teamsByAbility->remove(*buyer, &CompareByAbility);
        buyerLeader->SetNext(boughtLeader);
        boughtLeader->multiplySpirit(buyer->GetTeamSpirit());
        buyerLeader->multiplySpirit(boughtLeader->GetPartialSpirit().inv());
        buyerLeader->SetGamesPlayed(buyerLeader->GetGamesPlayed() - boughtLeader->GetGamesPlayed());
        buyerLeader->SetTeam(NULL);
        boughtLeader->SetTeam(buyer);
        buyer->SetLeader(boughtLeader);
        buyer->SetAbility(bought->GetAbility() + buyer->GetAbility());
        buyer->SetNumOfPlayers(bought->GetNumOfPlayers() + buyer->GetNumOfPlayers());
        buyer->SetNumGoalKeepers(bought->GetNumGoalkeepers() + buyer->GetNumGoalkeepers());
        buyer->SetPoints(buyer->GetPoints() + bought->GetPoints());
        buyer->SetActive(true);
        buyer->multiplyTeamSpirit(bought->GetTeamSpirit());
        buyer->UpdateStrength();
        buyerLeader->SetTeam(NULL);
        boughtLeader->SetTeam(buyer);
        teamsById->remove(*bought, &CompareById);
        teamsByAbility->remove(*bought, &CompareByAbility);
        delete bought;
        teamsByAbility->insert(buyer, &CompareByAbility);
    }
    numOfTeams--;
    return StatusType::SUCCESS;
}
