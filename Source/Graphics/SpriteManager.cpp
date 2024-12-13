#include "SpriteManager.h"

#include "SpriteManager.h"
#include <stdexcept>

// グローバルインスタンスの定義
SpriteManager g_SpriteManager;

void SpriteManager::LoadSprites() {
    sprites[SpriteNumber::Title] = std::make_unique<Sprite>("Data/Sprite/Title.png");

	sprites[SpriteNumber::NetError] = std::make_unique<Sprite>("Data/Sprite/neterror.png");

	sprites[SpriteNumber::OK] = std::make_unique<Sprite>("Data/Sprite/OK.png");
    //ゲスト
    sprites[SpriteNumber::Guest] = std::make_unique<Sprite>("Data/Sprite/guest.png");
	//ログイン
	sprites[SpriteNumber::Login] = std::make_unique<Sprite>("Data/Sprite/login.png");
	//新規ログイン
	sprites[SpriteNumber::NewLogin] = std::make_unique<Sprite>("Data/Sprite/newlogin.png");

	sprites[SpriteNumber::Number] = std::make_unique<Sprite>("Data/Sprite/number.png");

    sprites[SpriteNumber::TeamNumber] = std::make_unique<Sprite>("Data/Sprite/teamnumber.png");
	//チーム
	sprites[SpriteNumber::Team] = std::make_unique<Sprite>("Data/Sprite/team.png");
    //チームセレクト
    sprites[SpriteNumber::TeamSelect] = std::make_unique<Sprite>("Data/Sprite/teamselect.png");
    //チーム作成
    sprites[SpriteNumber::TeamCreate] = std::make_unique<Sprite>("Data/Sprite/teamcreate.png");
    //加入
    sprites[SpriteNumber::TeamJoin] = std::make_unique<Sprite>("Data/Sprite/teamjoin.png");
    //チーム加入する時のクリックできないとこ
    sprites[SpriteNumber::TeamJoinFrame] = std::make_unique<Sprite>("Data/Sprite/teamjoinframe.png");
    //チーム加入する時のクリックするとこ
    sprites[SpriteNumber::TeamjoinNumber] = std::make_unique<Sprite>("Data/Sprite/teamjoinnumber.png");
    //閉じるボタン
    sprites[SpriteNumber::Close] = std::make_unique<Sprite>("Data/Sprite/close.png");
	//準備完了
	sprites[SpriteNumber::Ready] = std::make_unique<Sprite>("Data/Sprite/ready.png");
	//準備キャンセル
	sprites[SpriteNumber::ReadyzCancel] = std::make_unique<Sprite>("Data/Sprite/readycancel.png");
	//ソロモード
	sprites[SpriteNumber::Solo] = std::make_unique<Sprite>("Data/Sprite/solo.png");
	//マルチモード
	sprites[SpriteNumber::Multi] = std::make_unique<Sprite>("Data/Sprite/multi.png");
	//スタート
	sprites[SpriteNumber::Start] = std::make_unique<Sprite>("Data/Sprite/start.png");
	//名前
	sprites[SpriteNumber::Name] = std::make_unique<Sprite>("Data/Sprite/font1.png");

	sprites[SpriteNumber::WASD] = std::make_unique<Sprite>("Data/Sprite/WASD.png");

	sprites[SpriteNumber::SelectEdge] = std::make_unique<Sprite>("Data/Sprite/selectedge.png");

	sprites[SpriteNumber::Setting] = std::make_unique<Sprite>("Data/Sprite/setting.png");

	sprites[SpriteNumber::Mouse] = std::make_unique<Sprite>("Data/Sprite/mouse.png");
	//ログアウト
	sprites[SpriteNumber::Logout] = std::make_unique<Sprite>("Data/Sprite/logout.png");

	sprites[SpriteNumber::BigCircle] = std::make_unique<Sprite>("Data/Sprite/bigcircle.png");

	sprites[SpriteNumber::SmallCircle] = std::make_unique<Sprite>("Data/Sprite/smallcircle.png");
	//チーム解散
	sprites[SpriteNumber::TeamDisbanded] = std::make_unique<Sprite>("Data/Sprite/teamdisbanded.png");

	sprites[SpriteNumber::ConnectionError] = std::make_unique<Sprite>("Data/Sprite/ConnectionError.png");

}

Sprite* SpriteManager::GetSprite(SpriteNumber number) {
    auto it = sprites.find(number);
    if (it != sprites.end()) {
        return it->second.get();
    }
    throw std::runtime_error("Sprite not found!");
}