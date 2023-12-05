#include "Player.h"
#include "Enemy.h"
#include "Camera.h"
#include "LittleQuest/Components/ComponentHP.h"
#include "LittleQuest/Scenes/GameOverScene.h"
#include "LittleQuest/Tool.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>

namespace LittleQuest {

    //! @brief Editor上でのCreateObject用の設定
    //! @detail BP_OBJECT_TYPEとセットで用意する
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

    bool Player::Init()    // override
    {
        Super::Init();

        // モデルコンポーネント(0.05倍)
        pModel = AddComponent<ComponentModel>("data/LittleQuest/Model/Mutant/Mutant.mv1");
        pModel.lock()->SetScaleAxisXYZ({0.05f});

        pModel.lock()->SetAnimation({{"idle", "data/LittleQuest/Anim/SwordIdle.mv1", 0, 1.0f},
                                     {"jump", "data/LittleQuest/Anim/SwordJump.mv1", 0, 1.0f},
                                     {"walk", "data/LittleQuest/Anim/SwordWalk.mv1", 0, 1.0f},
                                     {"attack1", "data/LittleQuest/Anim/SwordAttackCombo1.mv1", 0, 1.5f},
                                     {"attack2", "data/LittleQuest/Anim/SwordAttackCombo2.mv1", 0, 1.0f},
                                     {"attack3", "data/LittleQuest/Anim/SwordAttackCombo3.mv1", 0, 1.5f},
                                     {"getHit", "data/LittleQuest/Anim/SwordGetHit.mv1", 0, 1.0f}});

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

        atkVal      = 50;
        self_matrix = GetMatrix();
        getHit      = false;
        return true;
    }

    void Player::Update()    // override
    {
        movement = float3(0, 0, 0);

        if (!pCamera.lock()) {
            pCamera      = Scene::GetObjectPtr<Camera>("PlayerCamera");
            cameraLength = pCamera.lock()->GetComponent<ComponentSpringArm>()->GetSpringArmLength();
        } else {
            float3 v    = GetTranslate() - pCamera.lock()->GetTranslate();
            self_matrix = HelperLib::Math::CreateMatrixByFrontVector(-v);
        }

        InputHandle();

        if (currCombo != Combo::NO_COMBO) {
            playerState = PlayerState::ATTACK;
        }

        if (CheckFloat3Zero(movement) && playerState != PlayerState::GET_HIT && playerState != PlayerState::ATTACK) {
            playerState = PlayerState::IDLE;
        }

        switch (playerState) {
            case PlayerState::GET_HIT:
                if (!pModel.lock()->IsPlaying()) {
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

        movement *= speed_ * GetDeltaTime60() * !currCombo;

        // 地面移動スピードを決定する
        AddTranslate(movement);
    }

    // 基本描画の後に処理します
    void Player::LateDraw()    // override
    {
        // gauge_.Draw();
#if defined _DEBUG
        // printfDx("\ncombo:%d", combo);
        float* mat = pModel.lock()->GetMatrixFloat();
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        DecomposeMatrixToComponents(mat, matrixTranslation, matrixRotation, matrixScale);
        printfDx("\nAnimation Time:%f", pModel.lock()->GetAnimationTime());
        printfDx("\nmodel rotx:%f", pModel.lock()->GetRotationAxisXYZ().x);
        printfDx("\nmodel roty:%f", matrixRotation[1]);
        printfDx("\nmodel rotz:%f", pModel.lock()->GetRotationAxisXYZ().z);
        printfDx("\nTotal Animation Time:%f", pModel.lock()->GetAnimationTotalTime());
        printfDx("\nAnimation Play Time:%f", pModel.lock()->GetAnimationPlayTime());
        // printfDx("\nisAttack: %i", isAttack);
        printfDx("\nisCombo: %i", isCombo);
        printfDx("\n!CurrCombo: %i", !currCombo);
        // float x, y, z;

        printfDx("\nMatAYx: %f %f %f", GetMatrix().axisX().x, GetMatrix().axisX().y, GetMatrix().axisX().z);
        printfDx("\nMatAYz: %f %f %f", GetRotationAxisXYZ().z, GetMatrix().axisZ().y, GetMatrix().axisZ().z);
#endif
        pHP.lock()->DrawHPBar();
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
            if (currCombo != Combo::NO_COMBO) {
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

    void Player::InputHandle() {
        // カメラ距離の調整
        cameraLength -= GetMouseWheelRotVol() * 3;
        if (cameraLength < 10) {
            cameraLength = 10;
        } else if (cameraLength > 100) {
            cameraLength = 100;
        }
        pCamera.lock()->GetComponent<ComponentSpringArm>()->SetSpringArmLength(cameraLength);
        // #endif    // USE_MOUSE_CAMERA

        if (IsKeyRepeat(KEY_INPUT_W)) {
            float3 vec = self_matrix.axisZ();
            vec.y      = 0;
            movement += -vec;
            playerState = PlayerState::WALK;
        }
        if (IsKeyRepeat(KEY_INPUT_D)) {
            float3 vec = self_matrix.axisX();
            vec.y      = 0;
            movement += -vec;
            playerState = PlayerState::WALK;
        }
        if (IsKeyRepeat(KEY_INPUT_S)) {
            float3 vec = self_matrix.axisZ();
            vec.y      = 0;
            movement += vec;
            playerState = PlayerState::WALK;
        }
        if (IsKeyRepeat(KEY_INPUT_A)) {
            float3 vec = self_matrix.axisX();
            vec.y      = 0;
            movement += vec;
            playerState = PlayerState::WALK;
        }

        if (IsMouseDown(MOUSE_INPUT_LEFT)) {
            playerState = PlayerState::ATTACK;

            if (currCombo == 0) {
                currCombo = Combo::NORMAL_COMBO1;
            } else if (currCombo != 0 && waitForCombo) {
                isCombo = true;
            }
        }
    }

    void Player::Idle() {
        pModel.lock()->PlayAnimationNoSame("idle", true);
    }

    void Player::Walk() {
        if (CheckFloat3Zero(movement)) {
            return;
        }

        movement = normalize(movement);
        this->SetModelRotation();

        if (pModel.lock()->GetPlayAnimationName() != "walk") {
            pModel.lock()->PlayAnimation("walk", true, 0.2f, 14.0f);
        }
    }

    void Player::Jump() {}

    void Player::Attack() {
        if (currCombo == 1) {
            if (pModel.lock()->GetPlayAnimationName() != "attack1") {
                this->SetModelRotation();
                pModel.lock()->PlayAnimationNoSame("attack1");
                attackList.clear();
            }

            if (!pModel.lock()->IsPlaying()) {
                currCombo = Combo::NO_COMBO;
                if (isCombo) {
                    currCombo = Combo::NORMAL_COMBO2;
                    isCombo   = false;
                }
            } else {
                waitForCombo = true;
            }
        }

        if (currCombo == 2) {
            if (pModel.lock()->GetPlayAnimationName() != "attack2") {
                pModel.lock()->PlayAnimationNoSame("attack2");
                this->SetModelRotation();
                attackList.clear();
            }

            if (!pModel.lock()->IsPlaying()) {
                currCombo = Combo::NO_COMBO;
                if (isCombo) {
                    // currCombo = 3;
                    currCombo = Combo::NORMAL_COMBO3;
                    isCombo   = false;
                }
            } else {
                waitForCombo = true;
            }
        }

        if (currCombo == 3) {
            if (pModel.lock()->GetPlayAnimationName() != "attack3") {
                this->SetModelRotation();
                pModel.lock()->PlayAnimationNoSame("attack3");
                attackList.clear();
            }
            if (!pModel.lock()->IsPlaying()) {
                currCombo   = Combo::NO_COMBO;
                playerState = PlayerState::IDLE;
            }
        }

        if (currCombo == 0) {
            playerState = PlayerState::IDLE;
            currCombo   = Combo::NO_COMBO;
        }
    }

    void Player::SetModelRotation(/*float3& move*/) {
        if (movement.x != 0 || movement.z != 0) {
            float x     = -movement.x;
            float z     = -movement.z;
            float theta = atan2(x, z) * RadToDeg;
            pModel.lock()->SetRotationAxisXYZ({0, theta, 0});
        }
    }

    void Player::GetHit(int damage) {
        pHP.lock()->TakeDamage(damage);

        if (pHP.lock()->GetHP() > 0) {
            pModel.lock()->PlayAnimation("getHit");
            playerState = PlayerState::GET_HIT;
            getHit      = true;
        } else {
            Scene::Change(Scene::GetScene<GameOverScene>());
        }
    }
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Player)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Player)
