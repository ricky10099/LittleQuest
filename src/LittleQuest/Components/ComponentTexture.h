//---------------------------------------------------------------------------
//! @file   ComponentTexture.h
//! @brief  テクスチャコンポーネント
//---------------------------------------------------------------------------
#include <System/Scene.h>
#include <System/Component/Component.h>

#ifndef COMPONENTTEXTURE2D_HEADER
#    define COMPONENTTEXTURE2D_HEADER
namespace LittleQuest {
//////////////////////////////////////////////////////////////
//! @brief テクスチャコンポーネントクラス
//!
//! @memo シェーダーを使うテクスチャ
//////////////////////////////////////////////////////////////
class ComponentTexture2D: public Component {
   public:
    BP_COMPONENT_DECL(ComponentTexture2D, u8"Textureasdコンポーネント");
    //------------------------------------------------------------
    //! @brief コンストラクタです。
    //!
    //! @param[in] owner オーナー
    //------------------------------------------------------------
    void Construct(ObjectPtr owner) {
        Super::Construct(owner);
    }

    //------------------------------------------------------------
    //! @brief コンストラクタです。
    //!
    //! @param[in] owner オーナー
    //! @param path テクスチャの場所
    //------------------------------------------------------------
    void Construct(ObjectPtr owner, std::string_view path) {
        Construct(owner);
        m_pShaderPS = std::make_shared<ShaderPs>("data/Shader/ps_texture");
        m_pTexture  = std::make_shared<Texture>(path);
    }

    //------------------------------------------------------------
    //! @brief 初期化処理を行います。
    //------------------------------------------------------------
    void Init() override;

    //------------------------------------------------------------
    //! @brief テクスチャを描画します。
    //------------------------------------------------------------
    void DrawTexture();
    //------------------------------------------------------------
    //! @brief テクスチャの描画座標を設定します。
    //!
    //! @param x1 左辺の座標
    //! @param y1 上辺の座標
    //! @param x2 右辺の座標
    //! @param y2 下辺の座標
    //------------------------------------------------------------
    void SetPosition(float x1, float y1, float x2, float y2);
   private:
    //! 頂点のデータ
    VERTEX2DSHADER            m_vertex[4];
    //! テクスチャ
    std::shared_ptr<Texture>  m_pTexture;
    //! テクスチャが使用するシェーダー
    std::shared_ptr<ShaderPs> m_pShaderPS;

    CEREAL_SAVELOAD(arc, ver) {
        arc(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
    }
};
}    // namespace LittleQuest
#endif
