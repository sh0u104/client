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
// 初期化
void SceneGame::Initialize()
{
	// ステージ初期化1
	StageManager& stageManager = StageManager::instance();
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);

	//StageMoveFloor* stageMoveFloor = new StageMoveFloor();
	//stageMoveFloor->SetStartPosint(DirectX::XMFLOAT3(0, 1, 3));
	//stageMoveFloor->SetGoalPoint(DirectX::XMFLOAT3(10, 2, 3));
	//stageMoveFloor->SetTorque(DirectX::XMFLOAT3(0, 1.0f, 0));
	//stageManager.Register(stageMoveFloor);

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

    //スプライト
	{
		//sprites[static_cast<int>(Spritenumber:)] = std::make_unique<Sprite>("Data/Sprite/");
		//数字
		sprites[static_cast<int>(SpriteNumber::Number)] = std::make_unique<Sprite>("Data/Sprite/number.png");

		sprites[static_cast<int>(SpriteNumber::BigCircle)] = std::make_unique<Sprite>("Data/Sprite/bigcircle.png");

		sprites[static_cast<int>(SpriteNumber::SmallCircle)] = std::make_unique<Sprite>("Data/Sprite/smallcircle.png");

	}

	// プレイヤー初期化
	//player = new Player();

	// プレイヤーマネジャー初期化
	//playerManager = new PlayerManager();
	playerManager = SceneManager::Instance().GetPlayerManager();

	for(int i=0;i<playerManager->GetPlayers().size();++i)
	{
		int MYID = playerManager->GetMyPlayer()->Getteamsid(i);
		playerManager->GetPlayer(MYID)->SetAngle({ 0.0f,0.0f,0.0f });
		playerManager->GetPlayer(MYID)->SetPosition({ 1.0f * i,0.0f,0.0f });
	}

	//通信初期化
	//connection = new Connection();
	//connection->Initialize();
	//connection->SetplayerManager(playerManager);

	connection = SceneManager::Instance().GetConnection();
	

	// エネミー初期化
	//EnemyManager& enemyManager = EnemyManager::Instance();
	//for (int i = 0; i < 1; ++i)
	//{
	//	EnemySlime* slime = new EnemySlime();
	//	slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
	//	// 縄張り
	//	slime->SetTerritory(slime->GetPosition(), 10.0f);
	//	slime->IsEnemy();
	//	enemyManager.Register(slime);
	//
	//}
	

	//imgui用
	for (int i = 0; i < 3; ++i)
	{
		guiTeamsId[i] = playerManager->GetMyPlayer()->Getteamsid(i);
	}
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
	
	//timer -= elapsedTime;
	if (timer < 0)
	{
		//ゲーム終了を送る
		connection->SendGameEnd(playerManager->GetMyPlayer()->Getteamnumber());
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneStandby));
	}

	if (connection&& playerManager->GetPlayers().size()>0)
	{
		if (sendFlag)
		{
			if (!playerManager->GetMyPlayer()->Getoperation())
			{
				playerManager->GetMyPlayer()->Setoperation(true);
			}

			if (connection != nullptr)
			{
				connection->SendMove(
					playerManager->GetMyPlayer()->GetVelocity(),
					playerManager->GetMyPlayer()->GetPosition(),
					playerManager->GetMyPlayer()->GetState(),
					playerManager->GetMyPlayer()->GetAngle());

			}
		}
		//操作停止時
		else if (playerManager->GetMyPlayer()->Getoperation())
		{
			playerManager->GetMyPlayer()->SetRecvVelocity({ 0.0f,0.0f,0.0f });
			playerManager->GetMyPlayer()->Setoperation(false);
		}

		//imgui用
		guiPosition = playerManager->GetMyPlayer()->GetPosition();
		guiVelocity = playerManager->GetMyPlayer()->GetVelocity();
		guiRecvVelocity = playerManager->GetMyPlayer()->GetRecvVelocity();
		guiId = playerManager->GetMyPlayer()->GetPlayerID();
		guiAngle = playerManager->GetMyPlayer()->GetAngle();

		// カメラコントローラー更新処理
		DirectX::XMFLOAT3 target = playerManager->GetMyPlayer()->GetPosition();
		target.y += 0.5f;// 足元から５０センチぐらい
		cameraController->SetTarget(target);// プレイヤーの腰当たり
		cameraController->Update(elapsedTime);
		// プレイヤー更新処理
		//player->Update(elapsedTime);
		playerManager->Update(elapsedTime);
	}
	// ステージ更新処理
	StageManager::instance().Update(elapsedTime);
	
	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);
}



// 描画処理
void SceneGame::Render()
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


	//// ビュー行列
	//{
	//	DirectX::XMFLOAT3 eye = { 0, 10, -10 };	// カメラの視点（位置）
	//	DirectX::XMFLOAT3 focus = { 0, 0, 0 };	// カメラの注視点（ターゲット）
	//	DirectX::XMFLOAT3 up = { 0, 1, 0 };		// カメラの上方向

	//	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	//	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	//	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	//	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	//	DirectX::XMStoreFloat4x4(&rc.view, View);
	//}
	//// プロジェクション行列
	//{
	//	float fovY = DirectX::XMConvertToRadians(45);	// 視野角
	//	float aspectRatio = graphics.GetScreenWidth() / graphics.GetScreenHeight();	// 画面縦横比率
	//	float nearZ = 0.1f;	// カメラが映し出すの最近距離
	//	float farZ = 1000.0f;	// カメラが映し出すの最遠距離
	//	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);
	//	DirectX::XMStoreFloat4x4(&rc.projection, Projection);
	//}
	
	// 3Dモデル描画
	{
		Shader* shader = graphics.GetShader();
		// シェーダーに必要な情報を書く
		shader->Begin(dc, rc);// シェーダーにカメラの情報を渡す
		// ステージ描画
		StageManager::instance().Render(dc, shader);
	// プレイヤー描画
		//player->Render(dc, shader);
		playerManager->Render(dc, shader);
		// エネミー描画
		EnemyManager::Instance().Render(dc, shader);

		shader->End(dc);
	}
	MouseOpreration(dc);

	// 3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3Dデバッグ描画
	{
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
		// beginからendまでの内容が出来る
		if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
		{
			if (ImGui::CollapsingHeader("Mouse", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::InputFloat("length", &length);
				ImGui::InputFloat3("clickpos", &clickPos.x);
				ImGui::InputFloat3("pos", &oldMousePos.x);
				ImGui::Text("angle : %.2f", &playerManager->GetMyPlayer()->GetAngle().y);
				ImGui::InputFloat("angle", &mouseAngle);
				ImGui::InputFloat("angleDegrees", &mouseAngleDegrees);
			}
			// トランスフォーム
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Text("ID: %d", playerManager->GetMyPlayerID());
				ImGui::Text("TeamNumber: %d", playerManager->GetMyPlayer()->Getteamnumber());
				
				if (ImGui::Button("Change Operation"))
				{
					sendFlag = !sendFlag;
				}
				if (sendFlag)
				{
					ImGui::Text("true");
				}
				else
				{
					ImGui::Text("false");
				}
				ImGui::InputFloat3("GUIVelocity", &guiVelocity.x);
				ImGui::InputFloat3("RecvVelocity", &guiRecvVelocity.x);
				ImGui::InputFloat3("GUIPosition", &guiPosition.x);

				ImGui::InputInt4("TeamsID", guiTeamsId);
			}
		}
		ImGui::End();

		// 2Dスプライト描画
		{
			if (playerManager->GetPlayers().size() > 0)
			{
				RenderNumber(dc, rc.view, rc.projection);
				RenderTimer(dc, int(timer));
			}
		}


		if (connection && playerManager->GetPlayers().size() > 0)
		{
			// 当たり判定の形をうつ
			// プレイヤーデバッグプリミティブ描画
			playerManager->GetMyPlayer()->DrawDebugPrimitive();

			// エネミーデバッグプリミティブ描画
			EnemyManager::Instance().DrawDebugPrimitive();

			// ラインレンダラ描画実行
			graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

			// 実際の当たり判定描画
			// デバッグレンダラ描画実行
			graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);


			
			// 2DデバッグGUI描画
			{
				// プレイヤーデバッグ描画
				//player->DrawDebugGUI();
				//cameraController->DrawDebugGUI();
				// 
				//EnemyManager::Instance().Register(enemySlime);
				//EnemyManager::Instance().DrawDebugGUI();
				cameraController->DrawDebugGUI();
			}
		}
	}
	

	
}

void SceneGame::MouseOpreration(ID3D11DeviceContext* dc)
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
		playerManager->GetMyPlayer()->mouselength = length;
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
	

	
	
	sprites[static_cast<int>(SpriteNumber::BigCircle)]->Render(dc,
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
	playerManager->GetMyPlayer()->mouselength = length;
	sprites[static_cast<int>(SpriteNumber::SmallCircle)]->Render(dc,
		oldMousePos.x - 15, oldMousePos.y - 15, //描画位置
		30, 30,             //表示サイズ
		0, 0,                 //切り取りはじめ位置
		100, 100,           //画像サイズ
		0.0f,
		1, 1, 1, 1);
}

// プレイヤーID描画
void SceneGame::RenderNumber(ID3D11DeviceContext* dc,
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
		float numberposX = scereenPosition.x-10;
		float numberposY = scereenPosition.y-10;
		int digit = 0;

		// 各桁を描画するループ
		for (int i = numDigits - 1; i >= 0; --i)
		{
			// 各桁の数値を取得
			digit = (ID / static_cast<int>(pow(10, i))) % 10;

			// スプライトを描画
			sprites[static_cast<int>(SpriteNumber::Number)]->Render(dc,
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

	// プレイヤーIDの桁数を求める
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
			sprites[static_cast<int>(SpriteNumber::Number)]->Render(dc,
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


