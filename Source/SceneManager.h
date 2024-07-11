#pragma once

#include "Scene.h"
#include "Connection.h"
// �V�[���}�l�[�W���[
class SceneManager
{
private:
    SceneManager() {}
    ~SceneManager() {}

public:
    // �B��̃C���X�^���X�擾
    static SceneManager& Instance()
    {
        static SceneManager instance;
        return instance;
    }

    // �X�V����
    void Update(float elapsedTime);

    // �`�揈��
    void Render();

    // �V�[���N���A
    void Clear();

   void PlayermanagerInitialize() {playerManager = new PlayerManager(); }
   
   void ConnectionInitialize() { connection = new Connection(); }
   
   void ConnectionClear();

    // �V�[���؂�ւ�
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
