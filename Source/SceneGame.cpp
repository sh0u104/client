#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"

#include "EffectManager.h"

#include "Input/Input.h"

#include <DirectXMath.h>

#include "StageManager.h"
#include "StageMain.h"

#include "StageMoveFloor.h"

#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneTitle.h"

#include "SceneStandby.h"
#include "Logger.h"
#include "Graphics/SpriteManager.h"
// 初期化
void SceneGame::Initialize()
{
	// ステージ初期化1
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
	cameraController->DebugSetAngleX();

	playerManager = SceneManager::Instance().GetPlayerManager();

	if (playerManager->GetMyPlayer()->Getteamnumber() > 0)
	{
		for (int i = 0; i < playerManager->GetPlayers().size(); ++i)
		{
			int MYID = playerManager->GetMyPlayer()->Getteamsid(i);
			playerManager->GetPlayer(MYID)->SetAngle({ 0.0f,0.0f,0.0f });
			playerManager->GetPlayer(MYID)->SetPosition({ 1.0f * i,0.0f,0.0f });
		}
	}

	connection = SceneManager::Instance().GetConnection();
	

	// エネミー初期化
	EnemyManager& enemyManager = EnemyManager::Instance();
	for (int i = 0; i < 1; ++i)
	{
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 1, 3));
		// 縄張り
		slime->SetTerritory(slime->GetPosition(), 10.0f);
		slime->IsEnemy();
		++enemyCount;
		slime->SetMyEnemyId(enemyCount);
		enemyManager.Register(slime);
	}
	

	//imgui用
	for (int i = 0; i < 3; ++i)
	{
		guiTeamsId[i] = playerManager->GetMyPlayer()->Getteamsid(i);
	}

	//操作権を与える
	playerManager->GetMyPlayer()->Setoperation(true);
}

// 終了化
void SceneGame::Finalize()
{

	// エネミー終了化
	EnemyManager::Instance().Clear();


	// カメラコントーラー終了化
	if (this->cameraController)
	{
		delete cameraController;
		cameraController = nullptr;
	}

	// ステージ終了化
	StageManager::instance().Clear();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	
	PingUpdate(elapsedTime);
	
	//敵全部死んだらロビーに戻る	
	if (EnemyManager::Instance().GetEnemys().size() <= 0)
	{
		//ホストだけゲーム終わったを送る　クリア画面をUpdateで描画するため一度だけ送信
		if (playerManager->GetMyPlayer()->GetTeamHost() && playerManager->GetMyPlayer()->Getoperation())
		{
			//ゲーム終了を送る
			connection->SendGameEnd(playerManager->GetMyPlayer()->Getteamnumber());
		}
		//操作権をきる
		playerManager->GetMyPlayer()->Setoperation(false);
		//SceneManager::Instance().ChangeScene(new SceneLoading(new SceneStandby));
		return;
	}

	//エネミーの送信処理

	if (playerManager->GetMyPlayer()->GetTeamHost() && playerManager->GetMyPlayer()->Getteamnumber() > 0)
	{
		EnemyManager& enemyManager = EnemyManager::Instance();
		std::vector<Enemy*> enemys = enemyManager.GetEnemys();
		for (Enemy* enemy : enemys)
		{
			connection->SendEnemy(enemy);
		}
	}

	elapsedTimeSum += elapsedTime;
	//送信間隔
	sendInterval = 1.0f / sendIntervalCount;
	//プレイヤーが存在してたら
	if (connection && playerManager->GetPlayers().size() > 0)
	{
		//プレイヤーの送信処理
		while (elapsedTimeSum >= sendInterval)
		{
			elapsedTimeSum -= sendInterval;

			if (connection != nullptr && playerManager->GetMyPlayer()->Getteamnumber() > 1000)
			{
				connection->SendMove(
					playerManager->GetMyPlayer()->GetVelocity(),
					playerManager->GetMyPlayer()->GetPosition(),
					playerManager->GetMyPlayer()->GetState(),
					playerManager->GetMyPlayer()->GetAngle());

			}
		}
		//imgui用
		{
			guiPosition = playerManager->GetMyPlayer()->GetPosition();
			guiVelocity = playerManager->GetMyPlayer()->GetVelocity();
			guiId = playerManager->GetMyPlayer()->GetPlayerID();
			guiAngle = playerManager->GetMyPlayer()->GetAngle();
		}
		// カメラコントローラー更新処理
		DirectX::XMFLOAT3 target = playerManager->GetMyPlayer()->GetPosition();
		target.y += 0.5f;// 足元から５０センチぐらい
		cameraController->SetTarget(target);// プレイヤーの腰当たり
		cameraController->Update(elapsedTime);

		// プレイヤー更新処理
		playerManager->Update(elapsedTime);
	}

	// ステージ更新処理
	StageManager::instance().Update(elapsedTime);
	
	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);

	
	//消去リストのIDのプレイヤーを消す
	//connection->DeleteID();
	//プレイヤー消去リストにあるものを消す
	playerManager->DeletePlayer();

}

void SceneGame::PingUpdate(float elapsedTime)
{
	pingTimer += elapsedTime;
	if (pingTimer > 1.0f)
	{
		connection->SendPing();
		pingTimer = 0.0f;
	}
}

void SceneGame::RenderPing(ID3D11DeviceContext* dc)
{
	{
		// g_SpriteManager を使ってスプライトを取得
		Sprite* numberSprite = g_SpriteManager.GetSprite(SpriteNumber::TeamNumber);
		// 画像の長さ
		const float gaugeWidth = 25.0f;
		const float gaugeHeight = 33.0f;

		// pingの桁数を求める
		int numDigits = 1;
		float savePing = playerManager->GetMyPlayer()->GetPing();
		int ping = static_cast<int>(savePing);
		int number = ping;
		while (number >= 10)
		{
			number /= 10;
			numDigits++;
		}

		float positionX = 30;
		float positionY = 460;
		int digit = 0;

		// 各桁を描画するループ
		for (int i = numDigits - 1; i >= 0; --i)
		{
			// 各桁の数値を取得
			digit = (ping / static_cast<int>(pow(10, i))) % 10;

			// スプライトを描画
			Sprite* NumberSprite = g_SpriteManager.GetSprite(SpriteNumber::Number);
			NumberSprite->Render(dc,
				positionX, positionY,
				30, 30,
				gaugeWidth * digit + digit, 0,
				gaugeWidth, gaugeHeight,
				0.0f,
				1, 1, 1, 1);

			// 次の桁の位置に移動
			positionX += 25;
		}
		{
			float sizeX = 50;
			float sizeY = 50;
			Sprite* PointSprite = g_SpriteManager.GetSprite(SpriteNumber::Point);
			PointSprite->Render(dc,
				positionX+2, positionY+20,     //描画位置
				4, 4,                          //表示サイズ
				0,0,                           //切り取りはじめ位置
				sizeX, sizeY,                  //画像サイズ
				0.0f,
				1, 1, 1, 1);

		}
		{
			numDigits = 2;
			ping = static_cast<int>(savePing * 1000);
			// 各桁を描画するループ
			for (int i = numDigits ; i > 0; --i)
			{
				// 各桁の数値を取得
				digit = (ping / static_cast<int>(pow(10, i))) % 10;

				// スプライトを描画
				Sprite* NumberSprite = g_SpriteManager.GetSprite(SpriteNumber::Number);
				NumberSprite->Render(dc,
					positionX, positionY,
					30, 30,
					gaugeWidth * digit + digit, 0,
					gaugeWidth, gaugeHeight,
					0.0f,
					1, 1, 1, 1);

				// 次の桁の位置に移動
				positionX += 25;
			}
		}

	}
	//回線のマーク
	{
		int pingRank = 0;
		float ping = playerManager->GetMyPlayer()->GetPing();
		
		if (ping >= 30.0f)pingRank = 1;
		if (ping >= 50.0f)pingRank = 2;
		if (ping > 100.0f)pingRank = 3;


		float positionX = 10;
		float positionY = 460;
		// 画像の長さ
		const float gaugeWidth = 50.0f;
		const float gaugeHeight = 50.0f;

		Sprite* PingSprite = g_SpriteManager.GetSprite(SpriteNumber::Ping);

		PingSprite->Render(dc,
			positionX, positionY,
			20, 20,
			gaugeWidth*pingRank, 0,
			gaugeWidth, gaugeHeight,
			0.0f,
			1, 1, 1, 1);

		// 次の桁の位置に移動
		positionX += 15;
	}
	
}



// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0〜1.0)
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
		Shader* stageShader = graphics.GetStageShader();
		stageShader->Begin(dc, rc);
		// ステージ描画
		StageManager::instance().Render(dc, stageShader);
		stageShader->End(dc);

		Shader* shader = graphics.GetShader();
		// シェーダーに必要な情報を書く
		shader->Begin(dc, rc);// シェーダーにカメラの情報を渡す

	    // プレイヤー描画
		playerManager->Render(dc, shader);
		// エネミー描画
		EnemyManager::Instance().Render(dc, shader);

		shader->End(dc);

		
	}
	//2D
	RenderPing(dc);

	//サーバーと接続されていたら
	if (!connection->GetIsConectionError())
	{
		if (playerManager->GetMyPlayer()->GetisMouseOperation())
		{
			MouseOpreration(dc);
		}

		//チームのホストがいなくなったら
		if (playerManager->GetTeamDisbabded())
		{
			//ロビーに戻るを表示
			TeamNotHost(dc);

		}
		//設定画面
		OprerationSelect(dc);

		//設定を開いてるか
		if (isSetting)
		{
			//ログアウトボタン表示
			Logout(dc);
		}
		
		//操作を切っていたら(その間はゲーム外)
		if (!playerManager->GetMyPlayer()->Getoperation())
		{
			RenderGameClear(dc, screenWidth, screenHeight);
		}

	}
	else
	{
		//サーバーとの接続が切れたら
		connection->ConnectionCheck(dc);
	}

	// 3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3Dデバッグ描画
	//{
		//IMGUI描画
		ImGui::SetNextWindowPos(ImVec2(900, 10), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_FirstUseEver);
		// beginからendまでの内容が出来る
		EnemyManager& enemyManager = EnemyManager::Instance();
		std::vector<Enemy*> enemys = enemyManager.GetEnemys();
		std::vector<Player*> players = SceneManager::Instance().GetPlayerManager()->GetPlayers();

		if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
		{
			ImGui::SliderInt("sendInterval", &sendIntervalCount, 1, 60);
			for (Player* player : players)
			{
				//ImGui::Text("PlayerName: %s, PlayerHP: %d", player->GetName(), player->GetHealth());
				//ImGui::Text("PlayerName: %s", player->GetName());
			}

			//ImGui::Text("ms: %f", playerManager->GetMyPlayer()->GetPing());
			//ImGui::Text("UdpRecvID: %d", playerManager->GetudpRecvId());
			//ImGui::Text("UdpRecvSize: %d", playerManager->GetRecvSize());
		
			//ImGui::Text("Disbanded: %d", playerManager->GetTeamDisbabded());
			//ImGui::Text("LoginCount: %d", playerManager->GetLoginCount());
			//ImGui::Text("PlayersSize: %d", playerManager->GetPlayers().size());
			for (Enemy* enemy : enemys)
			{
				ImGui::Text("EnemyHP: %d, EnemyState: %d", enemy->GetHealth(), enemy->GetState());
			}
			ImGui::Text("Host: %s", playerManager->GetMyPlayer()->GetTeamHost() ? "true" : "false");
			
			//ImGui::Text("State: %d", static_cast<int>(playerManager->GetMyPlayer()->GetState()));
			 //トランスフォーム
			//if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			//{
			//	ImGui::Text("ID: %d", playerManager->GetMyPlayerID());
			//	ImGui::Text("TeamNumber: %d", playerManager->GetMyPlayer()->Getteamnumber());
			//	
			//	if (ImGui::Button("Change Operation"))
			//	{
			//		sendFlag = !sendFlag;
			//	}
			//	if (sendFlag)
			//	{
			//		ImGui::Text("true");
			//	}
			//	else
			//	{
			//		ImGui::Text("false");
			//	}
			//	//ImGui::InputFloat3("GUIVelocity", &guiVelocity.x);
			//	//ImGui::InputFloat3("RecvVelocity", &guiRecvVelocity.x);
			//	//ImGui::InputFloat3("GUIPosition", &guiPosition.x);
			//
			//	ImGui::InputInt4("TeamsID", guiTeamsId);
			//}
		}
		ImGui::End();

	//	// 2Dスプライト描画
	//	{
	//		if (playerManager->GetPlayers().size() > 0)
	//		{
	//			//RenderTimer(dc, int(timer));
	//			//名前があるなら表示
	//			if (playerManager->GetMyPlayer()->GetName()[0] == '\0')
	//			{
	//				RenderNumber(dc, rc.view, rc.projection);
	//			}
	//			else
	//			{
	//				RenderName(dc, rc.view, rc.projection);
	//			}
	//		}
	//	}
	//	if (connection && playerManager->GetPlayers().size() > 0)
	//	{
	//		// 当たり判定の形をうつ
	//		// プレイヤーデバッグプリミティブ描画
	//		//playerManager->GetMyPlayer()->DrawDebugPrimitive();
	//		// エネミーデバッグプリミティブ描画
	//		//EnemyManager::Instance().DrawDebugPrimitive();
	//		// ラインレンダラ描画実行
	//		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);
	//		// 実際の当たり判定描画
	//		// デバッグレンダラ描画実行
	//		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	//		
	//		// 2DデバッグGUI描画
	//		{
	//			// プレイヤーデバッグ描画
	//			//player->DrawDebugGUI();
	//			//cameraController->DrawDebugGUI();
	//			// 
	//			//EnemyManager::Instance().Register(enemySlime);
	//			//EnemyManager::Instance().DrawDebugGUI();
	//			//cameraController->DrawDebugGUI();
	//		}
	//	}
	//}
	

	
}

void SceneGame::MouseOpreration(ID3D11DeviceContext* dc)
{
	HWND hwnd;
	hwnd = GetActiveWindow();
	if (hwnd != NULL)
	{
		Mouse& mouse = Input::Instance().GetMouse();
		//押してたら
		if (mouse.GetButton() & Mouse::BTN_LEFT)
		{
			oldMousePos.x = static_cast<float>(mouse.GetOldPositionX());
			oldMousePos.y = static_cast<float>(mouse.GetOldPositionY());
		}
		else
		{
			length = 0.0f;
			playerManager->GetMyPlayer()->mouselength = 0.0f;

			const DirectX::XMFLOAT2 defaultpos = { 880,340 };
			const float adjustmentsize = 35;
			Sprite* BigCircleSprite = g_SpriteManager.GetSprite(SpriteNumber::BigCircle);
			BigCircleSprite->Render(dc,
				defaultpos.x, defaultpos.y, //描画位置
				100, 100,             //表示サイズ
				0, 0,                 //切り取りはじめ位置
				300, 300,           //画像サイズ
				0.0f,
				1, 1, 1, 1);
			Sprite* SmallCircleSprite = g_SpriteManager.GetSprite(SpriteNumber::SmallCircle);
			SmallCircleSprite->Render(dc,
				defaultpos.x+ adjustmentsize, defaultpos.y+ adjustmentsize, //描画位置
				30, 30,             //表示サイズ
				0, 0,                 //切り取りはじめ位置
				100, 100,           //画像サイズ
				0.0f,
				1, 1, 1, 1);
			return;
		}
		//クリック時
		if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
		{
			clickPos.x = static_cast<float>(mouse.GetPositionX());
			clickPos.y = static_cast<float>(mouse.GetPositionY());
		}

		DirectX::XMVECTOR vec, OldPos, Pos;
		OldPos = XMLoadFloat3(&oldMousePos);
		Pos = XMLoadFloat3(&clickPos);
		vec = XMVectorSubtract(Pos, OldPos);

		length = XMVectorGetX(XMVector3Length(vec));
		float deltaX = XMVectorGetX(vec);
		float deltaY = XMVectorGetY(vec);


		mouseAngle = atan2(deltaY, deltaX);
		playerAngle = mouseAngle;
		// ラジアンから度に変換
		mouseAngleDegrees = XMConvertToDegrees(mouseAngle);
		playerAngleDegrees = mouseAngleDegrees;

		// 0度を上方向にするために90度を追加
		playerAngleDegrees = 90.0f - playerAngleDegrees;

		// 角度を0-360度の範囲に調整
		if (playerAngleDegrees < 0) {
			playerAngleDegrees += 360.0f;
		}
		if (mouseAngleDegrees < 0) {
			mouseAngleDegrees += 360.0f;
		}

		//逆回転のため補正
		playerAngleDegrees -= 360;
		playerAngleDegrees *= -1;

		//角度を度からラジアンに戻す
		mouseAngle = XMConvertToRadians(mouseAngleDegrees);
		playerAngle = XMConvertToRadians(playerAngleDegrees);

		if (length < 0.2f) {
			playerAngle = playerManager->GetMyPlayer()->GetAngle().y;
		}


		playerManager->GetMyPlayer()->mouseAngle = mouseAngle;
		playerManager->GetMyPlayer()->SetAngle({ 0, playerAngle, 0 });

		//描画
		{
			Sprite* BigCircleSprite = g_SpriteManager.GetSprite(SpriteNumber::BigCircle);
			BigCircleSprite->Render(dc,
				clickPos.x - 50, clickPos.y - 50, //描画位置
				100, 100,             //表示サイズ
				0, 0,                 //切り取りはじめ位置
				300, 300,           //画像サイズ
				0.0f,
				1, 1, 1, 1);

			//小さい円が大きい円から出ないように補正
			if (length > 50.0f)
			{
				float scaleFactor = 50.0f / length;
				vec = DirectX::XMVectorScale(vec, scaleFactor);
				DirectX::XMFLOAT3 correctedOldPos;
				DirectX::XMStoreFloat3(&correctedOldPos, DirectX::XMVectorSubtract(Pos, vec));
				oldMousePos.x = correctedOldPos.x;
				oldMousePos.y = correctedOldPos.y;

				length = 50.0f;

			}

			//補正後を代入
			playerManager->GetMyPlayer()->mouselength = length;

			Sprite* SmallCircleSprite = g_SpriteManager.GetSprite(SpriteNumber::SmallCircle);
			SmallCircleSprite->Render(dc,
				oldMousePos.x - 15, oldMousePos.y - 15, //描画位置
				30, 30,             //表示サイズ
				0, 0,                 //切り取りはじめ位置
				100, 100,           //画像サイズ
				0.0f,
				1, 1, 1, 1);
		}
	}
	
}

void SceneGame::OprerationSelect(ID3D11DeviceContext* dc)
{
	//設定ボタン表示
	{
		DirectX::XMFLOAT2 size, pos;
		pos = { 10,10 };
		size = { 30,30 };

		Sprite* SettingSprite = g_SpriteManager.GetSprite(SpriteNumber::Setting);
		SettingSprite->Render(dc,
			pos.x, pos.y,  //描画位置
			size.x, size.y,  //表示サイズ
			0, 0,      //切り取りはじめ位置
			256, 256, //画像サイズ
			0.0f,
			1, 1, 1, 1);

		if (Uiclick(pos, size))
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
			if (Uiclick(WASDPos, size))
			{
				playerManager->GetMyPlayer()->SetisMouseOperation(false);
			}

			edgePos = mousePos;
		}
		else
		{
			if (Uiclick(mousePos, size))
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

void SceneGame::Logout(ID3D11DeviceContext* dc)
{
	DirectX::XMFLOAT2 size, pos;
	size = { 100,50 };
	pos = { 25,150 };
	//選択している縁描画
	Sprite* LogoutSprite = g_SpriteManager.GetSprite(SpriteNumber::Logout);
	LogoutSprite->Render(dc,
		pos.x, pos.y,  //描画位置
		size.x , size.y ,  //表示サイズ
		0, 0,              //切り取りはじめ位置
		500,200,           //元の画像サイズ
		0.0f,
		1, 1, 1, 1);

	if (Uiclick(pos, size))
	{
		if (playerManager->GetMyPlayer()->Getteamnumber() > 0)
		{
			connection->SendTeamLeave(playerManager->GetMyPlayer()->GetTeamHost());
			Sleep(10);
		}
		connection->SendLogout();
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
	}

}

void SceneGame::TeamNotHost(ID3D11DeviceContext* dc)
{
	Graphics& graphics = Graphics::Instance();
	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());
	DirectX::XMFLOAT2 size, pos;
	size = { screenWidth,screenHeight };
	pos = {0,0 };
	//選択している縁描画
	Sprite* TeamDisbandedSprite = g_SpriteManager.GetSprite(SpriteNumber::TeamDisbanded);
	TeamDisbandedSprite->Render(dc,
		pos.x, pos.y,        //描画位置
		size.x, size.y,      //表示サイズ
		0, 0,                //切り取りはじめ位置
		750, 500,            //元の画像サイズ
		0.0f,
		1, 1, 1, 1);

	Mouse& mouse = Input::Instance().GetMouse();
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		playerManager->SetTeamDisbabded(false);
		playerManager->GetMyPlayer()->ResetTeamsid();

		//playerManager->ResetGenerateCount();
		//playerManager->ResetLoginCount();
		//ロビーに戻る
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneStandby));
	}
}

bool SceneGame::Uiclick(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size)
{
	DirectX::XMFLOAT3 scereenPosition;

	Mouse& mouse = Input::Instance().GetMouse();
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		scereenPosition.x = static_cast<float>(mouse.GetPositionX());
		scereenPosition.y = static_cast<float>(mouse.GetPositionY());

		if (pos.x < scereenPosition.x && scereenPosition.x < pos.x + size.x
			&& pos.y < scereenPosition.y && scereenPosition.y < pos.y + size.y)
		{
			Sleep(10);
			return true;
		}
	}
	return false;
}

// プレイヤーID描画
void SceneGame::RenderNumber(ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection
)
{
	// スクリーン座標
	DirectX::XMFLOAT3 scereenPosition;
	PlayerOverheadPos(dc, view, projection, scereenPosition);

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
		float numberposX = scereenPosition.x-10;
		float numberposY = scereenPosition.y-10;
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

void SceneGame::RenderTimer(ID3D11DeviceContext* dc, int timer)
{
	// 画像の長さ
	const float gaugeWidth = 25.0f;
	const float gaugeHeight = 33.0f;
	int ID = timer;

	//桁数を求める
	int numDigits = 1;
	int tempID = ID;
	while (tempID >= 10)
	{
		tempID /= 10;
		numDigits++;
	}

	// 2Dスプライト描画
	{
		float positionX = 600;
		float positionY =20;
		int digit = 0;

		// 各桁を描画するループ
		for (int i = numDigits - 1; i >= 0; --i)
		{
			// 各桁の数値を取得
			digit = (ID / static_cast<int>(pow(10, i))) % 10;

			// スプライトを描画
			Sprite* NumberSprite = g_SpriteManager.GetSprite(SpriteNumber::Number);
			NumberSprite->Render(dc,
				positionX, positionY,
				15, 15,
				gaugeWidth * digit,0,
				gaugeWidth, gaugeHeight,
				0.0f,
				1, 1, 1, 1);

			// 次の桁の位置に移動
			positionX += 15;
		}
	}
}

void SceneGame::RenderName(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	
	// スクリーン座標
	DirectX::XMFLOAT3 scereenPosition;
	PlayerOverheadPos(dc, view, projection, scereenPosition);


	float positionX = scereenPosition.x - 30;
	float positionY = scereenPosition.y - 20;

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
			positionX, positionY,          //描画位置
			24, 24,                        //表示サイズ
			sizeX * width, sizeY * height, //切り取りはじめ位置
			sizeX, sizeY,                  //画像サイズ
			0.0f,
			1, 1, 1, 0.5);

		positionX += sizeX / 2 - 5;
	}
}

void SceneGame::RenderGameClear(ID3D11DeviceContext* dc, float screenWidth, float screenHeight)
{
	Sprite* clearSprite = g_SpriteManager.GetSprite(SpriteNumber::Gameclear);

	float textureWidth = static_cast<float>(clearSprite->GetTextureWidth());
	float textureHeight = static_cast<float>(clearSprite->GetTextureHeight());

	clearSprite->Render(dc,
		0,0,
		screenWidth, screenHeight,
		0, 0,
		textureWidth, textureHeight,
		0.0f,
		1, 1, 1, 1);

	Sprite* okSprite = g_SpriteManager.GetSprite(SpriteNumber::OK);
	textureWidth  = static_cast<float>(okSprite->GetTextureWidth());
	textureHeight = static_cast<float>(okSprite->GetTextureHeight());


	DirectX::XMFLOAT2 size,position;

	size.x = 200;
	size.y = 100;
	position.x = screenWidth / 2 - (size.x / 2);
	position.y = screenHeight / 2;

	okSprite->Render(dc,
		position.x,position.y,
		size.x,size.y,
		0, 0,
		textureWidth, textureHeight,
		0.0f,
		1, 1, 1, 1);

	//OKをクリックしたら
	if (Uiclick(position, size))
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneStandby));
	}
}

void SceneGame::PlayerOverheadPos(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, DirectX::XMFLOAT3& scereenPosition)
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
	DirectX::XMStoreFloat3(&scereenPosition, ScreenPosition);
}


