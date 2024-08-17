#pragma once


#include "Player.h"
#include "CameraController.h"
#include "Scene.h"
#include "Graphics/Sprite.h"

//�T�[�o�[�Ƃ̒ʐM
#include "Connection.h"
#include "PlayerManager.h"
// �Q�[���V�[��
class SceneGame : public Scene
{
public:
	SceneGame() {}
	~SceneGame() override{}

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;

	void MouseOpreration(ID3D11DeviceContext* dc);
	
private:
	DirectX::XMFLOAT3 clickPos;
	DirectX::XMFLOAT3 oldMousePos;
	float mouseAngle = 0.0f;
	float playerAngle = 0.0f;
	float length = 0.0f;

	// ���W�A������x�ɕϊ�
	float mouseAngleDegrees = 0.0f;
	float playerAngleDegrees = 0.0f;

	// �G�l�~�[HP�Q�[�W�`��
	void RenderNumber(
	ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection
	);
	void RenderTimer(ID3D11DeviceContext* dc,int timer);



private:
	Player* player = nullptr;
	CameraController* cameraController = nullptr;

	enum class SpriteNumber : int
	{
		Number,
		BigCircle,
		SmallCircle,
		Max
	};
	std::unique_ptr<Sprite> sprites[static_cast<int>(SpriteNumber::Max)] = {};



	//�������牺�ǉ�
	Connection* connection = nullptr;
	PlayerManager* playerManager = nullptr;

	DirectX::XMFLOAT3 guiPosition;
	DirectX::XMFLOAT3 guiVelocity;
	DirectX::XMFLOAT3 guiRecvVelocity;
	DirectX::XMFLOAT3 guiAngle;


	bool  sendFlag = true;
	int guiId;

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

	int guiTeamsId[4];

	float timer = 10.0;
};
