#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"



// デバッグプリミティブ描画
void Enemy::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    //// 衝突判定用のデバッグ球を描画
    //debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));

    // 衝突判定用のデバッグ円柱を描画
    debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

}


// 破棄
void Enemy::Destroy()
{
    //EnemyManager& manager = EnemyManager::Instance();
    ////// 自分自身を入れてそれで個体を消すための所に入れる。」
    ////manager->Remove(this);
    //manager.Remove(this);

    //EnemyManager::Instance().;
    EnemyManager::Instance().Remove(this);
}