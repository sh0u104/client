#pragma once


#include "Player.h"
#include "CameraController.h"
#include "Scene.h"
#include "Graphics/Sprite.h"

//サーバーとの通信
#include "Connection.h"
#include "PlayerManager.h"
// ゲームシーン
class SceneGame : public Scene
{
public:
	SceneGame() {}
	~SceneGame() override{}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	void PingUpdate(float elapsedTime);

	void RenderPing(ID3D11DeviceContext* dc);

	// 描画処理
	void Render() override;

	void MouseOpreration(ID3D11DeviceContext* dc);

	void OprerationSelect(ID3D11DeviceContext* dc);
	void Logout(ID3D11DeviceContext* dc);
	//チームホストがいなくなったら
	void TeamNotHost(ID3D11DeviceContext* dc);
	
	bool Uiclick(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size);
private:
	DirectX::XMFLOAT3 clickPos = {};
	DirectX::XMFLOAT3 oldMousePos = {};
	float mouseAngle = 0.0f;
	float playerAngle = 0.0f;
	float length = 0.0f;

	// ラジアンから度に変換
	float mouseAngleDegrees = 0.0f;
	float playerAngleDegrees = 0.0f;

	// エネミーHPゲージ描画
	void RenderNumber(
	ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection
	);
	void RenderTimer(ID3D11DeviceContext* dc,int timer);

	void RenderName(ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection);

	void RenderGameClear(ID3D11DeviceContext* dc,float screenWidth,float screenHeight);

private:
	Player* player = nullptr;
	CameraController* cameraController = nullptr;

	//enum class SpriteNumber : int
	//{
	//	Number,
	//	BigCircle,
	//	SmallCircle,
	//	SelectEdge,
	//	Mouse,
	//	WASD,
	//	Setting,
	//	Name,
	//	Logout,
	//	TeamDisbanded,
	//	Max
	//};
	//std::unique_ptr<Sprite> sprites[static_cast<int>(SpriteNumber::Max)] = {};



	//ここから下追加
	Connection* connection = nullptr;
	PlayerManager* playerManager = nullptr;

	DirectX::XMFLOAT3 guiPosition = {};
	DirectX::XMFLOAT3 guiVelocity = {};
	DirectX::XMFLOAT3 guiRecvVelocity = {};
	DirectX::XMFLOAT3 guiAngle = {};

	int guiId = 0;

	enum class State
	{
		Idle,
		Move,
		Jump,
		Land,
		JumpFlip,
		Attack,
		Damage,
		Death,
		Revive
	};

	int guiTeamsId[4] = {};

	float timer = 10.0;

	bool isSetting = false;

	int enemyCount = 0;

	float pingTimer = 0.0f;

	int sendIntervalCount = 60;      // ユーザーが指定する 1秒間の送信回数 (1～60)
	float elapsedTimeSum = 0.0f;// 経過時間の合計
	float sendInterval = 0;  // 送信間隔
};
