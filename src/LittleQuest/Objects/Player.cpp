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
        {   STR(PlayerState::IDLE),           "data/LittleQuest/Anim/AxeSet/AxeIdle.mv1", 0, 1.0f},
        {                   "jump",       "data/LittleQuest/Anim/SwordSet/SwordJump.mv1", 0, 1.0f},
        {   STR(PlayerState::WALK),     "data/LittleQuest/Anim/AxeSet/AxeRunForward.mv1", 0, 1.0f},
        {STR(Combo::NORMAL_COMBO1),         "data/LittleQuest/Anim/AxeSet/AxeCombo1.mv1", 0, 4.5f},
        {STR(Combo::NORMAL_COMBO2),         "data/LittleQuest/Anim/AxeSet/AxeCombo2.mv1", 0, 4.5f},
        {STR(Combo::NORMAL_COMBO3), "data/LittleQuest/Anim/AxeSet/AxeAttackDownward.mv1", 0, 4.5f},
        {STR(PlayerState::GET_HIT),     "data/LittleQuest/Anim/SwordSet/SwordGetHit.mv1", 0, 1.0f}
    });

    SetAnimInfo();

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

    if(currCombo == Combo::NO_COMBO) {
        movement *= speed * GetDeltaTime60();
        AddTranslate(movement);
    }
}

void Player::LateDraw() {
    pHP.lock()->DrawHPBar();
#if defined _DEBUG
    printfDx("\nAnimation Time:%f", pModel.lock()->GetAnimationTime());
    printfDx("\nTotal Animation Time:%f", pModel.lock()->GetAnimationTotalTime());
    printfDx("\nAnimation Play Time:%f", pModel.lock()->GetAnimationPlayTime());
    printfDx("\nAnimation Name:%s", pModel.lock()->GetPlayAnimationName().data());
#endif
}

void Player::GUI() {
    Super::GUI();
}

void Player::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo) {
    if(hitInfo.collision_->GetName() == "SwordCol") {
        auto* owner = hitInfo.hit_collision_->GetOwner();

        Enemy* enemy;
        if((enemy = dynamic_cast<Enemy*>(owner)) && currCombo != Combo::NO_COMBO) {
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
    }

    Super::OnHit(hitInfo);
}

void Player::GetHit(int damage) {
    pHP.lock()->TakeDamage(damage);

    if(pHP.lock()->GetHP() > 0) {
        pModel.lock()->PlayAnimation(STR(PlayerState::GET_HIT));
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
    pModel.lock()->PlayAnimationNoSame(STR(PlayerState::IDLE), true, 0.5f);
}

void Player::Walk() {
    if(IsFloat3Zero(movement)) {
        return;
    }

    movement = normalize(movement);
    this->SetModelRotation();

    pModel.lock()->PlayAnimationNoSame(STR(PlayerState::WALK), true, 0.2f, 14.0f);
}

void Player::Jump() {}

void Player::Attack() {
    switch(currCombo) {
    case Combo::NORMAL_COMBO1:
        if(pModel.lock()->GetPlayAnimationName() != STR(Combo::NORMAL_COMBO1)) {
            this->SetModelRotation();
            pModel.lock()->PlayAnimationNoSame(STR(Combo::NORMAL_COMBO1));
            attackList.clear();
        }

        if(!pModel.lock()->GetAnimationPlayTime() > animList[STR(Combo::NORMAL_COMBO1)].triggerEndTime) {
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
        if(pModel.lock()->GetPlayAnimationName() != STR(Combo::NORMAL_COMBO2)) {
            pModel.lock()->PlayAnimationNoSame(STR(Combo::NORMAL_COMBO2));
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
        if(pModel.lock()->GetPlayAnimationName() != STR(Combo::NORMAL_COMBO3)) {
            this->SetModelRotation();
            pModel.lock()->PlayAnimationNoSame(STR(Combo::NORMAL_COMBO3));
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

void Player::SetAnimInfo() {
    {
        AnimInfo info         = {};
        info.triggerStartTime = 56;
        info.triggerEndTime   = 66;
        info.animCutInTime    = 82;

        animList[STR(Combo::NORMAL_COMBO1)] = info;
    }
    {
        AnimInfo info                       = {};
        info.animStartTime                  = 8;
        info.triggerStartTime               = 20;
        info.triggerEndTime                 = 35;
        info.animCutInTime                  = 43;
        animList[STR(Combo::NORMAL_COMBO2)] = info;
    }
    {
        AnimInfo info                       = {};
        info.animStartTime                  = 33;
        info.triggerStartTime               = 45;
        info.triggerEndTime                 = 58;
        info.animCutInTime                  = 82;
        animList[STR(Combo::NORMAL_COMBO3)] = info;
    }
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Player)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Player)
