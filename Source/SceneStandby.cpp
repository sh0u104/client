#include "SceneStandby.h"

#include "Graphics/Graphics.h"
#include "Camera.h"
#include "StageManager.h"
#include "StageMain.h"

#include "Input/Input.h"

#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneGame.h"
void SceneStandby::Initialize()
{
	
	StageManager& stageManager = StageManager::instance();
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);

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
	DirectX::XMFLOAT3 target = { 0,0,0 };
	target.y += 0.5f;// ��������T�O�Z���`���炢
	cameraController->SetTarget(target);// �v���C���[�̍�������

	// �X�v���C�g�\���������
	//sprites[static_cast<int>(Spritenumber:)] = std::make_unique<Sprite>("Data/Sprite/");
	//����
	sprites[static_cast<int>(Spritenumber::Number)] =std::make_unique<Sprite>("Data/Sprite/number.png");
	//��
	sprites[static_cast<int>(Spritenumber::Flag)] = std::make_unique<Sprite>("Data/Sprite/flag.png");
	//�`�[��
	sprites[static_cast<int>(Spritenumber::Team)] = std::make_unique<Sprite>("Data/Sprite/team.png");
	//�`�[���Z���N�g
	sprites[static_cast<int>(Spritenumber::TeamSelect)] = std::make_unique<Sprite>("Data/Sprite/teamselect.png");
	//�`�[���쐬
	sprites[static_cast<int>(Spritenumber::TeamCreate)] = std::make_unique<Sprite>("Data/Sprite/teamcreate.png");
	//����
	sprites[static_cast<int>(Spritenumber::TeamJoin)] = std::make_unique<Sprite>("Data/Sprite/teamjoin.png");
	//�`�[���������鎞�̃N���b�N�ł��Ȃ��Ƃ�
	sprites[static_cast<int>(Spritenumber::TeamJoinFrame)] = std::make_unique<Sprite>("Data/Sprite/teamjoinframe.png");
	//�`�[���������鎞�̃N���b�N����Ƃ�
	sprites[static_cast<int>(Spritenumber::TeamjoinNumber)] = std::make_unique<Sprite>("Data/Sprite/teamjoinnumber.png");
	//�`�[���ԍ�
	sprites[static_cast<int>(Spritenumber::TeamNumber)] = std::make_unique<Sprite>("Data/Sprite/teamnumber.png");
	//�~�{�^��
	sprites[static_cast<int>(Spritenumber::Close)] = std::make_unique<Sprite>("Data/Sprite/close.png");

	//����������x�ڂ��ǂ���
	if (!SceneManager::Instance().GetstandbyInitialized())
	{
		// �v���C���[�}�l�W���[������
		//playerManager = new PlayerManager();
		SceneManager::Instance().PlayermanagerInitialize();
		playerManager = SceneManager::Instance().GetPlayerManager();

		//	�T�[�o�[�Ƃ̐ڑ��O��ID�O
		Player* player = new Player();
		player->SetPlayerID(0);						//�����ID�����X�g�b�N
		player->Setoperation(false);
		player->SetPosition(DirectX::XMFLOAT3(0, 0, 0));			//�����ʒu
		player->SetAngle({ 0.0f,3.0f,0.0f });

		playerManager->SetMyPlayerID(0);
		playerManager->AddPlayer(player);
		playerManager->GetPlayer(0)->SetReady(true);

		//����������
		playerManager->AddPlayersGenerateCount();


		SceneManager::Instance().ConnectionInitialize();
		connection = SceneManager::Instance().GetConnection();

		SceneManager::Instance().SetstandbyInitialized(true);
	}
	else
	{
		playerManager = SceneManager::Instance().GetPlayerManager();
		for (int i = 0; i < playerManager->GetPlayers().size(); ++i)
		{
			int MYID = playerManager->GetMyPlayer()->Getteamsid(i);
			playerManager->GetPlayer(MYID)->SetAngle({ 0.0f,3.0f,0.0f });
			playerManager->GetPlayer(MYID)->SetPosition({ 1.0f * i,0.0f,0.0f });
		}
		connection = SceneManager::Instance().GetConnection();
		connectionflag = true;
		if (playerManager->GetteamLeader())
		{
			teamcreate = true;
		}
		else
		{
			//�����`�F�b�N���O��
			playerManager->GetMyPlayer()->SetstartCheck(false);
			teamjoin = true;
		}
	}
}

void SceneStandby::Finalize()
{
	//// �i���o�[�X�v���C�g
	//if (this->spriteNumber)
	//{
	//	delete spriteNumber;
	//	spriteNumber = nullptr;
	//}

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
	
	
	//�V�[���J��
	if (playerManager->GetGameStart())
	{
		playerManager->SetGameStart(false);
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));

	}

	//�T�[�o�ɂȂ���Ƃ�
	if (connectionflag && !connection->isConnction)
	{
		connection->Initialize();
		if(connection->isConnction)
		connection->SetplayerManager(playerManager);
		else
		{
			connectionflag = false;
		}
	}

	//�`�[������鎞
	if (sendteamcreate&&!teamcreate)
	{
		sendteamcreate = false;
		teamcreate = true;
		connection->SendTeamcreate();
		playerManager->SetteamLeader(true);
	}

	//�`�[���ɎQ������Ƃ�
	if (sendteamjoin && !teamcreate && !teamjoin)
	{
		sendteamjoin = false;
		teamjoin = true;
		connection->SendTeamJoin(TeamNumber);
	}
	
	//����OK
	if (startcheck != sendstartcheck)
	{
		sendstartcheck = !sendstartcheck;
		connection->SendStartCheck(sendstartcheck);
	}

	//�Q�[���X�^�[�g�\��
	if(sendgamestart)
	{
		sendgamestart = false;
		connection->SendGamestart
		(playerManager->GetMyPlayer()->Getteamnumber());
	}



	
	int generateCount = playerManager->GetPlayersGenerateCount();

	if (playerManager->GetSynclogin())
	{
		int teamMax = 4;
		for (int i = 0; i < teamMax-1; ++i)
		{
			int ID = playerManager->GetMyPlayer()->Getteamsid(i);

			//�`�[���ɑ��݂��Ȃ�������
			if (ID == 0)break;
			//������ID����Ȃ�������
			if (playerManager->GetMyPlayerID() != ID)
			{
				//����
				Player* player = new Player();
				player->SetPlayerID(ID);						//�����ID�����X�g�b�N
				player->SetPosition(DirectX::XMFLOAT3(1.5 * generateCount, 0, 0));			//�����ʒu
				player->SetAngle({ 0.0f,3.0f,0.0f });
				player->Setoperation(false);

				playerManager->AddPlayer(player);
				playerManager->GetPlayer(ID)->SetReady(true);
				playerManager->AddPlayersGenerateCount();
			}
		}
		playerManager->SetSynclogin(false);
	}

	//�����ȊO�̃v���C���[����join�p
	generateCount = playerManager->GetPlayersGenerateCount();
	//���O�C�����Ă��Ă��������܂���������
	if (generateCount < playerManager->GetLoginCount()&&!playerManager->GetSynclogin())
	{
		//����
		int ID = playerManager->GetMyPlayer()->Getteamsid(generateCount);
		Player* player = new Player();
		player->SetPlayerID(ID);						//�����ID�����X�g�b�N
		player->SetPosition(DirectX::XMFLOAT3(1.5 * generateCount, 0, 0));			//�����ʒu
		player->SetAngle({ 0.0f,3.0f,0.0f });
		player->Setoperation(false);

		playerManager->AddPlayer(player);
		playerManager->GetPlayer(ID)->SetReady(true);
		playerManager->AddPlayersGenerateCount();
	}
		
	
	cameraController->Update(elapsedTime);
	//player->Update(elapsedTime);
	//�v���C���[�����݂����
	//if (playerManager->GetMyPlayerID() != 0)
	{
		playerManager->Update(elapsedTime);

		//imgui�ł��[�ނ̂h�c���m�F���邽��
		guiteamsid[0] = playerManager->GetMyPlayer()->Getteamsid(0);
		for (int i = 1; i < 3; ++i)
		{
			guiteamsid[i]= playerManager->GetMyPlayer()->Getteamsid(i);
		}
	}
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
	{
		if (numberinputflag)
	    {
	    	RenderTeamJoin(dc);
	    }
		if (playerManager->GetMyPlayerID() != 0)
		RenderID(dc, rc.view, rc.projection);

		if (!teamscreenflag)
		{
			RenderTeam(dc);
		}
		else
		{
			RenderTeamSelect(dc);
		}
		
	
		if (teamcreate)
		{
			RenderTeamNumber(dc, rc.view, rc.projection);
			
		}
	}
	
	
	// 2D�f�o�b�O�`��
	{
		
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
		// begin����end�܂ł̓��e���o����
		if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
		{
				//ImGui::Text("LoginCount: %d", playerManager->GetLoginCount());
				//ImGui::Text("GenerateCount: %d", playerManager->GetPlayersGenerateCount());
			if (playerManager->GetMyPlayerID() != 0)
			{

				ImGui::Text("Name: %s", playerManager->GetMyPlayer()->GetName());
				ImGui::Text("ID: %d", playerManager->GetMyPlayerID());
				ImGui::InputInt("SendTeamNumber: %d", &TeamNumber);
				ImGui::Text("RecvTeamNumber: %d", playerManager->GetMyPlayer()->Getteamnumber());
			}
			if (!connectionflag)
			{
				if (ImGui::Button("Connection"))
				{
					connectionflag = true;
				}
				/*if (connection->connctionflag)
				{
					ImGui::Text("true");
				}
				else
				{
					ImGui::Text("false");
				}*/
			}

			if (playerManager->GetMyPlayerID() != 0)
			{
				if (!teamcreate && !teamjoin)
				{
					if (TeamNumber == 0)
					{
						if (ImGui::Button("Team Creation"))
						{
							sendteamcreate = true;
						}
					}
					else
					{
						if (ImGui::Button("Team Join"))
						{
							sendteamjoin = true;
						}
					}
				}
				if (playerManager->GetMyPlayer()->Getteamnumber() != 0&& !teamcreate)
				{
					if (ImGui::Button("Start Check"))
					{
						startcheck = !startcheck;
					}
					if (playerManager->GetMyPlayer()->GetstartCheck())
					{
						ImGui::Text("true");
					}
					else
					{
						ImGui::Text("false");
					}
					
				}

				if(teamcreate)
				if (ImGui::Button("GameStart"))
				{
					sendgamestart = true;
				}
			}
			ImGui::InputInt4("TeamsID", guiteamsid);

			if (ImGui::Button("FriendList Update"))
			{
				connection->SendSeeFriend();
			}

			ImGui::Text("FriendList");
			for (int i = 0; i < playerManager->myFriendList.size(); ++i)
			{
				ImGui::Text("ID: %d", playerManager->myFriendList.at(i).ID);
				ImGui::Text("Name: %s", playerManager->myFriendList.at(i).name);
			}

			playerManager->DebugGUI();
		}
		ImGui::End();

		if (playerManager->GetLoginCount() > 1)
		{
			ImGui::SetNextWindowPos(ImVec2(500, 10), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(300, 320), ImGuiCond_Once);
			if (ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_None))
			{
				ImGui::Text("Message");
				ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 200), ImGuiWindowFlags_NoTitleBar);

				for (std::string message : playerManager->Getmessages()) {
					ImGui::Text(u8"%s", message.c_str());
				}
				ImGui::EndChild();
				ImGui::Spacing();

				ImGui::InputText("Message", input, sizeof(input));
				if (playerManager->GetMyPlayer()->Getteamnumber() != 0)
					if (ImGui::Button("Send"))
					{
						if (strcmp(input, "") != 0)
						{
							// ���M����
							connection->SendMessages(input);
						}
						input[0] = '\0';
					}
			}
			ImGui::End();
		}


		if(connectionflag)
		{
			ImGui::SetNextWindowPos(ImVec2(500, 200), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
			// begin����end�܂ł̓��e���o����
			if (ImGui::Begin("login", nullptr, ImGuiWindowFlags_None))
			{
				if (playerManager->GetMyPlayerID() == 0 && !signupflag && !signinflag)
				{
					if (ImGui::Button("sign up"))
					{
						signupflag = true;
					}
					if (ImGui::Button("sign in"))
					{
						signinflag = true;
					}if (ImGui::Button("geustlogin"))
					{
						geustloginflag = true;
						connection->SendGeustLogin();
					}
				}
				else if(signupflag || signinflag)
				{
					if (ImGui::Button("back"))
					{
						memset(name, 0, sizeof(name));
						memset(pass, 0, sizeof(pass));
						signupflag = false;
						signinflag = false;
					}
				}
				if (signupflag && !playerManager->GetSignUp())
				{
					ImGui::Text("Sign Up");
					ImGui::InputText("Name", name, sizeof(name));
					ImGui::InputText("password", pass, sizeof(pass));
					if (strcmp(name, "") != 0 && strcmp(pass, "") != 0)
						if (ImGui::Button("Decision"))
						{
							connection->SendSignUp(name, pass);
							signupflag = false;
						}
				}

				if (signinflag&& !playerManager->GetSignIn())
				{
					ImGui::Text("Sign In");
					ImGui::InputText("Name", name, sizeof(name));
					ImGui::InputText("password", pass, sizeof(pass));
					if (strcmp(pass, "") != 0)
					{
						if (ImGui::Button("Decision"))
						connection->SendSignIn(name, pass);
						
					}
				}
				if (playerManager->GetMyPlayerID() > 0)
				{
					ImGui::Text("Login OK");
					if (!searchflag)
					{
						ImGui::InputScalar("Input short", ImGuiDataType_S16, &searchId);
						ImGui::Text("Current value: %d", searchId);
						if (ImGui::Button("IDSerach"))
						{
							connection->SendIdSearch(searchId);
							searchflag = true;
						}
					}
					else
					{
						//��������
						if (playerManager->GetSearchResult())
						{
							//����
							ImGui::Text("SearchName: %s", playerManager->GetSearchName());
							ImGui::Text("MyID: %d", playerManager->GetMyPlayerID());
							ImGui::Text("recvID: %d", playerManager->GetSearchId());
							//������ID���ǂ���
							if (playerManager->GetMyPlayerID() != playerManager->GetSearchId())
							{
								if (ImGui::Button("FriendRequest"))
								{
									connection->SendFriendRequest(playerManager->GetSearchId());
									searchflag = false;
									searchId = false;
								}
								if (ImGui::Button("Back"))
								{
									searchflag = false;
									searchId = false;
								}
							}
							else
							{
								ImGui::Text("My ID");
								if (ImGui::Button("Back"))
								{
									searchflag = false;
									searchId = false;
								}
							}
						}
						else
						{
							//���s
							ImGui::Text("SearchError");
							if (ImGui::Button("Back"))
							{
								searchflag = false;
								searchId = false;
							}

						}
					}
				}
				if (playerManager->GetSendertId() > 0)
				{
					ImGui::Text("SendertName: %s", playerManager->GetSenderName());
					ImGui::Text("SenderID: %d", playerManager->GetSendertId());
					//���F
					if (ImGui::Button("Approval"))
					{
						char myname[10],youname[10];

						strcpy_s(myname, playerManager->GetMyPlayer()->GetName());
						strcpy_s(youname, playerManager->GetSenderName());
						connection->SendFriendApproval(
							playerManager->GetSendertId(),
							playerManager->GetMyPlayerID(),
							myname,
							youname);
					}
					ImGui::SameLine();
					//�۔F
					if (ImGui::Button("Denial"))
					{
						playerManager->ResetSenderName();
						playerManager->SetSendertId(0);
					}
				}
				
			}
			ImGui::End();
		}
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
	const float gaugeWidth = 50.0f;
	const float gaugeHeight = 70.0f;
	int ID = playerManager->GetMyPlayerID();

	// �v���C���[ID�̌��������߂�
	int numDigits = 1;
	int tempID = 1;
	while (tempID >= 10)
	{
		tempID /= 10;
		numDigits++;
	}

	
	// 2D�X�v���C�g�`��
	{
		float positionX = scereenPosition.x - 7;
		float positionY = scereenPosition.y;
		int digit = 0;

		// �e����`�悷�郋�[�v
		for (int i = numDigits - 1; i >= 0; --i)
		{
			// �e���̐��l���擾
			digit = (ID / static_cast<int>(pow(10, i))) % 10;
			// �X�v���C�g��`��
			sprites[static_cast<int>(Spritenumber::Number)]->Render(dc,
				positionX, positionY,
				15, 15,
				gaugeWidth * digit,gaugeHeight,
				gaugeWidth, gaugeHeight,
				0.0f,
				1, 0, 0, 1);

			// ���̌��̈ʒu�Ɉړ�
			positionX += 15;
		}
	}
}

void SceneStandby::RenderTeamNumber(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{

	float positionX = 150;
	float positionY = 70;
	sprites[static_cast<int>(Spritenumber::TeamNumber)]->Render(dc,
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

	// 2D�X�v���C�g�`��
	{
		float numberposX = positionX +90;
		float numberposY = positionY+8;
		int digit = 0;

		// �e����`�悷�郋�[�v
		for (int i = numDigits - 1; i >= 0; --i)
		{
			// �e���̐��l���擾
			digit = (ID / static_cast<int>(pow(10, i))) % 10;

			// �X�v���C�g��`��
			sprites[static_cast<int>(Spritenumber::Number)]->Render(dc,
				numberposX, numberposY,
				30, 30,
				gaugeWidth * digit+ digit, 0,
				gaugeWidth, gaugeHeight,
				0.0f,
				1, 1, 1, 1);

			// ���̌��̈ʒu�Ɉړ�
			numberposX += 20;
		}
	}
}

void SceneStandby::RenderTeam(ID3D11DeviceContext* dc)
{
	float positionX = 10;
	float positionY = 10;
	float sizeX = 100;
	float sizeY = 30;
	sprites[static_cast<int>(Spritenumber::Team)]->Render(dc,
		positionX, positionY,             //�`��ʒu
		sizeX, sizeY,             //�\���T�C�Y
		0, 0,                //�؂���͂��߈ʒu
		243,100,             //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);

	if (!teamscreenflag&&Uiclick(positionX, positionY, sizeX, sizeY))
	{
		teamscreenflag = true;
	}
}

void SceneStandby::RenderTeamSelect(ID3D11DeviceContext* dc)
{
	float positionX = 80;
	float positionY = 50;
	//�g�g��
	sprites[static_cast<int>(Spritenumber::TeamSelect)]->Render(dc,
		positionX, positionY, //�`��ʒu
		600, 250,               //�\���T�C�Y
		0, 0,                 //�؂���͂��߈ʒu
		1000,400,           //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);


	float sizeX = 200;
	float sizeY = 150;
	//�`�[���쐬�{�^��
	sprites[static_cast<int>(Spritenumber::TeamCreate)]->Render(dc,
		positionX + 70, positionY + 60, //�`��ʒu
		sizeX, sizeY,               //�\���T�C�Y
		0, 0,                 //�؂���͂��߈ʒu
		800, 600,           //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);

		if (Uiclick(positionX + 70, positionY + 60, 200, 150))
		{
			teamscreenflag = false;
			sendteamcreate = true;
		}

	//�`�[�������{�^��
	sprites[static_cast<int>(Spritenumber::TeamJoin)]->Render(dc,
		positionX+320, positionY+60, //�`��ʒu
		sizeX, sizeY,               //�\���T�C�Y
		0, 0,                 //�؂���͂��߈ʒu
		800,600,           //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);
	
		if (Uiclick(positionX + 320, positionY + 60, sizeX, sizeY))
		{
			teamscreenflag = false;
			numberinputflag = true;
		}
	
	//����{�^��
	sprites[static_cast<int>(Spritenumber::Close)]->Render(dc,
		positionX + 565, positionY-5, //�`��ʒu
		40,40,               //�\���T�C�Y
		0, 0,                 //�؂���͂��߈ʒu
		100,100,           //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);

	if (Uiclick(positionX + 565, positionY - 5, 40, 40))
	{
		teamscreenflag = false;
	}
}

void SceneStandby::RenderTeamJoin(ID3D11DeviceContext* dc)
{
	float positionX = 80;
	float positionY = 30;
	//�g�g��
	sprites[static_cast<int>(Spritenumber::TeamJoinFrame)]->Render(dc,
		positionX, positionY,   //�`��ʒu
		600, 300,               //�\���T�C�Y
		0, 0,                   //�؂���͂��߈ʒu
		1000, 400,              //�摜�T�C�Y
		0.0f,
		1, 1, 1, 1);

	//�{�^��������
	
	int count = 0;
	int sizeX = 90;
	int sizeY = 30;
	
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			++count;
			sprites[static_cast<int>(Spritenumber::TeamjoinNumber)]->Render(dc,
				positionX + 130 + (j * 100), positionY + 140+(i*40), //�`��ʒu
				sizeX, sizeY,                      //�\���T�C�Y
				0 + (j * 300), 0+(i*100),    //�؂���͂��߈ʒu
				300, 100,                    //�摜�T�C�Y
				0.0f,
				1, 1, 1, 1);

			int posx = positionX + 130 + (j * 100);
			int posy = positionY + 140 + (i * 40);
			posxy[count-1][0] = posx;
			posxy[count-1][1] = posy;
			
		}
	}
	if (numbers.size() < 4)
	{
		if (Uiclick(posxy[0][0], posxy[0][1], sizeX, sizeY))numbers.push_back(1);
		if (Uiclick(posxy[1][0], posxy[1][1], sizeX, sizeY))numbers.push_back(2);
		if (Uiclick(posxy[2][0], posxy[2][1], sizeX, sizeY))numbers.push_back(3);
		if (Uiclick(posxy[3][0], posxy[3][1], sizeX, sizeY))numbers.push_back(4);
		if (Uiclick(posxy[4][0], posxy[4][1], sizeX, sizeY))numbers.push_back(5);
		if (Uiclick(posxy[5][0], posxy[5][1], sizeX, sizeY))numbers.push_back(6);
		if (Uiclick(posxy[6][0], posxy[6][1], sizeX, sizeY))numbers.push_back(7);
		if (Uiclick(posxy[7][0], posxy[7][1], sizeX, sizeY))numbers.push_back(8);
		if (Uiclick(posxy[8][0], posxy[8][1], sizeX, sizeY))numbers.push_back(9);
		if (Uiclick(posxy[10][0], posxy[10][1], sizeX, sizeY))numbers.push_back(0);
	}
	if (numbers.size() > 0) 
	{
		if (Uiclick(posxy[9][0], posxy[9][1], sizeX, sizeY))numbers.pop_back();
	}
	if (numbers.size() ==4)
	{
		if (Uiclick(posxy[11][0], posxy[11][1], sizeX, sizeY))
		{
			sendteamjoin = true;
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
				sprites[static_cast<int>(Spritenumber::Number)]->Render(dc,
					numberposX+(51*i), numberposY,
					30, 30,
					gaugeWidth * number, 0,
					gaugeWidth, gaugeHeight,
					0.0f,
					1, 1, 1, 1);
			}
		}
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


