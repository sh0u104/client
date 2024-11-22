#include "SceneManager.h"

// �X�V����
void SceneManager::Update(float elapsedTime)
{
    if (this->nextScene)
    {
        // �Â��V�[���I������
        this->Clear();


        // �V�����V�[����ݒ�
        //nextScene->Initialize();
        this->currentScene = this->nextScene;
        this->nextScene = nullptr;

        // �V�[������������
        // �Ⴄ�ꏊ�ŌĂ΂�Ă邩������Ă΂�Ă�����ʂ�Ȃ����ɂ���B
        if (!this->currentScene->IsReady())
            this->currentScene->Initialize();

    }

    if (currentScene )
    {
        // ���t���[���X�Vnext�̒��g�̓z
        
        currentScene->Update(elapsedTime);
    }
}
// �`�揈��
void SceneManager::Render()
{
    // �������Ă���̂�����
    if (this->currentScene)
    {
        this->currentScene->Render();
    }
}
// �V�[���N���A
void SceneManager::Clear()
{
    if (this->currentScene)
    {
        currentScene->Finalize();
        delete currentScene;
        currentScene = nullptr;
    }
}





void SceneManager::ConnectionClear()
{
   
    //�ڑ��I����
    if (connection!=nullptr)
    {
        connection->Finalize();
        // �v���C���[�I����
        for (Player* player : playerManager->GetPlayers())
        {
            playerManager->ErasePlayer(player->GetPlayerID());
        }
        playerManager->DeletePlayer();


        delete playerManager;
        playerManager = nullptr;

        delete connection;
        connection = nullptr;


        SetstandbyInitialized(false);
        SetconnectionInitialized(false);
    }


}
// �V�[���؂�ւ�
void SceneManager::ChangeScene(Scene* scene)
{
    // �V�����V�[����ݒ�
    this->nextScene = scene;
}
