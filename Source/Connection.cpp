#include "Connection.h"
#include "Player.h"
#include "PlayerManager.h"
#include "SceneManager.h"
#include "StateMachine.h"
#include "Logger.h"
#pragma comment(lib, "ws2_32.lib")
Connection::Connection()
{
	input[0] = '\0';
}

Connection::~Connection()
{
}
void Connection::Initialize()
{
	// WinsockAPI初期化
	// バージョンを指定する場合MAKEWORDマクロ関数を使用する
	int wsaStartUp = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsaStartUp != 0)
	{
		// 初期化失敗 
		std::cout << "WSA Initialize Failed." << std::endl;
		return;
	}

	// 接続先情報設定
	struct sockaddr_in addr;
	// アドレスファミリ 
	addr.sin_family = AF_INET;
	// 接続先ポート 
	addr.sin_port = htons(7000);
	// 接続先IP設定 
	addr.sin_addr.S_un.S_un_b.s_b1 = 127;
	addr.sin_addr.S_un.S_un_b.s_b2 = 0;
	addr.sin_addr.S_un.S_un_b.s_b3 = 0;
	addr.sin_addr.S_un.S_un_b.s_b4 = 1;

	// ソケット作成
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		WSACleanup();
	}

	//u_long mode = 1; // ノンブロッキングモードを有効にするために1を設定
	//ioctlsocket(sock, FIONBIO, &mode);

	// サーバへ接続
	int Connect = connect(sock, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));

	if (Connect == 0)
	{

		// 受信スレッド実装
		std::cout << "thread" << std::endl;
		tcpTh = std::thread(&Connection::TcpRecvThread, this);
		udpTh = std::thread(&Connection::UdpRecvThread, this);

		isConnection = true;
	}
	else
	{
		isConnection = false;
		closesocket(sock);
		WSACleanup();
		return;
	}

	//失敗したらtrueがくる
	if (UDPInitialize())
	{
		udpTh.join();
		isConnection = false;
		closesocket(sock);
		closesocket(uSock);
		WSACleanup();
	}
}

bool Connection::UDPInitialize()
{
	// 接続先情報
	uAddr.sin_family = AF_INET;
	uAddr.sin_port = htons(8000);
	// 接続先IP設定
	uAddr.sin_addr.S_un.S_un_b.s_b1 = 127;
	uAddr.sin_addr.S_un.S_un_b.s_b2 = 0;
	uAddr.sin_addr.S_un.S_un_b.s_b3 = 0;
	uAddr.sin_addr.S_un.S_un_b.s_b4 = 1;

	// socket作成
	uSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (uSock == INVALID_SOCKET) {
		printf("ソケットの生成に失敗しました\n");
		// 7.WSAの解放
		WSACleanup();
		return false;
	}

	// サーバへ接続
	int r = connect(uSock, (struct sockaddr*)&uAddr, sizeof(uAddr));
	if (r != 0) {
		int error = WSAGetLastError();
		std::cout << "connect_error:" << error << std::endl;

		WSACleanup();
		return false;
	}
	//// ノンブロッキングの設定
	u_long mode = 1; // ノンブロッキングモードを有効にするために1を設定
	ioctlsocket(uSock, FIONBIO, &mode);

	//サーバーにおくって帰ってくるまで
	int count = 0;
	while (isUAddr)
	{
		SendUdpAddr();
		Sleep(10);
		++count;
		if (count > 10)
			return true;
	}
	return false;
}

void Connection::Finalize()
{
	//接続してたら
	if (isConnection)
	{
		// マルチスレッドのループフラグを下ろす
		loop = false;
		PlayerLogout logout{};
		logout.cmd = TcpTag::Logout;


		//ログイン処理が終わってたら
		if (playerManager != nullptr)
		{
			logout.id = playerManager->GetMyPlayerID();
		}
		//ログイン処理がまだなら
		else
		{
			logout.id = -1;
		}
		int s = send(sock, reinterpret_cast<char*>(&logout), sizeof(PlayerLogout), 0);

		// ソケット終了処理
		int c = closesocket(sock);
		c = closesocket(uSock);

		// スレッドの終了まで待機
		tcpTh.join();
		Logger::Print("TCPのスレッドを閉じた");
		udpTh.join();
		Logger::Print("UDPのスレッドを閉じた");


		// WSA終了処理
		int wsaCleanup = WSACleanup();
	}
}


void Connection::NetrowkUpdate(float elapsedTime)
{
}
void Connection::SendEnemy(Enemy* enemy)
{
	EnemyData enemyData;
	enemyData.cmd = UdpTag::EnemyMove;
	enemyData.id = enemy->GetMyEnemyId();
	enemyData.position = enemy->GetPosition();
	enemyData.angle = enemy->GetAngle();
	enemyData.state = enemy->GetState();

	char buffer[sizeof(EnemyData)];
	memcpy_s(buffer, sizeof(buffer), &enemyData, sizeof(EnemyData));
	sendto(uSock, buffer, sizeof(buffer), 0, (struct sockaddr*)&uAddr, sizeof(struct sockaddr_in));
}
void Connection::SendEnemyDamage(int enemuID)
{
	EnemyDamage enemyDamage;
	enemyDamage.cmd = TcpTag::EnemyDamage;
	enemyDamage.id = enemuID;

	char buffer[sizeof(EnemyDamage)];
	memcpy_s(buffer, sizeof(buffer), &enemyDamage, sizeof(EnemyDamage));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::UdpRecvThread()
{
  do {
	    //UDP
	    {
	    	char Buffer[256]{};
	    	int addrSize = sizeof(struct sockaddr_in);
	    
	    	int size = recvfrom(uSock, Buffer, sizeof(Buffer), 0, reinterpret_cast<sockaddr*>(&uAddr), &addrSize);
			if (size == -1) {
				int error = WSAGetLastError();
				if (error == WSAEWOULDBLOCK) {
					// データがまだ来ていない場合、処理をスキップして次のループへ
					continue;
				}
				else {
					// 他のエラーの場合、ループを終了
					Logger::Print("recvfrom error:\n",&error);
					break;
				}
			}
			else if (size == 0)
			{
				// クライアントが接続を閉じた場合の処理
				//std::cout << "接続を閉じた" << std::endl;
				Logger::Print("TUDP接続を閉じた");
				break;
			}
			
			if (size > 0)
	    	{
				short type = 0;
				memcpy_s(&type, sizeof(type), Buffer, sizeof(short));
				switch (static_cast<UdpTag>(type))
				{
				case UdpTag::Move:
				{
					PlayerInput input;
					memcpy_s(&input, sizeof(PlayerInput), Buffer, sizeof(PlayerInput));
					Player* player = playerManager->GetPlayer(input.id);

					if (playerManager->GetMyPlayerID() != input.id)
					{
						player->SetAngle(input.angle);
						player->SetPosition(input.position);
						player->SetVelovity(input.velocity);
						//アニメーションにいれる
						if (player->GetState() != input.state)
						{
							player->GetStateMachine()->ChangeState(static_cast<int>(input.state));
						}
					}
				}
				break;
				case UdpTag::EnemyMove:
				{
					EnemyData enemyData;
					memcpy_s(&enemyData, sizeof(EnemyData), Buffer, sizeof(EnemyData));
					EnemyManager& enemyManager = EnemyManager::Instance();
					Enemy* enemy = enemyManager.GetIDEnemy(enemyData.id);
					if (enemy != nullptr)
					{
						enemy->SetAngle(enemyData.angle);
						enemy->SetPosition(enemyData.position);
						//アニメーションにいれる
						if (enemy->GetState() != enemyData.state)
						{
							enemy->GetStateMachine()->ChangeState(static_cast<int>(enemyData.state));
						}
					}
				}
				break;
				}
	    	}
	    }

	} while (loop);

}


void Connection::TcpRecvThread()
{
	do {
		//TCP
		{
			char buffer[2048]{};

			int r = recv(sock, buffer, sizeof(buffer), 0);
			if (r == -1)
			{
				// エラーが発生した場合の処理
				//std::cout << "recv エラー" << std::endl;
				//Logger::Print("recv エラー");
			}
			else if (r == 0)
			{
				// クライアントが接続を閉じた場合の処理
				Logger::Print("TCP接続を閉じた");
			}
			else
			{
				short type = 0;
				memcpy_s(&type, sizeof(short), buffer, sizeof(short));
				std::cout << "recv cmd　：　" << type << std::endl;
				switch (static_cast<TcpTag>(type))
				{
				case TcpTag::SignIn:
				{
					SignIn signIn;
					memcpy_s(&signIn, sizeof(SignIn), buffer, sizeof(SignIn));

				}
				break;
				case TcpTag::SignUp:
				{
					SignUp signUp;
					memcpy_s(&signUp, sizeof(SignUp), buffer, sizeof(SignUp));

					//playerManager->GetMyPlayer()->SetName(signUp.name);
					playerManager->SetSignUp(true);
				}
				break;
				case TcpTag::Logout:
				{
					PlayerLogout logout;
					memcpy_s(&logout, sizeof(logout), buffer, sizeof(PlayerLogout));
					//std::cout << " 退出id " << logout.id << std::endl;
					Logger::Print("退出id \n",logout.id);
					if (playerManager->GetMyPlayerID() != logout.id)
					{
						playerManager->ErasePlayer(logout.id);
						//playerManager->DeletePlayer();
					}

				}
				break;
				case TcpTag::TeamCreate:
				{
					TeamCreate teamcreate;
					memcpy_s(&teamcreate, sizeof(teamcreate), buffer, sizeof(TeamCreate));
					if (teamcreate.Permission)
					{
						//チームを作れたら
						playerManager->GetPlayer(teamcreate.id)->Setteamnumber(teamcreate.number);
						playerManager->GetMyPlayer()->Setteamsid(0, teamcreate.id);
						playerManager->GetMyPlayer()->SetTeamHost(true);
					}
					else
					{
						//チームを作れなかったら
					}

				}
				break;
				case TcpTag::Teamjoin:
				{
					Teamjoin teamjoin;
					memcpy_s(&teamjoin, sizeof(teamjoin), buffer, sizeof(Teamjoin));

					//本人が加入失敗
					if (teamjoin.number == -1)
					{
						break;
					}

					//本人がチームに入れたら
					if (teamjoin.id == playerManager->GetMyPlayerID())
					{

						playerManager->GetMyPlayer()->Setteamnumber(teamjoin.number);
						break;
					}
					//本人以外がチームに入って来たら
					if (teamjoin.id != playerManager->GetMyPlayerID())
					{
						for (int i = 1; i < 3; ++i)
						{
							auto teamsid = playerManager->GetMyPlayer()->Getteamsid(i);

							//チームのi番目がすでにいたら
							if (teamsid != 0)continue;

							//チームのi番目が空席なら
							{
								//ログイン者をチーム一覧にIDを登録
								playerManager->GetMyPlayer()->Setteamsid(i, teamjoin.id);
								//ログイン数を加算
								playerManager->AddLoginCount();

								break;
							}
						}

					}
				}
				break;
				case TcpTag::Teamleave:
				{
					//チーム抜けた人がいたら
					TeamLeave teamLeave;
					memcpy_s(&teamLeave, sizeof(teamLeave), buffer, sizeof(TeamLeave));

					//本人の時
					if (teamLeave.id == playerManager->GetMyPlayerID())
					{
						for (int i = 0; i < 3; ++i)
						{
							int ID = playerManager->GetMyPlayer()->Getteamsid(i);
							if (ID != playerManager->GetMyPlayerID())
							{
								playerManager->ErasePlayer(ID);
							}
						}
						playerManager->GetMyPlayer()->Setteamnumber(0);
						playerManager->ResetGenerateCount();
						playerManager->ResetLoginCount();

						break;
					}

					//リーダーかつ本人でもない時
					if (teamLeave.isLeader)
					{
						playerManager->GetMyPlayer()->Setteamnumber(0);
						playerManager->SetTeamDisbabded(true);
						playerManager->ResetGenerateCount();
						playerManager->ResetLoginCount();

					}
					//リーダーでも本人でもないとき
					else
					{
						playerManager->SubtractLoginCount();
						playerManager->SubtractPlayersGenerateCount();
						for (int i = 0; i < 3; ++i)
						{
							int ID = playerManager->GetMyPlayer()->Getteamsid(i);
							if (ID != playerManager->GetMyPlayerID())
							{
								playerManager->ErasePlayer(ID);
							}
						}

					}

				}
				break;
				case TcpTag::Teamsync:
				{
					Teamsync teamsync;
					memcpy_s(&teamsync, sizeof(teamsync), buffer, sizeof(Teamsync));
					//チーム最大人数分
					for (int i = 0; i < 3; ++i)
					{
						//チームに存在しなかったら
						if (teamsync.id[i] == 0)break;

						//自分のチームリストにメンバーのIDを登録
						playerManager->GetMyPlayer()->Setteamsid(i, teamsync.id[i]);

						//自分のIDなら
						if (teamsync.id[i] == playerManager->GetMyPlayer()->GetPlayerID())continue;

						//ログイン数を加算
						playerManager->AddLoginCount();

						playerManager->SetSynclogin(true);
					}
				}
				break;
				case TcpTag::StartCheck:
				{
					StartCheck startcheck;
					memcpy_s(&startcheck, sizeof(startcheck), buffer, sizeof(StartCheck));
					playerManager->GetMyPlayer()->SetstartCheck(startcheck.check);
				}
				break;
				case TcpTag::Gamestart:
				{
					//ゲーム開始許可が下りた
					playerManager->SetGameStart(true);
				}
				break;
				case TcpTag::Login:
				{
					PlayerLogin login;
					std::cout << "login " << std::endl;
					memcpy_s(&login, sizeof(PlayerLogin), buffer, sizeof(PlayerLogin));
					std::cout << " id " << login.id << std::endl;
					if (playerManager->GetMyPlayerID() == 0)
					{
						//自分のプレイヤーに受信したIDを乗せる
						playerManager->GetMyPlayer()->SetPlayerID(login.id);
						playerManager->SetMyPlayerID(login.id);
						//ログイン数を加算
						playerManager->AddLoginCount();
						std::cout << std::endl;

					}
					else
					{
						//
					}
				}
				break;
				case TcpTag::UdpAddr:
				{
					isUAddr = false;
				}
				break;
				/*case NetworkTag::Sync:
				{
					PlayerSync sync;
					memcpy_s(&sync, sizeof(sync), buffer, sizeof(PlayerSync));
					std::cout << " cmd sync " << std::endl;
					std::cout << " id " << sync.id << std::endl;

					std::vector<Player*> players = playerManager->GetPlayers();
					for (Player* player : players)
					{
						if (sync.id == player->GetPlayerID())
						{
							player->SetPosition(sync.position);
							player->SetRecvVelocity(sync.velocity);
						}
					}
					Player* player = new Player();
					player->SetPlayerID(sync.id);
					player->SetPosition(sync.position);
					player->Setoperation(false);
					player->SetAngle(sync.angle);
					playerManager->AddPlayer(player);
					playerManager->GetPlayer(sync.id)->SetReady(true);

					std::cout << "syncプレイヤー生成 " << std::endl;
				}*/
				case TcpTag::Move:
				{
					PlayerInput input;
					memcpy_s(&input, sizeof(PlayerInput), buffer, sizeof(PlayerInput));
					Player* player = playerManager->GetPlayer(input.id);
					player->SetAngle(input.angle);
					if (playerManager->GetMyPlayerID() != input.id)
					{
						//player->SetPosition(input.position);
						player->SetPosition(input.position);
						//アニメーションにいれる
						if (player->GetState() != input.state)
						{
							player->GetStateMachine()->ChangeState(static_cast<int>(input.state));
						}
					}
				}
				break;
				case TcpTag::EnemyDamage:
				{
					EnemyDamage enemyDamage;
					memcpy_s(&enemyDamage, sizeof(EnemyDamage), buffer, sizeof(EnemyDamage));
					EnemyManager& enemyManager = EnemyManager::Instance();
					Enemy* enemy = enemyManager.GetIDEnemy(enemyDamage.id);
					if (enemy != nullptr)
					{
						enemy->ApplyDamage(1, 0.5f);
					}
				}
				break;
				//case TcpTag::Attack:
				//{
				//	PlayerInput input;
				//	memcpy_s(&input, sizeof(PlayerInput), buffer, sizeof(PlayerInput));
				//	Player* player = playerManager->GetPlayer(input.id);
				//	std::cout << "受信 Attack " << std::endl;
				//	std::cout << "id " << input.id << std::endl;
				//	std::cout << std::endl;
				//}
				//break;
				//case TcpTag::Message:
				//{
				//	Message message;
				//	memcpy_s(&message, sizeof(Message), buffer, sizeof(Message));
				//	//メッセージがきたら
				//	playerManager->Setmessages(message.text);
				//	playerManager->SetmessageEraseTime(5.0f);
				//}
				//break;
				}
			}
		}

	} while (loop);
}



//void Connection::DeleteID()
//{
//	//消去リストのIDのプレイヤーを消す
//	for (int i = 0; i <deleteID.size(); ++i)
//	{
//		if (deleteID.at(i) != playerManager->GetMyPlayerID())
//		{
//			playerManager->ErasePlayer(deleteID.at(i));
//			//playerManager->DeletePlayer();
//		}
//		deleteID.erase(deleteID.begin() + i);
//
//	}
//}


void Connection::SendSignIn(int Id)
{
	SignIn signin;
	signin.cmd = TcpTag::SignIn;
	signin.Id = Id;

	char buffer[sizeof(SignIn)];
	memcpy_s(buffer, sizeof(buffer), &signin, sizeof(SignIn));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendSignUp(int Id)
{
	SignUp signup;
	signup.cmd = TcpTag::SignUp;
	signup.Id = Id;

	char buffer[sizeof(SignUp)];
	memcpy_s(buffer, sizeof(buffer), &signup, sizeof(SignUp));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendGeustLogin()
{
	GeustLogin geustlogin;
	geustlogin.cmd = TcpTag::GeustLogin;

	char buffer[sizeof(GeustLogin)];
	memcpy_s(buffer, sizeof(buffer), &geustlogin, sizeof(GeustLogin));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendMove(DirectX::XMFLOAT3 velocity, DirectX::XMFLOAT3 position,
	Player::State state, DirectX::XMFLOAT3 angle)
{
	PlayerInput input;
	input.cmd = UdpTag::Move;
	input.id = playerManager->GetMyPlayerID();
	input.velocity = velocity;
	input.position = position;
	input.state = state;
	input.angle = angle;

	char buffer[sizeof(PlayerInput)];
	memcpy_s(buffer, sizeof(buffer), &input, sizeof(PlayerInput));
	//int s = send(sock, buffer, sizeof(buffer), 0);
	sendto(uSock, buffer, sizeof(buffer), 0, (struct sockaddr*)&uAddr, sizeof(struct sockaddr_in));
}

void Connection::SendSync(DirectX::XMFLOAT3 position)
{
	PlayerSync sync;
	sync.cmd = TcpTag::Sync;
	sync.id = playerManager->GetMyPlayerID();
	sync.velocity = { 0,0,0 };
	sync.position = position;

	char buffer[sizeof(PlayerSync)];
	memcpy_s(buffer, sizeof(buffer), &sync, sizeof(PlayerSync));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendTeamcreate()
{
	TeamCreate teamcreate;
	teamcreate.cmd = TcpTag::TeamCreate;
	teamcreate.id = playerManager->GetMyPlayerID();
	teamcreate.number = 0;
	teamcreate.Permission = false;

	char buffer[sizeof(TeamCreate)];
	memcpy_s(buffer, sizeof(buffer), &teamcreate, sizeof(TeamCreate));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendTeamJoin(int teamnumber)
{
	Teamjoin teamjoin;
	teamjoin.cmd = TcpTag::Teamjoin;
	teamjoin.id = playerManager->GetMyPlayerID();
	teamjoin.number = teamnumber;

	char buffer[sizeof(Teamjoin)];
	memcpy_s(buffer, sizeof(buffer), &teamjoin, sizeof(Teamjoin));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendTeamLeave(bool isLeader)
{
	TeamLeave teamLeave;
	teamLeave.cmd = TcpTag::Teamleave;
	teamLeave.id = playerManager->GetMyPlayerID();
	teamLeave.isLeader = isLeader;

	char buffer[sizeof(TeamLeave)];
	memcpy_s(buffer, sizeof(buffer), &teamLeave, sizeof(TeamLeave));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendGamestart(int teamnumber)
{
	GameStart gamestart;
	gamestart.cmd = TcpTag::Gamestart;
	gamestart.id = playerManager->GetMyPlayerID();
	gamestart.teamnunber = teamnumber;

	char buffer[sizeof(GameStart)];
	memcpy_s(buffer, sizeof(buffer), &gamestart, sizeof(GameStart));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendMessages(char input[32])
{
	Message message;
	message.cmd = UdpTag::Message;
	for (int i = 0; i < sizeof(input - 1); ++i)
	{
		message.text[i] = input[i];
	}

	char buffer[sizeof(Message)];
	memcpy_s(buffer, sizeof(buffer), &message, sizeof(Message));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendStartCheck(bool check)
{
	StartCheck startcheck;
	startcheck.cmd = TcpTag::StartCheck;
	startcheck.id = playerManager->GetMyPlayerID();
	startcheck.teamnunber = playerManager->GetMyPlayer()->Getteamnumber();
	startcheck.check = check;

	char buffer[sizeof(StartCheck)];
	memcpy_s(buffer, sizeof(buffer), &startcheck, sizeof(StartCheck));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendGameEnd(int teamnumber)
{
	GameEnd gameend;
	gameend.cmd = TcpTag::GameEnd;
	gameend.teamnunber = teamnumber;

	char buffer[sizeof(GameEnd)];
	memcpy_s(buffer, sizeof(buffer), &gameend, sizeof(GameEnd));
	int s = send(sock, buffer, sizeof(buffer), 0);
}





void Connection::SendIdSearch(short id)
{
	//IdSearch idSearch;
	//
	//idSearch.cmd = NetworkTag::IdSearch;
	//idSearch.id = id;
	//
	//char buffer[sizeof(IdSearch)];
	//memcpy_s(buffer, sizeof(IdSearch), &idSearch, sizeof(IdSearch));
	//int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendFriendRequest(short requestid)
{
//	FriendRequest friendRequest;
//
//	friendRequest.cmd = NetworkTag::FriendRequest;
//	strcpy_s(friendRequest.name, playerManager->GetMyPlayer()->GetName());
//	friendRequest.senderid = playerManager->GetMyPlayerID();
//	friendRequest.requestid = requestid;
//
//	char buffer[sizeof(FriendRequest)];
//	memcpy_s(buffer, sizeof(FriendRequest), &friendRequest, sizeof(FriendRequest));
//	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendFriendApproval(short youid, short myid, char myname[10], char youname[10])
{
	//FriendApproval friendApproval;
	//friendApproval.cmd = NetworkTag::FriendApproval;
	//friendApproval.myid = myid;
	//friendApproval.youid = youid;
	//strcpy_s(friendApproval.myname, myname);
	//strcpy_s(friendApproval.youname, youname);
	//
	//char buffer[sizeof(FriendApproval)];
	//memcpy_s(buffer, sizeof(FriendApproval), &friendApproval, sizeof(FriendApproval));
	//int s = send(sock, buffer, sizeof(buffer), 0);

}

void Connection::SendSeeFriend()
{
	//SeeFriend seeFriend;
	//seeFriend.cmd = NetworkTag::SeeFriend;
	//seeFriend.myid = playerManager->GetMyPlayerID();
	//
	//char buffer[sizeof(SeeFriend)];
	//memcpy_s(buffer, sizeof(SeeFriend), &seeFriend, sizeof(SeeFriend));
	//int s = send(sock, buffer, sizeof(buffer), 0);


}

void Connection::SendUdpAddr()
{
	UdpAddr udpAddr;
	udpAddr.cmd = UdpTag::UdpAddr;
	udpAddr.id = playerManager->GetMyPlayerID();

	char buffer[sizeof(UdpAddr)];
	memcpy_s(buffer, sizeof(UdpAddr), &udpAddr, sizeof(UdpAddr));
	//int s = send(sock, buffer, sizeof(buffer), 0);
	sendto(uSock, buffer, sizeof(buffer), 0, (struct sockaddr*)&uAddr, sizeof(struct sockaddr_in));
}

void Connection::SendLogout()
{
	PlayerLogout logout{};
	logout.cmd = TcpTag::Logout;
	logout.id = playerManager->GetMyPlayerID();
	int s = send(sock, reinterpret_cast<char*>(&logout), sizeof(PlayerLogout), 0);
}
