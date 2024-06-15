//---------------------------------------------------------------------------
//! @file   ComponentCombo.h
//! @brief  コンボコンポーネント
//---------------------------------------------------------------------------
#include <System/Scene.h>
#include <System/Component/Component.h>
#include <System/Component/ComponentModel.h>
#include <System/Utils/IniFileLib.h>

namespace LittleQuest {
//////////////////////////////////////////////////////////////
//! @brief コンボコンポーネントクラス
//////////////////////////////////////////////////////////////
class ComponentCombo: public Component {
   public:
    BP_COMPONENT_DECL(ComponentCombo, u8"Comboコンポーネント");
    //------------------------------------------------------------
    //! @brief 更新処理を行います。
    //------------------------------------------------------------
    void Update() override;

    //------------------------------------------------------------
    //! @brief コンボ数を増やします。
    //!
    //! @param combo コンボ数
    //------------------------------------------------------------
    void AddCombo(int combo);

    //------------------------------------------------------------
    //! @brief コンボゲージを描画します。
    //------------------------------------------------------------
    void DrawComboBar();

    //------------------------------------------------------------
    //! @brief コンボによる攻撃力バフ。
    //!
    //! @return バフ量。
    //------------------------------------------------------------
    float ComboBuff();

   private:
    //! コンボ維持する時間
    const float COMBO_TIMER  = 180;
    //! コンボゲージの上限
    const float MAX_COMBO    = 30;
    //! コンボバフなしのゲージの色
    const int   COMBO_BUFF_0 = GetColor(255, 255, 255);
    //! コンボバフ1段のゲージの色
    const int   COMBO_BUFF_1 = GetColor(0, 230, 230);
    //! コンボバフ2段のゲージの色
    const int   COMBO_BUFF_2 = GetColor(0, 200, 255);
    //! コンボバフ3段のゲージの色
    const int   COMBO_BUFF_3 = GetColor(0, 127, 255);

    //! 現在のコンボ数
    float m_currCombo  = 0;
    //! 現在コンボゲージの色
    int   m_comboColor = 0;
    //! コンボ維持するタイマー
    float m_comboTimer = 0;

    CEREAL_SAVELOAD(arc, ver) {
        arc(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
    }
};
}    // namespace LittleQuest
