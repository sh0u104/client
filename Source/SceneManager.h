#pragma once

#include "Scene.h"
#include "Connection.h"
// シーンマネージャー
class SceneManager
{
private:
    SceneManager() {}
    ~SceneManager() {}

public:
    // 唯一のインスタンス取得
    static SceneManager& Instance()
    {
        static SceneManager instance;
        return instance;
    }

    // 更新処理
    void Update(float elapsedTime);

    // 描画処理
    void Render();

    // シーンクリア
    void Clear();

   void PlayermanagerInitialize() {playerManager = new PlayerManager(); }
   
   void ConnectionInitialize() { connection = new Connection(); }
   
   void ConnectionClear();

    // シーン切り替え
    void ChangeScene(Scene* scene);

    void SetConnection(Connection* connection) { this->connection = connection; }
    Connection* GetConnection() { return this->connection; }
    
    void SetPlayerManager(PlayerManager* playerManager) { this->playerManager= playerManager; }
    PlayerManager* GetPlayerManager(){return this->playerManager;}

    void SetstandbyInitialized(bool flag) { this->standbyInitialized = flag; }
    bool GetstandbyInitialized() { return standbyInitialized; }
private:
    Scene* currentScene = nullptr;
    Scene* nextScene = nullptr;

    Connection* connection = nullptr;
    PlayerManager* playerManager = nullptr;
    bool standbyInitialized = false;
};
