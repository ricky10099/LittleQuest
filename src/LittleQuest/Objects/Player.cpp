#include "Player.h"
#include "Camera.h"

#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentSpringArm.h>
#include <System/Component/ComponentTargetTracking.h>

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

        return player;
    }

    bool Player::Init()    // override
    {
        Super::Init();

        // モデルコンポーネント(0.08倍)
        auto model = AddComponent<ComponentModel>(
            "data/LittleQuest/Model/Guard/Guard.mv1");

        model->SetScaleAxisXYZ({0.05f});    //

        model->SetAnimation(
            {{"idle", "data/LittleQuest/Anim/SwordIdle.mv1", 0, 1.0f},
             {"jump", "data/LittleQuest/Anim/SwordJump.mv1", 0, 1.0f},
             {"walk", "data/LittleQuest/Anim/SwordWalk.mv1", 0, 1.0f},
             {"attack1", "data/LittleQuest/Anim/SwordAttackCombo1.mv1", 0,
              1.5f},
             {"attack2", "data/LittleQuest/Anim/SwordAttackCombo2.mv1", 0,
              1.0f},
             {"attack3", "data/LittleQuest/Anim/SwordAttackCombo3.mv1", 0,
              1.5f},
             {"damaged", "data/LittleQuest/Anim/SwordDamaged.mv1", 0, 1.0f}});

        // modelPtr = GetComponent<ComponentModel>();
        //  コリジョン(カプセル)
        auto col = AddComponent<ComponentCollisionCapsule>();    //
        col->SetTranslate({0, 0, 0});
        col->SetRadius(2.5);
        col->SetHeight(10);
        col->UseGravity();

        this->AttachSword();
        /* auto target = AddComponent<ComponentTargetTracking>();
            target->SetTrackingNode("mixamorig:Neck");
            target->SetFrontVector({0, 0, -1});

            target->SetTrackingLimitLeftRight({70, 70});

            target->SetTrackingLimitUpDown({10, 10});*/

        return true;
    }

    void Player::AttachSword() {
        ////----------------------------------------------------------------------------------
        //// 持たせるオブジェクト(Knife)
        ////----------------------------------------------------------------------------------
        // auto obj = Scene::CreateObjectPtr<Object>()->SetName("Sword");
        // obj->SetTranslate({0, 0, 0});
        //// オブジェクトにモデルをつける
        // if (auto model = obj->AddComponent<ComponentModel>()) {
        //     // model->Load("data/LittleQuest/Model/Sword/Sword.mv1");
        //      model->Load("data/Sample/FPS_Knife/Knife_low.mv1");
        //     model->SetRotationAxisXYZ({0, 0, 0});
        //     model->SetScaleAxisXYZ({0.003f, 0.003f, 0.003f});
        // }

        // if (auto attach = obj->AddComponent<ComponentAttachModel>()) {
        //     // playerの右手にアタッチする
        //     attach->SetAttachObject("Player", "mixamorig:RightHand");
        //     // Knife回転量
        //     attach->SetAttachRotate({0, -60, 180});
        //     // Knifeオフセット
        //     attach->SetAttachOffset({13, 10, 5});
        // }
        //----------------------------------------------------------------------------------
        //  持たせるオブジェクト(Knife)
        //----------------------------------------------------------------------------------
        auto obj = Scene::CreateObjectPtr<Object>()->SetName("Knife");
        obj->SetTranslate({0, 10, 10});
        // オブジェクトにモデルをつける
        if (auto model = obj->AddComponent<ComponentModel>()) {
            model->Load("data/Sample/FPS_Knife/Knife_low.mv1");
            model->SetRotationAxisXYZ({0, 0, 0});
            model->SetScaleAxisXYZ({10.0f, 10.0f, 10.0f});

            // KnifeModelの標準描画をOFFする
            model->SetStatus(Component::StatusBit::NoDraw, true);

            // テクスチャ
            {
                // 刃物部分
                Material mat{};
                mat.albedo_ = std::make_shared<Texture>(
                    "data/Sample/FPS_Knife/Knife_low_Iron.001_BaseColor.png");
                mat.normal_ = std::make_shared<Texture>(
                    "data/Sample/FPS_Knife/"
                    "Knife_low_Iron.001_NormalOpenGLl.png");
                mat.roughness_ = std::make_shared<Texture>(
                    "data/Sample/FPS_Knife/Knife_low_Iron.001_Roughness.png");
                mat.metalness_ = std::make_shared<Texture>(
                    "data/Sample/FPS_Knife/Knife_low_Iron.001_Metallic.png");
                materials_.push_back(mat);
            }
            {
                // 鞘部分
                Material mat{};
                mat.albedo_ = std::make_shared<Texture>(
                    "data/Sample/FPS_Knife/Knife_low_Lever.001_BaseColor.png");
                mat.normal_ = std::make_shared<Texture>(
                    "data/Sample/FPS_Knife/"
                    "Knife_low_Lever.001_NormalOpenGLl.png");
                mat.roughness_ = std::make_shared<Texture>(
                    "data/Sample/FPS_Knife/Knife_low_Lever.001_Roughness.png");
                mat.metalness_ = std::make_shared<Texture>(
                    "data/Sample/FPS_Knife/Knife_low_Lever.001_Metallic.png");
                materials_.push_back(mat);
            }

            // モデルに設定されているテクスチャを上書き
            // model と this(Scene) をこの関数内で使用する
            // modelはshared_ptrとして使うためコピーをとる
            model->SetProc(
                "ModelDraw",
                [model, this]() {
                    // この部分をDrawタイミングで使用する
                    if (auto model_knife = model->GetModelClass()) {
                        {
                            auto& mat = materials_[0];
                            model_knife->overrideTexture(
                                Model::TextureType::Diffuse, mat.albedo_);
                            model_knife->overrideTexture(
                                Model::TextureType::Albedo, mat.albedo_);
                            model_knife->overrideTexture(
                                Model::TextureType::Normal, mat.normal_);
                            model_knife->overrideTexture(
                                Model::TextureType::Roughness, mat.roughness_);
                            model_knife->overrideTexture(
                                Model::TextureType::Metalness, mat.metalness_);

                            model_knife->renderByMesh(0);    // 金属刃物部分
                        }

                        {
                            auto& mat = materials_[1];
                            model_knife->overrideTexture(
                                Model::TextureType::Diffuse, mat.albedo_);
                            model_knife->overrideTexture(
                                Model::TextureType::Albedo, mat.albedo_);
                            model_knife->overrideTexture(
                                Model::TextureType::Normal, mat.normal_);
                            model_knife->overrideTexture(
                                Model::TextureType::Roughness, mat.roughness_);
                            model_knife->overrideTexture(
                                Model::TextureType::Metalness, mat.metalness_);

                            model_knife->renderByMesh(1);    // 鞘部分
                        }
                    }
                },
                ProcTiming::Draw);
        }

        if (auto attach = obj->AddComponent<ComponentAttachModel>()) {
            // mouseの右手にアタッチする
            attach->SetAttachObject("Player", "mixamorig:RightHand");
            // Knife回転量
            attach->SetAttachRotate({0, -60, 180});
            // Knifeオフセット
            attach->SetAttachOffset({13, 10, 5});
        }
    }

    void Player::Update()    // override
    {
        // auto mdl = GetComponent<ComponentModel>();
        ////auto near_enemy = TrackingNearEnemy(SharedThis());

        // if (mdl) {
        //     if (auto target = GetComponent<ComponentTargetTracking>()) {
        //         target->SetTargetObjectPtr(near_enemy);
        //         // target->SetFrontVector(-mdl->GetMatrix().axisZ());
        //     }
        // }

        float3 move = float3(0, 0, 0);
        isWalk      = false;
        /*if (IsKeyRepeat(KEY_INPUT_A)) {
            rot_y_ += -1.0f;
        }
        if (IsKeyRepeat(KEY_INPUT_D)) {
            rot_y_ += 1.0f;
        }
        SetRotationAxisXYZ({0, rot_y_, 0});*/

        auto mat = GetMatrix();

        if (IsMouseDown(MOUSE_INPUT_LEFT)) {
            isAttack    = true;
            playerState = PlayerState::ATTACK;

            if (combo == 0) {
                combo = 1;
            } else if (combo == 1 && canCombo2) {
                // combo   = 2;
                isCombo   = true;
                canCombo2 = false;
            } else if (combo == 2 && canCombo3) {
                // combo   = 3;
                isCombo   = true;
                canCombo3 = false;
            }
        }

        //#ifdef USE_MOUSE_CAMERA
        // カメラ方向を取得してその方向に動かす
        auto cam = Scene::GetObjectPtr<Camera>("PlayerCamera");
        float3 v = GetTranslate() - cam->GetTranslate();
        mat      = HelperLib::Math::CreateMatrixByFrontVector(-v);
        //#endif    // USE_MOUSE_CAMERA

        if (!isAttack) {
            if (IsKeyRepeat(KEY_INPUT_W)) {
                float3 vec = mat.axisZ();
                move += -vec;
                isWalk      = true;
                playerState = PlayerState::WALK;
            }
            if (IsKeyRepeat(KEY_INPUT_D)) {
                float3 vec = mat.axisX();
                move += -vec;
                isWalk      = true;
                playerState = PlayerState::WALK;
            }
            if (IsKeyRepeat(KEY_INPUT_S)) {
                float3 vec = mat.axisZ();
                move += vec;
                isWalk      = true;
                playerState = PlayerState::WALK;
            }
            if (IsKeyRepeat(KEY_INPUT_A)) {
                float3 vec = mat.axisX();
                move += vec;
                isWalk      = true;
                playerState = PlayerState::WALK;
            }
        }

        if (!isWalk && !isAttack) {
            playerState = PlayerState::IDLE;
        }

        switch (playerState) {
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

#if 0    // Animation
        if (mdl) {
            if (isAttack) {
                if (mdl->GetPlayAnimationName() != "attack")
                    mdl->PlayAnimationNoSame("attack");
                if ((mdl->GetAnimationTime() >= 0.3f || mdl->GetAnimationTime() >= 0.7f) && IsKeyDown(KEY_INPUT_C)) {
                    isCombo = true;
                }
                if (mdl->GetAnimationTime() >= 0.6f && !isCombo) isAttack = false;
                if (mdl->GetAnimationTime() >= 1.5f && !isCombo) {
                    isAttack = false;
                }

            } else if (length(move).x > 0) {
                // 動いてる
                move = normalize(move);

                float x     = -move.x;
                float z     = -move.z;
                float theta = atan2(x, z) * RadToDeg - rot_y_;
                // モデルだけ回転 (ついてるカメラは回らない)
                mdl->SetRotationAxisXYZ({0, theta, 0});

                if (mdl->GetPlayAnimationName() != "walk")
                    mdl->PlayAnimation("walk", true);
            } else {
                // 止まってる
                if (mdl->GetPlayAnimationName() != "idle")
                    mdl->PlayAnimation("idle", true);
            }
        }
#endif
        move *= speed_ * GetDeltaTime60();

        // 地面移動スピードを決定する
        AddTranslate(move);
        MouseWheelCounter += GetMouseWheelRotVol();
    }

    // 基本描画の後に処理します
    void Player::LateDraw()    // override
    {
        // gauge_.Draw();
        printfDx("\ncombo:%d", combo);
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            printfDx("\nAnimation Time:%f", modelPtr->GetAnimationTime());
        }

        printfDx("\nisAttack: %i", isAttack);
        printfDx("\nisCombo: %i", isCombo);
        printfDx("\nGetMouseHWheelRotVol: %i", MouseWheelCounter);
    }

    void Player::GUI()    // override
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

    void Player::OnHit([[maybe_unused]] const ComponentCollision::HitInfo&
                           hitInfo)    // override
    {
        // 次のownerのオブジェクトと当たった!
        auto owner = hitInfo.hit_collision_->GetOwnerPtr();
        printfDx("\nHit:%s", owner->GetName().data());

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

    void Player::Idle() {
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            if (modelPtr->GetPlayAnimationName() != "idle") {
                modelPtr->PlayAnimation("idle", true);
            }
        }
    }

    void Player::Walk(float3& move) {
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            // 動いてる
            move = normalize(move);

            float x     = -move.x;
            float z     = -move.z;
            float theta = atan2(x, z) * RadToDeg - rot_y_;
            // モデルだけ回転 (ついてるカメラは回らない)
            modelPtr->SetRotationAxisXYZ({0, theta, 0});

            if (modelPtr->GetPlayAnimationName() != "walk")
                modelPtr->PlayAnimation("walk", true);
        }
    }

    void Player::Jump() {}

    void Player::Attack() {
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            if (combo == 1) {
                if (modelPtr->GetPlayAnimationName() != "attack1") {
                    modelPtr->PlayAnimationNoSame("attack1");
                }
                if (modelPtr->GetAnimationTime() > 0.9f) {
                    if (!isCombo) {
                        isAttack    = false;
                        combo       = 0;
                        playerState = PlayerState::IDLE;
                    } else {
                        combo   = 2;
                        isCombo = false;
                    }
                }
                if (modelPtr->GetAnimationTime() > 0.6f) {
                    canCombo2 = true;
                }
            }

            if (combo == 2) {
                if (modelPtr->GetPlayAnimationName() != "attack2") {
                    modelPtr->PlayAnimationNoSame("attack2");
                }
                if (modelPtr->GetAnimationTime() > 0.9f) {
                    if (!isCombo) {
                        isAttack    = false;
                        isCombo     = false;
                        combo       = 0;
                        playerState = PlayerState::IDLE;
                    } else {
                        combo   = 3;
                        isCombo = false;
                    }
                }
                if (modelPtr->GetAnimationTime() > 0.3f && !isCombo) {
                    canCombo3 = true;
                    isCombo   = false;
                }
            }

            if (combo == 3) {
                if (modelPtr->GetPlayAnimationName() != "attack3") {
                    modelPtr->PlayAnimationNoSame("attack3");
                }
                if (modelPtr->GetAnimationTime() > 0.9f) {
                    isAttack    = false;
                    isCombo     = false;
                    combo       = 0;
                    playerState = PlayerState::IDLE;
                }
            }

            if (combo == 0) {
                isAttack    = false;
                isCombo     = false;
                playerState = PlayerState::IDLE;
            }
        }
    }

    void Player::Damaged() {
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            if (modelPtr->GetPlayAnimationName() != "damaged") {
                modelPtr->PlayAnimationNoSame("damaged");
            }
        }
    }

    void Player::SetSpeed(float s) {
        speed_ = s;
    }

    float Player::GetSpeed() {
        return speed_;
    }

    float& Player::Speed() {
        return speed_;
    }

}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Player)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Player)
