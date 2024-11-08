#include <WinSock2.h>		// WinSock version 2を使用するためのヘッダ
#include <WS2tcpip.h>		// WinSock version 2で使用する様々なオプションのヘッダ
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


// OpenSSL読み込み
#include <openssl/crypto.h>		// opensslの暗号化ヘッダ
#include <openssl/ssl.h>		// opensslのssl通信ヘッダ
#include <openssl/err.h>		// opensslのエラーヘッダ

#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

#include <conio.h>
// 初期化
void SceneConnection::Initialize()
{
  // スプライト初期化
    //エラー
    sprites[static_cast<int>(SpriteNumber::NetError)] = std::make_unique<Sprite>("Data/Sprite/neterror.png");
    //OKボタン
    sprites[static_cast<int>(SpriteNumber::OK)] = std::make_unique<Sprite>("Data/Sprite/OK.png");
    //ゲスト
    sprites[static_cast<int>(SpriteNumber::Guest)] = std::make_unique<Sprite>("Data/Sprite/guest.png");
    //ログイン
    sprites[static_cast<int>(SpriteNumber::Login)] = std::make_unique<Sprite>("Data/Sprite/login.png");
    //新規ログイン
    sprites[static_cast<int>(SpriteNumber::NewLogin)] = std::make_unique<Sprite>("Data/Sprite/newlogin.png");
    //名前
    sprites[static_cast<int>(SpriteNumber::Name)] = std::make_unique<Sprite>("Data/Sprite/font1.png");


    //一度だけ
    if (!SceneManager::Instance().GetconnectionInitialized())
    {
        SceneManager::Instance().SetconnectionInitialized(true);
        //初期化
        SceneManager::Instance().PlayermanagerInitialize();
        SceneManager::Instance().ConnectionInitialize();


        // プレイヤーマネジャー初期化
        playerManager = SceneManager::Instance().GetPlayerManager();

        connection = SceneManager::Instance().GetConnection();
        //	サーバーとの接続前はID０
        Player* player = new Player();
        player->SetPlayerID(0);						//貰ったID情報をストック
        player->Setoperation(false);
        player->SetPosition(DirectX::XMFLOAT3(0, 0, 0)); //発生位置
        player->SetAngle({ 0.0f,3.0f,0.0f });

        playerManager->SetMyPlayerID(0);
        playerManager->AddPlayer(player);
        playerManager->GetPlayer(0)->SetReady(true);

        //生成したら
        playerManager->AddPlayersGenerateCount();
        connection->SetPlayerManager(playerManager);
    }
    else
    {
        // プレイヤーマネジャー初期化
        playerManager = SceneManager::Instance().GetPlayerManager();

        connection = SceneManager::Instance().GetConnection();
    }

    //接続処理
    if (!connection->isConnection)
    {
        connection->Initialize();
    }


}
// 終了化
void SceneConnection::Finalize()
{
  
}
// 更新処理
void SceneConnection::Update(float elapsedTime)
{ 
    //接続成功
    //サインインサインアップで受け取りが終わったらに変更する
    if (connection->isConnection && playerManager->GetMyPlayerID()>0)
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneStandby));
    }
   
    if (isSignup)
    {
       // Signup();
      
        ID = GetDataJson();
        isSignin = false;

       
    }
    if (isSignin)
    {
        ID = GetDataJson();
        isSignin = false;
       // Signin();
        NameJson();
        playerManager->GetMyPlayer()->SetName(Name);
    }

    if (ID > 0 && playerManager->GetMyPlayerID() == 0)
    {
        //自分のプレイヤーに受信したIDを乗せる
        playerManager->GetMyPlayer()->SetPlayerID(ID);
        playerManager->SetMyPlayerID(ID);
        //ログイン数を加算
        playerManager->AddLoginCount();
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneStandby));
    }
}
// 描画処理
void SceneConnection::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    // 画面クリア＆レンダーターゲット設定
    FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };   // RGBA(0.0~1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    // 2Dスプライト描画
    {
        //名前
        RenderName(dc);

        //接続できなかったら
        if (!connection->isConnection)
        {
            RenderNetError(dc);
        }

        //ログインの種類
        if (connection->isConnection && !isSignin && !isSignup)
        {
            RenderLogin(dc);
        }
    }
   ImGui::SetNextWindowPos(ImVec2(500, -10), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);
   // beginからendまでの内容が出来る
   if (ImGui::Begin("login", nullptr, ImGuiWindowFlags_None))
   {
       ImGui::Text("name: % s", name);
       ImGui::Text("userId: %d", ID);
       ImGui::Text("userDay: %d", loginDay);
       ImGui::InputText("Name", name, sizeof(name));
       ImGui::InputText("password", pass, sizeof(pass));
       isname = true;
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
       if (isSignin || isSignup)
       {
           if (ImGui::Button("back"))
           {
               memset(name, 0, sizeof(name));
               memset(pass, 0, sizeof(pass));
               isSignin = false;
               isSignup = false;
           }
       }
       ImGui::End();
   }

}

void SceneConnection::Signin()
{

    std::string hostname = "localhost";
    std::string port = "7189";
    std::string path = "/Login/Login?userId=" + std::to_string(maxID);

    OPENSSL_init_ssl(0, nullptr);

    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    addrinfo hints = {}, * addrInfo = nullptr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSA初期化失敗" << std::endl;
        return ;
    }

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(hostname.c_str(), port.c_str(), &hints, &addrInfo) != 0) {
        std::cerr << "ドメインからアドレス取得に失敗しました" << std::endl;
        WSACleanup();
        return ;
    }

    sock = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
    if (sock == INVALID_SOCKET) {
        std::cerr << "ソケットの生成に失敗しました" << std::endl;
        freeaddrinfo(addrInfo);
        WSACleanup();
        return ;
    }

    if (connect(sock, addrInfo->ai_addr, static_cast<int>(addrInfo->ai_addrlen)) == SOCKET_ERROR) {
        std::cerr << "connectに失敗しました" << std::endl;
        closesocket(sock);
        freeaddrinfo(addrInfo);
        WSACleanup();
        return ;
    }

    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        std::cerr << "SSL_CTXの生成に失敗しました" << std::endl;
        closesocket(sock);
        WSACleanup();
        return ;
    }

    SSL* ssl = SSL_new(ctx);
    if (!ssl) {
        std::cerr << "SSLの生成に失敗しました" << std::endl;
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return ;
    }

    if (SSL_set_fd(ssl, static_cast<int>(sock)) == 0) {
        std::cerr << "ソケットとSSLの関連付けに失敗しました" << std::endl;
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return ;
    }

    if (SSL_connect(ssl) <= 0) {
        std::cerr << "SSL接続に失敗しました" << std::endl;
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
        std::cerr << "リクエストの送信に失敗しました" << std::endl;
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
        std::cerr << "受信エラー" << std::endl;
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return ;
    }

    // HTTPヘッダーを削除してJSON部分のみ抽出
    auto headerEnd = std::search(data.begin(), data.end(), "\r\n\r\n", "\r\n\r\n" + 4);
    std::string response_body;
    if (headerEnd != data.end()) {
        // ヘッダーの後のポインタを取得してボディ部分を設定
        response_body = std::string(headerEnd + 4, data.end());

        // チャンクサイズを削除する処理
        std::string cleaned_body;
        size_t pos = 0;

        while (pos < response_body.size()) {
            // チャンクサイズ（16進数表記）の取得
            size_t chunkEnd = response_body.find("\r\n", pos);
            if (chunkEnd == std::string::npos) break;

            // チャンクサイズを16進数から数値に変換
            std::string chunkSizeHex = response_body.substr(pos, chunkEnd - pos);
            int chunkSize = std::stoi(chunkSizeHex, nullptr, 16);
            pos = chunkEnd + 2; // チャンクサイズの後の `\r\n` をスキップ

            // チャンクデータの取得
            cleaned_body += response_body.substr(pos, chunkSize);
            pos += chunkSize + 2; // チャンクデータの後の `\r\n` をスキップ
        }

        response_body = cleaned_body;
    }

    std::ofstream file("signindata.json"); // 拡張子を .json に変更
    if (file.is_open()) {
        file << response_body; // JSON文字列としてそのまま書き込む
        file.close();
        std::cout << "レスポンスが'response.json'に保存されました。" << std::endl;
    }
    else {
        throw std::runtime_error("ファイルを開けませんでした");
    }

    

    // JSONの内容を出力して確認
    std::cout << "Response Body (after header removal): " << response_body << std::endl;

    // JSONのパース
    nlohmann::json json = nlohmann::json::parse(response_body, nullptr, false);
    if (json.is_discarded() || !json.contains("user_profile") || !json["user_profile"].contains("userId")) {
        std::cerr << "JSONのパースに失敗またはuserIdが見つかりません" << std::endl;
        
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
        std::cerr << "WSA初期化失敗" << std::endl;
        return;
    }

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(hostname.c_str(), port.c_str(), &hints, &addrInfo) != 0) {
        std::cerr << "ドメインからアドレス取得に失敗しました" << std::endl;
        WSACleanup();
        return;
    }

    sock = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
    if (sock == INVALID_SOCKET) {
        std::cerr << "ソケットの生成に失敗しました" << std::endl;
        freeaddrinfo(addrInfo);
        WSACleanup();
        return;
    }

    if (connect(sock, addrInfo->ai_addr, static_cast<int>(addrInfo->ai_addrlen)) == SOCKET_ERROR) {
        std::cerr << "connectに失敗しました" << std::endl;
        closesocket(sock);
        freeaddrinfo(addrInfo);
        WSACleanup();
        return;
    }

    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        std::cerr << "SSL_CTXの生成に失敗しました" << std::endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    SSL* ssl = SSL_new(ctx);
    if (!ssl) {
        std::cerr << "SSLの生成に失敗しました" << std::endl;
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return;
    }

    if (SSL_set_fd(ssl, static_cast<int>(sock)) == 0) {
        std::cerr << "ソケットとSSLの関連付けに失敗しました" << std::endl;
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return;
    }

    if (SSL_connect(ssl) <= 0) {
        std::cerr << "SSL接続に失敗しました" << std::endl;
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
        std::cerr << "リクエストの送信に失敗しました" << std::endl;
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
        std::cerr << "受信エラー" << std::endl;
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return;
    }

    // HTTPヘッダーを削除してJSON部分のみ抽出
    auto headerEnd = std::search(data.begin(), data.end(), "\r\n\r\n", "\r\n\r\n" + 4);
    std::string response_body;
    if (headerEnd != data.end()) {
        // ヘッダーの後のポインタを取得してボディ部分を設定
        response_body = std::string(headerEnd + 4, data.end());

        // チャンクサイズを削除する処理
        std::string cleaned_body;
        size_t pos = 0;

        while (pos < response_body.size()) {
            // チャンクサイズ（16進数表記）の取得
            size_t chunkEnd = response_body.find("\r\n", pos);
            if (chunkEnd == std::string::npos) break;

            // チャンクサイズを16進数から数値に変換
            std::string chunkSizeHex = response_body.substr(pos, chunkEnd - pos);
            int chunkSize = std::stoi(chunkSizeHex, nullptr, 16);
            pos = chunkEnd + 2; // チャンクサイズの後の `\r\n` をスキップ

            // チャンクデータの取得
            cleaned_body += response_body.substr(pos, chunkSize);
            pos += chunkSize + 2; // チャンクデータの後の `\r\n` をスキップ
        }

        response_body = cleaned_body;
    }

    std::ofstream file("response.json"); // 拡張子を .json に変更
    if (file.is_open()) {
        file << response_body; // JSON文字列としてそのまま書き込む
        file.close();
        std::cout << "レスポンスが'response.json'に保存されました。" << std::endl;
    }
    else {
        throw std::runtime_error("ファイルを開けませんでした");
    }



    // JSONの内容を出力して確認
    std::cout << "Response Body (after header removal): " << response_body << std::endl;

    // JSONのパース
    nlohmann::json json = nlohmann::json::parse(response_body, nullptr, false);
    if (json.is_discarded() || !json.contains("user_profile") || !json["user_profile"].contains("userId")) {
        std::cerr << "JSONのパースに失敗またはuserIdが見つかりません" << std::endl;

        return;
    }

    std::string userId = json["user_profile"]["userId"];
    std::string userIdNumber = userId.substr(3);

    std::cout << "User ID Number: " << userIdNumber << std::endl;

    playerManager->GetMyPlayer()->SetLoginDay(json["user_login"]["loginDay"]);

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    closesocket(sock);
    WSACleanup();

    isSignup = false;
}

int SceneConnection::GetDataJson()
{
    std::ifstream file("response.json");

    if (!file.is_open()) {
        throw std::runtime_error("ファイルを開けませんでした");
    }
    json jsonData;
    file >> jsonData;
    file.close();
    int userId = 0;
    if (jsonData.contains("user_profile") && jsonData["user_profile"].contains("userId")) {
        std::string userIdStr = jsonData["user_profile"]["userId"];
        maxID = std::stoi(userIdStr);

        std::string userIdNumber = userIdStr.substr(5);
        userId = std::stoi(userIdNumber); // 文字列からintへ変換
        std::cout << "userId: " << userId << std::endl;
    }
    else {
        std::cerr << "userIdが見つかりません" << std::endl;
    }

    return userId;

}

void SceneConnection::NameJson()
{
    std::ifstream file("signindata.json");

    if (!file.is_open()) {
        throw std::runtime_error("ファイルを開けませんでした");
    }
    json jsonData;
    file >> jsonData;
    file.close();

    std::string userName;
    if (jsonData.contains("user_profile") && jsonData["user_profile"].contains("userName")) {
        userName = jsonData["user_profile"]["userName"];
      
        std::cout << "userId: " << userName << std::endl;
        playerManager->GetMyPlayer()->SetLoginDay(jsonData["user_login"]["loginDay"]);
    }
    else {
        std::cerr << "userIdが見つかりません" << std::endl;
    }

    strcpy_s(Name, sizeof(Name), userName.c_str());
    
}

void SceneConnection::handleInput(char* inputBuffer, size_t bufferSize)
{
    size_t currentLength = strlen(inputBuffer);

    // キーが押されているか確認
    if (_kbhit()) {
        char ch = _getch();  // 1文字取得

        // Enterキーで終了
        if (ch == '\r') {
            std::cout << "\n入力終了" << std::endl;
            return;
        }

        // バックスペース処理
        if (ch == '\b' && currentLength > 0) {
            inputBuffer[--currentLength] = '\0';
        }
        // バッファに追加（最大長を超えない場合）
        else if (currentLength < bufferSize - 1) {
            inputBuffer[currentLength++] = ch;
            inputBuffer[currentLength] = '\0';
        }
    }
}

void SceneConnection::RenderNetError(ID3D11DeviceContext* dc)
{
    float positionX = 50;
    float positionY = 0;

    //枠組み
    sprites[static_cast<int>(SpriteNumber::NetError)]->Render(dc,
        positionX, positionY, //描画位置
        600, 350,               //表示サイズ
        0, 0,                 //切り取りはじめ位置
        750, 500,           //画像サイズ
        0.0f,
        1, 1, 1, 1);

    //OKボタン
    sprites[static_cast<int>(SpriteNumber::OK)]->Render(dc,
        positionX + 190, positionY + 250, //描画位置
        150, 50,               //表示サイズ
        0, 0,                 //切り取りはじめ位置
        400, 200,           //画像サイズ
        0.0f,
        1, 1, 1, 1);

    if (UiClick(positionX + 190, positionY + 250, 150, 50))
    {
       
       if (!connection->isConnection)
       {
           connection->Initialize();
         
           //接続成功
           if (connection->isConnection)
           {
              // connection->SetplayerManager(playerManager);
               //SceneManager::Instance().ChangeScene(new SceneLoading(new SceneStandby));
           }
           //失敗
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
    //枠組み
    sprites[static_cast<int>(SpriteNumber::Guest)]->Render(dc,
        positionX, positionY, //描画位置
        sizeX,sizeY,              //表示サイズ
        0, 0,                 //切り取りはじめ位置
        500,200,               //画像サイズ
        0.0f,
        1, 1, 1, 1);

    if (UiClick(positionX, positionY, sizeX, sizeY))
    {
        connection->SendGeustLogin();
    }

    sprites[static_cast<int>(SpriteNumber::Login)]->Render(dc,
        positionX+ interval, positionY, //描画位置
        sizeX, sizeY,                 //表示サイズ
        0, 0,                     //切り取りはじめ位置
        500, 200,                 //画像サイズ
        0.0f,
        1, 1, 1, 1);

    if (UiClick(positionX+ interval, positionY, sizeX, sizeY))
    {
        isSignin = true;
    }

    sprites[static_cast<int>(SpriteNumber::NewLogin)]->Render(dc,
        positionX + interval*2, positionY, //描画位置
        sizeX, sizeY,                  //表示サイズ
        0, 0,                      //切り取りはじめ位置
        500, 200,                  //画像サイズ
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
 
    //枠組み
    for (int i = 0; i < 9; ++i)
    {
        if (name[i] == '\0')break;
        int number = static_cast<int>(name[i]);
        int width = number % 16;
        int height = number / 16;
        sprites[static_cast<int>(SpriteNumber::Name)]->Render(dc,
            positionX, positionY,      //描画位置
            sizeX, sizeY,              //表示サイズ
            sizeX*width, sizeY*height, //切り取りはじめ位置
            sizeX, sizeY,              //画像サイズ
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


