#pragma once

#include <vector>
#include "Stage.h"

// ステージマネージャー
class StageManager
{
private:
    StageManager() {}
    ~StageManager() {}

public:
    // 唯一のインスタンス取得
    static StageManager& instance()
    {
        static StageManager instance;
        return instance;
    }

    // 更新処理
    void Update(float elapsedTime);

    // 描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    // ステージ登録
    void Register(Stage* stage);

    // ステージ全削除
    void Clear();

    // レイキャスト
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

    // スフィアキャスト
    bool SphereCast(
        const DirectX::XMFLOAT3& origin,
        const DirectX::XMFLOAT3& direction,
        float radius,
        float& distance,
        DirectX::XMFLOAT3& hitPosition,
        DirectX::XMFLOAT3& hitNormal);

    struct CollisionMesh
    {
        struct Triangle
        {
            DirectX::XMFLOAT3	positions[3];
            DirectX::XMFLOAT3	normal;
        };
        std::vector<Triangle>	triangles;
    };

    CollisionMesh						collisionMesh;

    // スフィアキャスト結果
    struct SphereCastResult
    {
        DirectX::XMVECTOR	position = { 0, 0, 0 };	// レイとポリゴンの交点
        DirectX::XMVECTOR	normal = { 0, 0, 0 };	// 衝突したポリゴンの法線ベクトル
        float				distance = 0.0f; 		// レイの始点から交点までの距離
        DirectX::XMVECTOR	verts[3];
        int					materialIndex = -1; 	// 衝突したポリゴンのマテリアル番号
    };
private:
    std::vector<Stage*> stages;
};
