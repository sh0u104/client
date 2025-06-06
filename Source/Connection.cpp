#include "Connection.h"
#include "Player.h"
#include "PlayerManager.h"
#include "SceneManager.h"
#include "StateMachine.h"
#include "SceneStandby.h"
#include "SceneLoading.h"
#include "SceneTitle.h"
#include "Graphics/Graphics.h"
#include "Graphics/SpriteManager.h"

#include "Input/Input.h"
#include "Logger.h"

//ログ用
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#pragma comment(lib, "ws2_32.lib")

#include <chrono>
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
#if 1
	//自分のPC内
	addr.sin_addr.S_un.S_un_b.s_b1 = 127;
	addr.sin_addr.S_un.S_un_b.s_b2 = 0;
	addr.sin_addr.S_un.S_un_b.s_b3 = 0;
	addr.sin_addr.S_un.S_un_b.s_b4 = 1;
#else
	// 接続先IP設定(ブロードキャストアドレス)
	//学校5の
	addr.sin_addr.S_un.S_un_b.s_b1 = 10;
	addr.sin_addr.S_un.S_un_b.s_b2 = 108;
	addr.sin_addr.S_un.S_un_b.s_b3 = 0;
	addr.sin_addr.S_un.S_un_b.s_b4 = 18;
	// 10.108.1.107
	
#endif

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
		WriteLog(LogLevel::Error, "TCPサーバーへの接続に失敗しました");
		return;
	}
	WriteLog(LogLevel::Info, "TCPサーバーに接続しました");

	//失敗したらtrueがくる
	if (!UDPInitialize())
	{
		Logger::Print("UDP初期化失敗");
		udpTh.join();
		tcpTh.join();
		isConnection = false;
		closesocket(sock);
		closesocket(uSock);
		WSACleanup();
		WriteLog(LogLevel::Error, "UDPサーバーへの接続に失敗しました");
		return;
	}
	WriteLog(LogLevel::Info, "UDPサーバーに接続しました");
}

bool Connection::UDPInitialize()
{
	// 接続先情報
	uAddr.sin_family = AF_INET;
	uAddr.sin_port = htons(8000);
	// 接続先IP設定
#if 1
	uAddr.sin_addr.S_un.S_un_b.s_b1 = 127;
	uAddr.sin_addr.S_un.S_un_b.s_b2 = 0;
	uAddr.sin_addr.S_un.S_un_b.s_b3 = 0;
	uAddr.sin_addr.S_un.S_un_b.s_b4 = 1;
#else
	uAddr.sin_addr.S_un.S_un_b.s_b1 = 10;
	uAddr.sin_addr.S_un.S_un_b.s_b2 = 108;
	uAddr.sin_addr.S_un.S_un_b.s_b3 = 0;
	uAddr.sin_addr.S_un.S_un_b.s_b4 = 18;
	//10.108.1.107
#endif

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
		Sleep(100);
		++count;
		if (count > 100)
	   return false;
	}
	return true;
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
			//チームを組んでたら抜けるを送る
			if (playerManager->GetMyPlayer()->Getteamnumber() > 0)
			{
				SendTeamLeave(playerManager->GetMyPlayer()->GetTeamHost());
			}
			SendLogout();
		}
		
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

		WriteLog(LogLevel::Info, "サーバーから切断しました");
	}
}

void Connection::ConnectionCheck(ID3D11DeviceContext* dc)
{
	if (isConectionError)
	{
		Graphics& graphics = Graphics::Instance();
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());

		float positionX = 0;
		float positionY = 0;
		//枠組み
		Sprite* ConnectionError = g_SpriteManager.GetSprite(SpriteNumber::ConnectionError);
		ConnectionError->Render(dc,
			positionX, positionY, //描画位置
			screenWidth, screenHeight,  //表示サイズ
			0, 0,                 //切り取りはじめ位置
			750, 500,           //画像サイズ
			0.0f,
			1, 1, 1, 1);

		Mouse& mouse = Input::Instance().GetMouse();
		if (mouse.GetPositionX() > graphics.GetScreenWidth() || mouse.GetPositionX() < 0 ||
			mouse.GetPositionY() > graphics.GetScreenHeight() || mouse.GetPositionY() < 0)
			return;

		if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
		{
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
		}

	}
}

bool Connection::UdpIdCheck(int Id)
{

	playerManager->SetudpRecvId(Id);
	for (int i = 0; i < 4; ++i)
	{
		if (Id == playerManager->GetMyPlayer()->Getteamsid(i)&&Id!= playerManager->GetMyPlayerID())
		{
			
			return true;
		}
	}
	return false;
}


void Connection::NetrowkUpdate(float elapsedTime)
{
}
void Connection::SendEnemy(Enemy* enemy)
{
	EnemyData enemyData;
	enemyData.cmd = UdpTag::EnemyMove;
	enemyData.PlayerId = playerManager->GetMyPlayerID();
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
void Connection::SendPing()
{
	Ping ping;
	ping.cmd = UdpTag::Ping;

	char buffer[sizeof(Ping)];
	memcpy_s(buffer, sizeof(buffer), &ping, sizeof(Ping));

	start = std::chrono::high_resolution_clock::now();
	sendto(uSock, buffer, sizeof(buffer), 0, (struct sockaddr*)&uAddr, sizeof(struct sockaddr_in));
}

void Connection::UdpRecvThread()
{
  do {
	    //UDP
	    if(!isUAddr)
	    {
	    	char Buffer[256]{};
	    	int addrSize = sizeof(struct sockaddr_in);
			sockaddr_in temp{};
	    	int size = recvfrom(uSock, Buffer, sizeof(Buffer), 0, reinterpret_cast<sockaddr*>(&temp), &addrSize);
			if (size == -1) {
				int error = WSAGetLastError();
				if (error == WSAEWOULDBLOCK) {
					// データがまだ来ていない場合、処理をスキップして次のループへ
					continue;
				}
				else {
					// 他のエラーの場合、ループを終了
					Logger::Print("recvfrom error:%d",error);
					WriteLog(LogLevel::Error, "recvfrom エラー（コード: " + std::to_string(error) + "）");
					if (error == WSAECONNRESET)
					{
						//サーバーとの接続切れ
						isConectionError = true;
						WriteLog(LogLevel::Error, "サーバーとのUDP接続が切断されました（WSAECONNRESET）");
					}
					break;
				}
			}
			else if (size == 0)
			{
				// クライアントが接続を閉じた場合の処理
				Logger::Print("UDP接続を閉じた");
				WriteLog(LogLevel::Info, "クライアントがUDP接続を閉じました");
				break;
			}
			
			if (size > 0)
	    	{
				playerManager->SetrecvSize(size);
				short type = 0;
				memcpy_s(&type, sizeof(type), Buffer, sizeof(short));
				switch (static_cast<UdpTag>(type))
				{
				case UdpTag::Move:
				{
					PlayerInput input;
					memcpy_s(&input, sizeof(PlayerInput), Buffer, sizeof(PlayerInput));
					if (UdpIdCheck(input.id))
					{
						Player* player = playerManager->GetPlayer(input.id);
						//自分じゃなかったら
						if (playerManager->GetMyPlayerID() != input.id)
						{
							//内容が多いのは確認用
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
				}
				break;
				case UdpTag::EnemyMove:
				{
					EnemyData enemyData;
					memcpy_s(&enemyData, sizeof(EnemyData), Buffer, sizeof(EnemyData));
					if (UdpIdCheck(enemyData.PlayerId))
					{
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
				}
				break;
				case UdpTag::Ping:
				{
					end = std::chrono::high_resolution_clock::now();
					std::chrono::duration<float, std::milli> duration = end - start;
					float durationMs = duration.count();

					playerManager->GetMyPlayer()->SetPing(durationMs);
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
				int error = WSAGetLastError();

				if (error == WSAEWOULDBLOCK) {
					// データがまだ来ていない場合、処理をスキップして次のループへ
					continue;
				}
				else {
					// 他のエラーの場合、ループを終了
					Logger::Print("recv error:%d", error);
					WriteLog(LogLevel::Error, "recv エラー（コード: " + std::to_string(error) + "）");
					if (error == WSAECONNRESET)
					{
						isConectionError = true;
						WriteLog(LogLevel::Error, "クライアントとのTCP接続が切断されました（WSAECONNRESET）");
					}
					break;
				}
			}
			else if (r == 0)
			{
				// クライアントが接続を閉じた場合の処理
				Logger::Print("TCP接続を閉じた");
				WriteLog(LogLevel::Info, "クライアントがTCP接続を閉じました");
				break;
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
					Logger::Print("退出id %d",logout.id);
					//if (playerManager->GetMyPlayerID() != logout.id)
					//{
					//	playerManager->ErasePlayer(logout.id);
					//	//playerManager->DeletePlayer();
					//}

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
						WriteLog(LogLevel::Info, "ID: " + std::to_string(teamcreate.id) + " がチームを作成しました (チームID: " + std::to_string(teamcreate.number) + ")");
					}
					else
					{
						//チームを作れなかったら
						WriteLog(LogLevel::Info, "ID: " + std::to_string(teamcreate.id) + " がチームを作成失敗");
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
						WriteLog(LogLevel::Info, "ID: " + std::to_string(teamjoin.id) + " がチームID: "+ std::to_string(teamjoin.number)+ "に加入失敗");
						break;
					}

					//本人がチームに入れたら
					if (teamjoin.id == playerManager->GetMyPlayerID())
					{
						playerManager->GetMyPlayer()->Setteamnumber(teamjoin.number);
						WriteLog(LogLevel::Info, "ID: " + std::to_string(teamjoin.id) + " がチームID: " + std::to_string(teamjoin.number) + "に加入成功");
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
								WriteLog(LogLevel::Info, "チームID: " + std::to_string(teamjoin.number) + "に新たにID: " + std::to_string(teamjoin.id)+"が加入");
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

					//本人じゃない時
					if (teamLeave.id != playerManager->GetMyPlayerID())
					{
						//ホストの時
						if (teamLeave.isHost)
						{
							WriteLog(LogLevel::Info,"ホストがチームを解散させた");
							playerManager->GetMyPlayer()->Setteamnumber(0);
							playerManager->SetTeamDisbabded(true);
							for (int i = 0; i < 3; ++i)
							{
								int ID = playerManager->GetMyPlayer()->Getteamsid(i);
								if (ID != playerManager->GetMyPlayerID())
								{
									playerManager->ErasePlayer(ID);
									playerManager->GetMyPlayer()->Setteamsid(i, 0);
								}
							}
						}
						//ホストじゃない時
						else
						{
							int ID = teamLeave.id;
							if (ID != playerManager->GetMyPlayerID())
							{
								playerManager->ErasePlayer(ID);
								for (int i = 0; i < 4; ++i)
								{
									if (ID == playerManager->GetMyPlayer()->Getteamsid(i))
									{
										playerManager->GetMyPlayer()->Setteamsid(i, 0);
										for (int j = i; j < 4 - 1; ++j) { // 現在の位置から順に移動
											int nextValue = playerManager->GetMyPlayer()->Getteamsid(j + 1);
											playerManager->GetMyPlayer()->Setteamsid(j, nextValue);
											if (nextValue != 0)
											{
												DirectX::XMFLOAT3 pos = playerManager->GetPlayer(nextValue)->GetPosition();
												pos.x -= 1.5f;
												Logger::Print("ずらす%d", nextValue);
												playerManager->GetPlayer(nextValue)->SetPosition(pos);
											}
										
										}
										 //最後の要素に0をセット
										playerManager->GetMyPlayer()->Setteamsid(4 - 1, 0);
										break; // 対応が1回で十分ならループを抜ける
									}
								}
								WriteLog(LogLevel::Info, "チームからID: "+ std::to_string(teamLeave.id) + "が退出した");
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
					playerManager->GetPlayer(startcheck.id)->SetstartCheck(startcheck.check);
					//playerManager->GetMyPlayer()->SetstartCheck(startcheck.check);
				}
				break;
				case TcpTag::Gamestart:
				{
					//ゲーム開始許可が下りた
					playerManager->SetGameStart(true);
					int teamID = playerManager->GetMyPlayer()->Getteamnumber();
					
					if (teamID > 0)
					{
						std::string teamComposition = "チームID: " + std::to_string(teamID) + " の構成: ";
						for (auto& member : playerManager->GetPlayers()) {
							teamComposition += "ID:" + std::to_string(member->GetPlayerID()) + ", ";
						}
						WriteLog(LogLevel::Info, teamComposition);
						WriteLog(LogLevel::Info, "チームID: " + std::to_string(teamID) + " ゲームスタート");
					}
					else
					{
						WriteLog(LogLevel::Info, "ID: "+ std::to_string(playerManager->GetMyPlayerID())+ " ソロでゲームスタート");
					}
				}
				break;
				case TcpTag::Login:
				{
					PlayerLogin login;
					Logger::Print("ログインした");
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

						playerManager->GetMyPlayer()->SetName("Guest");

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
					Logger::Print("UDPアドレスをサーバーに保存した");
				}
				break;
				case TcpTag::Move:
				{
					PlayerInput input;
					memcpy_s(&input, sizeof(PlayerInput), buffer, sizeof(PlayerInput));
					Player* player = playerManager->GetPlayer(input.id);
					player->SetAngle(input.angle);
					if (playerManager->GetMyPlayerID() != input.id)
					{
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
				case TcpTag::Message:
				{
					Message message;
					memcpy_s(&message, sizeof(Message), buffer, sizeof(Message));
					//メッセージがきたら
					playerManager->Setmessages(message.text);
					playerManager->SetmessageEraseTime(30.0f);
				}
				break;
				}
			}
		}

	} while (loop);
}

void Connection::WriteLog(LogLevel level, const std::string& message)
{
	// ログファイルを追記モードで開く
	std::ofstream logFile("client.log", std::ios::app);  // 追記モード
	if (!logFile.is_open()) {
		std::cout << "ログファイルのオープンに失敗しました。" << std::endl;
		return;
	}

	// 現在の時刻を取得
	time_t now = time(nullptr);
	tm localTime;  // tm 構造体をスタックに定義
	localtime_s(&localTime, &now);  // localtime_s は第二引数に tm のポインタを取る

	// ログレベルを文字列に変換
	std::string levelStr;
	switch (level) {
	case LogLevel::Info:  levelStr = "INFO"; break;
	case LogLevel::Warn:  levelStr = "WARN"; break;
	case LogLevel::Error: levelStr = "ERROR"; break;
	case LogLevel::Debug: levelStr = "DEBUG"; break;
	default: levelStr = "UNKNOWN"; break;
	}

	// ログの書き込み
	logFile << "[" << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S")  // 時刻
		<< "][" << levelStr << "] "  // ログレベル
		<< message << "\n";  // メッセージ

// ファイルを閉じる
	logFile.close();
}


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
	++playerSendCount;
	PlayerInput input;
	input.cmd = UdpTag::Move;
	input.id = playerManager->GetMyPlayerID();
	input.velocity = velocity;
	input.position = position;
	input.state = state;
	input.angle = angle;
	input.sendCount = playerSendCount;

	char buffer[sizeof(PlayerInput)];
	memcpy_s(buffer, sizeof(buffer), &input, sizeof(PlayerInput));
	//int s = send(sock, buffer, sizeof(buffer), 0);
	sendto(uSock, buffer, sizeof(buffer), 0, (struct sockaddr*)&uAddr, sizeof(struct sockaddr_in));
}

void Connection::SendPlayerDamage(int id)
{

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

void Connection::SendTeamLeave(bool isHost)
{
	TeamLeave teamLeave;
	teamLeave.cmd = TcpTag::Teamleave;
	teamLeave.id = playerManager->GetMyPlayerID();
	teamLeave.isHost = isHost;

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

	WriteLog(LogLevel::Info, "ゲームスタート送信しました");
}

void Connection::SendMessages(char input[32])
{
	Message message;
	message.cmd = TcpTag::Message;
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

	WriteLog(LogLevel::Info, "サーバーにチームID:" + std::to_string(teamnumber) + " ゲームクリア送信");
}

//過去のフレンドリスト

//void Connection::SendIdSearch(short id)
//{
//	IdSearch idSearch;
//	
//	idSearch.cmd = NetworkTag::IdSearch;
//	idSearch.id = id;
//	
//	char buffer[sizeof(IdSearch)];
//	memcpy_s(buffer, sizeof(IdSearch), &idSearch, sizeof(IdSearch));
//	int s = send(sock, buffer, sizeof(buffer), 0);
//}
//
//void Connection::SendFriendRequest(short requestid)
//{
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
//}
//
//void Connection::SendFriendApproval(short youid, short myid, char myname[10], char youname[10])
//{
//	FriendApproval friendApproval;
//	friendApproval.cmd = NetworkTag::FriendApproval;
//	friendApproval.myid = myid;
//	friendApproval.youid = youid;
//	strcpy_s(friendApproval.myname, myname);
//	strcpy_s(friendApproval.youname, youname);
//	
//	char buffer[sizeof(FriendApproval)];
//	memcpy_s(buffer, sizeof(FriendApproval), &friendApproval, sizeof(FriendApproval));
//	int s = send(sock, buffer, sizeof(buffer), 0);
//
//}
//
//void Connection::SendSeeFriend()
//{
//	SeeFriend seeFriend;
//	seeFriend.cmd = NetworkTag::SeeFriend;
//	seeFriend.myid = playerManager->GetMyPlayerID();
//	
//	char buffer[sizeof(SeeFriend)];
//	memcpy_s(buffer, sizeof(SeeFriend), &seeFriend, sizeof(SeeFriend));
//	int s = send(sock, buffer, sizeof(buffer), 0);
//}

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
