#pragma once
#include <vector>
#include "Player.h"
class PlayerManager
{
public:
	PlayerManager() {}
	~PlayerManager() {}

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	void DebugGUI();

	static PlayerManager& Instance()
	{
		static PlayerManager instance;
		return instance;
	}

	void AddPlayer(Player* player) { players.emplace_back(player); };
	std::vector<Player*> GetPlayers() { return players; }
	Player* GetPlayer(int id);
	Player* GetMyPlayer();
	void SetMyPlayerID(int id) { myPlayerId = id; }
	short GetMyPlayerID() { return myPlayerId; }
	void ErasePlayer(int id);
	void DeletePlayer();

	bool GetGameStart() { return gameStart; }
	void SetGameStart(bool flag) { this->gameStart = flag; }

	int GetLoginCount() { return this->loginCount; }
	void AddLoginCount() { ++this->loginCount; }
	
	int GetPlayersGenerateCount() { return this->PlayerGenerateCount; }
	void AddPlayersGenerateCount() { ++this->PlayerGenerateCount; }
	
	bool GetSynclogin() { return this->syncLogin; }
	void SetSynclogin(bool flag) { this->syncLogin = flag; }
	
	bool GetMyPlayerLogin() {return this ->myPlayerLogin;}
	void MyPlayerLogin() { this->myPlayerLogin = true; }
	
	std::vector<std::string> Getmessages() { return messages; }
	void Setmessages(char text[32]) { messages.push_back(text); }

	void SetmessageEraseTime(float time) { this->messageEraseTime = time; }
	float GetmessageEraseTime() { return messageEraseTime; }
	void reduceMessageEraseTime(float elapsedTime) { this->messageEraseTime -= elapsedTime; }

	void SetteamLeader(bool flag) { this->teamLeader = flag; }
	bool GetteamLeader() { return teamLeader; }

	void SetSignIn(bool flag) { this->signIn = flag; }
	bool GetSignIn() { return this->signIn; }

	void SetSignUp(bool flag) { this->signUp = flag; }
	bool GetSignUp() { return this->signUp; }

	void SetSearchName(char name[10]) { strcpy_s(this->searchName, name); }
	const char* GetSearchName() { return searchName; }

	void SetSearchResult(bool result) { this->searchResult = result; }
	bool GetSearchResult() { return searchResult; }

	void SetSearchId(short id) { this->searchId = id; }
	short GetSearchId() { return this->searchId; }

	void SetSenderName(char name[10]) { strcpy_s(this->senderName, name); }
	const char* GetSenderName() { return senderName; }

	void SetSendertId(short id) { this->sendertId = id; }
	short GetSendertId() { return this->sendertId; }
	void ResetSenderName(){ memset(senderName, 0, sizeof(senderName)); }

	struct User
	{
		short ID;
		char name[10];
	};
	std::vector<User>myFriendList;
private:

	std::vector<Player*> players;
	short myPlayerId = -1;
	std::vector<Player*> deleteList;


	bool gameStart = false;
	int loginCount =0;
	int PlayerGenerateCount = 0;
	
	bool syncLogin = false;
	bool myPlayerLogin = false;

	std::vector<std::string> messages;
	float messageEraseTime = 0;

	bool teamLeader = false;

	bool signIn = false;
	bool signUp = false;

	char searchName[10];
	bool searchResult = false;
	short searchId;

	char senderName[10];
	short sendertId;
	
	

};