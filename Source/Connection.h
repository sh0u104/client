#pragma once
#include <mutex>
#include <iostream>
#include <thread>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "PlayerManager.h" 
#include "Player.h"

using namespace DirectX;

class Connection
{
public:
	Connection();
	~Connection();

	void Initialize();
	void Finalize();

	
	void SendSignIn(char name[10], char password[10]);

	void SendSignUp(char name[10], char password[10]);

	void SendGeustLogin();

	void SendMove(DirectX::XMFLOAT3 velocity, DirectX::XMFLOAT3 position,
		Player::State state, DirectX::XMFLOAT3 angle);

	void SendSync(DirectX::XMFLOAT3 position);

	void SendTeamcreate();

	void SendTeamJoin(int teamnumber);

	void SendTeamLeave(bool isLeader);

	void SendGamestart(int teamnumber);


	void SendMessages(char input[32]);

	void SendStartCheck(bool check);

	void SendGameEnd(int teamnumber);

	void SendIdSearch(short id);

	void SendFriendRequest(short id);

	void SendFriendApproval(short youid,short myid, char myname[10],char youname[10]);

	void SendSeeFriend();

	// ネットワーク処理更新関数
	void NetrowkUpdate(float elapsedTime);

	void RecvThread();



	enum class NetworkTag : unsigned short
	{
		Message,		// チャットメッセージ
		Move,			// 移動
		Attack,			// 攻撃
		Sync,			// 同期
		TeamCreate,     //チーム作成
		Teamjoin,       //チーム加入
		Teamleave,      //チームを抜ける
		Teamsync,       // 後に入った人にチームメンバーのIDをおくる
		StartCheck,     //スタート準備ができてるか
		Gamestart,      //ゲームを始めでいいか
		GameEnd,        //ゲームが終わったら
		SignUp,         //アカウント作成
		SignIn,         //アカウントでログイン
		GeustLogin,		// ゲストアカウントでログイン
		Login,          //受信用
		Logout,			// サーバーからログアウト
		IdSearch,       //ID検索
		FriendRequest,  //フレンド申請
		FriendApproval, //フレンド承認
		SeeFriend,      //自分のフレンドを見る

	};
	

	struct Message
	{
		NetworkTag cmd;
		char text[32];
	};

	struct PlayerInput
	{
		NetworkTag cmd;
		short id;
		DirectX::XMFLOAT3 velocity;
		DirectX::XMFLOAT3 position;
		Player::State state;
		DirectX::XMFLOAT3 angle;
	};

	
	struct PlayerLogin
	{
		NetworkTag cmd;
		short id;
	};
	
	struct GeustLogin
	{
		NetworkTag cmd;
	};

	struct SignIn
	{
		NetworkTag cmd;
		char name[10];
		char pass[10];
		bool result;
	};

	struct SignUp
	{
		NetworkTag cmd;
		char name[10];
		char pass[10];
	};

	struct PlayerLogout
	{
		NetworkTag cmd;
		short id;
	};
	struct PlayerSync
	{
		NetworkTag cmd;
		short id;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 velocity;
		DirectX::XMFLOAT3 angle;
	};

	struct TeamCreate
	{
		NetworkTag cmd;
		short id;
		int number;
		bool Permission;
	};

	struct Teamjoin
	{
		NetworkTag cmd;
		short id;
		int number;
	};

	struct TeamLeave
	{
		NetworkTag cmd;
		short id;
		bool isLeader;
	};

	struct Teamsync
	{
		NetworkTag cmd;
		short id[4];
	};

	struct StartCheck
	{
		NetworkTag cmd;
		short id;
		short teamnunber;
		bool check;
	};

	struct GameStart
	{
		NetworkTag cmd;
		short id;
		short teamnunber;
	};

	struct GameEnd
	{
		NetworkTag cmd;
		short teamnunber;
	};

	struct IdSearch
	{
		NetworkTag cmd;
		short id;
		bool result;
		char name[10];
	};

	struct FriendRequest
	{
		NetworkTag cmd;
		char name[10];
		short senderid;
		short requestid;

	};
	struct FriendApproval
	{
		NetworkTag cmd;
		short myid;
		short youid;
		char myname[10];
		char youname[10];
	};
	
	struct SeeFriend
	{
		NetworkTag cmd;
		short myid;
		std::vector<PlayerManager::User>friendList;
	};
	

	PlayerManager* GetPlayerManager() { return playerManager; }
	void SetPlayerManager(PlayerManager* playerManager) { this->playerManager = playerManager; }

	bool isConnection = false;
	std::vector<int> deleteID;
private:
	PlayerManager* playerManager = nullptr;

	char input[32];
	WSADATA wsaData{};
	SOCKET sock{};
	std::thread th;
	bool loop = true;
	std::mutex mutex;
	
	
};

