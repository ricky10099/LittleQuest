#include "Sword.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionLine.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>

#include "Camera.h"
#include "Enemy.h"
#include "Player.h"

namespace LittleQuest {

    //! @brief Editor上でのCreateObject用の設定
    //! @detail BP_OBJECT_TYPEとセットで用意する
    BP_OBJECT_IMPL(Sword, "LittleQuest/Sword");

    SwordPtr Sword::Create(std::string name, const float3& pos) {
        // 箱の作成
        auto sword = Scene::CreateObjectPtr<Sword>();
        sword->SetName(name);
        // posの位置に設定
        sword->SetTranslate(pos);

        return sword;
    }

    bool Sword::Init()    // override
    {
        return Super::Init();
    }

    void Sword::Update()    // override
    {}

    // 基本描画の後に処理します
    void Sword::LateDraw()    // override
    {
    }

    void Sword::GUI()    // override
    {
        Super::GUI();
    }

    void Sword::OnHit([[maybe_unused]] const ComponentCollision::HitInfo&
                          hitInfo)    // override
    {
        Super::OnHit(hitInfo);
    }
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Sword)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Sword)
