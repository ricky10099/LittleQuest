#include <System/Scene.h>
#include <System/Component/Component.h>
#include <System/Component/ComponentModel.h>
#include <System/Utils/IniFileLib.h>

namespace LittleQuest {
class ComponentHP: public Component {
   public:
    BP_COMPONENT_TYPE(ComponentHP, Component);
    enum HP_TYPE {
        PLAYER,
        BOSS,
        MOB,
    };

    void Init() override;
    void Update() override;
    void Draw() override;
    void GUI() override;

    void  SetType(HP_TYPE type);
    void  SetHP(int HP);
    int   GetHP();
    float GetHPRate();
    void  TakeDamage(int damage);
    void  DrawHPBar();

   private:
    const float DAMAGE_TIME = 60.0f;

    HP_TYPE m_type  = HP_TYPE::MOB;
    VECTOR  m_pos2D = {0, 0};
    float3  m_pos3D = {0, 0, 0};

    int   m_maxHP       = 100;
    int   m_currHP      = m_maxHP;
    float m_backHP      = (float)m_maxHP;
    float m_damageTimer = 0;
    bool  m_decreasing  = false;
    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    //! @brief セーブ
    // @param arc アーカイバ
    // @param ver バージョン
    CEREAL_SAVELOAD(arc, ver) {
        arc(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
    }
};
}    // namespace LittleQuest
