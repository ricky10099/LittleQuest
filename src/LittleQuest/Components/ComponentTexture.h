#include <System/Scene.h>
#include <System/Component/Component.h>

#ifndef COMPONENTTEXTURE2D_HEADER
#    define COMPONENTTEXTURE2D_HEADER
namespace LittleQuest {
class ComponentTexture2D: public Component {
   public:
    BP_COMPONENT_TYPE(ComponentTexture2D, Component);

    void SetTexture(std::string_view path) {
        m_pTexture  = std::make_shared<Texture>(path);
        m_pShaderPS = std::make_shared<ShaderPs>("data/Shader/ps_texture");
    }

    void Construct(ObjectPtr owner) {
        Super::Construct(owner);
    }

    void Construct(ObjectPtr owner, std::string_view path) {
        Construct(owner);
        m_pShaderPS = std::make_shared<ShaderPs>("data/Shader/ps_texture");
        m_pTexture  = std::make_shared<Texture>(path);
    }

    void Init() override;
    void Update() override;
    void GUI() override;

    void DrawTexture();
    void SetPosition(float x1, float y1, float x2, float y2);
   private:
    VERTEX2DSHADER            m_vertex[4];
    std::shared_ptr<Texture>  m_pTexture;
    std::shared_ptr<ShaderPs> m_pShaderPS;

    CEREAL_SAVELOAD(arc, ver) {
        arc(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
    }
};
}    // namespace LittleQuest
#endif
