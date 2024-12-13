#pragma once
#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include <memory>
#include <unordered_map>
#include "Sprite.h"

enum class SpriteNumber {
    Title,
    NetError,
    OK,
    Guest,
    Login,
    NewLogin,
    Number,
    TeamNumber,
    Flag,
    Team,
    TeamSelect,
    TeamCreate,
    TeamJoin,
    TeamJoinFrame,
    TeamjoinNumber,
    Close,
    Ready,
    ReadyzCancel,
    Solo,
    Multi,
    Start,
    Name,
    WASD,
    SelectEdge,
    Setting,
    Mouse,
    Logout,
    BigCircle,
    SmallCircle,
    TeamDisbanded,
    ConnectionError
    // 他のスプライトを追加
};

class SpriteManager {
public:
    void LoadSprites();
    Sprite* GetSprite(SpriteNumber number);

private:
    std::unordered_map<SpriteNumber, std::unique_ptr<Sprite>> sprites;
};

// 外部からアクセス可能なグローバルインスタンスを宣言
extern SpriteManager g_SpriteManager;

#endif // SPRITE_MANAGER_H