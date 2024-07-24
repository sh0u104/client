#pragma once
#include "Graphics/Sprite.h"
#include "Scene.h"
#include "PlayerManager.h"
#include "Connection.h"
class SceneConnection : public Scene
{
public:
    SceneConnection() {};
    ~SceneConnection() override {}

    // ������
    void Initialize() override;

    // �I����
    void Finalize() override;
    // �X�V����
    void Update(float elapsedTime)override;
    // �`�揈��
    void Render() override;

    void RenderNetError(ID3D11DeviceContext* dc);
    bool Uiclick(float posX, float posY, float sizeX, float sizeY);
private:
    
    PlayerManager* playerManager = nullptr;
    Connection* connection = nullptr;
    enum class Spritenumber : int
    {
        NetError,
        OK,
        Max
    };
    std::unique_ptr<Sprite> sprites[(int)Spritenumber::Max] = {};

};

