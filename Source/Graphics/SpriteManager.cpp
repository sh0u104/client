#include "SpriteManager.h"

#include "SpriteManager.h"
#include <stdexcept>

// �O���[�o���C���X�^���X�̒�`
SpriteManager g_SpriteManager;

void SpriteManager::LoadSprites() {
    sprites[SpriteNumber::Title] = std::make_unique<Sprite>("Data/Sprite/Title.png");

	sprites[SpriteNumber::NetError] = std::make_unique<Sprite>("Data/Sprite/neterror.png");

	sprites[SpriteNumber::OK] = std::make_unique<Sprite>("Data/Sprite/OK.png");
    //�Q�X�g
    sprites[SpriteNumber::Guest] = std::make_unique<Sprite>("Data/Sprite/guest.png");
	//���O�C��
	sprites[SpriteNumber::Login] = std::make_unique<Sprite>("Data/Sprite/login.png");
	//�V�K���O�C��
	sprites[SpriteNumber::NewLogin] = std::make_unique<Sprite>("Data/Sprite/newlogin.png");

	sprites[SpriteNumber::Number] = std::make_unique<Sprite>("Data/Sprite/number.png");

    sprites[SpriteNumber::TeamNumber] = std::make_unique<Sprite>("Data/Sprite/teamnumber.png");
	//�`�[��
	sprites[SpriteNumber::Team] = std::make_unique<Sprite>("Data/Sprite/team.png");
    //�`�[���Z���N�g
    sprites[SpriteNumber::TeamSelect] = std::make_unique<Sprite>("Data/Sprite/teamselect.png");
    //�`�[���쐬
    sprites[SpriteNumber::TeamCreate] = std::make_unique<Sprite>("Data/Sprite/teamcreate.png");
    //����
    sprites[SpriteNumber::TeamJoin] = std::make_unique<Sprite>("Data/Sprite/teamjoin.png");
    //�`�[���������鎞�̃N���b�N�ł��Ȃ��Ƃ�
    sprites[SpriteNumber::TeamJoinFrame] = std::make_unique<Sprite>("Data/Sprite/teamjoinframe.png");
    //�`�[���������鎞�̃N���b�N����Ƃ�
    sprites[SpriteNumber::TeamjoinNumber] = std::make_unique<Sprite>("Data/Sprite/teamjoinnumber.png");
    //����{�^��
    sprites[SpriteNumber::Close] = std::make_unique<Sprite>("Data/Sprite/close.png");
	//��������
	sprites[SpriteNumber::Ready] = std::make_unique<Sprite>("Data/Sprite/ready.png");
	//�����L�����Z��
	sprites[SpriteNumber::ReadyzCancel] = std::make_unique<Sprite>("Data/Sprite/readycancel.png");
	//�\�����[�h
	sprites[SpriteNumber::Solo] = std::make_unique<Sprite>("Data/Sprite/solo.png");
	//�}���`���[�h
	sprites[SpriteNumber::Multi] = std::make_unique<Sprite>("Data/Sprite/multi.png");
	//�X�^�[�g
	sprites[SpriteNumber::Start] = std::make_unique<Sprite>("Data/Sprite/start.png");
	//���O
	sprites[SpriteNumber::Name] = std::make_unique<Sprite>("Data/Sprite/font1.png");

	sprites[SpriteNumber::WASD] = std::make_unique<Sprite>("Data/Sprite/WASD.png");

	sprites[SpriteNumber::SelectEdge] = std::make_unique<Sprite>("Data/Sprite/selectedge.png");

	sprites[SpriteNumber::Setting] = std::make_unique<Sprite>("Data/Sprite/setting.png");

	sprites[SpriteNumber::Mouse] = std::make_unique<Sprite>("Data/Sprite/mouse.png");
	//���O�A�E�g
	sprites[SpriteNumber::Logout] = std::make_unique<Sprite>("Data/Sprite/logout.png");

	sprites[SpriteNumber::BigCircle] = std::make_unique<Sprite>("Data/Sprite/bigcircle.png");

	sprites[SpriteNumber::SmallCircle] = std::make_unique<Sprite>("Data/Sprite/smallcircle.png");
	//�`�[�����U
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