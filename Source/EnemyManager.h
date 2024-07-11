#pragma once

#include <vector>
#include "Enemy.h"
#include <set>

// �G�l�~�[�}�l�[�W���[
class EnemyManager
{
private:
    EnemyManager() {}
    ~EnemyManager() {}

    // �G�l�~�[���m�̏Փˏ���
    void CollisiionEnemyVsEnemies();

    public:
        // �B��̃X�^���X�擾
        // ����������Ȃ�����V���O���g���ō��
        static EnemyManager& Instance()
        {
            static EnemyManager instance;
            return instance;
        }



        // �X�V����
        void Update(float elapsedTime);

        // �f�o�b�O�v���~�e�B�u�`��
        void DrawDebugPrimitive();

        // �`�揈��
        void Render(ID3D11DeviceContext* dc, Shader* shader);

        // �G�l�~�[�폜
        void Remove(Enemy* enemy);

        // �Ǘ�����ׂ̓z
        // �G�l�~�[�o�^
        void Register(Enemy* enemy);

        void DrawDebugGUI();

        // �G�l�~�[�S�폜
        void Clear();

        // ���ԂȂ̂��̊m�F
        // �ő吔�l�����o��
        // �G�l�~�[���擾
        int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

        // �G�l�~�[�擾
        // �����̔ԍ��̏ꏊ�ɓ����Ă��镨���o���B
        Enemy* GetEnemy(int index) { return enemies.at(index); }
private:
    // �X�V�@�`��𕡐�����ׂ̂���
    std::vector<Enemy*> enemies;
    // �E���z�Z�b�g
    std::set<Enemy*>  removes;

};