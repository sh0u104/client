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

	// �l�b�g���[�N�����X�V�֐�
	void NetrowkUpdate(float elapsedTime);

	void RecvThread();



	enum class NetworkTag : unsigned short
	{
		Message,		// �`���b�g���b�Z�[�W
		Move,			// �ړ�
		Attack,			// �U��
		Sync,			// ����
		TeamCreate,     //�`�[���쐬
		Teamjoin,       //�`�[������
		Teamleave,      //�`�[���𔲂���
		Teamsync,       // ��ɓ������l�Ƀ`�[�������o�[��ID��������
		StartCheck,     //�X�^�[�g�������ł��Ă邩
		Gamestart,      //�Q�[�����n�߂ł�����
		GameEnd,        //�Q�[�����I�������
		SignUp,         //�A�J�E���g�쐬
		SignIn,         //�A�J�E���g�Ń��O�C��
		GeustLogin,		// �Q�X�g�A�J�E���g�Ń��O�C��
		Login,          //��M�p
		Logout,			// �T�[�o�[���烍�O�A�E�g
		IdSearch,       //ID����
		FriendRequest,  //�t�����h�\��
		FriendApproval, //�t�����h���F
		SeeFriend,      //�����̃t�����h������

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

