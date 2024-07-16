#include "SceneStandby.h"

#include "Graphics/Graphics.h"
#include "Camera.h"
#include "StageManager.h"
#include "StageMain.h"

#include "Input/Input.h"

#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneGame.h"
void SceneStandby::Initialize()
{
	
	StageManager& stageManager = StageManager::instance();
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);

	// カメラ初期設定 見える位置追いかけるものなど
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)

	);

	// どの範囲をどれだけ見るか奥行含め
	camera.SetPerspedtiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);
	// カメラコントローラー初期化
	cameraController = new CameraController();
	DirectX::XMFLOAT3 target = { 0,0,0 };
	target.y += 0.5f;// 足元から５０センチぐらい
	cameraController->SetTarget(target);// プレイヤーの腰当たり

	// スプライト表示するもの
	//sprites[static_cast<int>(Spritenumber:)] = std::make_unique<Sprite>("Data/Sprite/");
	//数字
	sprites[static_cast<int>(Spritenumber::Number)] =std::make_unique<Sprite>("Data/Sprite/number.png");
	//旗
	sprites[static_cast<int>(Spritenumber::Flag)] = std::make_unique<Sprite>("Data/Sprite/flag.png");
	//チーム
	sprites[static_cast<int>(Spritenumber::Team)] = std::make_unique<Sprite>("Data/Sprite/team.png");
	//チームセレクト
	sprites[static_cast<int>(Spritenumber::TeamSelect)] = std::make_unique<Sprite>("Data/Sprite/teamselect.png");
	//チーム作成
	sprites[static_cast<int>(Spritenumber::TeamCreate)] = std::make_unique<Sprite>("Data/Sprite/teamcreate.png");
	//加入
	sprites[static_cast<int>(Spritenumber::TeamJoin)] = std::make_unique<Sprite>("Data/Sprite/teamjoin.png");
	//チーム加入する時のクリックできないとこ
	sprites[static_cast<int>(Spritenumber::TeamJoinFrame)] = std::make_unique<Sprite>("Data/Sprite/teamjoinframe.png");
	//チーム加入する時のクリックするとこ
	sprites[static_cast<int>(Spritenumber::TeamjoinNumber)] = std::make_unique<Sprite>("Data/Sprite/teamjoinnumber.png");
	//チーム番号
	sprites[static_cast<int>(Spritenumber::TeamNumber)] = std::make_unique<Sprite>("Data/Sprite/teamnumber.png");
	//×ボタン
	sprites[static_cast<int>(Spritenumber::Close)] = std::make_unique<Sprite>("Data/Sprite/close.png");

	//初期化が一度目かどうか
	if (!SceneManager::Instance().GetstandbyInitialized())
	{
		// プレイヤーマネジャー初期化
		//playerManager = new PlayerManager();
		SceneManager::Instance().PlayermanagerInitialize();
		playerManager = SceneManager::Instance().GetPlayerManager();

		//	サーバーとの接続前はID０
		Player* player = new Player();
		player->SetPlayerID(0);						//貰ったID情報をストック
		player->Setoperation(false);
		player->SetPosition(DirectX::XMFLOAT3(0, 0, 0));			//発生位置
		player->SetAngle({ 0.0f,3.0f,0.0f });

		playerManager->SetMyPlayerID(0);
		playerManager->AddPlayer(player);
		playerManager->GetPlayer(0)->SetReady(true);

		//生成したら
		playerManager->AddPlayersGenerateCount();


		SceneManager::Instance().ConnectionInitialize();
		connection = SceneManager::Instance().GetConnection();

		SceneManager::Instance().SetstandbyInitialized(true);
	}
	else
	{
		playerManager = SceneManager::Instance().GetPlayerManager();
		for (int i = 0; i < playerManager->GetPlayers().size(); ++i)
		{
			int MYID = playerManager->GetMyPlayer()->Getteamsid(i);
			playerManager->GetPlayer(MYID)->SetAngle({ 0.0f,3.0f,0.0f });
			playerManager->GetPlayer(MYID)->SetPosition({ 1.0f * i,0.0f,0.0f });
		}
		connection = SceneManager::Instance().GetConnection();
		connectionflag = true;
		if (playerManager->GetteamLeader())
		{
			teamcreate = true;
		}
		else
		{
			//準備チェックを外す
			playerManager->GetMyPlayer()->SetstartCheck(false);
			teamjoin = true;
		}
	}
}

void SceneStandby::Finalize()
{
	//// ナンバースプライト
	//if (this->spriteNumber)
	//{
	//	delete spriteNumber;
	//	spriteNumber = nullptr;
	//}

	// カメラコントーラー終了化
	if (this->cameraController)
	{
		delete cameraController;
		cameraController = nullptr;
	}
	// ステージ終了化
	StageManager::instance().Clear();


	if (this->player)
	{
		delete player;
		player = nullptr;
	}

	//プレイヤーと接続の管理はシーンマネージャーが持ってる

}

void SceneStandby::Update(float elapsedTime)
{
	
	
	//シーン遷移
	if (playerManager->GetGameStart())
	{
		playerManager->SetGameStart(false);
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));

	}

	//サーバにつなげるとき
	if (connectionflag && !connection->isConnction)
	{
		connection->Initialize();
		if(connection->isConnction)
		connection->SetplayerManager(playerManager);
		else
		{
			connectionflag = false;
		}
	}

	//チームを作る時
	if (sendteamcreate&&!teamcreate)
	{
		sendteamcreate = false;
		teamcreate = true;
		connection->SendTeamcreate();
		playerManager->SetteamLeader(true);
	}

	//チームに参加するとき
	if (sendteamjoin && !teamcreate && !teamjoin)
	{
		sendteamjoin = false;
		teamjoin = true;
		connection->SendTeamJoin(TeamNumber);
	}
	
	//準備OK
	if (startcheck != sendstartcheck)
	{
		sendstartcheck = !sendstartcheck;
		connection->SendStartCheck(sendstartcheck);
	}

	//ゲームスタート申請
	if(sendgamestart)
	{
		sendgamestart = false;
		connection->SendGamestart
		(playerManager->GetMyPlayer()->Getteamnumber());
	}



	
	int generateCount = playerManager->GetPlayersGenerateCount();

	if (playerManager->GetSynclogin())
	{
		int teamMax = 4;
		for (int i = 0; i < teamMax-1; ++i)
		{
			int ID = playerManager->GetMyPlayer()->Getteamsid(i);

			//チームに存在しなかったら
			if (ID == 0)break;
			//自分のIDじゃなかったら
			if (playerManager->GetMyPlayerID() != ID)
			{
				//生成
				Player* player = new Player();
				player->SetPlayerID(ID);						//貰ったID情報をストック
				player->SetPosition(DirectX::XMFLOAT3(1.5 * generateCount, 0, 0));			//発生位置
				player->SetAngle({ 0.0f,3.0f,0.0f });
				player->Setoperation(false);

				playerManager->AddPlayer(player);
				playerManager->GetPlayer(ID)->SetReady(true);
				playerManager->AddPlayersGenerateCount();
			}
		}
		playerManager->SetSynclogin(false);
	}

	//自分以外のプレイヤー生成join用
	generateCount = playerManager->GetPlayersGenerateCount();
	//ログインしてきてかつ生成がまだだったら
	if (generateCount < playerManager->GetLoginCount()&&!playerManager->GetSynclogin())
	{
		//生成
		int ID = playerManager->GetMyPlayer()->Getteamsid(generateCount);
		Player* player = new Player();
		player->SetPlayerID(ID);						//貰ったID情報をストック
		player->SetPosition(DirectX::XMFLOAT3(1.5 * generateCount, 0, 0));			//発生位置
		player->SetAngle({ 0.0f,3.0f,0.0f });
		player->Setoperation(false);

		playerManager->AddPlayer(player);
		playerManager->GetPlayer(ID)->SetReady(true);
		playerManager->AddPlayersGenerateCount();
	}
		
	
	cameraController->Update(elapsedTime);
	//player->Update(elapsedTime);
	//プレイヤーが存在すれば
	//if (playerManager->GetMyPlayerID() != 0)
	{
		playerManager->Update(elapsedTime);

		//imguiでちーむのＩＤを確認するため
		guiteamsid[0] = playerManager->GetMyPlayer()->Getteamsid(0);
		for (int i = 1; i < 3; ++i)
		{
			guiteamsid[i]= playerManager->GetMyPlayer()->Getteamsid(i);
		}
	}
}

void SceneStandby::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 描画処理 
	RenderContext rc;// 描画するために必要な情報をまとめた構造体
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ライト方向（下方向）

	// カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	// 3Dモデル描画
	{
		Shader* shader = graphics.GetShader();
		// シェーダーに必要な情報を書く
		shader->Begin(dc, rc);// シェーダーにカメラの情報を渡す
		// ステージ描画
		StageManager::instance().Render(dc, shader);
		// プレイヤー描画
		//player->Render(dc, shader);
		//if (playerManager->GetMyPlayerID() != 0)
		playerManager->Render(dc, shader);

		shader->End(dc);
	}

	// 2Dスプライト描画
	{
		if (numberinputflag)
	    {
	    	RenderTeamJoin(dc);
	    }
		if (playerManager->GetMyPlayerID() != 0)
		RenderID(dc, rc.view, rc.projection);

		if (!teamscreenflag)
		{
			RenderTeam(dc);
		}
		else
		{
			RenderTeamSelect(dc);
		}
		
	
		if (teamcreate)
		{
			RenderTeamNumber(dc, rc.view, rc.projection);
			
		}
	}
	
	
	// 2Dデバッグ描画
	{
		
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
		// beginからendまでの内容が出来る
		if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
		{
				//ImGui::Text("LoginCount: %d", playerManager->GetLoginCount());
				//ImGui::Text("GenerateCount: %d", playerManager->GetPlayersGenerateCount());
			if (playerManager->GetMyPlayerID() != 0)
			{

				ImGui::Text("Name: %s", playerManager->GetMyPlayer()->GetName());
				ImGui::Text("ID: %d", playerManager->GetMyPlayerID());
				ImGui::InputInt("SendTeamNumber: %d", &TeamNumber);
				ImGui::Text("RecvTeamNumber: %d", playerManager->GetMyPlayer()->Getteamnumber());
			}
			if (!connectionflag)
			{
				if (ImGui::Button("Connection"))
				{
					connectionflag = true;
				}
				/*if (connection->connctionflag)
				{
					ImGui::Text("true");
				}
				else
				{
					ImGui::Text("false");
				}*/
			}

			if (playerManager->GetMyPlayerID() != 0)
			{
				if (!teamcreate && !teamjoin)
				{
					if (TeamNumber == 0)
					{
						if (ImGui::Button("Team Creation"))
						{
							sendteamcreate = true;
						}
					}
					else
					{
						if (ImGui::Button("Team Join"))
						{
							sendteamjoin = true;
						}
					}
				}
				if (playerManager->GetMyPlayer()->Getteamnumber() != 0&& !teamcreate)
				{
					if (ImGui::Button("Start Check"))
					{
						startcheck = !startcheck;
					}
					if (playerManager->GetMyPlayer()->GetstartCheck())
					{
						ImGui::Text("true");
					}
					else
					{
						ImGui::Text("false");
					}
					
				}

				if(teamcreate)
				if (ImGui::Button("GameStart"))
				{
					sendgamestart = true;
				}
			}
			ImGui::InputInt4("TeamsID", guiteamsid);

			if (ImGui::Button("FriendList Update"))
			{
				connection->SendSeeFriend();
			}

			ImGui::Text("FriendList");
			for (int i = 0; i < playerManager->myFriendList.size(); ++i)
			{
				ImGui::Text("ID: %d", playerManager->myFriendList.at(i).ID);
				ImGui::Text("Name: %s", playerManager->myFriendList.at(i).name);
			}

			playerManager->DebugGUI();
		}
		ImGui::End();

		if (playerManager->GetLoginCount() > 1)
		{
			ImGui::SetNextWindowPos(ImVec2(500, 10), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(300, 320), ImGuiCond_Once);
			if (ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_None))
			{
				ImGui::Text("Message");
				ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 200), ImGuiWindowFlags_NoTitleBar);

				for (std::string message : playerManager->Getmessages()) {
					ImGui::Text(u8"%s", message.c_str());
				}
				ImGui::EndChild();
				ImGui::Spacing();

				ImGui::InputText("Message", input, sizeof(input));
				if (playerManager->GetMyPlayer()->Getteamnumber() != 0)
					if (ImGui::Button("Send"))
					{
						if (strcmp(input, "") != 0)
						{
							// 送信処理
							connection->SendMessages(input);
						}
						input[0] = '\0';
					}
			}
			ImGui::End();
		}


		if(connectionflag)
		{
			ImGui::SetNextWindowPos(ImVec2(500, 200), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
			// beginからendまでの内容が出来る
			if (ImGui::Begin("login", nullptr, ImGuiWindowFlags_None))
			{
				if (playerManager->GetMyPlayerID() == 0 && !signupflag && !signinflag)
				{
					if (ImGui::Button("sign up"))
					{
						signupflag = true;
					}
					if (ImGui::Button("sign in"))
					{
						signinflag = true;
					}if (ImGui::Button("geustlogin"))
					{
						geustloginflag = true;
						connection->SendGeustLogin();
					}
				}
				else if(signupflag || signinflag)
				{
					if (ImGui::Button("back"))
					{
						memset(name, 0, sizeof(name));
						memset(pass, 0, sizeof(pass));
						signupflag = false;
						signinflag = false;
					}
				}
				if (signupflag && !playerManager->GetSignUp())
				{
					ImGui::Text("Sign Up");
					ImGui::InputText("Name", name, sizeof(name));
					ImGui::InputText("password", pass, sizeof(pass));
					if (strcmp(name, "") != 0 && strcmp(pass, "") != 0)
						if (ImGui::Button("Decision"))
						{
							connection->SendSignUp(name, pass);
							signupflag = false;
						}
				}

				if (signinflag&& !playerManager->GetSignIn())
				{
					ImGui::Text("Sign In");
					ImGui::InputText("Name", name, sizeof(name));
					ImGui::InputText("password", pass, sizeof(pass));
					if (strcmp(pass, "") != 0)
					{
						if (ImGui::Button("Decision"))
						connection->SendSignIn(name, pass);
						
					}
				}
				if (playerManager->GetMyPlayerID() > 0)
				{
					ImGui::Text("Login OK");
					if (!searchflag)
					{
						ImGui::InputScalar("Input short", ImGuiDataType_S16, &searchId);
						ImGui::Text("Current value: %d", searchId);
						if (ImGui::Button("IDSerach"))
						{
							connection->SendIdSearch(searchId);
							searchflag = true;
						}
					}
					else
					{
						//検索結果
						if (playerManager->GetSearchResult())
						{
							//成功
							ImGui::Text("SearchName: %s", playerManager->GetSearchName());
							ImGui::Text("MyID: %d", playerManager->GetMyPlayerID());
							ImGui::Text("recvID: %d", playerManager->GetSearchId());
							//自分のIDかどうか
							if (playerManager->GetMyPlayerID() != playerManager->GetSearchId())
							{
								if (ImGui::Button("FriendRequest"))
								{
									connection->SendFriendRequest(playerManager->GetSearchId());
									searchflag = false;
									searchId = false;
								}
								if (ImGui::Button("Back"))
								{
									searchflag = false;
									searchId = false;
								}
							}
							else
							{
								ImGui::Text("My ID");
								if (ImGui::Button("Back"))
								{
									searchflag = false;
									searchId = false;
								}
							}
						}
						else
						{
							//失敗
							ImGui::Text("SearchError");
							if (ImGui::Button("Back"))
							{
								searchflag = false;
								searchId = false;
							}

						}
					}
				}
				if (playerManager->GetSendertId() > 0)
				{
					ImGui::Text("SendertName: %s", playerManager->GetSenderName());
					ImGui::Text("SenderID: %d", playerManager->GetSendertId());
					//承認
					if (ImGui::Button("Approval"))
					{
						char myname[10],youname[10];

						strcpy_s(myname, playerManager->GetMyPlayer()->GetName());
						strcpy_s(youname, playerManager->GetSenderName());
						connection->SendFriendApproval(
							playerManager->GetSendertId(),
							playerManager->GetMyPlayerID(),
							myname,
							youname);
					}
					ImGui::SameLine();
					//否認
					if (ImGui::Button("Denial"))
					{
						playerManager->ResetSenderName();
						playerManager->SetSendertId(0);
					}
				}
				
			}
			ImGui::End();
		}
	}
}

// プレイヤーのID描画
void SceneStandby::RenderID(ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection
)
{
	// ビューポート 画面のサイズ等
	// ビューポートとは2Dの画面に描画範囲の指定(クリッピング指定も出来る)位置を指定
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	// ラスタライザーステートにバインドされているビューポート配列を取得
	dc->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	// ローカルからワールドに行くときにいる奴相手のポジションを渡す。
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
	DirectX::XMVECTOR world = {};
	DirectX::XMVector3Transform(world, World);

	//プレイヤーの頭上
	DirectX::XMFLOAT3 worldPosition = playerManager->GetMyPlayer()->GetPosition();
	worldPosition.y += playerManager->GetMyPlayer()->GetHeight();

	// ワールドからスクリーン
	DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);


	// ゲージ描画 // ワールドからスクリーン
	DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
		WorldPosition,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		Projection,
		View,
		World

	);
	// スクリーン座標
	DirectX::XMFLOAT3 scereenPosition;
	DirectX::XMStoreFloat3(&scereenPosition, ScreenPosition);


	// 画像の長さ
	const float gaugeWidth = 50.0f;
	const float gaugeHeight = 70.0f;
	int ID = playerManager->GetMyPlayerID();

	// プレイヤーIDの桁数を求める
	int numDigits = 1;
	int tempID = 1;
	while (tempID >= 10)
	{
		tempID /= 10;
		numDigits++;
	}

	
	// 2Dスプライト描画
	{
		float positionX = scereenPosition.x - 7;
		float positionY = scereenPosition.y;
		int digit = 0;

		// 各桁を描画するループ
		for (int i = numDigits - 1; i >= 0; --i)
		{
			// 各桁の数値を取得
			digit = (ID / static_cast<int>(pow(10, i))) % 10;
			// スプライトを描画
			sprites[static_cast<int>(Spritenumber::Number)]->Render(dc,
				positionX, positionY,
				15, 15,
				gaugeWidth * digit,gaugeHeight,
				gaugeWidth, gaugeHeight,
				0.0f,
				1, 0, 0, 1);

			// 次の桁の位置に移動
			positionX += 15;
		}
	}
}

void SceneStandby::RenderTeamNumber(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{

	float positionX = 150;
	float positionY = 70;
	sprites[static_cast<int>(Spritenumber::TeamNumber)]->Render(dc,
		positionX, positionY,  //描画位置
		200,40,                //表示サイズ
		0, 0,                  //切り取りはじめ位置
		1214, 200,             //画像サイズ
		0.0f,
		1, 1, 1, 1);

	// 画像の長さ
	const float gaugeWidth = 25.0f;
	const float gaugeHeight =33.0f;
	
	// プレイヤーIDの桁数を求める
	int numDigits = 1;
	int ID =playerManager->GetMyPlayer()->Getteamnumber();
	int tempID = ID;
	while (tempID >= 10)
	{
		tempID /= 10;
		numDigits++;
	}

	// 2Dスプライト描画
	{
		float numberposX = positionX +90;
		float numberposY = positionY+8;
		int digit = 0;

		// 各桁を描画するループ
		for (int i = numDigits - 1; i >= 0; --i)
		{
			// 各桁の数値を取得
			digit = (ID / static_cast<int>(pow(10, i))) % 10;

			// スプライトを描画
			sprites[static_cast<int>(Spritenumber::Number)]->Render(dc,
				numberposX, numberposY,
				30, 30,
				gaugeWidth * digit+ digit, 0,
				gaugeWidth, gaugeHeight,
				0.0f,
				1, 1, 1, 1);

			// 次の桁の位置に移動
			numberposX += 20;
		}
	}
}

void SceneStandby::RenderTeam(ID3D11DeviceContext* dc)
{
	float positionX = 10;
	float positionY = 10;
	float sizeX = 100;
	float sizeY = 30;
	sprites[static_cast<int>(Spritenumber::Team)]->Render(dc,
		positionX, positionY,             //描画位置
		sizeX, sizeY,             //表示サイズ
		0, 0,                //切り取りはじめ位置
		243,100,             //画像サイズ
		0.0f,
		1, 1, 1, 1);

	if (!teamscreenflag&&Uiclick(positionX, positionY, sizeX, sizeY))
	{
		teamscreenflag = true;
	}
}

void SceneStandby::RenderTeamSelect(ID3D11DeviceContext* dc)
{
	float positionX = 80;
	float positionY = 50;
	//枠組み
	sprites[static_cast<int>(Spritenumber::TeamSelect)]->Render(dc,
		positionX, positionY, //描画位置
		600, 250,               //表示サイズ
		0, 0,                 //切り取りはじめ位置
		1000,400,           //画像サイズ
		0.0f,
		1, 1, 1, 1);


	float sizeX = 200;
	float sizeY = 150;
	//チーム作成ボタン
	sprites[static_cast<int>(Spritenumber::TeamCreate)]->Render(dc,
		positionX + 70, positionY + 60, //描画位置
		sizeX, sizeY,               //表示サイズ
		0, 0,                 //切り取りはじめ位置
		800, 600,           //画像サイズ
		0.0f,
		1, 1, 1, 1);

		if (Uiclick(positionX + 70, positionY + 60, 200, 150))
		{
			teamscreenflag = false;
			sendteamcreate = true;
		}

	//チーム加入ボタン
	sprites[static_cast<int>(Spritenumber::TeamJoin)]->Render(dc,
		positionX+320, positionY+60, //描画位置
		sizeX, sizeY,               //表示サイズ
		0, 0,                 //切り取りはじめ位置
		800,600,           //画像サイズ
		0.0f,
		1, 1, 1, 1);
	
		if (Uiclick(positionX + 320, positionY + 60, sizeX, sizeY))
		{
			teamscreenflag = false;
			numberinputflag = true;
		}
	
	//閉じるボタン
	sprites[static_cast<int>(Spritenumber::Close)]->Render(dc,
		positionX + 565, positionY-5, //描画位置
		40,40,               //表示サイズ
		0, 0,                 //切り取りはじめ位置
		100,100,           //画像サイズ
		0.0f,
		1, 1, 1, 1);

	if (Uiclick(positionX + 565, positionY - 5, 40, 40))
	{
		teamscreenflag = false;
	}
}

void SceneStandby::RenderTeamJoin(ID3D11DeviceContext* dc)
{
	float positionX = 80;
	float positionY = 30;
	//枠組み
	sprites[static_cast<int>(Spritenumber::TeamJoinFrame)]->Render(dc,
		positionX, positionY,   //描画位置
		600, 300,               //表示サイズ
		0, 0,                   //切り取りはじめ位置
		1000, 400,              //画像サイズ
		0.0f,
		1, 1, 1, 1);

	//ボタンを個数分
	
	int count = 0;
	int sizeX = 90;
	int sizeY = 30;
	
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			++count;
			sprites[static_cast<int>(Spritenumber::TeamjoinNumber)]->Render(dc,
				positionX + 130 + (j * 100), positionY + 140+(i*40), //描画位置
				sizeX, sizeY,                      //表示サイズ
				0 + (j * 300), 0+(i*100),    //切り取りはじめ位置
				300, 100,                    //画像サイズ
				0.0f,
				1, 1, 1, 1);

			int posx = positionX + 130 + (j * 100);
			int posy = positionY + 140 + (i * 40);
			posxy[count-1][0] = posx;
			posxy[count-1][1] = posy;
			
		}
	}
	if (numbers.size() < 4)
	{
		if (Uiclick(posxy[0][0], posxy[0][1], sizeX, sizeY))numbers.push_back(1);
		if (Uiclick(posxy[1][0], posxy[1][1], sizeX, sizeY))numbers.push_back(2);
		if (Uiclick(posxy[2][0], posxy[2][1], sizeX, sizeY))numbers.push_back(3);
		if (Uiclick(posxy[3][0], posxy[3][1], sizeX, sizeY))numbers.push_back(4);
		if (Uiclick(posxy[4][0], posxy[4][1], sizeX, sizeY))numbers.push_back(5);
		if (Uiclick(posxy[5][0], posxy[5][1], sizeX, sizeY))numbers.push_back(6);
		if (Uiclick(posxy[6][0], posxy[6][1], sizeX, sizeY))numbers.push_back(7);
		if (Uiclick(posxy[7][0], posxy[7][1], sizeX, sizeY))numbers.push_back(8);
		if (Uiclick(posxy[8][0], posxy[8][1], sizeX, sizeY))numbers.push_back(9);
		if (Uiclick(posxy[10][0], posxy[10][1], sizeX, sizeY))numbers.push_back(0);
	}
	if (numbers.size() > 0) 
	{
		if (Uiclick(posxy[9][0], posxy[9][1], sizeX, sizeY))numbers.pop_back();
	}
	if (numbers.size() ==4)
	{
		if (Uiclick(posxy[11][0], posxy[11][1], sizeX, sizeY))
		{
			sendteamjoin = true;
		}
	}

	// 2Dスプライト描画
	{
		float numberposX = positionX + 188;
		float numberposY = positionY + 80;
		const float gaugeWidth = 25.9f;
		const float gaugeHeight = 33.0f;

		// 各桁を描画するループ
		if (numbers.size() > 0)
		{
			for (int i = 0; i < numbers.size(); ++i)
			{
				int number = numbers.at(i);

				// スプライトを描画
				sprites[static_cast<int>(Spritenumber::Number)]->Render(dc,
					numberposX+(51*i), numberposY,
					30, 30,
					gaugeWidth * number, 0,
					gaugeWidth, gaugeHeight,
					0.0f,
					1, 1, 1, 1);
			}
		}
	}
}

bool SceneStandby::Uiclick(float posX, float posY, float sizeX, float sizeY)
{
	DirectX::XMFLOAT3 scereenPosition;

	Mouse& mouse = Input::Instance().GetMouse();
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		scereenPosition.x = static_cast<float>(mouse.GetPositionX());
		scereenPosition.y = static_cast<float>(mouse.GetPositionY());

		if (posX < scereenPosition.x&& scereenPosition.x < posX+ sizeX
			&& posY < scereenPosition.y && scereenPosition.y < posY+sizeY)
		{
			Sleep(10);
			return true;
		}
	}
	return false;
}


