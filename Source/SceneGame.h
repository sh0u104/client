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


	
private:
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

	Sprite* spriteNumber = nullptr;



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
