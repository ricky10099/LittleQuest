#include "Player.h"
#include "Enemy.h"
#include "Camera.h"
#include "LittleQuest/Components/ComponentHP.h"
#include "LittleQuest/Scenes/GameOverScene.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>

namespace LittleQuest {

BP_OBJECT_IMPL(Player, "LittleQuest/Player");

PlayerPtr Player::Create(const float3& pos, const float3& front) {
    auto player = Scene::CreateObjectPtr<Player>();
    player->SetName("Player");

    auto mat = HelperLib::Math::CreateMatrixByFrontVector(front);
    player->SetMatrix(mat);
    player->SetTranslate(pos);

    player->pHP = player->AddComponent<ComponentHP>();
    player->pHP.lock()->SetHP(100);

    auto sword = Scene::CreateObjectPtr<Object>("PlayerSword");

    auto model = sword->AddComponent<ComponentModel>();
    model->Load("data/LittleQuest/Model/Sword/Sword.mv1");
    model->SetRotationAxisXYZ({0, 0, 0});
    model->SetScaleAxisXYZ({0.1f, 0.06f, 0.1f});

    auto attach = sword->AddComponent<ComponentAttachModel>();
    attach->SetAttachObject(player, "mixamorig:RightHand");
    attach->SetAttachRotate({0, 0, -90});
    attach->SetAttachOffset({10, 13, -3});

    return player;
}

bool Player::Init() {
    pModel = AddComponent<ComponentModel>("data/LittleQuest/Model/Mutant/Mutant.mv1");
    pModel.lock()->SetScaleAxisXYZ({0.05f});

    pModel.lock()->SetAnimation({
        {   "idle",             "data/LittleQuest/Anim/AxeSet/AxeIdle.mv1", 0, 1.0f},
        {   "jump",         "data/LittleQuest/Anim/SwordSet/SwordJump.mv1", 0, 1.0f},
        {   "walk",       "data/LittleQuest/Anim/AxeSet/AxeRunForward.mv1", 0, 1.0f},
        {"attack1",           "data/LittleQuest/Anim/AxeSet/AxeCombo1.mv1", 0, 2.0f},
        {"attack2",           "data/LittleQuest/Anim/AxeSet/AxeCombo2.mv1", 0, 2.0f},
        {"attack3", "data/LittleQuest/Anim/SwordSet/SwordAttackCombo3.mv1", 0, 1.5f},
        { "getHit",       "data/LittleQuest/Anim/SwordSet/SwordGetHit.mv1", 0, 1.0f}
    });

    auto colCap = AddComponent<ComponentCollisionCapsule>();
    colCap->SetTranslate({0, 0, 0});
    colCap->SetRadius(2.5);
    colCap->SetHeight(10);
    colCap->UseGravity();
    colCap->SetCollisionGroup(ComponentCollision::CollisionGroup::PLAYER);

    auto colWeapon = AddComponent<ComponentCollisionCapsule>();
    colWeapon->AttachToModel("mixamorig:RightHand");
    colWeapon->SetTranslate({10, 12, 0});
    colWeapon->SetRotationAxisXYZ({0, 0, -90});
    colWeapon->SetRadius(0.3f);
    colWeapon->SetHeight(5);
    colWeapon->SetCollisionGroup(ComponentCollision::CollisionGroup::WEAPON);
    colWeapon->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY);
    colWeapon->Overlap((u32)ComponentCollision::CollisionGroup::ENEMY);
    colWeapon->SetName("SwordCol");

    atkVal     = 50;
    selfMatrix = GetMatrix();
    getHit     = false;

    return Super::Init();
}

void Player::Update() {
    movement = float3(0, 0, 0);

    if(!pCamera.lock()) {
        pCamera      = Scene::GetObjectPtr<Camera>("PlayerCamera");
        cameraLength = pCamera.lock()->GetComponent<ComponentSpringArm>()->GetSpringArmLength();
    } else {
        float3 v   = GetTranslate() - pCamera.lock()->GetTranslate();
        selfMatrix = HelperLib::Math::CreateMatrixByFrontVector(-v);
    }

    InputHandle();

    if(currCombo != Combo::NO_COMBO) {
        playerState = PlayerState::ATTACK;
    }

    if(IsFloat3Zero(movement) && playerState != PlayerState::GET_HIT && playerState != PlayerState::ATTACK) {
        playerState = PlayerState::IDLE;
    }

    switch(playerState) {
    case PlayerState::GET_HIT:
        if(!pModel.lock()->IsPlaying()) {
            playerState = PlayerState::IDLE;
        }
        break;
    case PlayerState::ATTACK:
        Attack();
        break;
    case PlayerState::JUMP:
        break;
    case PlayerState::WALK:
        Walk();
        break;
    default:
        Idle();
        break;
    }

    movement *= speed * GetDeltaTime60() * !currCombo;
    AddTranslate(movement);
}

void        Player::LateDraw() {
#if defined _DEBUG
    printfDx("\nAnimation Time:%f", pModel.lock()->GetAnimationTime());
    printfDx("\nTotal Animation Time:%f", pModel.lock()->GetAnimationTotalTime());
    printfDx("\nAnimation Play Time:%f", pModel.lock()->GetAnimationPlayTime());
#endif
    pHP.lock()->DrawHPBar();
}

void Player::GUI() {
    Super::GUI();
}

void Player::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo) {
    if(hitInfo.collision_->GetName() == "SwordCol") {
        auto* owner = hitInfo.hit_collision_->GetOwner();

        Enemy* enemy;
        if(!(enemy = dynamic_cast<Enemy*>(owner)) && currCombo == Combo::NO_COMBO) {
            return;
        }

        bool inList = false;
        for(int i = 0; i < attackList.size(); i++) {
            if(attackList[i] == enemy->GetName().data()) {
                inList = true;
                break;
            }
        }

        if(!inList) {
            attackList.push_back(enemy->GetName().data());
            enemy->GetHit(this->atkVal);
        }
    }

    Super::OnHit(hitInfo);
}

void Player::GetHit(int damage) {
    pHP.lock()->TakeDamage(damage);

    if(pHP.lock()->GetHP() > 0) {
        pModel.lock()->PlayAnimation("getHit");
        playerState = PlayerState::GET_HIT;
        getHit      = true;
    } else {
        Scene::Change(Scene::GetScene<GameOverScene>());
    }
}

void Player::InputHandle() {
    cameraLength -= GetMouseWheelRotVol() * 3;
    cameraLength = std::min((std::max(cameraLength, 10.0f)), 100.0f);
    pCamera.lock()->SetCameraLength(cameraLength);

    if(IsKeyRepeat(KEY_INPUT_W)) {
        float3 vec = selfMatrix.axisZ();
        vec.y      = 0;
        movement += -vec;
        playerState = PlayerState::WALK;
    }
    if(IsKeyRepeat(KEY_INPUT_D)) {
        float3 vec = selfMatrix.axisX();
        vec.y      = 0;
        movement += -vec;
        playerState = PlayerState::WALK;
    }
    if(IsKeyRepeat(KEY_INPUT_S)) {
        float3 vec = selfMatrix.axisZ();
        vec.y      = 0;
        movement += vec;
        playerState = PlayerState::WALK;
    }
    if(IsKeyRepeat(KEY_INPUT_A)) {
        float3 vec = selfMatrix.axisX();
        vec.y      = 0;
        movement += vec;
        playerState = PlayerState::WALK;
    }

    if(IsMouseDown(MOUSE_INPUT_LEFT)) {
        playerState = PlayerState::ATTACK;

        if(currCombo == Combo::NO_COMBO) {
            currCombo = Combo::NORMAL_COMBO1;
        } else if(waitForCombo) {
            isCombo = true;
        }
    }
}

void Player::Idle() {
    pModel.lock()->PlayAnimationNoSame("idle", true, .5f);
}

void Player::Walk() {
    if(IsFloat3Zero(movement)) {
        return;
    }

    movement = normalize(movement);
    this->SetModelRotation();

    if(pModel.lock()->GetPlayAnimationName() != "walk") {
        pModel.lock()->PlayAnimation("walk", true, 0.2f, 14.0f);
    }
}

void Player::Jump() {}

void Player::Attack() {
    switch(currCombo) {
    case Combo::NORMAL_COMBO1:
        if(pModel.lock()->GetPlayAnimationName() != "attack1") {
            this->SetModelRotation();
            pModel.lock()->PlayAnimationNoSame("attack1");
            attackList.clear();
        }

        if(!pModel.lock()->IsPlaying()) {
            currCombo = Combo::NO_COMBO;
            if(isCombo) {
                currCombo = Combo::NORMAL_COMBO2;
                isCombo   = false;
            }
        } else {
            waitForCombo = true;
        }
        break;
    case Combo::NORMAL_COMBO2:
        if(pModel.lock()->GetPlayAnimationName() != "attack2") {
            pModel.lock()->PlayAnimationNoSame("attack2");
            this->SetModelRotation();
            attackList.clear();
        }

        if(!pModel.lock()->IsPlaying()) {
            currCombo = Combo::NO_COMBO;
            if(isCombo) {
                currCombo = Combo::NORMAL_COMBO3;
                isCombo   = false;
            }
        } else {
            waitForCombo = true;
        }
        break;
    case Combo::NORMAL_COMBO3:
        if(pModel.lock()->GetPlayAnimationName() != "attack3") {
            this->SetModelRotation();
            pModel.lock()->PlayAnimationNoSame("attack3");
            attackList.clear();
        }
        if(!pModel.lock()->IsPlaying()) {
            currCombo   = Combo::NO_COMBO;
            playerState = PlayerState::IDLE;
        }
        break;
    default:
        playerState = PlayerState::IDLE;
        currCombo   = Combo::NO_COMBO;
        break;
    }
}

void Player::SetModelRotation() {
    if(!IsFloat3Zero(movement)) {
        float x     = -movement.x;
        float z     = -movement.z;
        float theta = atan2(x, z) * RadToDeg;
        pModel.lock()->SetRotationAxisXYZ({0, theta, 0});
    }
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Player)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Player)
