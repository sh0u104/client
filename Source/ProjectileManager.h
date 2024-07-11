#pragma once

#include <vector>
#include "Projectile.h"
#include <set>

//Projectil�P�̒e��
// �e�ۃ}�l�[�W���[
class ProjectileManager// �����̒e���񋅐�
{
public:
    ProjectileManager();
    // �N������������Ȃ����畡���̓z���ʁX�Ɏ���������Ȃ�
    ~ProjectileManager();

    // �X�V����
    void Update(float elapsedTime) ;

    // �`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    // �e�ۓo�^
    void Register(Projectile* projectile);

    // �e�ۑS�폜
    void Clear();

    // �e�ې��擾
    int GetProjectileCount() const { return static_cast<int> (projectiles.size()); }


    // �e�ێ擾
    Projectile* GetProjectile(int index) { return projectiles.at(index); }

    // �e�ۍ폜
    void Remove(Projectile* projectile);

private:
    std::vector<Projectile*>   projectiles;
    // ����̒e�ۍ폜����폜����
    std::set<Projectile*>       removes;
};