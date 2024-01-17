#include <System/Scene.h>
#include <System/Component/Component.h>
#include <System/Component/ComponentModel.h>
#include <System/Utils/IniFileLib.h>

namespace LittleQuest {
class ComponentCombo: public Component {
   public:
    BP_COMPONENT_TYPE(ComponentCombo, Component);
    enum HP_TYPE {
        PLAYER,
        BOSS,
        MOB,
    };

    void Init() override;
    void Update() override;
    //void Draw() override;
    void GUI() override;

    void  AddCombo(int combo);
    void  DrawComboBar();
    float ComboBuff();

   private:
    const float COMBO_TIMER  = 120;
    const float COMBO_PUASE  = 60;
    const float MAX_COMBO    = 30;
    const int   COMBO_BUFF_0 = GetColor(255, 255, 255);
    const int   COMBO_BUFF_1 = GetColor(0, 230, 230);
    const int   COMBO_BUFF_2 = GetColor(0, 200, 255);
    const int   COMBO_BUFF_3 = GetColor(0, 127, 255);

    float m_currCombo       = 0;
    int   m_comboColor      = 0;
    float m_comboTimer      = 0;
    float m_comboPauseTimer = 0;

    CEREAL_SAVELOAD(arc, ver) {
        arc(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
    }
};
}    // namespace LittleQuest
