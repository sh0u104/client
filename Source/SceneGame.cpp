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
// ������
void SceneGame::Initialize()
{
	// �X�e�[�W������1
	StageManager& stageManager = StageManager::instance();
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);

	//StageMoveFloor* stageMoveFloor = new StageMoveFloor();
	//stageMoveFloor->SetStartPosint(DirectX::XMFLOAT3(0, 1, 3));
	//stageMoveFloor->SetGoalPoint(DirectX::XMFLOAT3(10, 2, 3));
	//stageMoveFloor->SetTorque(DirectX::XMFLOAT3(0, 1.0f, 0));
	//stageManager.Register(stageMoveFloor);

	// �J���������ݒ� ������ʒu�ǂ���������̂Ȃ�
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
		
	);
	// �ǂ͈̔͂��ǂꂾ�����邩���s�܂�
	camera.SetPerspedtiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);
	// �J�����R���g���[���[������
	cameraController = new CameraController();

    //�X�v���C�g
	{
		//sprites[static_cast<int>(Spritenumber:)] = std::make_unique<Sprite>("Data/Sprite/");
		//����
		sprites[static_cast<int>(SpriteNumber::Number)] = std::make_unique<Sprite>("Data/Sprite/number.png");

		sprites[static_cast<int>(SpriteNumber::BigCircle)] = std::make_unique<Sprite>("Data/Sprite/bigcircle.png");

		sprites[static_cast<int>(SpriteNumber::SmallCircle)] = std::make_unique<Sprite>("Data/Sprite/smallcircle.png");

	}

	// �v���C���[������
	//player = new Player();

	// �v���C���[�}�l�W���[������
	//playerManager = new PlayerManager();
	playerManager = SceneManager::Instance().GetPlayerManager();

	for(int i=0;i<playerManager->GetPlayers().size();++i)
	{
		int MYID = playerManager->GetMyPlayer()->Getteamsid(i);
		playerManager->GetPlayer(MYID)->SetAngle({ 0.0f,0.0f,0.0f });
		playerManager->GetPlayer(MYID)->SetPosition({ 1.0f * i,0.0f,0.0f });
	}

	//�ʐM������
	//connection = new Connection();
	//connection->Initialize();
	//connection->SetplayerManager(playerManager);

	connection = SceneManager::Instance().GetConnection();
	

	// �G�l�~�[������
	//EnemyManager& enemyManager = EnemyManager::Instance();
	//for (int i = 0; i < 1; ++i)
	//{
	//	EnemySlime* slime = new EnemySlime();
	//	slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
	//	// �꒣��
	//	slime->SetTerritory(slime->GetPosition(), 10.0f);
	//	slime->IsEnemy();
	//	enemyManager.Register(slime);
	//
	//}
	

	//imgui�p
	for (int i = 0; i < 3; ++i)
	{
		guiTeamsId[i] = playerManager->GetMyPlayer()->Getteamsid(i);
	}
}

// �I����
void SceneGame::Finalize()
{

	// �G�l�~�[�I����
	EnemyManager::Instance().Clear();


	// �J�����R���g�[���[�I����
	if (this->cameraController)
	{
		delete cameraController;
		cameraController = nullptr;
	}

	// �X�e�[�W�I����
	StageManager::instance().Clear();
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
	
	//timer -= elapsedTime;
	if (timer < 0)
	{
		//�Q�[���I���𑗂�
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
		//�����~��
		else if (playerManager->GetMyPlayer()->Getoperation())
		{
			playerManager->GetMyPlayer()->SetRecvVelocity({ 0.0f,0.0f,0.0f });
			playerManager->GetMyPlayer()->Setoperation(false);
		}

		//imgui�p
		guiPosition = playerManager->GetMyPlayer()->GetPosition();
		guiVelocity = playerManager->GetMyPlayer()->GetVelocity();
		guiRecvVelocity = playerManager->GetMyPlayer()->GetRecvVelocity();
		guiId = playerManager->GetMyPlayer()->GetPlayerID();
		guiAngle = playerManager->GetMyPlayer()->GetAngle();

		// �J�����R���g���[���[�X�V����
		DirectX::XMFLOAT3 target = playerManager->GetMyPlayer()->GetPosition();
		target.y += 0.5f;// ��������T�O�Z���`���炢
		cameraController->SetTarget(target);// �v���C���[�̍�������
		cameraController->Update(elapsedTime);
		// �v���C���[�X�V����
		//player->Update(elapsedTime);
		playerManager->Update(elapsedTime);
	}
	// �X�e�[�W�X�V����
	StageManager::instance().Update(elapsedTime);
	
	// �G�l�~�[�X�V����
	EnemyManager::Instance().Update(elapsedTime);

	// �G�t�F�N�g�X�V����
	EffectManager::Instance().Update(elapsedTime);
}



// �`�揈��
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// �`�揈�� 
	RenderContext rc;// �`�悷�邽�߂ɕK�v�ȏ����܂Ƃ߂��\����
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ���C�g�����i�������j

	// �J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();


	//// �r���[�s��
	//{
	//	DirectX::XMFLOAT3 eye = { 0, 10, -10 };	// �J�����̎��_�i�ʒu�j
	//	DirectX::XMFLOAT3 focus = { 0, 0, 0 };	// �J�����̒����_�i�^�[�Q�b�g�j
	//	DirectX::XMFLOAT3 up = { 0, 1, 0 };		// �J�����̏����

	//	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	//	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	//	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	//	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	//	DirectX::XMStoreFloat4x4(&rc.view, View);
	//}
	//// �v���W�F�N�V�����s��
	//{
	//	float fovY = DirectX::XMConvertToRadians(45);	// ����p
	//	float aspectRatio = graphics.GetScreenWidth() / graphics.GetScreenHeight();	// ��ʏc���䗦
	//	float nearZ = 0.1f;	// �J�������f���o���̍ŋߋ���
	//	float farZ = 1000.0f;	// �J�������f���o���̍ŉ�����
	//	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);
	//	DirectX::XMStoreFloat4x4(&rc.projection, Projection);
	//}
	
	// 3D���f���`��
	{
		Shader* shader = graphics.GetShader();
		// �V�F�[�_�[�ɕK�v�ȏ�������
		shader->Begin(dc, rc);// �V�F�[�_�[�ɃJ�����̏���n��
		// �X�e�[�W�`��
		StageManager::instance().Render(dc, shader);
	// �v���C���[�`��
		//player->Render(dc, shader);
		playerManager->Render(dc, shader);
		// �G�l�~�[�`��
		EnemyManager::Instance().Render(dc, shader);

		shader->End(dc);
	}
	MouseOpreration(dc);

	// 3D�G�t�F�N�g�`��
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3D�f�o�b�O�`��
	{
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
		// begin����end�܂ł̓��e���o����
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
			// �g�����X�t�H�[��
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

		// 2D�X�v���C�g�`��
		{
			if (playerManager->GetPlayers().size() > 0)
			{
				RenderNumber(dc, rc.view, rc.projection);
				RenderTimer(dc, int(timer));
			}
		}


		if (connection && playerManager->GetPlayers().size() > 0)
		{
			// �����蔻��̌`������
			// �v���C���[�f�o�b�O�v���~�e�B�u�`��
			playerManager->GetMyPlayer()->DrawDebugPrimitive();

			// �G�l�~�[�f�o�b�O�v���~�e�B�u�`��
			EnemyManager::Instance().DrawDebugPrimitive();

			// ���C�������_���`����s
			graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

			// ���ۂ̓����蔻��`��
			// �f�o�b�O�����_���`����s
			graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);


			
			// 2D�f�o�b�OGUI�`��
			{
				// �v���C���[�f�o�b�O�`��
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

	//�����Ă���
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
	//�N���b�N��
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
	// ���W�A������x�ɕϊ�
	mouseAngleDegrees = XMConvertToDegrees(mouseAngle);
	playerAngleDegrees = mouseAngleDegrees;

	// 0�x��������ɂ��邽�߂�90�x��ǉ�
	playerAngleDegrees = 90.0f - playerAngleDegrees;

	// �p�x��0-360�x�͈̔͂ɒ���
	if (playerAngleDegrees < 0) {
		playerAngleDegrees += 360.0f;
	}
	if (mouseAngleDegrees < 0) {
		mouseAngleDegrees += 360.0f;
	}

	//�t��]�̂��ߕ␳
	playerAngleDegrees -= 360;
	playerAngleDegrees *= -1;

	//�p�x��x���烉�W�A���ɖ߂�
	mouseAngle = XMConvertToRadians(mouseAngleDegrees);
	playerAngle = XMConvertToRadians(playerAngleDegrees);

	if (length < 0.2f) {
		playerAngle = playerManager->GetMyPlayer()->GetAngle().y;
	}


	playerManager->GetMyPlayer()->mouseAngle = mouseAngle;
	playerManager->GetMyPlayer()->SetAngle({ 0, playerAngle, 0 });
	

	
	
	sprites[static_cast<int>(SpriteNumber::BigCircle)]->Render(dc,
		clickPos.x - 50, clickPos.y - 50, //�`��ʒu
		100, 100,             //�\���T�C�Y
		0, 0,                 //�؂���͂��߈ʒu
		300, 300,           //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);

	//�������~���傫���~����o�Ȃ��悤�ɕ␳
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
		oldMousePos.x - 15, oldMousePos.y - 15, //�`��ʒu
		30, 30,             //�\���T�C�Y
		0, 0,                 //�؂���͂��߈ʒu
		100, 100,           //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);
}

// �v���C���[ID�`��
void SceneGame::RenderNumber(ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection
)
{
	// �r���[�|�[�g ��ʂ̃T�C�Y��
	// �r���[�|�[�g�Ƃ�2D�̉�ʂɕ`��͈͂̎w��(�N���b�s���O�w����o����)�ʒu���w��
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	// ���X�^���C�U�[�X�e�[�g�Ƀo�C���h����Ă���r���[�|�[�g�z����擾
	dc->RSGetViewports(&numViewports, &viewport);

	// �ϊ��s��
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	// ���[�J�����烏�[���h�ɍs���Ƃ��ɂ���z����̃|�W�V������n���B
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
	DirectX::XMVECTOR world = {};
	DirectX::XMVector3Transform(world, World);

	//�v���C���[�̓���
	DirectX::XMFLOAT3 worldPosition = playerManager->GetMyPlayer()->GetPosition();
	worldPosition.y += playerManager->GetMyPlayer()->GetHeight();

	// ���[���h����X�N���[��
	DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);


	// �Q�[�W�`�� // ���[���h����X�N���[��
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
	// �X�N���[�����W
	DirectX::XMFLOAT3 scereenPosition;
	DirectX::XMStoreFloat3(&scereenPosition, ScreenPosition);


	// �摜�̒���
	const float gaugeWidth = 25.0f;
	const float gaugeHeight = 33.0f;

	// �v���C���[ID�̌��������߂�
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

		// �e����`�悷�郋�[�v
		for (int i = numDigits - 1; i >= 0; --i)
		{
			// �e���̐��l���擾
			digit = (ID / static_cast<int>(pow(10, i))) % 10;

			// �X�v���C�g��`��
			sprites[static_cast<int>(SpriteNumber::Number)]->Render(dc,
				numberposX, numberposY,
				30, 30,
				gaugeWidth * digit + digit, 0,
				gaugeWidth, gaugeHeight,
				0.0f,
				1, 1, 1, 1);

			// ���̌��̈ʒu�Ɉړ�
			numberposX += 20;
		}
	}
}

void SceneGame::RenderTimer(ID3D11DeviceContext* dc, int timer)
{
	// �摜�̒���
	const float gaugeWidth = 25.0f;
	const float gaugeHeight = 33.0f;
	int ID = timer;

	// �v���C���[ID�̌��������߂�
	int numDigits = 1;
	int tempID = ID;
	while (tempID >= 10)
	{
		tempID /= 10;
		numDigits++;
	}

	

	// 2D�X�v���C�g�`��
	{
		float positionX = 600;
		float positionY =20;
		int digit = 0;

		// �e����`�悷�郋�[�v
		for (int i = numDigits - 1; i >= 0; --i)
		{
			// �e���̐��l���擾
			digit = (ID / static_cast<int>(pow(10, i))) % 10;

			// �X�v���C�g��`��
			sprites[static_cast<int>(SpriteNumber::Number)]->Render(dc,
				positionX, positionY,
				15, 15,
				gaugeWidth * digit,0,
				gaugeWidth, gaugeHeight,
				0.0f,
				1, 1, 1, 1);

			// ���̌��̈ʒu�Ɉړ�
			positionX += 15;
		}
	}
}


