#include "SceneConnection.h"
#include "Graphics/Graphics.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneStandby.h"
#include <thread>
#include <fstream>

#include <nlohmann/json.hpp>
//#include <openssl/ssl.h>
//#include <openssl/err.h>

using json = nlohmann::json;
//#pragma comment(lib, "libssl.lib")
//#pragma comment(lib, "libcrypto.lib")
// ������
void SceneConnection::Initialize()
{

    //�l�b�g���[�N�ڑ����s
  // �X�v���C�g������
    sprites[static_cast<int>(SpriteNumber::NetError)] = std::make_unique<Sprite>("Data/Sprite/neterror.png");
    //OK�{�^��
    sprites[static_cast<int>(SpriteNumber::OK)] = std::make_unique<Sprite>("Data/Sprite/OK.png");
    //
    sprites[static_cast<int>(SpriteNumber::Guest)] = std::make_unique<Sprite>("Data/Sprite/guest.png");
    //
    sprites[static_cast<int>(SpriteNumber::Login)] = std::make_unique<Sprite>("Data/Sprite/login.png");
    //
    sprites[static_cast<int>(SpriteNumber::NewLogin)] = std::make_unique<Sprite>("Data/Sprite/newlogin.png");

    sprites[static_cast<int>(SpriteNumber::Name)] = std::make_unique<Sprite>("Data/Sprite/font1.png");

    //��x����
    if (!SceneManager::Instance().GetconnectionInitialized())
    {
        SceneManager::Instance().SetconnectionInitialized(true);
        //������
        SceneManager::Instance().PlayermanagerInitialize();
        SceneManager::Instance().ConnectionInitialize();


        // �v���C���[�}�l�W���[������
        playerManager = SceneManager::Instance().GetPlayerManager();

        connection = SceneManager::Instance().GetConnection();
        //	�T�[�o�[�Ƃ̐ڑ��O��ID�O
        Player* player = new Player();
        player->SetPlayerID(0);						//�����ID�����X�g�b�N
        player->Setoperation(false);
        player->SetPosition(DirectX::XMFLOAT3(0, 0, 0)); //�����ʒu
        player->SetAngle({ 0.0f,3.0f,0.0f });

        playerManager->SetMyPlayerID(0);
        playerManager->AddPlayer(player);
        playerManager->GetPlayer(0)->SetReady(true);

        //����������
        playerManager->AddPlayersGenerateCount();
        connection->SetPlayerManager(playerManager);
    }
    else
    {
        // �v���C���[�}�l�W���[������
        playerManager = SceneManager::Instance().GetPlayerManager();

        connection = SceneManager::Instance().GetConnection();
    }

    //�ڑ�����
    if (!connection->isConnection)
    {
        connection->Initialize();
    }


}
// �I����
void SceneConnection::Finalize()
{
  
}
// �X�V����
void SceneConnection::Update(float elapsedTime)
{ 
    //�ڑ�����
    //�T�C���C���T�C���A�b�v�Ŏ󂯎�肪�I�������ɕύX����
    if (connection->isConnection && playerManager->GetMyPlayerID()>0)
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneStandby));
    }
   
    if (isNewLogin)
    {
        //NewLogin();
        //strcpy_s(name, "isNewLogin");
    }
    if ( isLogin)
    {
        strcpy_s(name, "isLogin");
     
    }
}
// �`�揈��
void SceneConnection::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };   // RGBA(0.0~1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    // 2D�X�v���C�g�`��
    RenderName(dc);
    if (!connection->isConnection)
    {
        RenderNetError(dc);
    }

    if (connection->isConnection && !isLogin&&!isNewLogin)
    {
        RenderLogin(dc);
    }

   ImGui::SetNextWindowPos(ImVec2(500, 200), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
   // begin����end�܂ł̓��e���o����
   if (ImGui::Begin("login", nullptr, ImGuiWindowFlags_None))
   {
       ImGui::Text("name: % s", name);
      // if(ImGui::InputInt("ID",playerManager->get))
      //if (isNewLogin)
      //{
      //    ImGui::Text("login");
      //    ImGui::InputText("Name", name, sizeof(name));
      //    ImGui::InputText("password", pass, sizeof(pass));
      //    if (strcmp(name, "") != 0 && strcmp(pass, "") != 0)
      //        if (ImGui::Button("Decision"))
      //        {
      //            connection->SendSignUp(name, pass);
      //        }
      //}
       if (isLogin)
       {
           ImGui::Text("Newlogin");
           ImGui::InputText("Name", name, sizeof(name));
           ImGui::InputText("password", pass, sizeof(pass));
           if (strcmp(pass, "") != 0)
           {
               if (ImGui::Button("Decision"))
                   connection->SendSignIn(name, pass);
           }
       }

       if (isLogin || isNewLogin)
       {
           if (ImGui::Button("back"))
           {
               memset(name, 0, sizeof(name));
               memset(pass, 0, sizeof(pass));
               isNewLogin = false;
               isLogin = false;
           }
       }
       ImGui::End();
   }

}

void SceneConnection::NewLogin()
{
    WSADATA wsaData{};
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSA�̏������Ɏ��s���܂����B" << std::endl;
        return ;
    }

    // �\�P�b�g�̍쐬
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "�\�P�b�g�쐬�Ɏ��s���܂����B" << std::endl;
        WSACleanup();
        return ;
    }

    // �ڑ���̃A�h���X���ݒ�ilocalhost:7189�j
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7189);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IPv4�A�h���X��ݒ�

    // �T�[�o�[�֐ڑ�
    if (connect(sock, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) != 0) {
        std::cerr << "�T�[�o�[�ւ̐ڑ��Ɏ��s���܂����B" << std::endl;
        closesocket(sock);
        WSACleanup();
        return ;
    }
    std::cout << "�T�[�o�[�ɐڑ����܂����B" << std::endl;

    // HTTP���N�G�X�g���쐬���đ��M
    std::string request =
        "GET /Registry/Registration HTTP/1.1\r\n"
        "Host: localhost:7189\r\n"
        "Connection: close\r\n\r\n";

    if (send(sock, request.c_str(), request.length(), 0) == SOCKET_ERROR) {
        std::cerr << "���N�G�X�g���M�Ɏ��s���܂����B" << std::endl;
        closesocket(sock);
        WSACleanup();
        return ;
    }

    // �f�[�^�̎�M
    std::vector<char> data;
    char buffer[1024];
    int bytesReceived;
    while ((bytesReceived = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        data.insert(data.end(), buffer, buffer + bytesReceived);
    }

    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "�f�[�^��M�Ɏ��s���܂����B" << std::endl;
        closesocket(sock);
        WSACleanup();
        return ;
    }

    // ��M�f�[�^��JSON�Ƃ��ĉ��
    try {
        std::string response(data.begin(), data.end());
        auto jsonResponse = nlohmann::json::parse(response);
        std::cout << "��͂��ꂽJSON�f�[�^:\n" << jsonResponse.dump(4) << std::endl;
    }
    catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON��̓G���[: " << e.what() << std::endl;
    }

    // ��M�f�[�^���t�@�C���ɏ�������
    std::ofstream outputFile("response.json", std::ios::binary);
    if (outputFile.is_open()) {
        outputFile.write(data.data(), data.size());
        std::cout << "response.json �ɏ������݂܂����B" << std::endl;
    }
    else {
        std::cerr << "�t�@�C���̏������݂Ɏ��s���܂����B" << std::endl;
    }

    // �\�P�b�g�̃N���[���A�b�v
    closesocket(sock);
    WSACleanup();

    isNewLogin = false;
}

void SceneConnection::RenderNetError(ID3D11DeviceContext* dc)
{
    float positionX = 50;
    float positionY = 0;

    //�g�g��
    sprites[static_cast<int>(SpriteNumber::NetError)]->Render(dc,
        positionX, positionY, //�`��ʒu
        600, 350,               //�\���T�C�Y
        0, 0,                 //�؂���͂��߈ʒu
        750, 500,           //�摜�T�C�Y
        0.0f,
        1, 1, 1, 1);

    //OK�{�^��
    sprites[static_cast<int>(SpriteNumber::OK)]->Render(dc,
        positionX + 190, positionY + 250, //�`��ʒu
        150, 50,               //�\���T�C�Y
        0, 0,                 //�؂���͂��߈ʒu
        400, 200,           //�摜�T�C�Y
        0.0f,
        1, 1, 1, 1);

    if (UiClick(positionX + 190, positionY + 250, 150, 50))
    {
       
       if (!connection->isConnection)
       {
           connection->Initialize();
         
           //�ڑ�����
           if (connection->isConnection)
           {
              // connection->SetplayerManager(playerManager);
               //SceneManager::Instance().ChangeScene(new SceneLoading(new SceneStandby));
           }
           //���s
           else
           {
               SceneManager::Instance().ChangeScene(new SceneLoading(new SceneConnection));
           }
       }
    }
}

void SceneConnection::RenderLogin(ID3D11DeviceContext* dc)
{
    Mouse& mouse = Input::Instance().GetMouse();
    float positionX = 20;
    float positionY = 150;

    float sizeX = 200;
    float sizeY = 100;

    //�g�g��
    sprites[static_cast<int>(SpriteNumber::Guest)]->Render(dc,
        positionX, positionY, //�`��ʒu
        sizeX,sizeY,              //�\���T�C�Y
        0, 0,                 //�؂���͂��߈ʒu
        500,200,               //�摜�T�C�Y
        0.0f,
        1, 1, 1, 1);

    if (UiClick(positionX, positionY, sizeX, sizeY))
    {
        connection->SendGeustLogin();
    }

    sprites[static_cast<int>(SpriteNumber::Login)]->Render(dc,
        positionX+250, positionY, //�`��ʒu
        sizeX, sizeY,                 //�\���T�C�Y
        0, 0,                     //�؂���͂��߈ʒu
        500, 200,                 //�摜�T�C�Y
        0.0f,
        1, 1, 1, 1);

    if (UiClick(positionX+250, positionY, sizeX, sizeY))
    {
        isLogin = true;
    }

    sprites[static_cast<int>(SpriteNumber::NewLogin)]->Render(dc,
        positionX + 500, positionY, //�`��ʒu
        sizeX, sizeY,                  //�\���T�C�Y
        0, 0,                      //�؂���͂��߈ʒu
        500, 200,                  //�摜�T�C�Y
        0.0f,
        1, 1, 1, 1);

    if (UiClick(positionX+500, positionY, sizeX, sizeY))
    {
        isNewLogin = true;
    }
}

void SceneConnection::RenderName(ID3D11DeviceContext* dc)
{
    float positionX = 0;
    float positionY = 0;

    float sizeX = 32;
    float sizeY = 32;
 
    //�g�g��
    for (int i = 0; i < 9; ++i)
    {
        if (name[i] == '\0')break;
        int number = static_cast<int>(name[i]);
        int width = number % 16;
        int height = number / 16;
        sprites[static_cast<int>(SpriteNumber::Name)]->Render(dc,
            positionX, positionY,      //�`��ʒu
            sizeX, sizeY,              //�\���T�C�Y
            sizeX*width, sizeY*height, //�؂���͂��߈ʒu
            sizeX, sizeY,              //�摜�T�C�Y
            0.0f,
            1, 1, 1, 1);
      
        positionX += sizeX/2;
    }
}

bool SceneConnection::UiClick(float posX, float posY, float sizeX, float sizeY)
{
    DirectX::XMFLOAT3 scereenPosition;
    Mouse& mouse = Input::Instance().GetMouse();

    if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
    {
        scereenPosition.x = static_cast<float>(mouse.GetPositionX());
        scereenPosition.y = static_cast<float>(mouse.GetPositionY());

        if (posX < scereenPosition.x && scereenPosition.x < posX + sizeX
            && posY < scereenPosition.y && scereenPosition.y < posY + sizeY)
        {
            Sleep(10);
            return true;
        }
    }
    return false;
}


