#include "Player.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionLine.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>
#include <System/Component/ComponentTargetTracking.h>

#include "Camera.h"
#include "Enemy.h"

namespace LittleQuest
{

//! @brief Editor上でのCreateObject用の設定
//! @detail BP_OBJECT_TYPEとセットで用意する
BP_OBJECT_IMPL(Player, "LittleQuest/Player");

PlayerPtr Player::Create(const float3& pos, const float3& front)
{
    // 箱の作成
    auto player = Scene::CreateObjectPtr<Player>();
    player->SetName("Player");

    // vecの方向に向ける
    auto mat = HelperLib::Math::CreateMatrixByFrontVector(front);
    player->SetMatrix(mat);

    // posの位置に設定
    player->SetTranslate(pos);

    {
        auto sword = Sword::Create("PlayerSword", float3{0, 0, 0});
        sword->SetTranslate({0, 0, 0});

        // オブジェクトにモデルをつける
        if(auto model = sword->AddComponent<ComponentModel>()) {
            model->Load("data/LittleQuest/Model/Sword/Sword.mv1");
            model->SetRotationAxisXYZ({0, 0, 0});
            model->SetScaleAxisXYZ({0.1f, 0.06f, 0.1f});
        }

        if(auto attach = sword->AddComponent<ComponentAttachModel>()) {
            // playerの右手にアタッチする
            attach->SetAttachObject(player, "mixamorig:RightHand");
            // Knife回転量
            attach->SetAttachRotate({0, 0, -90});
            // Knifeオフセット
            attach->SetAttachOffset({10, 13, -3});
        }
    }
    return player;
}

bool Player::Init()   // override
{
    Super::Init();

    // モデルコンポーネント(0.08倍)
    auto model = AddComponent<ComponentModel>("data/LittleQuest/Model/Guard/Guard.mv1");

    model->SetScaleAxisXYZ({0.05f});   //

    model->SetAnimation({
        {   "idle",         "data/LittleQuest/Anim/SwordIdle.mv1", 0, 1.0f},
        {   "jump",         "data/LittleQuest/Anim/SwordJump.mv1", 0, 1.0f},
        {   "walk",         "data/LittleQuest/Anim/SwordWalk.mv1", 0, 1.0f},
        {"attack1", "data/LittleQuest/Anim/SwordAttackCombo1.mv1", 0, 1.5f},
        {"attack2", "data/LittleQuest/Anim/SwordAttackCombo2.mv1", 0, 1.0f},
        {"attack3", "data/LittleQuest/Anim/SwordAttackCombo3.mv1", 0, 1.5f},
        {"damaged",      "data/LittleQuest/Anim/SwordDamaged.mv1", 0, 1.0f}
    });

    // modelPtr = GetComponent<ComponentModel>();
    //  コリジョン(カプセル)
    if(auto colCap = AddComponent<ComponentCollisionCapsule>()) {
        colCap->SetTranslate({0, 0, 0});
        colCap->SetRadius(2.5);
        colCap->SetHeight(10);
        colCap->UseGravity();
        colCap->SetCollisionGroup(ComponentCollision::CollisionGroup::PLAYER);
    }

    if(auto colLine = AddComponent<ComponentCollisionLine>()) {
        colLine->AttachToModel("mixamorig:RightHand");
        colLine->SetTranslate({0, 0, 0});
        colLine->SetLine(float3{0, 15, 0}, float3{110, 15, 1});
        colLine->SetCollisionGroup(ComponentCollision::CollisionGroup::WEAPON);
        colLine->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY);
        colLine->Overlap((u32)ComponentCollision::CollisionGroup::ENEMY);
        colLine->SetName("SwordCol");
    }
    /* auto target = AddComponent<ComponentTargetTracking>();
                    target->SetTrackingNode("mixamorig:Neck");
                    target->SetFrontVector({0, 0, -1});

                    target->SetTrackingLimitLeftRight({70, 70});

                    target->SetTrackingLimitUpDown({10, 10});*/

    /*auto sword = Sword::Create();*/

    atkVal = 50;

    return true;
}

void Player::Update()   // override
{
    if(!sword) {
        sword = Scene::GetObjectPtr<Sword>("PlayerSword");
    }

    auto   mat  = GetMatrix();
    float3 move = float3(0, 0, 0);
    isWalk      = false;

    // #ifdef USE_MOUSE_CAMERA
    // カメラ方向を取得してその方向に動かす
    auto   cam = Scene::GetObjectPtr<Camera>("PlayerCamera");
    float3 v   = GetTranslate() - cam->GetTranslate();
    mat        = HelperLib::Math::CreateMatrixByFrontVector(-v);
    // #endif    // USE_MOUSE_CAMERA

#pragma region INPUT
    if(IsMouseDown(MOUSE_INPUT_LEFT)) {
        isAttack    = true;
        playerState = PlayerState::ATTACK;

        if(combo == 0) {
            combo = 1;
        }
        else if(combo == 1 && canCombo2) {
            isCombo   = true;
            canCombo2 = false;
        }
        else if(combo == 2 && canCombo3) {
            isCombo   = true;
            canCombo3 = false;
        }
    }

    if(!isAttack) {
        if(IsKeyRepeat(KEY_INPUT_W)) {
            float3 vec = mat.axisZ();
            vec.y      = 0;
            move += -vec;
            isWalk      = true;
            playerState = PlayerState::WALK;
        }
        if(IsKeyRepeat(KEY_INPUT_D)) {
            float3 vec = mat.axisX();
            vec.y      = 0;
            move += -vec;
            isWalk      = true;
            playerState = PlayerState::WALK;
        }
        if(IsKeyRepeat(KEY_INPUT_S)) {
            float3 vec = mat.axisZ();
            vec.y      = 0;
            move += vec;
            isWalk      = true;
            playerState = PlayerState::WALK;
        }
        if(IsKeyRepeat(KEY_INPUT_A)) {
            float3 vec = mat.axisX();
            vec.y      = 0;
            move += vec;
            isWalk      = true;
            playerState = PlayerState::WALK;
        }
    }
#pragma endregion

    if(!isWalk && !isAttack) {
        playerState = PlayerState::IDLE;
    }

    switch(playerState) {
    case PlayerState::DAMAGED:
        break;
    case PlayerState::ATTACK:
        Attack();
        break;
    case PlayerState::JUMP:
        break;
    case PlayerState::WALK:
        Walk(move);
        break;
    default:
        Idle();
        break;
    }

    /*if (isAttack) {
                    } else if (length(move).x > 0) {
                        this->Walk(move);
                    } else {
                        this->Idle();
                    }*/

    move *= speed_ * GetDeltaTime60();

    // 地面移動スピードを決定する
    AddTranslate(move);
    MouseWheelCounter += GetMouseWheelRotVol();
}

// 基本描画の後に処理します
void Player::LateDraw()   // override
{
    // gauge_.Draw();
    printfDx("\ncombo:%d", combo);
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        printfDx("\nAnimation Time:%f", modelPtr->GetAnimationTime());
    }
    printfDx("\nisAttack: %i", isAttack);
    printfDx("\nisCombo: %i", isCombo);
    printfDx("\nGetMouseHWheelRotVol: %i", MouseWheelCounter);

    // auto sword = Scene::GetObjectPtr<Object>("PlayerSword");
    // auto col   = sword->GetComponent<ComponentCollisionModel>();
    // printfDx("\nCol name : %s", col->GetOwnerPtr()->GetName());
}

void Player::GUI()   // override
{
    Super::GUI();

    //// GUI描画
    // ImGui::Begin(GetName().data());
    //{
    //     ImGui::Separator();

    //    ImGui::DragFloat(u8"速度", &speed_, 0.01f, 0.01f, 10.0f, "%2.2f");
    //    // ImGui::DragFloat(u8"HitPoint", &gauge_.Value(), 1, 0, 100,
    //    // "%3.0f");
    //}
    // ImGui::End();
}

void Player::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo)   // override
{
    if(hitInfo.collision_->GetName() == "Player") {
        // 次のownerのオブジェクトと当たった!
        auto owner = hitInfo.hit_collision_->GetOwnerPtr();
        printfDx("\nBody Hit:%s", owner->GetName().data());
    }

    if(hitInfo.collision_->GetName() == "SwordCol") {
        auto* owner = hitInfo.hit_collision_->GetOwner();
        printfDx("\nSword Hit:%s", owner->GetName().data());
        if(isAttack) {
            if(auto enemy = dynamic_cast<Enemy*>(owner)) {
                bool inList = false;
                for(int i = 0; i < attackList.size(); i++) {
                    if(attackList[i] == enemy->GetName().data()) {
                        inList = true;
                        break;
                    }
                }
                if(!inList) {
                    attackList.push_back(enemy->GetName().data());
                    enemy->Damaged(this->atkVal);
                }

                // attackList.push_back(enemy);
            }
        }
    }

    // if (owner->GetNameDefault() == "Enemy") {
    //     if (gauge_.Value() > 0) {
    //         gauge_ -= 1;
    //         if (gauge_ <= 0) Scene::Change(Scene::GetScene<OverScene>());
    //     }

    //    // Scene::ReleaseObject( owner );
    //}

    // 当たりで移動させる(これが無ければめり込みます)
    __super::OnHit(hitInfo);
}

void Player::Idle()
{
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        if(modelPtr->GetPlayAnimationName() != "idle") {
            modelPtr->PlayAnimation("idle", true);
        }
    }
}

void Player::Walk(float3& move)
{
    if(move.x == 0 || move.z == 0) {
        return;
    }

    if(auto modelPtr = GetComponent<ComponentModel>()) {
        // 動いてる
        move = normalize(move);

        float x     = -move.x;
        float z     = -move.z;
        float theta = atan2(x, z) * RadToDeg - rot_y_;
        // モデルだけ回転 (ついてるカメラは回らない)
        modelPtr->SetRotationAxisXYZ({0, theta, 0});

        if(modelPtr->GetPlayAnimationName() != "walk")
            modelPtr->PlayAnimation("walk", true);
    }
}

void Player::Jump()
{
}

void Player::Attack()
{
    // auto sword = Scene::GetObjectPtr<Object>("PlayerSword");
    // auto col   = sword->GetComponent<ComponentCollisionModel>();

    if(auto modelPtr = GetComponent<ComponentModel>()) {
        if(combo == 1) {
            if(modelPtr->GetPlayAnimationName() != "attack1") {
                modelPtr->PlayAnimationNoSame("attack1");
                /*sword->Attack();*/
                attackList.clear();
            }
            if(modelPtr->GetAnimationTime() > 0.9f) {
                if(!isCombo) {
                    isAttack = false;
                    combo    = 0;

                    playerState = PlayerState::IDLE;
                }
                else {
                    combo   = 2;
                    isCombo = false;
                }
            }
            if(modelPtr->GetAnimationTime() > 0.6f) {
                canCombo2 = true;
            }
        }

        if(combo == 2) {
            if(modelPtr->GetPlayAnimationName() != "attack2") {
                modelPtr->PlayAnimationNoSame("attack2");
                attackList.clear();
            }
            if(modelPtr->GetAnimationTime() > 0.9f) {
                if(!isCombo) {
                    isAttack    = false;
                    isCombo     = false;
                    combo       = 0;
                    playerState = PlayerState::IDLE;
                }
                else {
                    combo   = 3;
                    isCombo = false;
                }
            }
            if(modelPtr->GetAnimationTime() > 0.3f && !isCombo) {
                canCombo3 = true;
                isCombo   = false;
            }
        }

        if(combo == 3) {
            if(modelPtr->GetPlayAnimationName() != "attack3") {
                modelPtr->PlayAnimationNoSame("attack3");
                attackList.clear();
            }
            if(modelPtr->GetAnimationTime() > 0.9f) {
                isAttack    = false;
                isCombo     = false;
                combo       = 0;
                playerState = PlayerState::IDLE;
            }
        }

        if(combo == 0) {
            isAttack = false;
            isCombo  = false;
            sword->FinishAttack();
            playerState = PlayerState::IDLE;
        }
    }
}

void Player::Damaged()
{
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        if(modelPtr->GetPlayAnimationName() != "damaged") {
            modelPtr->PlayAnimationNoSame("damaged");
        }
    }
}

void Player::SetSpeed(float s)
{
    speed_ = s;
}

float Player::GetSpeed()
{
    return speed_;
}

float& Player::Speed()
{
    return speed_;
}

}   // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Player)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Player)
