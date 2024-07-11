#include <imgui.h>
#include "EnemyManager.h"
#include "Collision.h"




// �X�V����
void EnemyManager::Update(float elapsedTime)
{
    for (Enemy* enemy :  enemies)
    {
        enemy->Update(elapsedTime);
    }

   
    // �j������ ���t���[�������ň�C�ɏ����B
    // ����projectiles�͈̔�for������erase()����ƕs����������Ă��܂����߁A
    //  �X�V�������I�������ɔj�����X�g�ɐς܂ꂽ�I�u�W�F�N�g���폜����B
    for (Enemy* enemy : removes)// �E���܂��惊�X�g���E��
    {
        // std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
        //      �C�e���[�^�[�ׂ̈�std::find���g��              (enemies�̍ŏ�����Ō�܂Œ��ׂ�enemy����������)
        std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(),
                                                                              enemy);//vector����enemy�T���ďo��
        if (it != enemies.end())// ����������
        {
            //erase�֐���vector�v�f���폜�A�j�����X�g�̃|�C���^����C�e���[�^�[���������A�֐��ɓn��
            enemies.erase(it);// �폜
            // �Ǘ��ۂ������������
        }

        // �������󂶂�Ȃ��̂�
        // �G�̔j������
        delete enemy;// ���ꕨ�j��

    }
    // �j�����X�g���N���A
    removes.clear();

    // �G���m�̏Փˏ���
    CollisiionEnemyVsEnemies();
}

void EnemyManager::DrawDebugPrimitive()
{
    // �S�Ă̓z������Ă���
    for (Enemy* enemy : enemies)
    {
        enemy->DrawDebugPrimitive();
    }
}


// �`�揈��
void EnemyManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Render(context, shader);
    }
}

void EnemyManager::Remove(Enemy* enemy)
{
    // vector�Ƃ͈Ⴄset�͍폜����
    // �j�����X�g�ɒǉ� ����enemies�v�f�������ƕs����N���邩��j�����X�g�ɓo�^
    removes.insert(enemy);// �E�����X�g�E���ׂ̃��X�g
    // ���̂����ŏ����Ȃ������ŏ�������for���ɂ���Ă��܂�����
}




void EnemyManager::Register(Enemy* enemy)
{
    // ��������납��ǉ����Ă��������Ă�������
    enemies.emplace_back(enemy);
}

void EnemyManager::CollisiionEnemyVsEnemies()
{
    // �ő吔
    size_t enemyCount = enemies.size();
    for (int i = 0; i < enemyCount; ++i) {
        // i�Ԗڂ̈ʒu
        Enemy* enemyA = enemies.at(i);
        for (int j = i + 1; j < enemyCount; ++j)
            {
            Enemy * enemyB = enemies.at(j);
            DirectX::XMFLOAT3 outPosition;
            if (Collision::IntersectSpherVsSphere(
                enemyA->GetPosition(),
                enemyA->GetRadius(),
                enemyB->GetPosition(),
                enemyB->GetRadius(),
                outPosition))
            {
                enemyB->SetPosition(outPosition);
            }
            }
            
    }

}



void EnemyManager::DrawDebugGUI()
{
    
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
    {
        // �g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Enemy", ImGuiTreeNodeFlags_DefaultOpen))
        {
            
          //  ImGui::InputFloat3("Position", &);
   

            ImGui::Button("Add");

            //for (Enemy* enemy : enemies)
            //{
            //    enemies.emplace_back(enemy);
            //}

            
            


        }
    }
    ImGui::End();
}

// �G�l�~�[�S�폜
void EnemyManager::Clear()
{
    for (Enemy* enemy : enemies)// 
    {
        // ���̂��������Ǘ����Ă��鐔�͂��̂܂�
        delete enemy;

    }
    // vector�̐����O��
    enemies.clear();
}
