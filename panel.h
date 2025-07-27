#ifndef _PANEL_H_
#define _PANEL_H_

#include "tile.h"              // 地图瓦片相关
#include "resources_manager.h" // 资源管理

#include <SDL.h>
#include <string>


//这是一个通用的 UI 面板基类，用于在游戏地图上创建交互式操作面板（如塔升级/出售面板）。
// 它提供鼠标交互、文本渲染和基本布局功能，采用三按钮设计（上/左/右区域）
class Panel
{
public:
    Panel()
    {
        // 加载选择光标纹理
        tex_select_cursor = ResourcesManager::instance()
            ->get_texture_pool().find(ResID::Tex_UISelectCursor)->second;
    }

    ~Panel()
    {
        // 清理文本纹理
        SDL_DestroyTexture(tex_text_background);
        SDL_DestroyTexture(tex_text_foreground);
    }

    // 显示面板
    void show() { visible = true; }

    // 设置关联的瓦片索引
    void set_idx_tile(const SDL_Point& idx) { idx_tile_selected = idx; }

    // 设置面板中心位置（屏幕坐标）
    void set_center_pos(const SDL_Point& pos) { center_pos = pos; }

    // 输入事件处理
    void on_input(const SDL_Event& event)
    {
        if (!visible) return;  // 面板不可见时不处理

        switch (event.type)
        {
        case SDL_MOUSEMOTION:  // 鼠标移动检测
        {
            SDL_Point pos_cursor = { event.motion.x, event.motion.y };
            SDL_Rect rect_target = { 0, 0, size_button, size_button };

            // 检测顶部按钮悬停
            rect_target.x = center_pos.x - width / 2 + offset_top.x;
            rect_target.y = center_pos.y - width / 2 + offset_top.y;
            if (SDL_PointInRect(&pos_cursor, &rect_target)) {
                hovered_target = HoveredTarget::Top;
                return;
            }

            // 检测左侧按钮悬停
            rect_target.x = center_pos.x - width / 2 + offset_left.x;
            rect_target.y = center_pos.y - width / 2 + offset_left.y;
            if (SDL_PointInRect(&pos_cursor, &rect_target)) {
                hovered_target = HoveredTarget::Left;
                return;
            }

            // 检测右侧按钮悬停
            rect_target.x = center_pos.x - width / 2 + offset_right.x;
            rect_target.y = center_pos.y - width / 2 + offset_right.y;
            if (SDL_PointInRect(&pos_cursor, &rect_target)) {
                hovered_target = HoveredTarget::Right;
                return;
            }

            hovered_target = HoveredTarget::None;  // 无悬停区域
        }
        break;
        case SDL_MOUSEBUTTONUP:  // 鼠标点击处理
        {
            switch (hovered_target)
            {
            case HoveredTarget::Top:
                on_click_top_area();  // 顶部区域点击回调
                break;
            case HoveredTarget::Left:
                on_click_left_area(); // 左侧区域点击回调
                break;
            case HoveredTarget::Right:
                on_click_right_area();// 右侧区域点击回调
                break;
            }
            visible = false;  // 点击后隐藏面板
        }
        break;
        default:
            break;
        }
    }

    // 更新面板状态（每帧调用）
    virtual void on_update(SDL_Renderer* renderer)
    {
        static TTF_Font* font = ResourcesManager::instance()
            ->get_font_pool().find(ResID::Font_Main)->second;

        if (hovered_target == HoveredTarget::None)
            return;  // 无悬停时不更新文本

        // 获取当前悬停区域的值
        int val = 0;
        switch (hovered_target)
        {
        case HoveredTarget::Top:   val = val_top;   break;
        case HoveredTarget::Left:  val = val_left;  break;
        case HoveredTarget::Right: val = val_right; break;
        }

        // 清理旧文本纹理
        SDL_DestroyTexture(tex_text_background);
        tex_text_background = nullptr;
        SDL_DestroyTexture(tex_text_foreground);
        tex_text_foreground = nullptr;

        // 创建新文本纹理（特殊值显示"MAX"）
        std::string str_val = val < 0 ? "MAX" : std::to_string(val);
        SDL_Surface* suf_text_background = TTF_RenderText_Blended(
            font, str_val.c_str(), color_text_background);
        SDL_Surface* suf_text_foreground = TTF_RenderText_Blended(
            font, str_val.c_str(), color_text_foreground);

        // 保存文本尺寸并创建纹理
        width_text = suf_text_background->w;
        height_text = suf_text_background->h;
        tex_text_background = SDL_CreateTextureFromSurface(renderer, suf_text_background);
        tex_text_foreground = SDL_CreateTextureFromSurface(renderer, suf_text_foreground);

        // 清理临时表面
        SDL_FreeSurface(suf_text_background);
        SDL_FreeSurface(suf_text_foreground);
    }

    // 渲染面板（每帧调用）
    virtual void on_render(SDL_Renderer* renderer)
    {
        if (!visible) return;

        // 1. 渲染选择光标（在选定瓦片位置）
        SDL_Rect rect_dst_cursor = {
            center_pos.x - SIZE_TILE / 2,
            center_pos.y - SIZE_TILE / 2,
            SIZE_TILE, SIZE_TILE
        };
        SDL_RenderCopy(renderer, tex_select_cursor, nullptr, &rect_dst_cursor);

        // 2. 渲染面板主体
        SDL_Rect rect_dst_panel = {
            center_pos.x - width / 2,
            center_pos.y - height / 2,
            width, height
        };

        // 根据悬停状态选择面板纹理
        SDL_Texture* tex_panel = nullptr;
        switch (hovered_target)
        {
        case HoveredTarget::None:    tex_panel = tex_idle;          break;
        case HoveredTarget::Top:     tex_panel = tex_hovered_top;   break;
        case HoveredTarget::Left:    tex_panel = tex_hovered_left;  break;
        case HoveredTarget::Right:   tex_panel = tex_hovered_right; break;
        }
        SDL_RenderCopy(renderer, tex_panel, nullptr, &rect_dst_panel);

        // 3. 渲染悬停区域的文本（带阴影效果）
        if (hovered_target == HoveredTarget::None) return;

        SDL_Rect rect_dst_text;
        // 先渲染背景（阴影）
        rect_dst_text.x = center_pos.x - width_text / 2 + offset_shadow.x;
        rect_dst_text.y = center_pos.y + height / 2 + offset_shadow.y;
        rect_dst_text.w = width_text;
        rect_dst_text.h = height_text;
        SDL_RenderCopy(renderer, tex_text_background, nullptr, &rect_dst_text);

        // 再渲染前景（主文本）
        rect_dst_text.x -= offset_shadow.x;
        rect_dst_text.y -= offset_shadow.y;
        SDL_RenderCopy(renderer, tex_text_foreground, nullptr, &rect_dst_text);
    }

protected:
    // 悬停目标区域枚举
    enum class HoveredTarget { None, Top, Left, Right };

protected:
    bool visible = false;               // 面板可见性
    SDL_Point idx_tile_selected;        // 关联的瓦片索引
    SDL_Point center_pos = { 0 };       // 面板中心位置（屏幕坐标）
    SDL_Texture* tex_idle = nullptr;    // 默认面板纹理
    SDL_Texture* tex_hovered_top = nullptr;   // 顶部悬停纹理
    SDL_Texture* tex_hovered_left = nullptr;  // 左侧悬停纹理
    SDL_Texture* tex_hovered_right = nullptr; // 右侧悬停纹理
    SDL_Texture* tex_select_cursor = nullptr; // 选择光标纹理
    int val_top = 0, val_left = 0, val_right = 0; // 各区域显示的值
    HoveredTarget hovered_target = HoveredTarget::None; // 当前悬停区域

protected:
    // 纯虚函数 - 需要子类实现具体点击逻辑
    virtual void on_click_top_area() = 0;
    virtual void on_click_left_area() = 0;
    virtual void on_click_right_area() = 0;

private:
    // 布局常量（像素）
    const int size_button = 48;     // 按钮尺寸
    const int width = 144, height = 144; // 面板尺寸
    const SDL_Point offset_top = { 48, 6 };    // 顶部按钮偏移
    const SDL_Point offset_left = { 8, 80 };   // 左侧按钮偏移
    const SDL_Point offset_right = { 90, 80 }; // 右侧按钮偏移
    const SDL_Point offset_shadow = { 3, 3 };  // 文本阴影偏移

    // 文本颜色
    const SDL_Color color_text_background = { 175, 175, 175, 255 }; // 阴影色（灰）
    const SDL_Color color_text_foreground = { 255, 255, 255, 255 }; // 前景色（白）

    // 文本渲染相关
    int width_text = 0, height_text = 0;
    SDL_Texture* tex_text_background = nullptr; // 文本阴影纹理
    SDL_Texture* tex_text_foreground = nullptr; // 文本主体纹理
};

#endif // !_PANEL_H_