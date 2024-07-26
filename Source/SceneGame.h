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

	// 描画処理
	void Render() override;

	void mouse(ID3D11DeviceContext* dc);
	
private:
	DirectX::XMFLOAT3 clickpos;
	DirectX::XMFLOAT3 oldmousepos;
	float mouseangle,playerangle;
	float length;

	// ラジアンから度に変換
	float mouseangleDegrees,playerangleDegrees;

	// エネミーHPゲージ描画
	void RenderNumber(
	ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection
	);
	void RenderTimer(ID3D11DeviceContext* dc,int timer);



private:
	Player* player = nullptr;
	CameraController* cameraController = nullptr;

	enum class Spritenumber : int
	{
		Number,
		BigCircle,
		SmallCircle,
		Max
	};
	std::unique_ptr<Sprite> sprites[static_cast<int>(Spritenumber::Max)] = {};



	//ここから下追加
	Connection* connection = nullptr;
	PlayerManager* playerManager = nullptr;

	DirectX::XMFLOAT3 guiposition;
	DirectX::XMFLOAT3 guivelocity;
	DirectX::XMFLOAT3 guirecvvelocity;
	DirectX::XMFLOAT3 guiangle;


	bool  SendFlag = true;

	int guiID;

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

	int guiteamsid[4];

	float timer = 10.0;
};
