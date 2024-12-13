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

	//�J����
	{
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
		cameraController->SetRange(10.0f);
		//DirectX::XMFLOAT3 target = { 0,0,0 };
		//target.y += 0.5f;// ��������T�O�Z���`���炢
		//cameraController->SetTarget(target);// �v���C���[�̍�������
	}

	// �X�v���C�g�\���������
	{
		// �X�v���C�g�\���������
		//sprites[static_cast<int>(Spritenumber:)] = std::make_unique<Sprite>("Data/Sprite/");
		//����
		//sprites[static_cast<int>(Spritenumber::Number)] = std::make_unique<Sprite>("Data/Sprite/number.png");
		////��
		//sprites[static_cast<int>(Spritenumber::Flag)] = std::make_unique<Sprite>("Data/Sprite/flag.png");
		////�`�[��
		//sprites[static_cast<int>(Spritenumber::Team)] = std::make_unique<Sprite>("Data/Sprite/team.png");
		////�`�[���Z���N�g
		//sprites[static_cast<int>(Spritenumber::TeamSelect)] = std::make_unique<Sprite>("Data/Sprite/teamselect.png");
		////�`�[���쐬
		//sprites[static_cast<int>(Spritenumber::TeamCreate)] = std::make_unique<Sprite>("Data/Sprite/teamcreate.png");
		////����
		//sprites[static_cast<int>(Spritenumber::TeamJoin)] = std::make_unique<Sprite>("Data/Sprite/teamjoin.png");
		////�`�[���������鎞�̃N���b�N�ł��Ȃ��Ƃ�
		//sprites[static_cast<int>(Spritenumber::TeamJoinFrame)] = std::make_unique<Sprite>("Data/Sprite/teamjoinframe.png");
		////�`�[���������鎞�̃N���b�N����Ƃ�
		//sprites[static_cast<int>(Spritenumber::TeamjoinNumber)] = std::make_unique<Sprite>("Data/Sprite/teamjoinnumber.png");
		////�`�[���ԍ�
		//sprites[static_cast<int>(Spritenumber::TeamNumber)] = std::make_unique<Sprite>("Data/Sprite/teamnumber.png");
		////����{�^��
		//sprites[static_cast<int>(Spritenumber::Close)] = std::make_unique<Sprite>("Data/Sprite/close.png");
		////��������
		//sprites[static_cast<int>(Spritenumber::Ready)] = std::make_unique<Sprite>("Data/Sprite/ready.png");
		////�����L�����Z��
		//sprites[static_cast<int>(Spritenumber::ReadyzCancel)] = std::make_unique<Sprite>("Data/Sprite/readycancel.png");
		////�\�����[�h
		//sprites[static_cast<int>(Spritenumber::Solo)] = std::make_unique<Sprite>("Data/Sprite/solo.png");
		////�}���`���[�h
		//sprites[static_cast<int>(Spritenumber::Multi)] = std::make_unique<Sprite>("Data/Sprite/multi.png");
		////�X�^�[�g
		//sprites[static_cast<int>(Spritenumber::Start)] = std::make_unique<Sprite>("Data/Sprite/start.png");
		////���O
		//sprites[static_cast<int>(Spritenumber::Name)] = std::make_unique<Sprite>("Data/Sprite/font1.png");
		//sprites[static_cast<int>(Spritenumber::WASD)] = std::make_unique<Sprite>("Data/Sprite/WASD.png");
		//
		//sprites[static_cast<int>(Spritenumber::SelectEdge)] = std::make_unique<Sprite>("Data/Sprite/selectedge.png");
		//
		//sprites[static_cast<int>(Spritenumber::Setting)] = std::make_unique<Sprite>("Data/Sprite/setting.png");
		//
		//sprites[static_cast<int>(Spritenumber::Mouse)] = std::make_unique<Sprite>("Data/Sprite/mouse.png");
		//
		////���O�A�E�g
		//sprites[static_cast<int>(Spritenumber::Logout)] = std::make_unique<Sprite>("Data/Sprite/logout.png");
	}

	SceneManager& sceneManager = SceneManager::Instance();
	//����������x�ڂ��ǂ���
	if (!sceneManager.GetstandbyInitialized())
	{
		sceneManager.SetstandbyInitialized(true);
		playerManager = sceneManager.GetPlayerManager();
		connection = sceneManager.GetConnection();
	}
	else
	{
		playerManager = sceneManager.GetPlayerManager();
		connection = sceneManager.GetConnection();

		playerManager->GetMyPlayer()->SetstartCheck(false);
	    //�\���̏ꍇ�@
		if (playerManager->GetMyPlayer()->Getteamnumber() == 0)
		{
			playerManager->GetMyPlayer()->SetAngle({ 0.0f,3.0f,0.0f });
			playerManager->GetMyPlayer()->SetPosition({ 0.0f,0.0f,0.0f });
		}
		else
		{
			for (int i = 0; i < playerManager->GetPlayers().size(); ++i)
			{
				int MYID = playerManager->GetMyPlayer()->Getteamsid(i);
				playerManager->GetPlayer(MYID)->SetAngle({ 0.0f,3.0f,0.0f });
				playerManager->GetPlayer(MYID)->SetPosition({ 1.5f * i,0.0f,0.0f });
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
	// �J�����R���g�[���[�I����
	if (this->cameraController)
	{
		delete cameraController;
		cameraController = nullptr;
	}
	// �X�e�[�W�I����
	StageManager::instance().Clear();


	if (this->player)
	{
		delete player;
		player = nullptr;
	}

	//�v���C���[�Ɛڑ��̊Ǘ��̓V�[���}�l�[�W���[�������Ă�

}

void SceneStandby::Update(float elapsedTime)
{
	
	//playerManager->GetMyPlayer()->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
		
	//�V�[���J��
	if (playerManager->GetGameStart())
	{
		playerManager->SetGameStart(false);
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
	}

	//�f�o�b�O�p
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

	//�`�[������鎞
	//if (sendteamcreate&&!teamcreate)
	//{
	//	sendteamcreate = false;
	//	teamcreate = true;
	//	connection->SendTeamcreate();
	//	playerManager->SetteamLeader(true);
	//}

	//�`�[���ɎQ������Ƃ�
	if (sendteamjoin && !teamcreate && !teamjoin)
	{
		sendteamjoin = false;
		teamjoin = true;
		connection->SendTeamJoin(TeamNumber);
	}
	
	//����OK
	//if (startcheck != sendstartcheck)
	//{
	//	sendstartcheck = !sendstartcheck;
	//	connection->SendStartCheck(sendstartcheck);
	//}

	//�Q�[���X�^�[�g�\��
	if(sendgamestart&& playerManager->GetMyPlayer()->GetTeamHost())
	{
		sendgamestart = false;
		connection->SendGamestart(playerManager->GetMyPlayer()->Getteamnumber());
	}

	playerManager->Update(elapsedTime);

	//�`�[���̃z�X�g�����Ȃ��Ȃ�����
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
	


	//�`�[���ɐ�ɂ����v���C���[�𐶐�
	SyncPlayerGenerate();
	//�ォ�痈���v���C���[�𐶐�
	LoginPlayerGenerate();

	//�v���C���[�������X�g�ɂ�����̂�����
	playerManager->DeletePlayer();


	//imgui�p
	for (int i = 0; i < 3; ++i)
	{
		guiteamsid[i] = playerManager->GetMyPlayer()->Getteamsid(i);
	}

	// �J�����R���g���[���[�X�V����
	DirectX::XMFLOAT3 target = playerManager->GetMyPlayer()->GetPosition();
	target.y += 0.5f;// ��������T�O�Z���`���炢
	cameraController->SetTarget(target);// �v���C���[�̍�������
	cameraController->Update(elapsedTime);

	// �X�e�[�W�X�V����
	StageManager::instance().Update(elapsedTime);


}

void SceneStandby::Render()
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

	// 3D���f���`��
	{
		Shader* shader = graphics.GetShader();
		// �V�F�[�_�[�ɕK�v�ȏ�������
		shader->Begin(dc, rc);// �V�F�[�_�[�ɃJ�����̏���n��
		// �X�e�[�W�`��
		StageManager::instance().Render(dc, shader);
		// �v���C���[�`��
		//player->Render(dc, shader);
		//if (playerManager->GetMyPlayerID() != 0)
		playerManager->Render(dc, shader);

		shader->End(dc);
	}

	// 2D�X�v���C�g�`��
	if (!connection->GetIsConectionError())
	{

		//���݂̃��[�h
		RenderMode(dc);

		//�Q�[���J�n�{�^��
		//�`�[���̃z�X�g�̎��� �`�[����g��Ŗ����`�[���֘A�̏��������Ă��Ȃ���
		if (playerManager->GetMyPlayer()->GetTeamHost() || playerManager->GetMyPlayer()->Getteamnumber() < 1&& teamRenderflag)
		{
			RenderGameStart(dc);
		}

		//OprerationSelect(dc);
		//if(isSetting)
		//Logout(dc);

		//���O���������
		if (playerManager->GetMyPlayerID() != 0 && !numberinputflag && playerManager->GetMyPlayer()->GetName()[0] == '\0')
		{
			//ID�\��
			RenderID(dc, rc.view, rc.projection);
		}

		//���O�������
		if (playerManager->GetMyPlayer()->GetName() != '\0'&& !numberinputflag)
		{
			//���O�\��
			RenderName(dc, rc.view, rc.projection);
		}
		
		//�`�[����g��ł����
		if (playerManager->GetMyPlayer()->Getteamnumber() > 0)
		{
			//�`�[���ԍ��\��
			RenderTeamNumber(dc, rc.view, rc.projection);
			//�z�X�g����Ȃ����
			if (!playerManager->GetMyPlayer()->GetTeamHost())
			{
				//���������{�^���\��
				RenderReady(dc, playerManager->GetMyPlayer()->GetstartCheck());
			}
		}
		//�`�[����g��łȂ�������
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
					RenderTeamJoin(dc);
				}
				else
				{
					RenderTeamSelect(dc);
				}
			}
		}
	}
	else
	{
		//�T�[�o�[�Ƃ̐ڑ����؂ꂽ��
		connection->ConnectionCheck(dc);
	}
	
	
	// 2D�f�o�b�O�`��
	//IMGUI�`��
	{
		
		ImGui::SetNextWindowPos(ImVec2(310, 10), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
		// begin����end�܂ł̓��e���o����
		if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
		{
			ImGui::Text("UserID: %d", playerManager->GetMyPlayer()->GetPlayerID());
			ImGui::Text("LoginDay: %d", playerManager->GetMyPlayer()->GetLoginDay());
			ImGui::Text("Name: %s", playerManager->GetMyPlayer()->GetName());
			
			
			if (playerManager->GetMyPlayer()->Getteamnumber() != 0)
			{
				ImGui::Text("Host: %d", playerManager->GetMyPlayer()->GetTeamHost());
				ImGui::InputText("Message", input, sizeof(input));
				if (ImGui::Button("Send"))
				{
					if (strcmp(input, "") != 0)
					{
						// ���M����
						connection->SendMessages(input);
					}
					input[0] = '\0';
				}
				ImGui::Text("Messeages");
				for (std::string message : playerManager->Getmessages())
				{
					ImGui::Text(u8"%s", message.c_str());
				}
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
		//
		////if (playerManager->GetLoginCount() > 1)
		////{
		////	ImGui::SetNextWindowPos(ImVec2(500, 10), ImGuiCond_Once);
		////	ImGui::SetNextWindowSize(ImVec2(300, 320), ImGuiCond_Once);
		////	if (ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_None))
		//	{
		//		ImGui::Text("Message");
		//		ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 200), ImGuiWindowFlags_NoTitleBar);
		//
		//		for (std::string message : playerManager->Getmessages()) {
		//			ImGui::Text(u8"%s", message.c_str());
		//		}
		//		ImGui::EndChild();
		//		ImGui::Spacing();
		//
		//		ImGui::InputText("Message", input, sizeof(input));
		//		if (playerManager->GetMyPlayer()->Getteamnumber() != 0)
		//			if (ImGui::Button("Send"))
		//			{
		//				if (strcmp(input, "") != 0)
		//				{
		//					// ���M����
		//					connection->SendMessages(input);
		//				}
		//				input[0] = '\0';
		//			}
		//	}
		////	ImGui::End();
		////}

		
	}
}

void SceneStandby::OprerationSelect(ID3D11DeviceContext* dc)
{
	//�ݒ�{�^���\��
	{
		DirectX::XMFLOAT2 size, pos;
		pos = { 410,10 };
		size = { 30,30 };

		Sprite* SettingSprite = g_SpriteManager.GetSprite(SpriteNumber::Setting);
		SettingSprite->Render(dc,
			pos.x, pos.y,  //�`��ʒu
			size.x, size.y,  //�\���T�C�Y
			0, 0,      //�؂���͂��߈ʒu
			256, 256, //�摜�T�C�Y
			0.0f,
			1, 1, 1, 1);

		if (Uiclick(pos.x,pos.y, size.x,size.y))
		{
			isSetting = !isSetting;
			Sleep(10);
		};
	}

	//�ݒ�{�^���������Ă�����
	if (isSetting)
	{
		DirectX::XMFLOAT2 size, mousePos, WASDPos, edgePos;

		size = { 50,75 };
		mousePos = { 75,25 };
		WASDPos = { 175,25 };

		//�}�E�X�̃C���X�g�`��
		Sprite* MouseSprite = g_SpriteManager.GetSprite(SpriteNumber::Mouse);
		MouseSprite->Render(dc,
			mousePos.x, mousePos.y,  //�`��ʒu
			size.x, size.y,  //�\���T�C�Y
			0, 0,      //�؂���͂��߈ʒu
			780, 1178, //�摜�T�C�Y
			0.0f,
			1, 1, 1, 1);

		//WASD�{�^���̕`��
		Sprite* WASDSprite = g_SpriteManager.GetSprite(SpriteNumber::WASD);
		WASDSprite->Render(dc,
			WASDPos.x, WASDPos.y,  //�`��ʒu
			size.x, size.y,  //�\���T�C�Y
			0, 0,    //�؂���͂��߈ʒu
			255, 194, //�摜�T�C�Y
			0.0f,
			1, 1, 1, 1);


		//������@�ǂ��炪�I������Ă邩�Ɖ��̍��W
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

		//�I�����Ă��鉏�`��
		Sprite* SelectEdgeSprite = g_SpriteManager.GetSprite(SpriteNumber::SelectEdge);
		SelectEdgeSprite->Render(dc,
			edgePos.x, edgePos.y,  //�`��ʒu
			size.x * 1.5f, size.y * 1.5f,  //�\���T�C�Y
			0, 0,      //�؂���͂��߈ʒu
			100, 100, //�摜�T�C�Y
			0.0f,
			1, 1, 1, 1);

	}
}

void SceneStandby::Logout(ID3D11DeviceContext* dc)
{
	DirectX::XMFLOAT2 size, pos;
	size = { 100,50 };
	pos = { 25,150 };
	//���O�A�E�g�{�^��
	Sprite* LogoutSprite = g_SpriteManager.GetSprite(SpriteNumber::Logout);
	LogoutSprite->Render(dc,
		pos.x, pos.y,        //�`��ʒu
		size.x, size.y,      //�\���T�C�Y
		0, 0,                //�؂���͂��߈ʒu
		500, 200,            //���̉摜�T�C�Y
		0.0f,
		1, 1, 1, 1);

	if (Uiclick(pos.x,pos.y, size.x,size.y))
	{
		//�`�[����g��ł��甲����𑗂�
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

			//�`�[���ɑ��݂��Ȃ�������
			if (ID == 0)break;
			//������ID����Ȃ�������
			if (playerManager->GetMyPlayerID() != ID)
			{
				//����
				Player* player = new Player();
				player->SetPlayerID(ID);						//�����ID�����X�g�b�N
				player->SetPosition(DirectX::XMFLOAT3(1.5f *count, 0, 0));			//�����ʒu
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
	//�����ȊO�̃v���C���[����join�p
	int generateCount = playerManager->GetPlayersGenerateCount();
	//���O�C�����Ă��Ă��������܂���������
	if (generateCount < playerManager->GetLoginCount() && !playerManager->GetSynclogin())
	{
		//����
		int ID = playerManager->GetMyPlayer()->Getteamsid(generateCount);
		Player* player = new Player();
		player->SetPlayerID(ID);						//�����ID�����X�g�b�N
		player->SetPosition(DirectX::XMFLOAT3(1.5f * generateCount, 0, 0));			//�����ʒu
		player->SetAngle({ 0.0f,3.0f,0.0f });
		player->Setoperation(false);

		playerManager->AddPlayer(player);
		playerManager->GetPlayer(ID)->SetReady(true);
		playerManager->AddPlayersGenerateCount();
	}
}

// �v���C���[��ID�`��
void SceneStandby::RenderID(ID3D11DeviceContext* dc,
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
		float numberposX = scereenPosition.x - 10;
		float numberposY = scereenPosition.y - 10;
		int digit = 0;

		// �e����`�悷�郋�[�v
		for (int i = numDigits - 1; i >= 0; --i)
		{
			// �e���̐��l���擾
			digit = (ID / static_cast<int>(pow(10, i))) % 10;

			// �X�v���C�g��`��
			Sprite* NumberSprite = g_SpriteManager.GetSprite(SpriteNumber::Number);
			NumberSprite->Render(dc,
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

void SceneStandby::RenderTeamNumber(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{

	
	// g_SpriteManager ���g���ăX�v���C�g���擾
	Sprite* numberSprite = g_SpriteManager.GetSprite(SpriteNumber::TeamNumber);
	// �擾�����X�v���C�g�� Render ���\�b�h���Ăяo��
	//numberSprite->Render();
	float positionX = 150;
	float positionY = 70;
	numberSprite->Render(dc,
		positionX, positionY,  //�`��ʒu
		200,40,                //�\���T�C�Y
		0, 0,                  //�؂���͂��߈ʒu
		1214, 200,             //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);

	// �摜�̒���
	const float gaugeWidth = 25.0f;
	const float gaugeHeight =33.0f;
	
	// �v���C���[ID�̌��������߂�
	int numDigits = 1;
	int ID =playerManager->GetMyPlayer()->Getteamnumber();
	int tempID = ID;
	while (tempID >= 10)
	{
		tempID /= 10;
		numDigits++;
	}

	
	
	float numberposX = positionX +85;
	float numberposY = positionY+7;
	int digit = 0;

	// �e����`�悷�郋�[�v
	for (int i = numDigits - 1; i >= 0; --i)
	{
		// �e���̐��l���擾
		digit = (ID / static_cast<int>(pow(10, i))) % 10;

		// �X�v���C�g��`��
		Sprite* NumberSprite = g_SpriteManager.GetSprite(SpriteNumber::Number);
		NumberSprite->Render(dc,
			numberposX, numberposY,
			30, 30,
			gaugeWidth * digit+ digit, 0,
			gaugeWidth, gaugeHeight,
			0.0f,
			1, 1, 1, 1);

		// ���̌��̈ʒu�Ɉړ�
		numberposX += 20;
	}
	
	//����{�^��
	Sprite* CloseSprite = g_SpriteManager.GetSprite(SpriteNumber::Close);
	CloseSprite->Render(dc,
		positionX + 160, positionY, //�`��ʒu
		40, 40,               //�\���T�C�Y
		0, 0,                 //�؂���͂��߈ʒu
		100, 100,           //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);

	if (Uiclick(positionX + 160, positionY, 40, 40))
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




	float positionX = scereenPosition.x-30;
	float positionY = scereenPosition.y;

	float sizeX = 32;
	float sizeY = 32;

	char name[10];
	strcpy_s(name, playerManager->GetMyPlayer()->GetName());
	//�g�g��
	for (int i = 0; i < 9; ++i)
	{
		if (name[i] == '\0')break;
		int number = static_cast<int>(name[i]);
		int width = number % 16;
		int height = number / 16;

		Sprite* NameSprite = g_SpriteManager.GetSprite(SpriteNumber::Name);
		NameSprite->Render(dc,
			positionX, positionY,      //�`��ʒu
			24,24,              //�\���T�C�Y
			sizeX * width, sizeY * height, //�؂���͂��߈ʒu
			sizeX, sizeY,              //�摜�T�C�Y
			0.0f,
			1, 1, 1, 1);

		positionX += sizeX / 2;
	}
}

void SceneStandby::RenderTeam(ID3D11DeviceContext* dc)
{
	float positionX = 10;
	float positionY = 10;
	float sizeX = 100;
	float sizeY = 30;

	Sprite* TeamSprite = g_SpriteManager.GetSprite(SpriteNumber::Team);
	TeamSprite->Render(dc,
		positionX, positionY,     //�`��ʒu
		sizeX, sizeY,             //�\���T�C�Y
		0, 0,                     //�؂���͂��߈ʒu
		243,100,                  //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);

	if(teamRenderflag &&Uiclick(positionX, positionY, sizeX, sizeY))
	{
		teamRenderflag = false;
	}
}

void SceneStandby::RenderTeamSelect(ID3D11DeviceContext* dc)
{
	float positionX = 80;
	float positionY = 50;
	//�g�g��
	Sprite* TeamSelectSprite = g_SpriteManager.GetSprite(SpriteNumber::TeamSelect);
	TeamSelectSprite->Render(dc,
		positionX, positionY, //�`��ʒu
		600, 250,               //�\���T�C�Y
		0, 0,                 //�؂���͂��߈ʒu
		1000,400,           //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);


	float sizeX = 200;
	float sizeY = 150;
	//�`�[���쐬�{�^��
	Sprite* TeamCreate = g_SpriteManager.GetSprite(SpriteNumber::TeamCreate);
	TeamCreate->Render(dc,
		positionX + 70, positionY + 60, //�`��ʒu
		sizeX, sizeY,               //�\���T�C�Y
		0, 0,                 //�؂���͂��߈ʒu
		800, 600,           //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);

	if (Uiclick(positionX + 70, positionY + 60, 200, 150))
	{
		teamcreate = true;
		teamRenderflag = false;
		connection->SendTeamcreate();
		playerManager->GetMyPlayer()->SetTeamHost(true);
	}

	//�`�[�������{�^��
	Sprite* TeamJoinCreate = g_SpriteManager.GetSprite(SpriteNumber::TeamJoin);
	TeamJoinCreate->Render(dc,
		positionX+320, positionY+60, //�`��ʒu
		sizeX, sizeY,               //�\���T�C�Y
		0, 0,                 //�؂���͂��߈ʒu
		800,600,           //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);
	
		if (Uiclick(positionX + 320, positionY + 60, sizeX, sizeY))
		{
			teamRenderflag = false;
			numberinputflag = true;
		}
	
	//����{�^��
		Sprite* CloseCreate = g_SpriteManager.GetSprite(SpriteNumber::Close);
		CloseCreate->Render(dc,
		positionX + 565, positionY-5, //�`��ʒu
		40,40,               //�\���T�C�Y
		0, 0,                 //�؂���͂��߈ʒu
		100,100,           //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);

	if (Uiclick(positionX + 565, positionY - 5, 40, 40))
	{
		teamRenderflag = true;
	}
}

void SceneStandby::RenderTeamJoin(ID3D11DeviceContext* dc)
{
	float positionX = 80;
	float positionY = 30;
	//�g�g��
	Sprite* TeamJoinFrameCreate = g_SpriteManager.GetSprite(SpriteNumber::TeamJoinFrame);
	TeamJoinFrameCreate->Render(dc,
		positionX, positionY,   //�`��ʒu
		600, 300,               //�\���T�C�Y
		0, 0,                   //�؂���͂��߈ʒu
		1000, 400,              //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);

	//�{�^��������
	
	int count = 0;
	float sizeX = 90;
	float sizeY = 30;
	
	for (float i = 0; i < 4; ++i)
	{
		for (float j = 0; j < 3; ++j)
		{
			++count;

			Sprite* TeamjoinNumberCreate = g_SpriteManager.GetSprite(SpriteNumber::TeamjoinNumber);
			TeamjoinNumberCreate->Render(dc,
				positionX + 130.0f + (j * 100.0f), positionY + 140.0f+(i*40.0f), //�`��ʒu
				sizeX, sizeY,                      //�\���T�C�Y
				(j * 300.0f), (i*100.0f),    //�؂���͂��߈ʒu
				300, 100,                    //�摜�T�C�Y
				0.0f,
				1, 1, 1, 1);

			float posx = positionX + 130.0f + (j * 100.0f);
			float posy = positionY + 140.0f + (i * 40.0f);
			posxy[count-1][0] = posx;
			posxy[count-1][1] = posy;
			
		}
	}
	if (numbers.size() < 4)
	{
		for (int i = 0; i < 10; ++i)
		{
			//9���ڂ��ꌅ������10���߂�0������
			if (i == 9)
			{
				if (Uiclick(posxy[10][0], posxy[10][1], sizeX, sizeY))numbers.emplace_back(0);
				break;
			}
			if (Uiclick(posxy[i][0], posxy[i][1], sizeX, sizeY))numbers.emplace_back(i+1);

		}
	}
	//�ꌅ����
	if (numbers.size() > 0) 
	{
		if (Uiclick(posxy[9][0], posxy[9][1], sizeX, sizeY))numbers.pop_back();
	}
	//���M
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

	// 2D�X�v���C�g�`��
	{
		float numberposX = positionX + 188;
		float numberposY = positionY + 80;
		const float gaugeWidth = 25.9f;
		const float gaugeHeight = 33.0f;

		// �e����`�悷�郋�[�v
		if (numbers.size() > 0)
		{
			for (int i = 0; i < numbers.size(); ++i)
			{
				int number = numbers.at(i);

				// �X�v���C�g��`��
				Sprite* NumberCreate = g_SpriteManager.GetSprite(SpriteNumber::Number);
				NumberCreate->Render(dc,
					numberposX+(51*i), numberposY,
					30, 30,
					gaugeWidth * number, 0,
					gaugeWidth, gaugeHeight,
					0.0f,
					1, 1, 1, 1);
			}
		}
	}

	//����{�^��
	Sprite* CloseCreate = g_SpriteManager.GetSprite(SpriteNumber::Close);
	CloseCreate->Render(dc,
		positionX + 565, positionY - 5, //�`��ʒu
		40, 40,               //�\���T�C�Y
		0, 0,                 //�؂���͂��߈ʒu
		100, 100,           //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);

	if (Uiclick(positionX + 565.0f, positionY - 5.0f, 40.0f, 40.0f))
	{
		numberinputflag = false;
		teamRenderflag = true;
		numbers.erase(numbers.begin(), numbers.end());
	}
}

void SceneStandby::RenderReady(ID3D11DeviceContext* dc, bool isready)
{
	float positionX = 10;
	float positionY = 300;

	//
	if (!isready)
	{
		Sprite* ReadyCreate = g_SpriteManager.GetSprite(SpriteNumber::Ready);
		ReadyCreate->Render(dc,
			positionX, positionY, //�`��ʒu
			150, 50,               //�\���T�C�Y
			0, 0,                 //�؂���͂��߈ʒu
			500,184,           //�摜�T�C�Y
			0.0f,
			1, 1, 1, 1);

		if (Uiclick(positionX, positionY, 150.0f, 50.0f))
		{
			playerManager->GetMyPlayer()->SetstartCheck(true);
			connection->SendStartCheck(true);
		}
	}
	else
	{
		Sprite* ReadyzCancelCreate = g_SpriteManager.GetSprite(SpriteNumber::ReadyzCancel);
		ReadyzCancelCreate->Render(dc,
			positionX, positionY, //�`��ʒu
			150, 50,             //�\���T�C�Y
			0, 0,                 //�؂���͂��߈ʒu
			500,184,           //�摜�T�C�Y
			0.0f,
			1, 1, 1, 1);

		if (Uiclick(positionX, positionY, 150.0f, 50.0f))
		{
			playerManager->GetMyPlayer()->SetstartCheck(false);
			connection->SendStartCheck(false);
		}
	}
}

void SceneStandby::RenderMode(ID3D11DeviceContext* dc)
{
	float positionX = 10;
	float positionY = 200;

	//
	if (playerManager->GetMyPlayer()->Getteamnumber() > 0)
	{
		Sprite* MultiCreate = g_SpriteManager.GetSprite(SpriteNumber::Multi);
		MultiCreate->Render(dc,
			positionX, positionY, //�`��ʒu
			150, 50,               //�\���T�C�Y
			0, 0,                 //�؂���͂��߈ʒu
			500, 184,           //�摜�T�C�Y
			0.0f,
			1, 1, 1, 1);
		
	}
	else
	{
		Sprite* SoloCreate = g_SpriteManager.GetSprite(SpriteNumber::Solo);
		SoloCreate->Render(dc,
			positionX, positionY, //�`��ʒu
			150, 50,               //�\���T�C�Y
			0, 0,                 //�؂���͂��߈ʒu
			500, 184,           //�摜�T�C�Y
			0.0f,
			1, 1, 1, 1);
	}

}

void SceneStandby::RenderGameStart(ID3D11DeviceContext* dc)
{
	float positionX = 10;
	float positionY = 300;

	Sprite* StartCreate = g_SpriteManager.GetSprite(SpriteNumber::Start);
	StartCreate->Render(dc,
		positionX, positionY, //�`��ʒu
		150, 50,             //�\���T�C�Y
		0, 0,                 //�؂���͂��߈ʒu
		500, 184,           //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);
	if (Uiclick(positionX, positionY, 150.0f, 50.0f))
	{
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


