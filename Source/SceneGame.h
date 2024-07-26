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

	void mouse(ID3D11DeviceContext* dc);
	
private:
	DirectX::XMFLOAT3 clickpos;
	DirectX::XMFLOAT3 oldmousepos;
	float mouseangle,playerangle;
	float length;

	// ���W�A������x�ɕϊ�
	float mouseangleDegrees,playerangleDegrees;

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

	enum class Spritenumber : int
	{
		Number,
		BigCircle,
		SmallCircle,
		Max
	};
	std::unique_ptr<Sprite> sprites[static_cast<int>(Spritenumber::Max)] = {};



	//�������牺�ǉ�
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
