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

	void PingUpdate(float elapsedTime);

	void RenderPing(ID3D11DeviceContext* dc);

	// �`�揈��
	void Render() override;

	void MouseOpreration(ID3D11DeviceContext* dc);

	void OprerationSelect(ID3D11DeviceContext* dc);
	void Logout(ID3D11DeviceContext* dc);
	//�`�[���z�X�g�����Ȃ��Ȃ�����
	void TeamNotHost(ID3D11DeviceContext* dc);
	
	bool Uiclick(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size);
private:
	DirectX::XMFLOAT3 clickPos = {};
	DirectX::XMFLOAT3 oldMousePos = {};
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



	//�������牺�ǉ�
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

	int sendIntervalCount = 60;      // ���[�U�[���w�肷�� 1�b�Ԃ̑��M�� (1�`60)
	float elapsedTimeSum = 0.0f;// �o�ߎ��Ԃ̍��v
	float sendInterval = 0;  // ���M�Ԋu
};
