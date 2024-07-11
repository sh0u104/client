#pragma once

#include <vector>
#include "Enemy.h"
#include <set>

// エネミーマネージャー
class EnemyManager
{
private:
    EnemyManager() {}
    ~EnemyManager() {}

    // エネミー同士の衝突処理
    void CollisiionEnemyVsEnemies();

    public:
        // 唯一のスタンス取得
        // 自分じゃ作れないからシングルトンで作る
        static EnemyManager& Instance()
        {
            static EnemyManager instance;
            return instance;
        }



        // 更新処理
        void Update(float elapsedTime);

        // デバッグプリミティブ描画
        void DrawDebugPrimitive();

        // 描画処理
        void Render(ID3D11DeviceContext* dc, Shader* shader);

        // エネミー削除
        void Remove(Enemy* enemy);

        // 管理する為の奴
        // エネミー登録
        void Register(Enemy* enemy);

        void DrawDebugGUI();

        // エネミー全削除
        void Clear();

        // 何番なのかの確認
        // 最大数値を取り出す
        // エネミー数取得
        int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

        // エネミー取得
        // 引数の番号の場所に入っている物を出す。
        Enemy* GetEnemy(int index) { return enemies.at(index); }
private:
    // 更新　描画を複数する為のもの
    std::vector<Enemy*> enemies;
    // 殺す奴セット
    std::set<Enemy*>  removes;

};