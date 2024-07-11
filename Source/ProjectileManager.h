#pragma once

#include <vector>
#include "Projectile.h"
#include <set>

//Projectil１個の弾丸
// 弾丸マネージャー
class ProjectileManager// 複数の弾がん球数
{
public:
    ProjectileManager();
    // 誰が持つか分からないから複数の奴が別々に持つかもしれない
    ~ProjectileManager();

    // 更新処理
    void Update(float elapsedTime) ;

    // 描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    // デバッグプリミティブ描画
    void DrawDebugPrimitive();

    // 弾丸登録
    void Register(Projectile* projectile);

    // 弾丸全削除
    void Clear();

    // 弾丸数取得
    int GetProjectileCount() const { return static_cast<int> (projectiles.size()); }


    // 弾丸取得
    Projectile* GetProjectile(int index) { return projectiles.at(index); }

    // 弾丸削除
    void Remove(Projectile* projectile);

private:
    std::vector<Projectile*>   projectiles;
    // 特定の弾丸削除だれ削除する
    std::set<Projectile*>       removes;
};