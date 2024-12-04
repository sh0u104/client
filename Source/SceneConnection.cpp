#include <WinSock2.h>		// WinSock version 2���g�p���邽�߂̃w�b�_
#include <WS2tcpip.h>		// WinSock version 2�Ŏg�p����l�X�ȃI�v�V�����̃w�b�_
//#include <windows.h> 

#include "SceneConnection.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneStandby.h"
#include "Graphics/Graphics.h"

#include <thread>
#include <fstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;


// OpenSSL�ǂݍ���
#include <openssl/crypto.h>		// openssl�̈Í����w�b�_
#include <openssl/ssl.h>		// openssl��ssl�ʐM�w�b�_
#include <openssl/err.h>		// openssl�̃G���[�w�b�_

#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

#include <conio.h>
#include "Logger.h"
// ������
void SceneConnection::Initialize()
{
  // �X�v���C�g������
    //�G���[
    sprites[static_cast<int>(SpriteNumber::NetError)] = std::make_unique<Sprite>("Data/Sprite/neterror.png");
    //OK�{�^��
    sprites[static_cast<int>(SpriteNumber::OK)] = std::make_unique<Sprite>("Data/Sprite/OK.png");
    //�Q�X�g
    sprites[static_cast<int>(SpriteNumber::Guest)] = std::make_unique<Sprite>("Data/Sprite/guest.png");
    //���O�C��
    sprites[static_cast<int>(SpriteNumber::Login)] = std::make_unique<Sprite>("Data/Sprite/login.png");
    //�V�K���O�C��
    sprites[static_cast<int>(SpriteNumber::NewLogin)] = std::make_unique<Sprite>("Data/Sprite/newlogin.png");
    //���O
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
   
    if (Getfile)
    {
        Getfile = false;
        httpPngDownload();
    }

    if (isSignup)
    {
        //Signup();
        //ID�擾
        if (httpSignup())
        {
            Logger::Print("�T�C���A�b�vhttp����");
        }
        else
        {
            Logger::Print("�T�C���A�b�vhttp�ڑ����s");
            ID = GetDataJson();
        }
       
        isSignup = false;

       
    }
    if (isSignin)
    {
        //Signin();
        
        //json�ŏ���������ID���擾
        ID = GetDataJson();
        //���O�擾
        if (httpSignin())
        {
            Logger::Print("�T�C���C��http����");
        }
        else
        {
            Logger::Print("�T�C���C��http�ڑ����s");
            if (NameJson())
            {
                Logger::Print("���Ojson�f�[�^�擾����");
            }
            else
            {
                Logger::Print("�l�[���擾���s");
            }
        }
       
        isSignin = false;
       
        playerManager->GetMyPlayer()->SetName(Name);
    }

    if (ID > 0 && playerManager->GetMyPlayerID() == 0)
    {
        //�����̃v���C���[�Ɏ�M����ID���悹��
        playerManager->GetMyPlayer()->SetPlayerID(ID);
        playerManager->SetMyPlayerID(ID);
        //���O�C���������Z
        playerManager->AddLoginCount();
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneStandby));
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
    {
        //���O
        RenderName(dc);

        //�ڑ��ł��Ȃ�������
        if (!connection->isConnection)
        {
            RenderNetError(dc);
        }

        //���O�C���̎��
        if (connection->isConnection && !isSignin && !isSignup)
        {
            RenderLogin(dc);
        }
    }
   ImGui::SetNextWindowPos(ImVec2(500, -10), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);
   // begin����end�܂ł̓��e���o����
   if (ImGui::Begin("login", nullptr, ImGuiWindowFlags_None))
   {
       if (ImGui::Button("Getfile"))
       {
           Getfile = true;
       }
      //ImGui::Text("name: % s", name);
      //ImGui::Text("userId: %d", ID);
      //ImGui::Text("userDay: %d", loginDay);
      //ImGui::InputText("Name", name, sizeof(name));
      //ImGui::InputText("password", pass, sizeof(pass));
      //isname = true;
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
       //if (isLogin)
       //{
       //    ImGui::Text("Newlogin");
       //    ImGui::InputText("Name", name, sizeof(name));
       //    ImGui::InputText("password", pass, sizeof(pass));
       //    if (strcmp(pass, "") != 0)
       //    {
       //        if (ImGui::Button("Decision"))
       //            connection->SendSignIn(name, pass);
       //    }
       //}
       //
      // if (isSignin || isSignup)
      // {
      //     if (ImGui::Button("back"))
      //     {
      //         memset(name, 0, sizeof(name));
      //         memset(pass, 0, sizeof(pass));
      //         isSignin = false;
      //         isSignup = false;
      //     }
      // }
       ImGui::End();
   }

}

void SceneConnection::Signin()
{

    std::string hostname = "localhost";
    std::string port = "5000";
    std::string path = "/Login/Login?userId=" + std::to_string(maxID);

    OPENSSL_init_ssl(0, nullptr);

    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    addrinfo hints = {}, * addrInfo = nullptr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSA���������s" << std::endl;
        return ;
    }

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(hostname.c_str(), port.c_str(), &hints, &addrInfo) != 0) {
        std::cerr << "�h���C������A�h���X�擾�Ɏ��s���܂���" << std::endl;
        WSACleanup();
        return ;
    }

    sock = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
    if (sock == INVALID_SOCKET) {
        std::cerr << "�\�P�b�g�̐����Ɏ��s���܂���" << std::endl;
        freeaddrinfo(addrInfo);
        WSACleanup();
        return ;
    }

    if (connect(sock, addrInfo->ai_addr, static_cast<int>(addrInfo->ai_addrlen)) == SOCKET_ERROR) {
        std::cerr << "connect�Ɏ��s���܂���" << std::endl;
        closesocket(sock);
        freeaddrinfo(addrInfo);
        WSACleanup();
        return ;
    }

    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        std::cerr << "SSL_CTX�̐����Ɏ��s���܂���" << std::endl;
        closesocket(sock);
        WSACleanup();
        return ;
    }

    SSL* ssl = SSL_new(ctx);
    if (!ssl) {
        std::cerr << "SSL�̐����Ɏ��s���܂���" << std::endl;
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return ;
    }

    if (SSL_set_fd(ssl, static_cast<int>(sock)) == 0) {
        std::cerr << "�\�P�b�g��SSL�̊֘A�t���Ɏ��s���܂���" << std::endl;
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return ;
    }

    if (SSL_connect(ssl) <= 0) {
        std::cerr << "SSL�ڑ��Ɏ��s���܂���" << std::endl;
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return ;
    }

    std::string request = "GET " + path + " HTTP/1.1\r\n"
        "Host: " + hostname + ":" + port + "\r\n"
        "Connection: Close\r\n\r\n";

    if (SSL_write(ssl, request.c_str(), static_cast<int>(request.length())) <= 0) {
        std::cerr << "���N�G�X�g�̑��M�Ɏ��s���܂���" << std::endl;
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return ;
    }

    std::vector<char> data;
    data.reserve(4096);
    char buf[1024];
    int size = 0;

    while ((size = SSL_read(ssl, buf, sizeof(buf))) > 0) {
        data.insert(data.end(), buf, buf + size);
    }

    if (size < 0) {
        std::cerr << "��M�G���[" << std::endl;
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return ;
    }

    // HTTP�w�b�_�[���폜����JSON�����̂ݒ��o
    auto headerEnd = std::search(data.begin(), data.end(), "\r\n\r\n", "\r\n\r\n" + 4);
    std::string response_body;
    if (headerEnd != data.end()) {
        // �w�b�_�[�̌�̃|�C���^���擾���ă{�f�B������ݒ�
        response_body = std::string(headerEnd + 4, data.end());

        // �`�����N�T�C�Y���폜���鏈��
        std::string cleaned_body;
        size_t pos = 0;

        while (pos < response_body.size()) {
            // �`�����N�T�C�Y�i16�i���\�L�j�̎擾
            size_t chunkEnd = response_body.find("\r\n", pos);
            if (chunkEnd == std::string::npos) break;

            // �`�����N�T�C�Y��16�i�����琔�l�ɕϊ�
            std::string chunkSizeHex = response_body.substr(pos, chunkEnd - pos);
            int chunkSize = std::stoi(chunkSizeHex, nullptr, 16);
            pos = chunkEnd + 2; // �`�����N�T�C�Y�̌�� `\r\n` ���X�L�b�v

            // �`�����N�f�[�^�̎擾
            cleaned_body += response_body.substr(pos, chunkSize);
            pos += chunkSize + 2; // �`�����N�f�[�^�̌�� `\r\n` ���X�L�b�v
        }

        response_body = cleaned_body;
    }

    std::ofstream file("signindata.json"); // �g���q�� .json �ɕύX
    if (file.is_open()) {
        file << response_body; // JSON������Ƃ��Ă��̂܂܏�������
        file.close();
        std::cout << "���X�|���X��'response.json'�ɕۑ�����܂����B" << std::endl;
    }
    else {
        throw std::runtime_error("�t�@�C�����J���܂���ł���");
    }

    

    // JSON�̓��e���o�͂��Ċm�F
    std::cout << "Response Body (after header removal): " << response_body << std::endl;

    // JSON�̃p�[�X
    nlohmann::json json = nlohmann::json::parse(response_body, nullptr, false);
    if (json.is_discarded() || !json.contains("user_profile") || !json["user_profile"].contains("userId")) {
        std::cerr << "JSON�̃p�[�X�Ɏ��s�܂���userId��������܂���" << std::endl;
        
        return;
    }

    std::string userName = json["user_profile"]["userName"];

    strcpy_s(Name, sizeof(Name), userName.c_str());

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    closesocket(sock);
    WSACleanup();
  
    isSignup = false;
}

bool SceneConnection::httpSignin()
{
    std::string hostname = "localhost";
    std::string port = "5000";
    std::string path = "/Login/Login?userId=" + std::to_string(maxID);

    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    addrinfo hints = {}, * addrInfo = nullptr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSA���������s" << std::endl;
        return false;
    }

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(hostname.c_str(), port.c_str(), &hints, &addrInfo) != 0) {
        std::cerr << "�h���C������A�h���X�擾�Ɏ��s���܂���" << std::endl;
        WSACleanup();
        return false;
    }

    sock = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
    if (sock == INVALID_SOCKET) {
        std::cerr << "�\�P�b�g�̐����Ɏ��s���܂���" << std::endl;
        freeaddrinfo(addrInfo);
        WSACleanup();
        return false;
    }

    if (connect(sock, addrInfo->ai_addr, static_cast<int>(addrInfo->ai_addrlen)) == SOCKET_ERROR) {
        std::cerr << "connect�Ɏ��s���܂���" << std::endl;
        closesocket(sock);
        freeaddrinfo(addrInfo);
        WSACleanup();
        return false;
    }

    // HTTP���N�G�X�g�̐���
    std::string request = "GET " + path + " HTTP/1.1\r\n"
        "Host: " + hostname + ":" + port + "\r\n"
        "Connection: Close\r\n\r\n";

    // ���N�G�X�g�̑��M
    if (send(sock, request.c_str(), static_cast<int>(request.length()), 0) == SOCKET_ERROR) {
        std::cerr << "���N�G�X�g�̑��M�Ɏ��s���܂���" << std::endl;
        closesocket(sock);
        freeaddrinfo(addrInfo);
        WSACleanup();
        return false;
    }

    // ���X�|���X�̎�M
    std::vector<char> data;
    data.reserve(4096);
    char buf[1024];
    int size = 0;

    while ((size = recv(sock, buf, sizeof(buf), 0)) > 0) {
        data.insert(data.end(), buf, buf + size);
    }

    if (size < 0) {
        std::cerr << "��M�G���[" << std::endl;
        closesocket(sock);
        freeaddrinfo(addrInfo);
        WSACleanup();
        return false;
    }

    // HTTP���X�|���X�̉��
    auto headerEnd = std::search(data.begin(), data.end(), "\r\n\r\n", "\r\n\r\n" + 4);
    std::string response_body;
    if (headerEnd != data.end()) {
        response_body = std::string(headerEnd + 4, data.end());

        // �`�����N�f�[�^�̏���
        std::string cleaned_body;
        size_t pos = 0;

        while (pos < response_body.size()) {
            size_t chunkEnd = response_body.find("\r\n", pos);
            if (chunkEnd == std::string::npos) break;

            std::string chunkSizeHex = response_body.substr(pos, chunkEnd - pos);
            int chunkSize = std::stoi(chunkSizeHex, nullptr, 16);
            pos = chunkEnd + 2;

            cleaned_body += response_body.substr(pos, chunkSize);
            pos += chunkSize + 2;
        }

        response_body = cleaned_body;
    }
        

    std::ofstream file("signindata.json");
    if (file.is_open()) {
        file << response_body;
        file.close();
        std::cout << "���X�|���X��'signindata.json'�ɕۑ�����܂����B" << std::endl;
    }
    else {
        throw std::runtime_error("�t�@�C�����J���܂���ł���");
    }

    // JSON�̓��e���o�͂��Ċm�F
    Logger::Print("json�f�[�^: %s", response_body.c_str());

    // JSON�̃p�[�X
        nlohmann::json json = nlohmann::json::parse(response_body, nullptr, false);
    if (json.is_discarded() || !json.contains("user_profile") || !json["user_profile"].contains("userId")) {
        std::cerr << "JSON�̃p�[�X�Ɏ��s�܂���userId��������܂���" << std::endl;
        return false;
    }

    std::string userName = json["user_profile"]["userName"];

    strcpy_s(Name, sizeof(Name), userName.c_str());

    closesocket(sock);
    freeaddrinfo(addrInfo);
    WSACleanup();

    isSignup = false;

    return true;
}

void SceneConnection::Signup()
{
    std::string hostname = "localhost";
    std::string port = "7189";
    std::string path = "/Registry/Registration";

    OPENSSL_init_ssl(0, nullptr);

    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    addrinfo hints = {}, * addrInfo = nullptr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        Logger::Print("WSA���������s");
        return;
    }

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(hostname.c_str(), port.c_str(), &hints, &addrInfo) != 0) {
        Logger::Print("�h���C������A�h���X�擾�Ɏ��s���܂���");
        WSACleanup();
        return;
    }

    sock = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
    if (sock == INVALID_SOCKET) {
        Logger::Print("�\�P�b�g�̐����Ɏ��s���܂���");
        freeaddrinfo(addrInfo);
        WSACleanup();
        return;
    }

    if (connect(sock, addrInfo->ai_addr, static_cast<int>(addrInfo->ai_addrlen)) == SOCKET_ERROR) {
        Logger::Print("connect�Ɏ��s���܂���");
        closesocket(sock);
        freeaddrinfo(addrInfo);
        WSACleanup();
        return;
    }

    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        Logger::Print("SSL_CTX�̐����Ɏ��s���܂���");
        closesocket(sock);
        WSACleanup();
        return;
    }

    SSL* ssl = SSL_new(ctx);
    if (!ssl) {
        Logger::Print("SSL�̐����Ɏ��s���܂���");
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return;
    }

    if (SSL_set_fd(ssl, static_cast<int>(sock)) == 0) {
        Logger::Print("�\�P�b�g��SSL�̊֘A�t���Ɏ��s���܂���");
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return;
    }

    if (SSL_connect(ssl) <= 0) {
        // SSL�ڑ��Ɏ��s�����ꍇ�AOpenSSL�̃G���[�X�^�b�N����G���[���擾
        unsigned long err_code = ERR_get_error();
        if (err_code != 0) {
            // �G���[�R�[�h�𕶎���ɕϊ�
            const char* error_string = ERR_error_string(err_code, nullptr);

            // Logger::Print�ŃG���[���b�Z�[�W��\��
            Logger::Print("SSL�ڑ��Ɏ��s���܂���: %s", error_string);
        }
        else {
            // �G���[�R�[�h���擾�ł��Ȃ��ꍇ
            Logger::Print("SSL�ڑ��Ɏ��s���܂���: �s���ȃG���[");
        }

        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return;
    }

    std::string request = "GET " + path + " HTTP/1.1\r\n"
        "Host: " + hostname + ":" + port + "\r\n"
        "Connection: Close\r\n\r\n";

    if (SSL_write(ssl, request.c_str(), static_cast<int>(request.length())) <= 0) {
        Logger::Print("���N�G�X�g�̑��M�Ɏ��s���܂���");
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return;
    }

    std::vector<char> data;
    data.reserve(4096);
    char buf[1024];
    int size = 0;

    while ((size = SSL_read(ssl, buf, sizeof(buf))) > 0) {
        data.insert(data.end(), buf, buf + size);
    }

    if (size < 0) {
        Logger::Print("��M�G���[");
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return;
    }

    // HTTP�w�b�_�[���폜����JSON�����̂ݒ��o
    auto headerEnd = std::search(data.begin(), data.end(), "\r\n\r\n", "\r\n\r\n" + 4);
    std::string response_body;
    if (headerEnd != data.end()) {
        // �w�b�_�[�̌�̃|�C���^���擾���ă{�f�B������ݒ�
        response_body = std::string(headerEnd + 4, data.end());

        // �`�����N�T�C�Y���폜���鏈��
        std::string cleaned_body;
        size_t pos = 0;

        while (pos < response_body.size()) {
            // �`�����N�T�C�Y�i16�i���\�L�j�̎擾
            size_t chunkEnd = response_body.find("\r\n", pos);
            if (chunkEnd == std::string::npos) break;

            // �`�����N�T�C�Y��16�i�����琔�l�ɕϊ�
            std::string chunkSizeHex = response_body.substr(pos, chunkEnd - pos);
            int chunkSize = std::stoi(chunkSizeHex, nullptr, 16);
            pos = chunkEnd + 2; // �`�����N�T�C�Y�̌�� `\r\n` ���X�L�b�v

            // �`�����N�f�[�^�̎擾
            cleaned_body += response_body.substr(pos, chunkSize);
            pos += chunkSize + 2; // �`�����N�f�[�^�̌�� `\r\n` ���X�L�b�v
        }

        response_body = cleaned_body;
    }

    std::ofstream file("response.json"); // �g���q�� .json �ɕύX
    if (file.is_open()) {
        file << response_body; // JSON������Ƃ��Ă��̂܂܏�������
        file.close();
        Logger::Print("���X�|���X��'response.json'�ɕۑ�����܂����B");
    }
    else {
        Logger::Print("�t�@�C�����J���܂���ł���");
    }



    // JSON�̓��e���o�͂��Ċm�F
    std::cout << "Response Body (after header removal): " << response_body << std::endl;

    // JSON�̃p�[�X
    nlohmann::json json = nlohmann::json::parse(response_body, nullptr, false);
    if (json.is_discarded() || !json.contains("user_profile") || !json["user_profile"].contains("userId")) {
        Logger::Print("JSON�̃p�[�X�Ɏ��s�܂���userId��������܂���");
        return;
    }

    std::string userId = json["user_profile"]["userId"];
    std::string userIdNumber = userId.substr(3);

    std::cout << "User ID Number: " << userIdNumber << std::endl;
    Logger::Print("UserID %d", userIdNumber);
    //playerManager->GetMyPlayer()->SetLoginDay(json["user_login"]["loginDay"]);
    Logger::Print("Sinup����");
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    closesocket(sock);
    WSACleanup();

    isSignup = false;
}


bool SceneConnection::httpSignup()
{
    std::string hostname = "localhost";
    std::string port = "5000";
    std::string path = "/Registry/Registration";

    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    addrinfo hints = {}, * addrInfo = nullptr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        Logger::Print("WSA���������s");
        return false;
    }

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(hostname.c_str(), port.c_str(), &hints, &addrInfo) != 0) {
        Logger::Print("�h���C������A�h���X�擾�Ɏ��s���܂���");
        WSACleanup();
        return false;
    }

    sock = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
    if (sock == INVALID_SOCKET) {
        Logger::Print("�\�P�b�g�̐����Ɏ��s���܂���");
        freeaddrinfo(addrInfo);
        WSACleanup();
        return false;
    }

    if (connect(sock, addrInfo->ai_addr, static_cast<int>(addrInfo->ai_addrlen)) == SOCKET_ERROR) {
        Logger::Print("connect�Ɏ��s���܂���");
        closesocket(sock);
        freeaddrinfo(addrInfo);
        WSACleanup();
        return false;
    }

    std::string request = "GET " + path + " HTTP/1.1\r\n"
        "Host: " + hostname + ":" + port + "\r\n"
        "Connection: Close\r\n\r\n";

    if (send(sock, request.c_str(), static_cast<int>(request.length()), 0) == SOCKET_ERROR) {
        Logger::Print("���N�G�X�g�̑��M�Ɏ��s���܂���");
        closesocket(sock);
        freeaddrinfo(addrInfo);
        WSACleanup();
        return false;
    }

    std::vector<char> data;
    data.reserve(4096);
    char buf[1024];
    int size = 0;

    while ((size = recv(sock, buf, sizeof(buf), 0)) > 0) {
        data.insert(data.end(), buf, buf + size);
    }

    if (size < 0) {
        Logger::Print("��M�G���[");
        closesocket(sock);
        freeaddrinfo(addrInfo);
        WSACleanup();
        return false;
    }

    closesocket(sock);
    freeaddrinfo(addrInfo);
    WSACleanup();

    std::string response_body;
    //����Ȃ��`�����N�T�C�Y������
    {
        auto headerEnd = std::search(data.begin(), data.end(), "\r\n\r\n", "\r\n\r\n" + 4);

        if (headerEnd != data.end()) {
            // �w�b�_�[�̌�̃|�C���^���擾���ă{�f�B������ݒ�
            response_body = std::string(headerEnd + 4, data.end());

            // �`�����N�T�C�Y���폜���鏈��
            std::string cleaned_body;
            size_t pos = 0;

            while (pos < response_body.size()) {
                // �`�����N�T�C�Y�i16�i���\�L�j�̎擾
                size_t chunkEnd = response_body.find("\r\n", pos);
                if (chunkEnd == std::string::npos) break;

                // �`�����N�T�C�Y��16�i�����琔�l�ɕϊ�
                std::string chunkSizeHex = response_body.substr(pos, chunkEnd - pos);
                int chunkSize = std::stoi(chunkSizeHex, nullptr, 16);
                pos = chunkEnd + 2; // �`�����N�T�C�Y�̌�� `\r\n` ���X�L�b�v

                // �`�����N�f�[�^�̎擾
                cleaned_body += response_body.substr(pos, chunkSize);
                pos += chunkSize + 2; // �`�����N�f�[�^�̌�� `\r\n` ���X�L�b�v
            }

            response_body = cleaned_body;  // �`�����N���폜�����{�f�B��ێ�
        }

        else {
            // �w�b�_�[��������Ȃ���΃G���[�n���h�����O
            std::cerr << "�w�b�_�[������������܂���ł���" << std::endl;
            return false;
        }
    }

    // JSON�̕ۑ�
    std::ofstream file("response.json");
    if (file.is_open()) {
        file << response_body;
        file.close();
        Logger::Print("���X�|���X��'response.json'�ɕۑ�����܂����B");
    }
    else {
        Logger::Print("�t�@�C�����J���܂���ł���");
        return false;
    }

  
    // JSON�̓��e���o�͂��Ċm�F
    Logger::Print("json�f�[�^: %s", response_body.c_str());

    // JSON�̃p�[�X
    nlohmann::json json = nlohmann::json::parse(response_body, nullptr, false);
    if (json.is_discarded()) {
        Logger::Print("JSON�̃p�[�X�Ɏ��s���܂���");
        return false;
    }

    // user_profile �̃`�F�b�N
    if (!json.contains("user_profile") || !json["user_profile"].contains("userId")) {
        Logger::Print("JSON���ɕK�v�ȃf�[�^������܂���");
        return false;
    }

    std::string userId = json["user_profile"]["userId"];
    std::string userIdNumber = userId.substr(5);

    Logger::Print("UserID %s", userIdNumber);
    Logger::Print("SignUp����");
    ID = std::stoi(userIdNumber);

    return true;
}

int SceneConnection::GetDataJson()
{
    std::ifstream file("response.json");

    if (!file.is_open()) {
        Logger::Print("�t�@�C�����J���܂���ł���");
    }
    json jsonData;
    file >> jsonData;
    file.close();
    int userId = 0;
    if (jsonData.contains("user_profile") && jsonData["user_profile"].contains("userId")) {
        std::string userIdStr = jsonData["user_profile"]["userId"];
        maxID = std::stoi(userIdStr);

        std::string userIdNumber = userIdStr.substr(5);
        userId = std::stoi(userIdNumber); // �����񂩂�int�֕ϊ�
        Logger::Print("response.json�ǂݍ��ݐ���");
        std::cout << "userId: " << userId << std::endl;
    }
    else {
        Logger::Print("userId��������܂���");
    }

    return userId;

}

bool SceneConnection::NameJson()
{
    std::ifstream file("signindata.json");

    if (!file.is_open()) {
        Logger::Print("�t�@�C�����J���܂���ł���");
        return false;
    }
    json jsonData;
    file >> jsonData;
    file.close();

    std::string userName;
    if (jsonData.contains("user_profile") && jsonData["user_profile"].contains("userName")) {
        userName = jsonData["user_profile"]["userName"];
      
        Logger::Print("userName��������܂���\%s", userName);
        playerManager->GetMyPlayer()->SetLoginDay(jsonData["user_login"]["loginDay"]);
    }
    else {
        Logger::Print("userId��������܂���");
        return false;
    }

    strcpy_s(Name, sizeof(Name), userName.c_str());
    return true;
}


void SceneConnection::pngDownload()
{
    std::string hostname = "localhost";
    std::string port = "5000";
    const std::string path = "/File/DownloadPng";
    const std::string outputFile = "login.png";
    OPENSSL_init_ssl(0, nullptr);

    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    addrinfo hints = {}, * addrInfo = nullptr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        Logger::Print("WSA���������s");
        return;
    }

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(hostname.c_str(), port.c_str(), &hints, &addrInfo) != 0) {
        Logger::Print("�h���C������A�h���X�擾�Ɏ��s���܂���");
        WSACleanup();
        return;
    }

    sock = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
    if (sock == INVALID_SOCKET) {
        Logger::Print("�\�P�b�g�̐����Ɏ��s���܂���");
        freeaddrinfo(addrInfo);
        WSACleanup();
        return;
    }

    if (connect(sock, addrInfo->ai_addr, static_cast<int>(addrInfo->ai_addrlen)) == SOCKET_ERROR) {
        Logger::Print("connect�Ɏ��s���܂���");
        closesocket(sock);
        freeaddrinfo(addrInfo);
        WSACleanup();
        return;
    }

    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        Logger::Print("SSL_CTX�̐����Ɏ��s���܂���");
        closesocket(sock);
        WSACleanup();
        return;
    }

    SSL* ssl = SSL_new(ctx);
    if (!ssl) {
        Logger::Print("SSL�̐����Ɏ��s���܂���");
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return;
    }

    if (SSL_set_fd(ssl, static_cast<int>(sock)) == 0) {
        Logger::Print("�\�P�b�g��SSL�̊֘A�t���Ɏ��s���܂���");
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return;
    }

    if (SSL_connect(ssl) <= 0) {
        Logger::Print("SSL�ڑ��Ɏ��s���܂���");
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return;
    }

    std::string request = "GET " + path + " HTTP/1.1\r\n"
        "Host: " + hostname + ":" + port + "\r\n"
        "Connection: Close\r\n\r\n";

    if (SSL_write(ssl, request.c_str(), static_cast<int>(request.length())) <= 0) {
        Logger::Print("���N�G�X�g�̑��M�Ɏ��s���܂���");
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return;
    }

    std::vector<char> data;
    data.reserve(4096);
    char buf[1024];
    int size = 0;

    while ((size = SSL_read(ssl, buf, sizeof(buf))) > 0) {
        data.insert(data.end(), buf, buf + size);
    }

    if (size < 0) {
        Logger::Print("��M�G���[");
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return;
    }

  auto headerEnd = std::search(data.begin(), data.end(), "\r\n\r\n", "\r\n\r\n" + 4);
  if (headerEnd != data.end()) {
      data.erase(data.begin(), headerEnd + 4);
  }
  else {
      Logger::Print("HTTP���X�|���X�w�b�_�[��������܂���");
      return;
  }
    // �t�@�C�������o��
    std::ofstream writingFile("response_debug.txt", std::ios::out);
    if (!writingFile) {
        Logger::Print("�t�@�C���̃I�[�v���Ɏ��s���܂���");
    }
    writingFile.write(data.data(), data.size());


    std::ofstream WritingFile("login.png", std::ios::binary);
    if (!WritingFile) {
        Logger::Print("�t�@�C���̃I�[�v���Ɏ��s���܂���");
    }
    WritingFile.write(data.data(), data.size());

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    closesocket(sock);
    WSACleanup();
}

bool SceneConnection::httpPngDownload()
{
    std::string hostname = "localhost";
    std::string port = "5000";
    const std::string path = "/File/DownloadPng";
    const std::string outputFile = "login.png";

    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    addrinfo hints = {}, * addrInfo = nullptr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        Logger::Print("WSA���������s");
        return false;
    }

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(hostname.c_str(), port.c_str(), &hints, &addrInfo) != 0) {
        Logger::Print("�h���C������A�h���X�擾�Ɏ��s���܂���");
        WSACleanup();
        return false;
    }

    sock = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
    if (sock == INVALID_SOCKET) {
        Logger::Print("�\�P�b�g�̐����Ɏ��s���܂���");
        freeaddrinfo(addrInfo);
        WSACleanup();
        return false;
    }

    if (connect(sock, addrInfo->ai_addr, static_cast<int>(addrInfo->ai_addrlen)) == SOCKET_ERROR) {
        Logger::Print("connect�Ɏ��s���܂���");
        closesocket(sock);
        freeaddrinfo(addrInfo);
        WSACleanup();
        return false;
    }

    // HTTP���N�G�X�g�̍쐬
    std::string request = "GET " + path + " HTTP/1.1\r\n"
        "Host: " + hostname + ":" + port + "\r\n"
        "Connection: Close\r\n\r\n";

    if (send(sock, request.c_str(), static_cast<int>(request.length()), 0) == SOCKET_ERROR) {
        Logger::Print("���N�G�X�g�̑��M�Ɏ��s���܂���");
        closesocket(sock);
        freeaddrinfo(addrInfo);
        WSACleanup();
        return false;
    }

    // ���X�|���X��M
    std::vector<char> data;
    data.reserve(4096);
    char buf[1024];
    int size = 0;

    while ((size = recv(sock, buf, sizeof(buf), 0)) > 0) {
        data.insert(data.end(), buf, buf + size);
    }

    if (size < 0) {
        Logger::Print("��M�G���[");
        closesocket(sock);
        freeaddrinfo(addrInfo);
        WSACleanup();
        return false;
    }

    auto headerEnd = std::search(data.begin(), data.end(), "\r\n\r\n", "\r\n\r\n" + 4);
    if (headerEnd != data.end()) {
        data.erase(data.begin(), headerEnd + 4);
    }
    else {
        Logger::Print("HTTP���X�|���X�w�b�_�[��������܂���");
        closesocket(sock);
        freeaddrinfo(addrInfo);
        WSACleanup();
        return false;
    }

    // �t�@�C�������o��
    std::ofstream writingFile("response_debug.txt", std::ios::out);
    if (!writingFile) {
        Logger::Print("�t�@�C���̃I�[�v���Ɏ��s���܂���");
    }
    writingFile.write(data.data(), data.size());

    std::ofstream WritingFile(outputFile, std::ios::binary);
    if (!WritingFile) {
        Logger::Print("�t�@�C���̃I�[�v���Ɏ��s���܂���");
    }
    WritingFile.write(data.data(), data.size());

    closesocket(sock);
    freeaddrinfo(addrInfo);
    WSACleanup();
    return true;
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


    float interval = 250;
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
        positionX+ interval, positionY, //�`��ʒu
        sizeX, sizeY,                 //�\���T�C�Y
        0, 0,                     //�؂���͂��߈ʒu
        500, 200,                 //�摜�T�C�Y
        0.0f,
        1, 1, 1, 1);

    if (UiClick(positionX+ interval, positionY, sizeX, sizeY))
    {
        isSignin = true;
    }

    sprites[static_cast<int>(SpriteNumber::NewLogin)]->Render(dc,
        positionX + interval*2, positionY, //�`��ʒu
        sizeX, sizeY,                  //�\���T�C�Y
        0, 0,                      //�؂���͂��߈ʒu
        500, 200,                  //�摜�T�C�Y
        0.0f,
        1, 1, 1, 1);

    if (UiClick(positionX+ interval*2, positionY, sizeX, sizeY))
    {
        isSignup = true;
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


