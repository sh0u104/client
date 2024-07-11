#include "ProjectileManager.h"

ProjectileManager::ProjectileManager()
{
}

ProjectileManager::~ProjectileManager()
{
    Clear();
}

void ProjectileManager::Update(float elapsedTime)
{

    // �X�V����
    for (Projectile* projectile : projectiles)
    {
        projectile->Update(elapsedTime);
    }

    // �j������ ���t���[�������ň�C�ɏ����B
    // ����projectiles�͈̔�for������erase()����ƕs����������Ă��܂����߁A
    //  �X�V�������I�������ɔj�����X�g�ɐς܂ꂽ�I�u�W�F�N�g���폜����B
    for (Projectile* projectile : removes)// �E���܂��惊�X�g���E��
    {
        // std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
        //      �C�e���[�^�[�ׂ̈�std::find���g��              (projectiles�̍ŏ�����Ō�܂Œ��ׂ�projectile����������)
        std::vector<Projectile*>::iterator it = std::find(projectiles.begin(), projectiles.end(),
                                                                                  projectile);
        if (it != projectiles.end())
        {
            //erase�֐���vector�v�f���폜�A�j�����X�g�̃|�C���^����C�e���[�^�[���������A�֐��ɓn��
            projectiles.erase(it);// �폜
        }

        // �e�ۂ̔j������
        delete projectile;// ���ꕨ�j��

    }
    // �j�����X�g���N���A
    removes.clear();

}

void ProjectileManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    for (Projectile* projectile : projectiles)
    {
        projectile->Render(dc, shader);
    }
}

void ProjectileManager::DrawDebugPrimitive()
{

    // �S�Ă̓z������Ă���
    for (Projectile* projectile : projectiles)
    {
        projectile->DrawDebugPrimitive();
    }
}

void ProjectileManager::Register(Projectile* projectile)
{
    // ��������납��ǉ����Ă��������Ă�������
    projectiles.emplace_back(projectile);
}

void ProjectileManager::Clear()
{
    for (Projectile* projectile : projectiles)// 
    {
        // ���̂��������Ǘ����Ă��鐔�͂��̂܂�
        delete projectile;

    }
    // vector�̐����O��
    projectiles.clear();
}

// �e�ۍ폜 �}�l�[�W���[�̂��̒e�ۏ����ĉ�����
void ProjectileManager::Remove(Projectile* projectile)
{
    // �j�����X�g�ɒǉ� ����projectiles�v�f�������ƕs����N���邩��j�����X�g�ɓo�^
    removes.insert(projectile);// �E�����X�g�E���ׂ̃��X�g
    // ���̂����ŏ����Ȃ������ŏ�������for���ɂ���Ă��܂�����
}
