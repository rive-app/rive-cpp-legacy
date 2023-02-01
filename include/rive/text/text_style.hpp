#ifndef _RIVE_TEXT_STYLE_HPP_
#define _RIVE_TEXT_STYLE_HPP_
#include "rive/generated/text/text_style_base.hpp"
#include "rive/shapes/shape_paint_container.hpp"
#include "rive/assets/file_asset_referencer.hpp"
#include "rive/assets/file_asset.hpp"
#include "rive/assets/font_asset.hpp"

namespace rive
{
class FontAsset;
class Font;
class FileAsset;
class Renderer;
class RenderPath;

class TextStyle : public TextStyleBase, public ShapePaintContainer, public FileAssetReferencer
{
private:
    Artboard* getArtboard() override { return artboard(); }

public:
    TextStyle();
    void buildDependencies() override;
    const rcp<Font> font() const;
    void assets(const std::vector<FileAsset*>& assets) override;
    StatusCode import(ImportStack& importStack) override;

    bool addPath(const RawPath& rawPath);
    void resetPath();
    void draw(Renderer* renderer);
    Core* clone() const override;

protected:
    void fontSizeChanged() override;

private:
    FontAsset* m_fontAsset = nullptr;
    std::unique_ptr<RenderPath> m_path;
    bool m_hasContents = false;
};
} // namespace rive

#endif