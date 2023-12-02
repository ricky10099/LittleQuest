#include <System/Scene.h>
#include <System/Component/Component.h>
#include <System/Component/ComponentModel.h>
#include <System/Utils/IniFileLib.h>

namespace LittleQuest {
    //! ■コンポーネントの作成方法
    //! 1. Component○○○○という名前をつけ、Componentから継承させます
    //! 2. BP_COMPONENT_TYPE( Component○○○○, Component );
    //! というものをクラス内のpublicに書きます
    //! 3. cpp に、BP_COMPONENT_IMPL( Component○○○○,
    //! u8"ここに処理機能を書いておく" ); を追加します
    //! 4. Init(), Update(), Draw()などにその機能を追加します
    //! 5. セーブ/ロードを可能にするには、
    //!    CEREAL_SAVELOAD()、CEREAL_REGISTER_TYPE()、CEREAL_REGISTER_POLYMORPHIC_RELATION()
    //!    を追加する必要があります(セーブロードが必要なければ書かなくても良い)
    //! 6. あとは、オブジェクト内で、AddComponent<Component○○○○>()
    //! で追加するだけです ※使用するにはインクルードをする必要があります
    //!   GameホルダにComponentを作りそこで用意するのが良いです
    //! #include <Game/Component/Component○○○○.h> などとします
    class ComponentHP : public Component {
        public:
            BP_COMPONENT_TYPE(ComponentHP, Component);

            void Init() override;
            void Update() override;
            void Draw() override;
            void GUI() override;

            void SetHP(int HP);
            int GetHP();

            void TakeDamage(int damage);
            void DrawHPBar();

        private:
            VECTOR pos2D;
            float3 pos3D;

            int maxHP;
            int currHP;

            //--------------------------------------------------------------------
            //! @name Cereal処理
            //--------------------------------------------------------------------
            //@{

            //! @brief セーブ
            // @param arc アーカイバ
            // @param ver バージョン
            CEREAL_SAVELOAD(arc, ver) { arc(cereal::make_nvp("Component", cereal::base_class<Component>(this))); }
    };
}    // namespace LittleQuest
