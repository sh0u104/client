#include "SceneStandby.h"

#include "Graphics/Graphics.h"
#include "Camera.h"
#include "StageManager.h"
#include "StageMain.h"

#include "Input/Input.h"

#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneGame.h"
#include "SceneTitle.h"

#include "Graphics/SpriteManager.h"
void SceneStandby::Initialize()
{
	
	StageManager& stageManager = StageManager::instance();
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);

	//カメラ
	{
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
		cameraController->SetRange(10.0f);
		//DirectX::XMFLOAT3 target = { 0,0,0 };
		//target.y += 0.5f;// 足元から５０センチぐらい
		//cameraController->SetTarget(target);// プレイヤーの腰当たり
	}

	SceneManager& sceneManager = SceneManager::Instance();

	//初期化が一度目かどうか
	if (!sceneManager.GetstandbyInitialized())
	{
		sceneManager.SetstandbyInitialized(true);
		playerManager = sceneManager.GetPlayerManager();
		connection = sceneManager.GetConnection();
	}
	//二度目以降
	else
	{
		playerManager = sceneManager.GetPlayerManager();
		connection = sceneManager.GetConnection();

		playerManager->GetMyPlayer()->SetstartCheck(false);
	    //ソロの場合　
		if (playerManager->GetMyPlayer()->Getteamnumber() == 0)
		{
			playerManager->GetMyPlayer()->SetAngle({ 0.0f,3.0f,0.0f });
			playerManager->GetMyPlayer()->SetPosition({ 0.0f,0.0f,0.0f });
			playerManager->GetMyPlayer()->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
		}
		else
		{
			for (int i = 0; i < playerManager->GetPlayers().size(); ++i)
			{
				int MYID = playerManager->GetMyPlayer()->Getteamsid(i);
				playerManager->GetPlayer(MYID)->SetAngle({ 0.0f,3.0f,0.0f });
				playerManager->GetPlayer(MYID)->SetPosition({ 1.5f * i,0.0f,0.0f });
				playerManager->GetPlayer(MYID)->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
				playerManager->GetPlayer(MYID)->SetstartCheck(false);

			}
			
			if (playerManager->GetMyPlayer()->GetTeamHost())
			{
				teamcreate = true;
			}
			else
			{
				teamjoin = true;
			}
		}
	}
}

void SceneStandby::Finalize()
{
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

	//デバッグ用
	if (debugGameStart)
	{
		debugGameStart = false;
		if (playerManager->GetMyPlayerID() > 0)
		{
			sendgamestart = true;
			if (playerManager->GetMyPlayer()->Getteamnumber() == 0)
			{
				playerManager->GetMyPlayer()->SetTeamHost(true);
			}
		}
	}

	//ゲームスタート申請
	if (sendgamestart && playerManager->GetMyPlayer()->GetTeamHost())
	{
		sendgamestart = false;
		connection->SendGamestart(playerManager->GetMyPlayer()->Getteamnumber());
	}

	//チームに参加するとき
	if (sendteamjoin && !teamcreate && !teamjoin)
	{
		sendteamjoin = false;
		teamjoin = true;
		connection->SendTeamJoin(TeamNumber);
	}

	

	playerManager->Update(elapsedTime);

	//チームのホストがいなくなったら
	if (playerManager->GetTeamDisbabded())
	{
		for (int i = 0; i < 3; ++i)
		{
			int ID = playerManager->GetMyPlayer()->Getteamsid(i);
			if (ID != playerManager->GetMyPlayerID())
			{
				playerManager->ErasePlayer(ID);
			}
		}
		playerManager->SetTeamDisbabded(false);
		playerManager->GetMyPlayer()->ResetTeamsid();
		playerManager->GetMyPlayer()->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
		teamRenderflag = true;
	}
	


	//チームに先にいたプレイヤーを生成
	SyncPlayerGenerate();
	//後から来たプレイヤーを生成
	LoginPlayerGenerate();

	//プレイヤー消去リストにあるものを消す
	playerManager->DeletePlayer();


	//imgui用
	for (int i = 0; i < 3; ++i)
	{
		guiteamsid[i] = playerManager->GetMyPlayer()->Getteamsid(i);
	}

	// カメラコントローラー更新処理
	DirectX::XMFLOAT3 target = playerManager->GetMyPlayer()->GetPosition();
	target.y += 0.5f;// 足元から５０センチぐらい
	cameraController->SetTarget(target);// プレイヤーの腰当たり
	cameraController->Update(elapsedTime);

	// ステージ更新処理
	StageManager::instance().Update(elapsedTime);


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
		//if (playerManager->GetMyPlayerID() != 0)
		playerManager->Render(dc, shader);

		shader->End(dc);
	}

	// 2Dスプライト描画
	if (!connection->GetIsConectionError())
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		//現在のモード
		RenderMode(dc, screenHeight);

		//ゲーム開始ボタン
		//チームのホストの時か チームを組んで無くチーム関連の処理をしていない時
		if (playerManager->GetMyPlayer()->GetTeamHost() || playerManager->GetMyPlayer()->Getteamnumber() < 1&& teamRenderflag)
		{
			RenderGameStart(dc, screenHeight);
		}

		//名前が無ければ
		//if (playerManager->GetMyPlayerID() != 0 && !numberinputflag && playerManager->GetMyPlayer()->GetName()[0] == '\0')
		//{
		//	//ID表示
		//	RenderID(dc, rc.view, rc.projection);
		//}
		//名前表示
			RenderName(dc, rc.view, rc.projection);
		//名前があれば
		//if (playerManager->GetMyPlayer()->GetName() != '\0'&& !numberinputflag)
		//{
		//	//名前表示
		//	RenderName(dc, rc.view, rc.projection);
		//}
		
		//チームを組んでいれば
		if (playerManager->GetMyPlayer()->Getteamnumber() > 0)
		{
			//チーム番号表示
			RenderTeamNumber(dc, rc.view, rc.projection);
			//ホストじゃなければ
			if (!playerManager->GetMyPlayer()->GetTeamHost())
			{
				//準備完了ボタン表示
				RenderReady(dc,screenHeight, playerManager->GetMyPlayer()->GetstartCheck());
			}
		}
		//チームを組んでなかったら
		else
		{
			if (teamRenderflag)
			{
				RenderTeam(dc);
			}
			else
			{
				if (numberinputflag)
				{
					RenderTeamJoin(dc,screenWidth,screenHeight);
				}
				else
				{
					RenderTeamSelect(dc,screenWidth,screenHeight);
				}
			}
		}
	}
	else
	{
		//サーバーとの接続が切れたら
		connection->ConnectionCheck(dc);
	}
	
	
	// 2Dデバッグ描画
	//IMGUI描画
	{
		ImGui::SetNextWindowPos(ImVec2(800, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(150, 150), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Startcheck", nullptr, ImGuiWindowFlags_None))
		{
			if (playerManager->GetMyPlayer()->Getteamsid(0) > 0)
			{
				ImGui::Text("HostID %d", playerManager->GetMyPlayer()->Getteamsid(0));
			}
			for (auto& player : playerManager->GetPlayers())
			{
				ImGui::Text("ID %d", player->GetPlayerID());
				ImGui::Text("Check %s", player->GetstartCheck() ? "true" : "false");
			}
		}
		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(1000, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(150, 150), ImGuiCond_FirstUseEver);
		// beginからendまでの内容が出来る
		if (ImGui::Begin("PlayerData", nullptr, ImGuiWindowFlags_None))
		{
			ImGui::Text("UserID: %d", playerManager->GetMyPlayer()->GetMaxID());
			ImGui::Text("LoginDay: %d", playerManager->GetMyPlayer()->GetLoginDay());
			ImGui::Text("Name: %s", playerManager->GetMyPlayer()->GetName());
			
			
			if (playerManager->GetMyPlayer()->Getteamnumber() != 0)
			{
				ImGui::Text("Host: %d", playerManager->GetMyPlayer()->GetTeamHost());
			}
				
			//ImGui::Text("Disbanded: %d", playerManager->GetTeamDisbabded());
			//ImGui::Text("GenerateCount: %d", playerManager->GetPlayersGenerateCount());
			//ImGui::Text("LoginCount: %d", playerManager->GetLoginCount());
			//
			//ImGui::Text("PlayersSize: %d", playerManager->GetPlayers().size());
			//
		    //ImGui::Text("State: %d", static_cast<int>(playerManager->GetMyPlayer()->GetState()));
			//if (ImGui::Button("debugGameStart"))
			//{
			//	debugGameStart = true;
			//	
			//}
			//ImGui::InputInt4("TeamsID", guiteamsid);
			//if (ImGui::Button("FriendList Update"))
			//{
			//	connection->SendSeeFriend();
			//}
			//
			//ImGui::Text("FriendList");
			//for (int i = 0; i < playerManager->myFriendList.size(); ++i)
			//{
			//	ImGui::Text("ID: %d", playerManager->myFriendList.at(i).ID);
			//	ImGui::Text("Name: %s", playerManager->myFriendList.at(i).name);
			//}
		
			playerManager->DebugGUI();
		}
		ImGui::End();
		
		//プレイヤーやが二人以上なら
		if (playerManager->GetLoginCount() > 1)
		{
			ImGui::SetNextWindowPos(ImVec2(950, 10), ImGuiCond_Once);
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
				//ImGui::Text("startcheck");
				//for (auto& player : playerManager->GetPlayers())
				//{
				//	//ホストは非表示
				//	if (player->GetPlayerID() == player->Getteamsid(0))
				//		continue;
				//
				//	ImGui::Text("ID %d", player->GetPlayerID());
				//	ImGui::Text("Check %s", player->GetstartCheck() ? "true" : "false");
				//}
				
			}
			ImGui::End();
		}

		
	}
}

void SceneStandby::OprerationSelect(ID3D11DeviceContext* dc)
{
	//設定ボタン表示
	{
		DirectX::XMFLOAT2 size, pos;
		pos = { 410,10 };
		size = { 30,30 };

		Sprite* SettingSprite = g_SpriteManager.GetSprite(SpriteNumber::Setting);
		SettingSprite->Render(dc,
			pos.x, pos.y,  //描画位置
			size.x, size.y,  //表示サイズ
			0, 0,      //切り取りはじめ位置
			256, 256, //画像サイズ
			0.0f,
			1, 1, 1, 1);

		if (Uiclick(pos.x,pos.y, size.x,size.y))
		{
			isSetting = !isSetting;
			Sleep(10);
		};
	}

	//設定ボタンを押していたら
	if (isSetting)
	{
		DirectX::XMFLOAT2 size, mousePos, WASDPos, edgePos;

		size = { 50,75 };
		mousePos = { 75,25 };
		WASDPos = { 175,25 };

		//マウスのイラスト描画
		Sprite* MouseSprite = g_SpriteManager.GetSprite(SpriteNumber::Mouse);
		MouseSprite->Render(dc,
			mousePos.x, mousePos.y,  //描画位置
			size.x, size.y,  //表示サイズ
			0, 0,      //切り取りはじめ位置
			780, 1178, //画像サイズ
			0.0f,
			1, 1, 1, 1);

		//WASDボタンの描画
		Sprite* WASDSprite = g_SpriteManager.GetSprite(SpriteNumber::WASD);
		WASDSprite->Render(dc,
			WASDPos.x, WASDPos.y,  //描画位置
			size.x, size.y,  //表示サイズ
			0, 0,    //切り取りはじめ位置
			255, 194, //画像サイズ
			0.0f,
			1, 1, 1, 1);


		//操作方法どちらが選択されてるかと縁の座標
		if (playerManager->GetMyPlayer()->GetisMouseOperation())
		{
			if (Uiclick(WASDPos.x,WASDPos.y, size.x,size.y))
			{
				playerManager->GetMyPlayer()->SetisMouseOperation(false);
			}

			edgePos = mousePos;
		}
		else
		{
			if (Uiclick(mousePos.x,mousePos.y, size.x,size.y))
			{
				playerManager->GetMyPlayer()->SetisMouseOperation(true);
			}
			edgePos = WASDPos;
		}
		edgePos.x -= size.x / 4;
		edgePos.y -= size.y / 4;

		//選択している縁描画
		Sprite* SelectEdgeSprite = g_SpriteManager.GetSprite(SpriteNumber::SelectEdge);
		SelectEdgeSprite->Render(dc,
			edgePos.x, edgePos.y,  //描画位置
			size.x * 1.5f, size.y * 1.5f,  //表示サイズ
			0, 0,      //切り取りはじめ位置
			100, 100, //画像サイズ
			0.0f,
			1, 1, 1, 1);

	}
}

void SceneStandby::Logout(ID3D11DeviceContext* dc)
{
	DirectX::XMFLOAT2 size, pos;
	size = { 100,50 };
	pos = { 25,150 };
	//ログアウトボタン
	Sprite* LogoutSprite = g_SpriteManager.GetSprite(SpriteNumber::Logout);
	LogoutSprite->Render(dc,
		pos.x, pos.y,        //描画位置
		size.x, size.y,      //表示サイズ
		0, 0,                //切り取りはじめ位置
		500, 200,            //元の画像サイズ
		0.0f,
		1, 1, 1, 1);

	if (Uiclick(pos.x,pos.y, size.x,size.y))
	{
		//チームを組んでたら抜けるを送る
		if (playerManager->GetMyPlayer()->Getteamnumber() > 0)
		{
			connection->SendTeamLeave(playerManager->GetMyPlayer()->GetTeamHost());
		}
		connection->SendLogout();
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
	}
}

void SceneStandby::SyncPlayerGenerate()
{
	if (playerManager->GetSynclogin())
	{
		int teamMax = 4;
		int count = 0;
		for (int i = 0; i < teamMax - 1; ++i)
		{
			int generateCount = playerManager->GetPlayersGenerateCount();
			int ID = playerManager->GetMyPlayer()->Getteamsid(i);

			//チームに存在しなかったら
			if (ID == 0)break;
			//自分のIDじゃなかったら
			if (playerManager->GetMyPlayerID() != ID)
			{
				//生成
				Player* player = new Player();
				player->SetPlayerID(ID);						//貰ったID情報をストック
				player->SetPosition(DirectX::XMFLOAT3(1.5f *count, 0, 0));			//発生位置
				player->SetAngle({ 0.0f,3.0f,0.0f });
				player->Setoperation(false);

				playerManager->AddPlayer(player);
				playerManager->GetPlayer(ID)->SetReady(true);
				playerManager->AddPlayersGenerateCount();
			}
			else
			{
				playerManager->GetPlayer(ID)->SetPosition(DirectX::XMFLOAT3(1.5f * count, 0, 0));
			}
			count++;
		}
		playerManager->SetSynclogin(false);
	}
}

void SceneStandby::LoginPlayerGenerate()
{
	//自分以外のプレイヤー生成join用
	int generateCount = playerManager->GetPlayersGenerateCount();
	//ログインしてきてかつ生成がまだだったら
	if (generateCount < playerManager->GetLoginCount() && !playerManager->GetSynclogin())
	{
		//生成
		int ID = playerManager->GetMyPlayer()->Getteamsid(generateCount);
		Player* player = new Player();
		player->SetPlayerID(ID);						//貰ったID情報をストック
		player->SetPosition(DirectX::XMFLOAT3(1.5f * generateCount, 0, 0));			//生成位置
		player->SetAngle({ 0.0f,3.0f,0.0f });
		player->Setoperation(false);

		playerManager->AddPlayer(player);
		playerManager->GetPlayer(ID)->SetReady(true);
		playerManager->AddPlayersGenerateCount();
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
	const float gaugeWidth = 25.0f;
	const float gaugeHeight = 33.0f;

	// プレイヤーIDの桁数を求める
	int numDigits = 1;
	int ID = playerManager->GetMyPlayerID();
	int tempID = ID;
	while (tempID >= 10)
	{
		tempID /= 10;
		numDigits++;
	}

	{
		float numberposX = scereenPosition.x - 10;
		float numberposY = scereenPosition.y - 10;
		int digit = 0;

		// 各桁を描画するループ
		for (int i = numDigits - 1; i >= 0; --i)
		{
			// 各桁の数値を取得
			digit = (ID / static_cast<int>(pow(10, i))) % 10;

			// スプライトを描画
			Sprite* NumberSprite = g_SpriteManager.GetSprite(SpriteNumber::Number);
			NumberSprite->Render(dc,
				numberposX, numberposY,
				30, 30,
				gaugeWidth * digit + digit, 0,
				gaugeWidth, gaugeHeight,
				0.0f,
				1, 1, 1, 1);

			// 次の桁の位置に移動
			numberposX += 20;
		}
	}
}

void SceneStandby::RenderTeamNumber(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	// g_SpriteManager を使ってスプライトを取得
	Sprite* numberSprite = g_SpriteManager.GetSprite(SpriteNumber::TeamNumber);
	// 取得したスプライトで Render メソッドを呼び出す
	//numberSprite->Render();
	float positionX = 150;
	float positionY = 70;
	numberSprite->Render(dc,
		positionX, positionY,  //描画位置
		300,60,                //表示サイズ
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

	
	
	float numberposX = positionX +130;
	float numberposY = positionY+15;
	int digit = 0;
	DirectX::XMFLOAT2 size = { 40,40};
	// 各桁を描画するループ
	for (int i = numDigits - 1; i >= 0; --i)
	{
		// 各桁の数値を取得
		digit = (ID / static_cast<int>(pow(10, i))) % 10;

		// スプライトを描画
		Sprite* NumberSprite = g_SpriteManager.GetSprite(SpriteNumber::Number);
		NumberSprite->Render(dc,
			numberposX, numberposY,
			size.x, size.y,
			gaugeWidth * digit+ digit, 0,
			gaugeWidth, gaugeHeight,
			0.0f,
			1, 1, 1, 1);

		// 次の桁の位置に移動
		numberposX += 30;
	}
	
	//閉じるボタン
	DirectX::XMFLOAT2 closeSize = { 50,50 };
	Sprite* CloseSprite = g_SpriteManager.GetSprite(SpriteNumber::Close);
	CloseSprite->Render(dc,
		positionX + 250, positionY+10, //描画位置
		closeSize.x, closeSize.y,               //表示サイズ
		0, 0,                 //切り取りはじめ位置
		100, 100,           //画像サイズ
		0.0f,
		1, 1, 1, 1);

	if (Uiclick(positionX + 250, positionY+10, closeSize.x, closeSize.y))
	{
		playerManager->GetMyPlayer()->Setteamnumber(0);
		teamcreate = false;
		teamRenderflag = true;

		connection->SendTeamLeave(playerManager->GetMyPlayer()->GetTeamHost());
		playerManager->GetMyPlayer()->SetTeamHost(false);
		for (int i = 0; i < 3; ++i)
		{
			int ID = playerManager->GetMyPlayer()->Getteamsid(i);
			if (ID != 0 && ID != playerManager->GetMyPlayerID())
			{
				playerManager->ErasePlayer(ID);
			}
			
		}
		playerManager->GetMyPlayer()->ResetTeamsid();
	}


}

void SceneStandby::RenderName(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
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




	float positionX = scereenPosition.x-30;
	float positionY = scereenPosition.y;

	float sizeX = 32;
	float sizeY = 32;

	char name[10];
	strcpy_s(name, playerManager->GetMyPlayer()->GetName());
	//枠組み
	for (int i = 0; i < 9; ++i)
	{
		if (name[i] == '\0')break;
		int number = static_cast<int>(name[i]);
		int width = number % 16;
		int height = number / 16;

		Sprite* NameSprite = g_SpriteManager.GetSprite(SpriteNumber::Name);
		NameSprite->Render(dc,
			positionX, positionY,      //描画位置
			24,24,              //表示サイズ
			sizeX * width, sizeY * height, //切り取りはじめ位置
			sizeX, sizeY,              //画像サイズ
			0.0f,
			1, 1, 1, 1);

		positionX += sizeX / 2-5;
	}
}

void SceneStandby::RenderTeam(ID3D11DeviceContext* dc)
{
	float positionX = 0;
	float positionY = 0;
	float sizeX = 150;
	float sizeY = 75;

	Sprite* TeamSprite = g_SpriteManager.GetSprite(SpriteNumber::Team);
	TeamSprite->Render(dc,
		positionX, positionY,     //描画位置
		sizeX, sizeY,             //表示サイズ
		0, 0,                     //切り取りはじめ位置
		243,100,                  //画像サイズ
		0.0f,
		1, 1, 1, 1);

	if(teamRenderflag &&Uiclick(positionX, positionY, sizeX, sizeY))
	{
		teamRenderflag = false;
	}
}

void SceneStandby::RenderTeamSelect(ID3D11DeviceContext* dc, float screenWidth, float screenHeight)
{
	float positionX = screenWidth / 8;
	float positionY = screenHeight / 16;
	//枠組み
	Sprite* TeamSelectSprite = g_SpriteManager.GetSprite(SpriteNumber::TeamSelect);
	TeamSelectSprite->Render(dc,
		positionX, positionY, //描画位置
		900, 430,               //表示サイズ
		0, 0,                 //切り取りはじめ位置
		1000,400,           //画像サイズ
		0.0f,
		1, 1, 1, 1);


	float sizeX = 400;
	float sizeY = 300;
	float addposY = 100;
	float addposX = 40;
	//チーム作成ボタン
	Sprite* TeamCreate = g_SpriteManager.GetSprite(SpriteNumber::TeamCreate);
	TeamCreate->Render(dc,
		positionX + addposX, positionY + addposY, //描画位置
		sizeX, sizeY,               //表示サイズ
		0, 0,                 //切り取りはじめ位置
		800, 600,           //画像サイズ
		0.0f,
		1, 1, 1, 1);

	if (Uiclick(positionX + addposX, positionY + addposY, sizeX, sizeY))
	{
		teamcreate = true;
		teamRenderflag = false;
		connection->SendTeamcreate();
		playerManager->GetMyPlayer()->SetTeamHost(true);
	}

	//チーム加入ボタン
	addposX = 450;
	Sprite* TeamJoinCreate = g_SpriteManager.GetSprite(SpriteNumber::TeamJoin);
	TeamJoinCreate->Render(dc,
		positionX+ addposX, positionY+ addposY, //描画位置
		sizeX, sizeY,               //表示サイズ
		0, 0,                 //切り取りはじめ位置
		800,600,           //画像サイズ
		0.0f,
		1, 1, 1, 1);
	
		if (Uiclick(positionX + addposX, positionY + addposY, sizeX, sizeY))
		{
			teamRenderflag = false;
			numberinputflag = true;
		}
	
	//閉じるボタン
		addposX = 840;
		DirectX::XMFLOAT2 closeSize = { 70,70 };
		Sprite* CloseCreate = g_SpriteManager.GetSprite(SpriteNumber::Close);
		CloseCreate->Render(dc,
		positionX + addposX, positionY-5, //描画位置
		closeSize.x, closeSize.y,               //表示サイズ
		0, 0,                 //切り取りはじめ位置
		100,100,           //画像サイズ
		0.0f,
		1, 1, 1, 1);

	if (Uiclick(positionX + addposX, positionY - 5, closeSize.x, closeSize.y))
	{
		teamRenderflag = true;
	}
}

void SceneStandby::RenderTeamJoin(ID3D11DeviceContext* dc, float screenWidth, float screenHeight)
{
	float positionX = screenWidth / 8;
	float positionY = screenHeight / 16;
	//枠組み
	Sprite* TeamJoinFrameCreate = g_SpriteManager.GetSprite(SpriteNumber::TeamJoinFrame);
	TeamJoinFrameCreate->Render(dc,
		positionX, positionY,   //描画位置
		900, 430,               //表示サイズ
		0, 0,                   //切り取りはじめ位置
		1000, 400,              //画像サイズ
		0.0f,
		1, 1, 1, 1);

	//ボタンを個数分
	
	int count = 0;
	float sizeX = 150;
	float sizeY = 45;
	float intervalX = 170.0f;
	float intervalY = 50.0f;
	float gapX = 180.0f;
	float gapY = 200.0f;
	for (float i = 0; i < 4; ++i)
	{
		for (float j = 0; j < 3; ++j)
		{
			++count;

			Sprite* TeamjoinNumberCreate = g_SpriteManager.GetSprite(SpriteNumber::TeamjoinNumber);
			TeamjoinNumberCreate->Render(dc,
				positionX + gapX + (j * intervalX), positionY + gapY +(i* intervalY), //描画位置
				sizeX, sizeY,                      //表示サイズ
				(j * 300.0f), (i*100.0f),    //切り取りはじめ位置
				300, 100,                    //画像サイズ
				0.0f,
				1, 1, 1, 1);

			float posx = positionX + gapX + (j * intervalX);
			float posy = positionY + gapY + (i * intervalY);
			posxy[count-1][0] = posx;
			posxy[count-1][1] = posy;
			
		}
	}
	if (numbers.size() < 4)
	{
		for (int i = 0; i < 10; ++i)
		{
			//9こ目が一桁消去で10こめが0だから
			if (i == 9)
			{
				if (Uiclick(posxy[10][0], posxy[10][1], sizeX, sizeY))numbers.emplace_back(0);
				break;
			}
			if (Uiclick(posxy[i][0], posxy[i][1], sizeX, sizeY))numbers.emplace_back(i+1);

		}
	}
	//一桁消去
	if (numbers.size() > 0) 
	{
		if (Uiclick(posxy[9][0], posxy[9][1], sizeX, sizeY))numbers.pop_back();
	}
	//送信
	if (numbers.size() == 4)
	{
		if (Uiclick(posxy[11][0], posxy[11][1], sizeX, sizeY))
		{
			int result = 0;
			int size = static_cast<int>(numbers.size());
			for (int i = 0; i < size; ++i) {
				result += numbers[i] * static_cast<int>(std::pow(10, size - i - 1));
			}
			TeamNumber = result;
			
			connection->SendTeamJoin(TeamNumber);
			numberinputflag = false;
			numbers.erase(numbers.begin(), numbers.end());
		}
	}

	// 2Dスプライト描画
	{
		float numberposX = positionX + 290;
		float numberposY = positionY + 130;
		const float gaugeWidth = 25.9f;
		const float gaugeHeight = 33.0f;


		// 各桁を描画するループ
		if (numbers.size() > 0)
		{
			for (int i = 0; i < numbers.size(); ++i)
			{
				int number = numbers.at(i);

				// スプライトを描画
				Sprite* NumberCreate = g_SpriteManager.GetSprite(SpriteNumber::Number);
				NumberCreate->Render(dc,
					numberposX+(77*i), numberposY,
					30, 30,
					gaugeWidth * number, 0,
					gaugeWidth, gaugeHeight,
					0.0f,
					1, 1, 1, 1);
			}
		}
	}

	//閉じるボタン
	float addposX = 840;
	DirectX::XMFLOAT2 closeSize = { 70,70 };
	Sprite* CloseCreate = g_SpriteManager.GetSprite(SpriteNumber::Close);
	CloseCreate->Render(dc,
		positionX + addposX, positionY - 5, //描画位置
		closeSize.x, closeSize.y,               //表示サイズ
		0, 0,                 //切り取りはじめ位置
		100, 100,           //画像サイズ
		0.0f,
		1, 1, 1, 1);

	if (Uiclick(positionX + addposX, positionY - 5.0f, closeSize.x, closeSize.y))
	{
		numberinputflag = false;
		teamRenderflag = true;
		numbers.erase(numbers.begin(), numbers.end());
	}
}

void SceneStandby::RenderReady(ID3D11DeviceContext* dc, float screenHeight, bool isready)
{
	DirectX::XMFLOAT2 size = { 150,75 };
	float positionX = 0;
	float positionY = screenHeight - size.y;
	//
	if (!isready)
	{
		Sprite* ReadyCreate = g_SpriteManager.GetSprite(SpriteNumber::Ready);
		ReadyCreate->Render(dc,
			positionX, positionY, //描画位置
			size.x,size.y,               //表示サイズ
			0, 0,                 //切り取りはじめ位置
			500,184,           //画像サイズ
			0.0f,
			1, 1, 1, 1);

		if (Uiclick(positionX, positionY, size.x, size.y))
		{
			playerManager->GetMyPlayer()->SetstartCheck(true);

			connection->SendStartCheck(true);
		}
	}
	else
	{
		Sprite* ReadyzCancelCreate = g_SpriteManager.GetSprite(SpriteNumber::ReadyzCancel);
		ReadyzCancelCreate->Render(dc,
			positionX, positionY, //描画位置
			size.x, size.y,             //表示サイズ
			0, 0,                 //切り取りはじめ位置
			500,184,           //画像サイズ
			0.0f,
			1, 1, 1, 1);

		if (Uiclick(positionX, positionY, size.x, size.y))
		{
			playerManager->GetMyPlayer()->SetstartCheck(false);
			connection->SendStartCheck(false);
		}
	}
}

void SceneStandby::RenderMode(ID3D11DeviceContext* dc, float screenHeight)
{
	DirectX::XMFLOAT2 size = { 150,75 };
	float positionX = 0;
	float positionY = screenHeight- (size.y*2.5f);

	//
	if (playerManager->GetMyPlayer()->Getteamnumber() > 0)
	{
		Sprite* MultiCreate = g_SpriteManager.GetSprite(SpriteNumber::Multi);
		MultiCreate->Render(dc,
			positionX, positionY, //描画位置
			size.x,size.y,               //表示サイズ
			0, 0,                 //切り取りはじめ位置
			500, 184,           //画像サイズ
			0.0f,
			1, 1, 1, 1);
		
	}
	else
	{
		Sprite* SoloCreate = g_SpriteManager.GetSprite(SpriteNumber::Solo);
		SoloCreate->Render(dc,
			positionX, positionY, //描画位置
			size.x, size.y,               //表示サイズ
			0, 0,                 //切り取りはじめ位置
			500, 184,           //画像サイズ
			0.0f,
			1, 1, 1, 1);
	}

}

void SceneStandby::RenderGameStart(ID3D11DeviceContext* dc, float screenHeight)
{
	DirectX::XMFLOAT2 size = { 150,75 };
	float positionX = 0;
	float positionY = screenHeight-size.y;

	Sprite* StartCreate = g_SpriteManager.GetSprite(SpriteNumber::Start);
	StartCreate->Render(dc,
		positionX, positionY, //描画位置
		size.x,size.y,             //表示サイズ
		0, 0,                 //切り取りはじめ位置
		500, 184,           //画像サイズ
		0.0f,
		1, 1, 1, 1);

	if (Uiclick(positionX, positionY, size.x,size.y))
	{
		//チームを組んでいるか(人数を見る)
		if (playerManager->GetPlayers().size() > 1)
		{
			//チーム全員のスタートチェックを確認
			for (const auto& player : playerManager->GetPlayers())
			{
				//ホストはスキップ
				if (player->GetPlayerID() == playerManager->GetMyPlayer()->Getteamsid(0))
					continue;

				if(!player->GetstartCheck())
					return;
			}
		}
		debugGameStart = true;
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


