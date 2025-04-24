#pragma once
#include <mutex>
#include <iostream>
#include <thread>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "PlayerManager.h" 
#include "Player.h"
#include "Enemy.h"
using namespace DirectX;

class Connection
{
public:
	Connection();
	~Connection();


	// �B��̃C���X�^���X�擾
	//static Connection& Instance()
	//{
	//	static Connection instance;
	//	return instance;
	//}

	void Initialize();
	bool UDPInitialize();

	void Finalize();

	void ConnectionCheck(ID3D11DeviceContext* dc);

	bool UdpIdCheck(int Id);
	
	void SendSignIn(int Id);

	void SendSignUp(int Id);

	void SendGeustLogin();

	void SendMove(DirectX::XMFLOAT3 velocity, DirectX::XMFLOAT3 position,
		Player::State state, DirectX::XMFLOAT3 angle);

	void SendPlayerDamage(int id);

	void SendSync(DirectX::XMFLOAT3 position);

	void SendTeamcreate();

	void SendTeamJoin(int teamnumber);

	void SendTeamLeave(bool isLeader);

	void SendGamestart(int teamnumber);

	void SendMessages(char input[32]);

	void SendStartCheck(bool check);

	void SendGameEnd(int teamnumber);

	//void SendIdSearch(short id);
	//void SendFriendRequest(short id);
	//void SendFriendApproval(short youid,short myid, char myname[10],char youname[10]);
	//void SendSeeFriend();

	void SendUdpAddr();

	void SendLogout();
	void SendEnemy(Enemy* enemy);
	void SendEnemyDamage(int enemuID);

	void SendPing();

	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point end;
	std::chrono::milliseconds duration;

	// �l�b�g���[�N�����X�V�֐�
	void NetrowkUpdate(float elapsedTime);

	void TcpRecvThread();
	void UdpRecvThread();

	

	
	enum class TcpTag : unsigned short
	{
		SignUp,         //�A�J�E���g�쐬
		SignIn,         //�A�J�E���g�Ń��O�C��
		GeustLogin,		// �Q�X�g�A�J�E���g�Ń��O�C��
		Logout,			// �T�[�o�[���烍�O�A�E�g
		TeamCreate,     //�`�[���쐬
		Teamjoin,       //�`�[������
		Teamleave,      //�`�[���𔲂���
		Teamsync,       // ��ɓ������l�Ƀ`�[�������o�[��ID��������
		StartCheck,     //�X�^�[�g�������ł��Ă邩
		Gamestart,      //�Q�[�����n�߂ł�����
		GameEnd,        //�Q�[�����I�������
		Move,
		Sync,			// ����
		Login,
		UdpAddr,        //�T�[�o�[��UDP�̃A�h���X�ۑ��p
		EnemyDamage,    //�G���_���[�W���󂯂���
		Message,
		
	};
	enum class UdpTag : unsigned short
	{
		Message,		// �`���b�g���b�Z�[�W
		Move,			// �ړ�
		EnemyMove,       //�G�̈ړ�
		Attack,			// �U��
		UdpAddr,          //�T�[�o�[��UDP�̃A�h���X�ۑ��p
		Ping,
	};

	struct Ping
	{
		UdpTag cmd;
	};
	
	struct EnemyData
	{
		UdpTag cmd;
		int PlayerId;
		int id;
		//DirectX::XMFLOAT3 velocity;
		DirectX::XMFLOAT3 position;
		Enemy::State state;
		DirectX::XMFLOAT3 angle;
	};

	struct EnemyDamage
	{
		TcpTag cmd;
		int id;
	};

	struct Message
	{
		TcpTag cmd;
		char text[32];
	};

	struct PlayerInput
	{
		UdpTag cmd;
		int id;
		DirectX::XMFLOAT3 velocity;
		DirectX::XMFLOAT3 position;
		Player::State state;
		DirectX::XMFLOAT3 angle;
		short sendCount;
	};

	struct PlayerDamage
	{
		TcpTag cmd;
		int id;
	};
	
	struct PlayerLogin
	{
		TcpTag cmd;
		int id;
	};
	
	struct GeustLogin
	{
		TcpTag cmd;
	};

	struct SignIn
	{
		TcpTag cmd;
		int Id;
		//char name[10];
		//char pass[10];
		//bool result;
	};

	struct SignUp
	{
		TcpTag cmd;
		int Id;
		//char name[10];
		//char pass[10];
	};

	struct PlayerLogout
	{
		TcpTag cmd;
		int id;
	};
	struct PlayerSync
	{
		TcpTag cmd;
		int id;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 velocity;
		DirectX::XMFLOAT3 angle;
	};

	struct TeamCreate
	{
		TcpTag cmd;
		int id;
		int number;
		bool Permission;
	};

	struct Teamjoin
	{
		TcpTag cmd;
		int id;
		int number;
	};

	struct TeamLeave
	{
		TcpTag cmd;
		int id;
		bool isHost;
	};

	struct Teamsync
	{
		TcpTag cmd;
		int id[4];
	};

	struct StartCheck
	{
		TcpTag cmd;
		int id;
		short teamnunber;
		bool check;
	};

	struct GameStart
	{
		TcpTag cmd;
		int id;
		short teamnunber;
	};

	struct GameEnd
	{
		TcpTag cmd;
		short teamnunber;
	};

	struct UdpAddr
	{
		UdpTag cmd;
		int id;
	};

	/*{
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
	}*/

	PlayerManager* GetPlayerManager() { return playerManager; }
	void SetPlayerManager(PlayerManager* playerManager) { this->playerManager = playerManager; }

	bool isConnection = false;
	bool GetIsConectionError() { return isConectionError; }

	enum class LogLevel { Info, Warn, Error, Debug };
	void WriteLog(LogLevel level, const std::string& message);
private:
	PlayerManager* playerManager = nullptr;

	char input[32];
	WSADATA wsaData{};
	SOCKET sock{};
	std::thread tcpTh;
	std::atomic<bool> loop = true;
	std::mutex mutex;
	

	struct sockaddr_in uAddr;
	bool isUAddr = true;
	SOCKET uSock{};
	std::thread udpTh;

	bool isConectionError = false;

	short playerSendCount = 0;

};

