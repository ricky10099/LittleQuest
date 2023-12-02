#include "Player.h"
#include "Enemy.h"
#include "Camera.h"

#include "../Components/ComponentHP.h"
#include <LittleQuest/Scenes/GameOverScene.h>

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionLine.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>

namespace LittleQuest {

    //! @brief Editor上でのCreateObject用の設定
    //! @detail BP_OBJECT_TYPEとセットで用意する
    BP_OBJECT_IMPL(Player, "LittleQuest/Player");

    PlayerPtr Player::Create(const float3& pos, const float3& front) {
        // 箱の作成
        auto player = Scene::CreateObjectPtr<Player>();
        player->SetName("Player");

        // vecの方向に向ける
        auto mat = HelperLib::Math::CreateMatrixByFrontVector(front);
        player->SetMatrix(mat);

        // posの位置に設定
        player->SetTranslate(pos);

        auto HP = player->AddComponent<ComponentHP>();
        HP->SetHP(100);

        {
            auto sword = Scene::CreateObjectPtr<Object>("PlayerSword");

            if (auto model = sword->AddComponent<ComponentModel>()) {
                model->Load("data/LittleQuest/Model/Sword/Sword.mv1");
                model->SetRotationAxisXYZ({0, 0, 0});
                model->SetScaleAxisXYZ({0.1f, 0.06f, 0.1f});
            }

            if (auto attach = sword->AddComponent<ComponentAttachModel>()) {
                attach->SetAttachObject(player, "mixamorig:RightHand");
                attach->SetAttachRotate({0, 0, -90});
                attach->SetAttachOffset({10, 13, -3});
            }
        }

        return player;
    }

    bool Player::Init()    // override
    {
        Super::Init();

        // モデルコンポーネント(0.05倍)
        auto model = AddComponent<ComponentModel>("data/LittleQuest/Model/Mutant/Mutant.mv1");
        model->SetScaleAxisXYZ({0.05f});

        model->SetAnimation({{"idle", "data/LittleQuest/Anim/SwordIdle.mv1", 0, 1.0f},
                             {"jump", "data/LittleQuest/Anim/SwordJump.mv1", 0, 1.0f},
                             {"walk", "data/LittleQuest/Anim/SwordWalk.mv1", 0, 1.0f},
                             {"attack1", "data/LittleQuest/Anim/SwordAttackCombo1.mv1", 0, 1.5f},
                             {"attack2", "data/LittleQuest/Anim/SwordAttackCombo2.mv1", 0, 1.0f},
                             {"attack3", "data/LittleQuest/Anim/SwordAttackCombo3.mv1", 0, 1.5f},
                             {"getHit", "data/LittleQuest/Anim/SwordGetHit.mv1", 0, 1.0f}});

        //  コリジョン(カプセル)
        if (auto colCap = AddComponent<ComponentCollisionCapsule>()) {
            colCap->SetTranslate({0, 0, 0});
            colCap->SetRadius(2.5);
            colCap->SetHeight(10);
            colCap->UseGravity();
            colCap->SetCollisionGroup(ComponentCollision::CollisionGroup::PLAYER);
        }

        // コリジョン(武器)
        if (auto colWeapon = AddComponent<ComponentCollisionCapsule>()) {
            colWeapon->AttachToModel("mixamorig:RightHand");
            colWeapon->SetTranslate({10, 12, 0});
            colWeapon->SetRotationAxisXYZ({0, 0, -90});
            colWeapon->SetRadius(0.3f);
            colWeapon->SetHeight(5);
            colWeapon->SetCollisionGroup(ComponentCollision::CollisionGroup::WEAPON);
            colWeapon->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY);
            colWeapon->Overlap((u32)ComponentCollision::CollisionGroup::ENEMY);
            colWeapon->SetName("SwordCol");
        }

        atkVal = 50;

        return true;
    }

    void Player::Update()    // override
    {
        auto mat    = GetMatrix();
        float3 move = float3(0, 0, 0);
        // isWalk      = false;

        // #ifdef USE_MOUSE_CAMERA
        // カメラ方向を取得してその方向に動かす
        auto cam        = Scene::GetObjectPtr<Camera>("PlayerCamera");
        float3 v        = GetTranslate() - cam->GetTranslate();
        mat             = HelperLib::Math::CreateMatrixByFrontVector(-v);
        float camLength = cam->GetComponent<ComponentSpringArm>()->GetSpringArmLength();
        // カメラ距離の調整
        camLength -= GetMouseWheelRotVol() * 3;
        if (camLength < 10) {
            camLength = 10;
        } else if (camLength > 100) {
            camLength = 100;
        }
        cam->GetComponent<ComponentSpringArm>()->SetSpringArmLength(camLength);
        // #endif    // USE_MOUSE_CAMERA

#pragma region INPUT
        if (IsMouseDown(MOUSE_INPUT_LEFT)) {
            // isAttack    = true;
            playerState = PlayerState::ATTACK;

            if (combo == 0) {
                combo = 1;
            } else if (combo == 1 && canCombo2) {
                isCombo = true;
                // canCombo2 = false;
            } else if (combo == 2 && canCombo3) {
                // isCombo   = true;
                // canCombo3 = false;
            }

            // switch (currCombo) {
            //     case Combo::NO_COMBO:
            //         currCombo = Combo::COMBO1;
            //         break;
            //     case Combo::COMBO1:
            //         currCombo = (Combo)0;
            //         break;
            //     case Combo::COMBO2:
            //         break;
            //     case Combo::COMBO3:
            //         break;
            // }
        }

        /*if (!isAttack) */ {
            if (IsKeyRepeat(KEY_INPUT_W)) {
                float3 vec = mat.axisZ();
                vec.y      = 0;
                move += -vec;
                if (!combo) {
                    // isWalk      = true;
                    playerState = PlayerState::WALK;
                }
            }
            if (IsKeyRepeat(KEY_INPUT_D)) {
                float3 vec = mat.axisX();
                vec.y      = 0;
                move += -vec;
                if (!combo) {
                    // isWalk      = true;
                    playerState = PlayerState::WALK;
                }
            }
            if (IsKeyRepeat(KEY_INPUT_S)) {
                float3 vec = mat.axisZ();
                vec.y      = 0;
                move += vec;
                if (!combo) {
                    // isWalk      = true;
                    playerState = PlayerState::WALK;
                }
            }
            if (IsKeyRepeat(KEY_INPUT_A)) {
                float3 vec = mat.axisX();
                vec.y      = 0;
                move += vec;
                if (!combo) {
                    // isWalk      = true;
                    playerState = PlayerState::WALK;
                }
            }
        }
#pragma endregion

        if (move.x == 0 && move.y == 0 && move.z == 0 && combo == 0) {
            playerState = PlayerState::IDLE;
        }

        switch (playerState) {
            case PlayerState::GET_HIT:
                break;
            case PlayerState::ATTACK:
                Attack(move);
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

        move *= speed_ * GetDeltaTime60() * !combo;

        // 地面移動スピードを決定する
        AddTranslate(move);
    }

    // 基本描画の後に処理します
    void Player::LateDraw()    // override
    {
// gauge_.Draw();
#if defined _DEBUG
        printfDx("\ncombo:%d", combo);
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            float* mat = modelPtr->GetMatrixFloat();
            float matrixTranslation[3], matrixRotation[3], matrixScale[3];
            DecomposeMatrixToComponents(mat, matrixTranslation, matrixRotation, matrixScale);
            printfDx("\nAnimation Time:%f", modelPtr->GetAnimationTime());
            printfDx("\nmodel rotx:%f", modelPtr->GetRotationAxisXYZ().x);
            printfDx("\nmodel roty:%f", matrixRotation[1]);
            printfDx("\nmodel rotz:%f", modelPtr->GetRotationAxisXYZ().z);
            printfDx("\nTotal Animation Time:%f", modelPtr->GetAnimationTotalTime());
            printfDx("\nAnimation Play Time:%f", modelPtr->GetAnimationPlayTime());
        }
        // printfDx("\nisAttack: %i", isAttack);
        printfDx("\nisCombo: %i", isCombo);
        // float x, y, z;

        printfDx("\nMatAYx: %f %f %f", GetMatrix().axisX().x, GetMatrix().axisX().y, GetMatrix().axisX().z);
        printfDx("\nMatAYz: %f %f %f", GetRotationAxisXYZ().z, GetMatrix().axisZ().y, GetMatrix().axisZ().z);
#endif
    }

    void Player::GUI()    // override
    {
        Super::GUI();
    }

    void Player::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo)    // override
    {
        // 武器の衝突判定
        if (hitInfo.collision_->GetName() == "SwordCol") {
            auto* owner = hitInfo.hit_collision_->GetOwner();
            if (combo != 0) {
                if (auto enemy = dynamic_cast<Enemy*>(owner)) {
                    bool inList = false;
                    for (int i = 0; i < attackList.size(); i++) {
                        if (attackList[i] == enemy->GetName().data()) {
                            inList = true;
                            break;
                        }
                    }

                    if (!inList) {
                        attackList.push_back(enemy->GetName().data());
                        enemy->GetHit(this->atkVal);
                    }
                }
            }
        }

        Super::OnHit(hitInfo);
    }

    void Player::Idle() {
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            // if (modelPtr->GetPlayAnimationName() != "idle") {
            modelPtr->PlayAnimationNoSame("idle", true);
            //}
        }
    }

    void Player::Walk(float3& move) {
        // 逆方向同時に押すと消えるので
        if (move.x == 0 || move.z == 0) {
            return;
        }

        if (auto modelPtr = GetComponent<ComponentModel>()) {
            move = normalize(move);

            this->SetModelRotation(move);

            if (modelPtr->GetPlayAnimationName() != "walk") modelPtr->PlayAnimation("walk", true, 0.2f, 14.0f);
        }
    }

    void Player::Jump() {}

    void Player::Attack(float3& move) {
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            if (combo == 1) {
                if (modelPtr->GetPlayAnimationName() != "attack1") {
                    this->SetModelRotation(move);
                    modelPtr->PlayAnimationNoSame("attack1");
                    attackList.clear();
                }

                // 攻撃判定終わる時間
                // if (modelPtr->GetAnimationTime() > 0.9f) {
                // if (!modelPtr->IsPlaying()){
                //    if (!isCombo) {
                //        isAttack    = false;
                //        combo       = 0;
                //        playerState = PlayerState::IDLE;
                //    } else {
                //        combo   = 2;
                //        isCombo = false;
                //    }
                //}

                if (!modelPtr->IsPlaying()) {
                    // isAttack = false;
                    combo = 0;
                    if (isCombo) {
                        combo   = 2;
                        isCombo = false;
                    }
                }
                // コンボ判定する時間
                // if (modelPtr->GetAnimationTime() > 0.6f) {
                if (modelPtr->IsPlaying()) {
                    canCombo2 = true;
                }
            }

            if (combo == 2) {
                if (modelPtr->GetPlayAnimationName() != "attack2") {
                    modelPtr->PlayAnimationNoSame("attack2");
                    this->SetModelRotation(move);
                    attackList.clear();
                }

                if (modelPtr->GetAnimationTime() > 0.9f) {
                    // if (!isCombo) {
                    // isAttack    = false;
                    //    isCombo     = false;
                    combo = 0;
                    //    playerState = PlayerState::IDLE;
                    //} else {
                    //    combo   = 3;
                    //    isCombo = false;
                    //}
                }
                if (modelPtr->GetAnimationTime() > 0.3f /*&& !isCombo*/) {
                    canCombo3 = true;
                    // isCombo   = false;
                }
            }

            if (combo == 3) {
                if (modelPtr->GetPlayAnimationName() != "attack3") {
                    this->SetModelRotation(move);
                    modelPtr->PlayAnimationNoSame("attack3");
                    attackList.clear();
                }
                if (modelPtr->GetAnimationTime() > 0.9f) {
                    // isAttack    = false;
                    // isCombo     = false;
                    combo       = 0;
                    playerState = PlayerState::IDLE;
                }
            }

            if (combo == 0) {
                // isAttack    = false;
                // isCombo     = false;
                playerState = PlayerState::IDLE;
            }
        }
    }

    void Player::SetModelRotation(float3& move) {
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            if (move.x != 0 || move.z != 0) {
                float x     = -move.x;
                float z     = -move.z;
                float theta = atan2(x, z) * RadToDeg;
                modelPtr->SetRotationAxisXYZ({0, theta, 0});
            }
        }
    }

    void Player::GetHit(int damage) {
        auto hpcomponent = GetComponent<ComponentHP>();
        hpcomponent->TakeDamage(damage);

        if (auto modelPtr = GetComponent<ComponentModel>()) {
            if (hpcomponent->GetHP() > 0) modelPtr->PlayAnimation("getHit");
            else {
                Scene::Change(Scene::GetScene<GameOverScene>());
            }
        }
    }
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Player)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Player)
