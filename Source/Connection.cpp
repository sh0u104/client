#include "Connection.h"
#include "Player.h"
#include "PlayerManager.h"
#include "SceneManager.h"
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

	// サーバへ接続
	int Connect = connect(sock, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));

	if (Connect == 0)
	{

		// 受信スレッド実装
		std::cout << "thread" << std::endl;
		th = std::thread(&Connection::recvThread, this);

		isConnction = true;
	}
	else
	{
		isConnction = false;
		closesocket(sock);
		WSACleanup();
	}
}

void Connection::Finalize()
{
	//接続してたら
	if (isConnction)
	{
		// マルチスレッドのループフラグを下ろす
		loop = false;
		PlayerLogout logout{};
		
		
			logout.cmd = NetworkTag::Logout;
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
	

		// スレッドの終了まで待機
		th.join();

		// ソケット終了処理
		int c = closesocket(sock);

		// WSA終了処理
		int wsaCleanup = WSACleanup();
	}
}

void Connection::SendSignIn(char name[10], char password[10])
{
	SignIn signin;
	signin.cmd = NetworkTag::SignIn;
	strcpy_s(signin.name, name);
	strcpy_s(signin.pass, password);

	char buffer[sizeof(SignIn)];
	memcpy_s(buffer, sizeof(buffer), &signin, sizeof(SignIn));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendSignUp(char name[10], char password[10])
{
	SignUp signup;
	signup.cmd= NetworkTag::SignUp;
	strcpy_s(signup.name, name);
	strcpy_s(signup.pass, password);

	char buffer[sizeof(SignUp)];
	memcpy_s(buffer, sizeof(buffer), &signup, sizeof(SignUp));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendGeustLogin()
{
	GeustLogin geustlogin;
	geustlogin.cmd = NetworkTag::GeustLogin;

	char buffer[sizeof(GeustLogin)];
	memcpy_s(buffer, sizeof(buffer), &geustlogin, sizeof(GeustLogin));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendMove(DirectX::XMFLOAT3 velocity, DirectX::XMFLOAT3 position,
	Player::State state, DirectX::XMFLOAT3 angle)
{
	PlayerInput input;
	input.cmd = NetworkTag::Move;
	input.id = playerManager->GetMyPlayerID();
	input.velocity = velocity;
	input.position = position;
	input.state = state;
	input.angle = angle;

	char buffer[sizeof(PlayerInput)];
	memcpy_s(buffer,sizeof(buffer), &input, sizeof(PlayerInput));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendSync(DirectX::XMFLOAT3 position)
{
	PlayerSync sync;
	sync.cmd = NetworkTag::Sync;
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
	teamcreate.cmd = NetworkTag::TeamCreate;
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
	teamjoin.cmd = NetworkTag::Teamjoin;
	teamjoin.id = playerManager->GetMyPlayerID();
	teamjoin.number = teamnumber;

	char buffer[sizeof(Teamjoin)];
	memcpy_s(buffer, sizeof(buffer), &teamjoin, sizeof(Teamjoin));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendTeamLeave(bool isLeader)
{
	TeamLeave teamLeave;
	teamLeave.cmd = NetworkTag::Teamleave;
	teamLeave.id = playerManager->GetMyPlayerID();
	teamLeave.isLeader = isLeader;

	char buffer[sizeof(TeamLeave)];
	memcpy_s(buffer, sizeof(buffer), &teamLeave, sizeof(TeamLeave));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendGamestart(int teamnumber)
{
	GameStart gamestart;
	gamestart.cmd = NetworkTag::Gamestart;
	gamestart.id = playerManager->GetMyPlayerID();
	gamestart.teamnunber = teamnumber;

	char buffer[sizeof(GameStart)];
	memcpy_s(buffer, sizeof(buffer), &gamestart, sizeof(GameStart));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendMessages(char input[32])
{
	Message message;
	message.cmd = NetworkTag::Message;
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
	startcheck.cmd = NetworkTag::StartCheck;
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
	gameend.cmd = NetworkTag::GameEnd;
	gameend.teamnunber = teamnumber;

	char buffer[sizeof(GameEnd)];
	memcpy_s(buffer, sizeof(buffer), &gameend, sizeof(GameEnd));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendIdSearch(short id)
{
	IdSearch idSearch;

	idSearch.cmd = NetworkTag::IdSearch;
	idSearch.id = id;
	
	char buffer[sizeof(IdSearch)];
	memcpy_s(buffer, sizeof(IdSearch), &idSearch, sizeof(IdSearch));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendFriendRequest(short requestid)
{
	FriendRequest friendRequest;

	friendRequest.cmd = NetworkTag::FriendRequest;
	strcpy_s(friendRequest.name, playerManager->GetMyPlayer()->GetName());
	friendRequest.senderid = playerManager->GetMyPlayerID();
	friendRequest.requestid = requestid;

	char buffer[sizeof(FriendRequest)];
	memcpy_s(buffer, sizeof(FriendRequest), &friendRequest, sizeof(FriendRequest));
	int s = send(sock, buffer, sizeof(buffer), 0);
}

void Connection::SendFriendApproval(short youid, short myid, char myname[10], char youname[10])
{
	FriendApproval friendApproval;
	friendApproval.cmd = NetworkTag::FriendApproval;
	friendApproval.myid = myid;
	friendApproval.youid = youid;
	strcpy_s(friendApproval.myname, myname);
	strcpy_s(friendApproval.youname, youname);
	
	char buffer[sizeof(FriendApproval)];
	memcpy_s(buffer, sizeof(FriendApproval), &friendApproval, sizeof(FriendApproval));
	int s = send(sock, buffer, sizeof(buffer), 0);
	
}

void Connection::SendSeeFriend()
{
	SeeFriend seeFriend;
	seeFriend.cmd = NetworkTag::SeeFriend;
	seeFriend.myid = playerManager->GetMyPlayerID();
		
	char buffer[sizeof(SeeFriend)];
	memcpy_s(buffer, sizeof(SeeFriend), &seeFriend, sizeof(SeeFriend));
	int s = send(sock, buffer, sizeof(buffer), 0);


}

void Connection::recvThread()
{
	do {
		char buffer[2048]{};

		int r = recv(sock, buffer, sizeof(buffer), 0);
		if (r == -1)
		{
			// エラーが発生した場合の処理
			std::cout << "recv エラー" << std::endl;

		}
		else if (r == 0)
		{
			// クライアントが接続を閉じた場合の処理
			std::cout << "接続を閉じた" << std::endl;
		}
		else
		{
			short type = 0;
			memcpy_s(&type, sizeof(short), buffer, sizeof(short));
			std::cout << "recv cmd　：　" << type << std::endl;
			switch (static_cast<NetworkTag>(type))
			{
			case NetworkTag::Message:
			{
				Message message;
				memcpy_s(&message,sizeof(Message), buffer, sizeof(Message));
				//メッセージがきたら
				playerManager->Setmessages(message.text);
				playerManager->SetmessageEraseTime(5.0f);
			}
			break;

			case NetworkTag::Move:
			{
				PlayerInput input;
				memcpy_s(&input, sizeof(PlayerInput), buffer, sizeof(PlayerInput));
				Player* player = playerManager->GetPlayer(input.id);
				player->SetRecvVelocity(input.velocity);
				
				
				if (playerManager->GetMyPlayerID()!= input.id)
				{
					//player->SetPosition(input.position);
					player->SetPosition(input.position);
					player->SetAngle(input.angle);
					//アニメーションにいれる
					player->ChangeState(input.state);
				}
			}
			break;
			case NetworkTag::Attack:
			{
				PlayerInput input;
				memcpy_s(&input,sizeof(PlayerInput), buffer, sizeof(PlayerInput));

				Player* player = playerManager->GetPlayer(input.id);



				std::cout << "受信 Attack " << std::endl;
				std::cout << "id " << input.id << std::endl;

				std::cout << std::endl;
			}
			break;
			case NetworkTag::Login:
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
			case NetworkTag::Sync:
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
			}
			break;
			case NetworkTag::Logout:
			{
				PlayerLogout logout;
				memcpy_s(&logout, sizeof(logout), buffer, sizeof(PlayerLogout));
				std::cout << " 退出id " << logout.id << std::endl;
			
				if(playerManager->GetMyPlayerID()!= logout.id)
			    playerManager->ErasePlayer(logout.id);
			}
			break;
			case NetworkTag::TeamCreate:
			{
				TeamCreate teamcreate;
				memcpy_s(&teamcreate, sizeof(teamcreate), buffer, sizeof(TeamCreate));
				if (teamcreate.Permission)
				{
					//チームを作れたら
					playerManager->GetPlayer(teamcreate.id)->Setteamnumber(teamcreate.number);
					playerManager->GetMyPlayer()->Setteamsid(0, teamcreate.id);
				}
				else
				{
					//チームを作れなかったら
				}

			}
			break;
			case NetworkTag::Teamjoin:
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
				if(teamjoin.id != playerManager->GetMyPlayerID())
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
			case NetworkTag::Teamleave:
			{
				TeamLeave teamLeave;
				memcpy_s(&teamLeave, sizeof(teamLeave), buffer, sizeof(TeamLeave));
				//自分じゃなかったら
				if (playerManager->GetMyPlayerID() != teamLeave.id)
				{
					deleteID.push_back(teamLeave.id);
					
					for (int i = 0; i < 3; ++i)
					{
						if (playerManager->GetMyPlayer()->Getteamsid(i) == teamLeave.id)
						{
							playerManager->GetMyPlayer()->Setteamsid(i, 0);
						}
					}
				}

			}
			break;
			case NetworkTag::Teamsync:
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
			case NetworkTag::StartCheck:
			{
				StartCheck startcheck;
				memcpy_s(&startcheck, sizeof(startcheck), buffer, sizeof(StartCheck));
				playerManager->GetMyPlayer()->SetstartCheck(startcheck.check);
			}
			break;
			case NetworkTag::Gamestart:
			{
				//ゲーム開始許可が下りた
				playerManager->SetGameStart(true);
			}
			break;
			case NetworkTag::SignIn:
			{
				SignIn signIn;
				memcpy_s(&signIn, sizeof(SignIn), buffer, sizeof(SignIn));

				//サーバーに送信したアカウントの情報があれば
				if (signIn.result)
				{
					playerManager->GetMyPlayer()->SetName(signIn.name);
					playerManager->SetSignIn(true);
				}
				//なかったら
				else
				{
					
				}
			}
			break;
			case NetworkTag::SignUp:
			{
				SignUp signUp;
				memcpy_s(&signUp, sizeof(SignUp), buffer, sizeof(SignUp));

				playerManager->GetMyPlayer()->SetName(signUp.name);
				playerManager->SetSignUp(true);
			}
			break;
			case NetworkTag::IdSearch:
			{
				IdSearch idSearch;
				memcpy_s(&idSearch, sizeof(IdSearch), buffer, sizeof(IdSearch));
				//検索結果
				//成功時
				if (idSearch.result)
				{
					playerManager->SetSearchResult(idSearch.result);
					playerManager->SetSearchName(idSearch.name);
					playerManager->SetSearchId(idSearch.id);

				}
				//失敗時
				else
				{
					playerManager->SetSearchResult(idSearch.result);
				}
			}
			break;
			case NetworkTag::FriendRequest:
			{
				FriendRequest friendRequest;
				memcpy_s(&friendRequest, sizeof(FriendRequest), buffer, sizeof(FriendRequest));

				playerManager->SetSenderName(friendRequest.name);
				playerManager->SetSendertId(friendRequest.senderid);
			}
			break;
			}
		}
	} while (loop);
}