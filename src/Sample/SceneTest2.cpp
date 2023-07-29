#include "SceneTest2.h"
#include "SceneTest1.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionLine.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentTargetTracking.h>
#include <System/Component/ComponentSpringArm.h>

#include <cmath>

namespace Sample
{

BP_CLASS_IMPL(SceneTest2, u8"ThirdPerson Gameテスト");

// STAGE01を使用するときはこちらを有効にする
//#define USE_STAGE_FPS
//#define USE_MOUSE_CAMERA

//! @brief HPゲージ
namespace HP
{
constexpr int HP_BAR_W = 320;
constexpr int HP_BAR_H = 50;
class HpGauge
{
public:
    HpGauge(int hp = 100)
        : hp_(hp)
        , damage_(hp)
        , org_hp_(hp)
    {
        // 位置
        pos_ = float3(WINDOW_W - (HP_BAR_W + 20), 0, 50);
    }
    virtual ~HpGauge() {}

    void Update()
    {
        --damage_frame_;
        if(damage_frame_ > 0)
            return;
        // ダメージ分をちょっとだけ見せてから減らす処理する
        damage_ -= static_cast<int>(static_cast<float>(damage_ - hp_) / 30.f);
    }
    void Draw()
    {
        int x = (int)pos_.x;
        int y = (int)pos_.z;

        // 下地
        DrawBox(x, y, x + HP_BAR_W, y + HP_BAR_H, GetColor(75, 75, 75), TRUE);
        // ダメージ用
        float bar2_per = (damage_ / (float)org_hp_) * 100 * 0.01f;
        DrawBox(x, y, x + static_cast<int>(HP_BAR_W * bar2_per), y + HP_BAR_H, RED, TRUE);
        // 現在のHP
        float bar_per = (hp_ / (float)org_hp_) * 100 * 0.01f;
        DrawBox(x, y, x + static_cast<int>(HP_BAR_W * bar_per), y + HP_BAR_H, SKYBLUE, TRUE);
        // フチ
        DrawBox(x, y, x + HP_BAR_W, y + HP_BAR_H, WHITE, FALSE);
    }
    // 全回復
    void Reset()
    {
        hp_     = org_hp_;
        damage_ = hp_;
    }

    HpGauge& operator++()
    {
        *this += 1;
        return *this;
    }
    HpGauge operator++(int)
    {
        HpGauge ret = *this;
        *this += 1;
        return ret;
    }
    HpGauge& operator--()
    {
        *this -= 1;
        return *this;
    }
    HpGauge operator--(int)
    {
        HpGauge ret = *this;
        *this -= 1;
        return ret;
    }

    void operator-=(int value)
    {
        if(hp_ <= 0)
            return;
        // 減らせるダメージがある場合のみ以下を実行
        damage_frame_ = 30;
        hp_ -= value;
        if(hp_ <= 0)
            hp_ = 0;
    }
    void operator+=(int value)
    {
        if(hp_ >= org_hp_)
            return;

        // 増やせる分がある場合のみ以下を実行
        hp_ += value;
        if(hp_ >= org_hp_)
            hp_ = org_hp_;

        damage_ = hp_;
    }
    bool operator<=(int value) { return hp_ <= value; }
    bool operator<(int value) { return hp_ < value; }
    bool operator>=(int value) { return hp_ >= value; }
    bool operator>(int value) { return hp_ > value; }
    bool operator==(int value) { return hp_ == value; }
    bool operator!=(int value) { return !((*this) == value); }

private:
    float3 pos_ = float3(0, 0, 0);   // 位置
    int    hp_;                      // HP（水色のバー）
    int    damage_;                  // ダメージ（赤色のバー）
    int    damage_frame_ = 0;        // ダメージが与えられた後、赤いゲージが減るまで待つ用
    int    org_hp_;                  // HPの基本量
};

HpGauge player_hp;   // 仮) プレイヤーのHPゲージ
}   // namespace HP

class Box : public Object
{
public:
    bool Init() override
    {
        __super::Init();

        SetName("Box");

        auto model = AddComponent<ComponentModel>("data/Sample/Sci-fi_Box/Sci-fi Box.mv1");

        SetTranslate({0, 12, 0});

        auto col = AddComponent<ComponentCollisionModel>();
        col->AttachToModel(true);

        model->SetScaleAxisXYZ({3.0f});   //

        return true;
    }

    void Update() override { AddTranslate({0, 0, -0.05f * GetDeltaTime60()}); }
};

class Shot : public Object
{
public:
    bool Init() override
    {
        __super::Init();

        SetName("Shot");
        AddComponent<ComponentCollisionSphere>()->SetRadius(radius_)->SetHitCollisionGroup(
            (u32)ComponentCollision::CollisionGroup::ENEMY | (u32)ComponentCollision::CollisionGroup::GROUND);
        return true;
    }

    void Update(float delta) { AddTranslate(vec_ * speed_ * delta); }

    void Draw() { DrawSphere3D(cast(GetTranslate()), radius_, 10, GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE); }

    void OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo) override
    {
        auto owner = hitInfo.hit_collision_->GetOwnerPtr();
        if(owner->GetNameDefault() == "Enemy") {
            Scene::ReleaseObject(owner);
            Scene::ReleaseObject(SharedThis());
        }
        if(owner->GetNameDefault() == "Ground") {
            Scene::ReleaseObject(SharedThis());
        }
    }

    void SetPositionAndDirection(float3 pos, float3 vec)
    {
        SetTranslate(pos);
        vec_ = normalize(vec);
    }

private:
    float3 vec_    = {0, 0, 0};   //< 進む方向
    float3 speed_  = 100.0f;      //< 1秒間に進む量
    float  radius_ = 1.0f;        //< 弾の大きさ
};

ObjectPtr TrackingNearEnemy(ObjectPtr player);

USING_PTR(Camera);

// カメラ
class Camera : public Object
{
public:
    static CameraPtr Create(ObjectPtr obj)
    {
        auto camobj = Scene::CreateObjectPtr<Camera>();
        auto camera = camobj->AddComponent<ComponentCamera>();
        camera->SetPositionAndTarget({0, 0, 0}, {0, 0, 1});

        auto col = camobj->AddComponent<ComponentCollisionSphere>();
        col->SetRadius(2.0f);
        col->SetMass(0.0f);

        auto spring_arm = camobj->AddComponent<ComponentSpringArm>();

        spring_arm->SetSpringArmObject(obj);

        // armの回転は上20度(X軸回転)の角度としておく
        spring_arm->SetSpringArmRotate({-20, 0, 0});
        // ターゲットまでの距離
        spring_arm->SetSpringArmLength(30);
        // 少しだけ上を見る
        spring_arm->SetSpringArmOffset({0, 4, 0});

        return camobj;
    }

#ifdef USE_MOUSE_CAMERA
    void Update() override
    {
        auto spring_arm = GetComponent<ComponentSpringArm>();

        // SprintArmの回転にマウスの移動量を足しこむ
        rot_ += {GetMouseMoveY(), GetMouseMoveX(), 0};

        // 足しこんだ回転を利用する
        spring_arm->SetSpringArmRotate(rot_);
    }
#endif

private:
    // armの回転は上20度(X軸回転)の角度としておく
    float3 rot_{-20, 0, 0};
};

class Mouse : public Object
{
public:
    BP_OBJECT_TYPE(Mouse, Object);

    bool Init() override
    {
        __super::Init();

        // モデルコンポーネント(0.08倍)
        auto model = AddComponent<ComponentModel>("data/Sample/Player/model.mv1");

        model->SetScaleAxisXYZ({0.08f});   //

        model->SetAnimation({
            {  "idle",   "data/Sample/Player/Anim/Idle.mv1", 1, 1.0f},
            {  "jump",   "data/Sample/Player/Anim/Jump.mv1", 1, 1.0f},
            {  "walk",   "data/Sample/Player/Anim/Walk.mv1", 1, 1.0f},
            { "walk2",  "data/Sample/Player/Anim/Walk2.mv1", 1, 1.0f},
            {"dance1", "data/Sample/Player/Anim/Dance1.mv1", 0, 1.0f},
            {"dance2", "data/Sample/Player/Anim/Dance2.mv1", 0, 1.0f},
            {"dance3", "data/Sample/Player/Anim/Dance3.mv1", 0, 1.0f},
            {"dance4", "data/Sample/Player/Anim/Dance4.mv1", 0, 1.0f},
            {"dance5", "data/Sample/Player/Anim/Dance5.mv1", 0, 1.0f}
        });

        // コリジョン(カプセル)
        auto col = AddComponent<ComponentCollisionCapsule>();   //
        col->SetTranslate({0, 0, 0});
        col->SetRadius(2.5);
        col->SetHeight(10);
        col->UseGravity();

        auto line = AddComponent<ComponentCollisionLine>();
        line->SetLine({0, 5, 0}, {0, 5, -10000});

#if 0
			// カメラ
			auto camera = AddComponent<ComponentCamera>();
			camera->SetPositionAndTarget( { 0, 35, 60 }, { 0, 20, 0 } );
#endif

        auto target = AddComponent<ComponentTargetTracking>();
        target->SetTrackingNode("mixamorig:Neck");
        target->SetFrontVector({0, 0, -1});

        // 180,180にすると正しく向いているかチェックできる
        target->SetTrackingLimitLeftRight({70, 70});

        target->SetTrackingLimitUpDown({10, 10});

        return true;
    }

    void Update() override
    {
        auto mdl        = GetComponent<ComponentModel>();
        auto near_enemy = TrackingNearEnemy(SharedThis());
#if 0
			if( near_enemy )
			{
				near_enemy->SetScaleAxisXYZ( { 2, 2, 2 } );
			}
#endif
        if(mdl)
            if(auto target = GetComponent<ComponentTargetTracking>()) {
                target->SetTargetObjectPtr(near_enemy);
                //target->SetFrontVector(-mdl->GetMatrix().axisZ());
            }

        float3 move = float3(0, 0, 0);

        if(IsKeyRepeat(KEY_INPUT_A)) {
            rot_y_ += -1.0f;
        }
        if(IsKeyRepeat(KEY_INPUT_D)) {
            rot_y_ += 1.0f;
        }
        SetRotationAxisXYZ({0, rot_y_, 0});

        auto mat = GetMatrix();

#ifdef USE_MOUSE_CAMERA
        // カメラ方向を取得してその方向に動かす
        auto   cam = Scene::GetObjectPtr<Camera>("PlayerCamera");
        float3 v   = GetTranslate() - cam->GetTranslate();
        mat        = HelperLib::Math::CreateMatrixByFrontVector(-v);
#endif   //USE_MOUSE_CAMERA

        if(IsKeyRepeat(KEY_INPUT_UP)) {
            float3 vec = mat.axisZ();   //GetMatrix().axisZ();
            move += -vec;
        }
        if(IsKeyRepeat(KEY_INPUT_RIGHT)) {
            float3 vec = mat.axisX();   //GetMatrix().axisX();
            move += -vec;
        }
        if(IsKeyRepeat(KEY_INPUT_DOWN)) {
            float3 vec = mat.axisZ();   //GetMatrix().axisZ();
            move += vec;
        }
        if(IsKeyRepeat(KEY_INPUT_LEFT)) {
            float3 vec = mat.axisX();   //GetMatrix().axisX();
            move += vec;
        }
#if 1   // Animation
        if(mdl)
            if(length(move).x > 0) {
                // 動いてる
                move = normalize(move);

                float x     = -move.x;
                float z     = -move.z;
                float theta = atan2(x, z) * RadToDeg - rot_y_;
                // モデルだけ回転 (ついてるカメラは回らない)
                mdl->SetRotationAxisXYZ({0, theta, 0});

                if(mdl->GetPlayAnimationName() != "walk")
                    mdl->PlayAnimation("walk", true);
            }
            else {
                // 止まってる
                if(mdl->GetPlayAnimationName() != "idle")
                    mdl->PlayAnimation("idle", true);
            }
#endif
        move *= speed_ * GetDeltaTime60();

        // 地面移動スピードを決定する
        AddTranslate(move);

        // Shot
        if(IsKeyOn(KEY_INPUT_SPACE)) {
            float3 vec = {0, 0, 1};

            if(mdl)
                vec = mul(-float4(mdl->GetMatrix().axisZ(), 0), GetMatrix()).xyz;

            Scene::CreateObjectPtr<Shot>()->SetPositionAndDirection(GetTranslate() + float3(0, 5, 0), vec);
        }
    }

    // 基本描画の後に処理します
    void PostDraw() override
    {
        HP::player_hp.Draw();
    }

    void GUI() override
    {
        __super::GUI();

        // GUI描画
        ImGui::Begin(GetName().data());
        {
            ImGui::Separator();

            ImGui::DragFloat(u8"速度", &speed_, 0.01f, 0.01f, 10.0f, "%2.2f");
            ImGui::DragInt(u8"HitPoint", &hit_point_, 1, 0, 100);
        }
        ImGui::End();
    }

    void OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo) override
    {
        // 次のownerのオブジェクトと当たった!
        auto owner = hitInfo.hit_collision_->GetOwnerPtr();
        printfDx("\nHit:%s", owner->GetName().data());

        if(owner->GetNameDefault() == "Enemy") {
            if(hit_point_ > 0) {
                hit_point_--;
                // HPゲージも減らす
                //HP::player_hp--;
                HP::player_hp -= 1;
            }

            //Scene::ReleaseObject( owner );
        }

        // 当たりで移動させる(これが無ければめり込みます)
        __super::OnHit(hitInfo);
    }

    void SetSpeed(float s)
    {
        speed_ = s;
    }

    float GetSpeed()
    {
        return speed_;
    }

    float& Speed()
    {
        return speed_;
    }

private:
    float speed_     = 1.0f;
    float rot_y_     = 0.0f;
    float rot_x_     = 0.0f;
    int   hit_point_ = 100;
};

BP_OBJECT_IMPL(Mouse, u8"Mouse");

//! @brief 敵
class Enemy : public Object
{
public:
    bool Init() override
    {
        __super::Init();

        // モデルコンポーネント(0.08倍)
        auto model = AddComponent<ComponentModel>("data/Sample/Player/model.mv1");

        model->SetScaleAxisXYZ({0.08f});   //

        model->SetAnimation({
            {  "idle",   "data/Sample/Player/Anim/Idle.mv1", 1, 1.0f},
            {  "jump",   "data/Sample/Player/Anim/Jump.mv1", 1, 1.0f},
            {  "walk",   "data/Sample/Player/Anim/Walk.mv1", 1, 1.0f},
            { "walk2",  "data/Sample/Player/Anim/Walk2.mv1", 1, 1.0f},
            {"dance1", "data/Sample/Player/Anim/Dance1.mv1", 0, 1.0f},
            {"dance2", "data/Sample/Player/Anim/Dance2.mv1", 0, 1.0f},
            {"dance3", "data/Sample/Player/Anim/Dance3.mv1", 0, 1.0f},
            {"dance4", "data/Sample/Player/Anim/Dance4.mv1", 0, 1.0f},
            {"dance5", "data/Sample/Player/Anim/Dance5.mv1", 0, 1.0f}
        });

        // コリジョン(カプセル)
        auto col = AddComponent<ComponentCollisionCapsule>();   //
        col->SetTranslate({0, 0, 0});
        col->SetRadius(2.5);
        col->SetHeight(10);
        col->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY);
        col->UseGravity();

        // カメラ
        //auto camera = AddComponent<ComponentCamera>();
        //camera->SetPositionAndTarget( { 0, 35, 60 }, { 0, 20, 0 } );

        return true;
    }

    void Update() override
    {
        auto mdl = GetComponent<ComponentModel>();

        float3 target = GetTranslate();

        if(auto mouse = Scene::GetObjectPtr<Mouse>("Mouse"))
            target = mouse->GetTranslate();

        auto pos = GetTranslate();

        float3 move = (target - pos);

        if(length(move).x > 0) {
            // 動いてる
            move = normalize(move);

            float x     = -move.x;
            float z     = -move.z;
            float theta = atan2(x, z) * RadToDeg - rot_y_;
#if 0
				// モデルだけ回転 (ついてるカメラは回らない)
				mdl->SetRotationAxisXYZ( { 0, theta, 0 } );
#else
            // 軸ごと回転 (カメラも一緒に回る)
            SetRotationAxisXYZ({0, theta, 0});
#endif
            if(mdl && mdl->GetPlayAnimationName() != "walk")
                mdl->PlayAnimation("walk", true);
        }
        else {
            // 止まってる
            if(mdl && mdl->GetPlayAnimationName() != "idle")
                mdl->PlayAnimation("idle", true);
        }
        move *= speed_ * GetDeltaTime60();

        // 地面移動スピードを決定する
        AddTranslate(move);
    }

    void LateDraw()
    {
        DrawFormatString(100, 500, GetColor(255, 255, 0), "AAA");
    }

    void PostDraw()
    {
        SetScaleAxisXYZ({1, 1, 1});
    }

    void GUI() override
    {
        __super::GUI();

        // GUI描画
        ImGui::Begin(GetName().data());
        {
            ImGui::Separator();

            ImGui::DragFloat(u8"速度", &speed_, 0.01f, 0.01f, 10.0f, "%2.2f");
        }
        ImGui::End();
    }

    void OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo) override
    {
        // 当たりで移動させる(これが無ければめり込みます)
        __super::OnHit(hitInfo);
    }

    void SetSpeed(float s)
    {
        speed_ = s;
    }

    float GetSpeed()
    {
        return speed_;
    }

    float& Speed()
    {
        return speed_;
    }

private:
    float speed_ = 1.0f;
    float rot_y_ = 0.0f;
};

// 近い敵を見つける
ObjectPtr TrackingNearEnemy(ObjectPtr player)
{
    // シーンにいるEnemyをすべて取得する
    auto enemies = Scene::GetObjectsPtr<Enemy>();

    // 一番近いエネミー
    ObjectPtr near_enemy  = nullptr;
    float     near_length = 99999999.0f;

    float3 player_pos = player->GetTranslate();

    for(auto enemy : enemies) {
        if(near_enemy == nullptr) {
            near_enemy = enemy;
            continue;
        }

        float3 enemy_pos = enemy->GetTranslate();

        float len = length(enemy_pos - player_pos);
        if(len < near_length) {
            near_enemy  = enemy;
            near_length = len;
        }
    }

    return near_enemy;
}

bool SceneTest2::Init()
{
    // カメラ
    //auto cam = Scene::CreateObjectPtr<Object>()->AddComponent<ComponentCamera>();
    //cam->SetPositionAndTarget( { 0, 35, -60 }, { 0, 20, 0 } );

    // ステージ
#ifdef USE_STAGE_FPS
    {
        auto obj = Scene::CreateObjectPtr<Object>()->SetName("Ground");
        obj->AddComponent<ComponentModel>("data/Sample/FPS_Industrial/Map.mv1")->SetScaleAxisXYZ({5, 5, 5});
        obj->AddComponent<ComponentCollisionModel>()->AttachToModel();
    }
#else
    {
        auto obj = Scene::CreateObjectPtr<Object>("Ground");   //->SetName( "Ground" );
        obj->AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage00.mv1");
        if(auto cmpmdl = obj->AddComponent<ComponentCollisionModel>())
            cmpmdl->AttachToModel(true);
    }
#endif

    //----------------------------------------------------------------------------------
    // Mouse
    //----------------------------------------------------------------------------------
    auto m = Scene::CreateObjectPtr<Mouse>();
    m->SetName("Mouse");
#ifdef USE_STAGE01
    m->SetTranslate({0, 100, 0});
#else
    m->SetTranslate({0, 100, 0});
#endif
    m->SetSpeed(0.5f);

    Scene::CreateObjectPtr<Box>();

#if 1
    {
        auto enemy = Scene::CreateObjectPtr<Enemy>();
        enemy->SetName("Enemy");
        enemy->SetTranslate({GetRand(1000) - 500, 100, -GetRand(500)});
        enemy->SetSpeed(0.1f);
        Camera::Create(enemy)->SetName("EnemyCamera");
    }
#endif

#if 1
    for(int i = 0; i < 10; i++) {
        auto enemy = Scene::CreateObjectPtr<Enemy>();
        enemy->SetName("Enemy");
        enemy->SetTranslate({GetRand(1000) - 500, 100, -GetRand(500)});
        enemy->SetSpeed(0.1f);
    }
#endif
    Camera::Create(m)->SetName("PlayerCamera");

    return true;
}

void SceneTest2::Update()
{
    if(IsKeyOn(KEY_INPUT_1))
        Scene::SetCurrentCamera("PlayerCamera");

    if(IsKeyOn(KEY_INPUT_2))
        Scene::SetCurrentCamera("EnemyCamera");

#if 1
    // 10秒たったら敵を速くする
    if(Scene::GetTime() > 10.0f) {
        auto enemies = Scene::GetObjectsPtr<Enemy>();
        for(auto& enemy : enemies) {
            enemy->SetSpeed(0.4f);
        }
    }
#endif
    HP::player_hp.Update();
    if(IsKeyOn(KEY_INPUT_SPACE)) {
        HP::player_hp.Reset();
    }
}

void SceneTest2::Draw()
{
    // とりあえずTitleという文字を表示しておく
    DrawFormatString(100, 50, GetColor(255, 255, 255), "Title");

    //HP::player_hp.Draw();
}

void SceneTest2::Exit()
{
    // タイトル終了時に行いたいことは今はない
}

void SceneTest2::GUI()
{
}
}   // namespace Sample

CEREAL_REGISTER_TYPE(Sample::Mouse)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, Sample::Mouse)
CEREAL_REGISTER_TYPE(Sample::Enemy)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, Sample::Enemy)
CEREAL_REGISTER_TYPE(Sample::Camera)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, Sample::Camera)
CEREAL_REGISTER_TYPE(Sample::Box)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, Sample::Box)
