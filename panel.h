#ifndef _PANEL_H_
#define _PANEL_H_

#include "tile.h"              // ��ͼ��Ƭ���
#include "resources_manager.h" // ��Դ����

#include <SDL.h>
#include <string>


//����һ��ͨ�õ� UI �����࣬��������Ϸ��ͼ�ϴ�������ʽ������壨��������/������壩��
// ���ṩ��꽻�����ı���Ⱦ�ͻ������ֹ��ܣ���������ť��ƣ���/��/������
class Panel
{
public:
    Panel()
    {
        // ����ѡ��������
        tex_select_cursor = ResourcesManager::instance()
            ->get_texture_pool().find(ResID::Tex_UISelectCursor)->second;
    }

    ~Panel()
    {
        // �����ı�����
        SDL_DestroyTexture(tex_text_background);
        SDL_DestroyTexture(tex_text_foreground);
    }

    // ��ʾ���
    void show() { visible = true; }

    // ���ù�������Ƭ����
    void set_idx_tile(const SDL_Point& idx) { idx_tile_selected = idx; }

    // �����������λ�ã���Ļ���꣩
    void set_center_pos(const SDL_Point& pos) { center_pos = pos; }

    // �����¼�����
    void on_input(const SDL_Event& event)
    {
        if (!visible) return;  // ��岻�ɼ�ʱ������

        switch (event.type)
        {
        case SDL_MOUSEMOTION:  // ����ƶ����
        {
            SDL_Point pos_cursor = { event.motion.x, event.motion.y };
            SDL_Rect rect_target = { 0, 0, size_button, size_button };

            // ��ⶥ����ť��ͣ
            rect_target.x = center_pos.x - width / 2 + offset_top.x;
            rect_target.y = center_pos.y - width / 2 + offset_top.y;
            if (SDL_PointInRect(&pos_cursor, &rect_target)) {
                hovered_target = HoveredTarget::Top;
                return;
            }

            // �����ఴť��ͣ
            rect_target.x = center_pos.x - width / 2 + offset_left.x;
            rect_target.y = center_pos.y - width / 2 + offset_left.y;
            if (SDL_PointInRect(&pos_cursor, &rect_target)) {
                hovered_target = HoveredTarget::Left;
                return;
            }

            // ����Ҳఴť��ͣ
            rect_target.x = center_pos.x - width / 2 + offset_right.x;
            rect_target.y = center_pos.y - width / 2 + offset_right.y;
            if (SDL_PointInRect(&pos_cursor, &rect_target)) {
                hovered_target = HoveredTarget::Right;
                return;
            }

            hovered_target = HoveredTarget::None;  // ����ͣ����
        }
        break;
        case SDL_MOUSEBUTTONUP:  // ���������
        {
            switch (hovered_target)
            {
            case HoveredTarget::Top:
                on_click_top_area();  // �����������ص�
                break;
            case HoveredTarget::Left:
                on_click_left_area(); // ����������ص�
                break;
            case HoveredTarget::Right:
                on_click_right_area();// �Ҳ��������ص�
                break;
            }
            visible = false;  // ������������
        }
        break;
        default:
            break;
        }
    }

    // �������״̬��ÿ֡���ã�
    virtual void on_update(SDL_Renderer* renderer)
    {
        static TTF_Font* font = ResourcesManager::instance()
            ->get_font_pool().find(ResID::Font_Main)->second;

        if (hovered_target == HoveredTarget::None)
            return;  // ����ͣʱ�������ı�

        // ��ȡ��ǰ��ͣ�����ֵ
        int val = 0;
        switch (hovered_target)
        {
        case HoveredTarget::Top:   val = val_top;   break;
        case HoveredTarget::Left:  val = val_left;  break;
        case HoveredTarget::Right: val = val_right; break;
        }

        // ������ı�����
        SDL_DestroyTexture(tex_text_background);
        tex_text_background = nullptr;
        SDL_DestroyTexture(tex_text_foreground);
        tex_text_foreground = nullptr;

        // �������ı���������ֵ��ʾ"MAX"��
        std::string str_val = val < 0 ? "MAX" : std::to_string(val);
        SDL_Surface* suf_text_background = TTF_RenderText_Blended(
            font, str_val.c_str(), color_text_background);
        SDL_Surface* suf_text_foreground = TTF_RenderText_Blended(
            font, str_val.c_str(), color_text_foreground);

        // �����ı��ߴ粢��������
        width_text = suf_text_background->w;
        height_text = suf_text_background->h;
        tex_text_background = SDL_CreateTextureFromSurface(renderer, suf_text_background);
        tex_text_foreground = SDL_CreateTextureFromSurface(renderer, suf_text_foreground);

        // ������ʱ����
        SDL_FreeSurface(suf_text_background);
        SDL_FreeSurface(suf_text_foreground);
    }

    // ��Ⱦ��壨ÿ֡���ã�
    virtual void on_render(SDL_Renderer* renderer)
    {
        if (!visible) return;

        // 1. ��Ⱦѡ���꣨��ѡ����Ƭλ�ã�
        SDL_Rect rect_dst_cursor = {
            center_pos.x - SIZE_TILE / 2,
            center_pos.y - SIZE_TILE / 2,
            SIZE_TILE, SIZE_TILE
        };
        SDL_RenderCopy(renderer, tex_select_cursor, nullptr, &rect_dst_cursor);

        // 2. ��Ⱦ�������
        SDL_Rect rect_dst_panel = {
            center_pos.x - width / 2,
            center_pos.y - height / 2,
            width, height
        };

        // ������ͣ״̬ѡ���������
        SDL_Texture* tex_panel = nullptr;
        switch (hovered_target)
        {
        case HoveredTarget::None:    tex_panel = tex_idle;          break;
        case HoveredTarget::Top:     tex_panel = tex_hovered_top;   break;
        case HoveredTarget::Left:    tex_panel = tex_hovered_left;  break;
        case HoveredTarget::Right:   tex_panel = tex_hovered_right; break;
        }
        SDL_RenderCopy(renderer, tex_panel, nullptr, &rect_dst_panel);

        // 3. ��Ⱦ��ͣ������ı�������ӰЧ����
        if (hovered_target == HoveredTarget::None) return;

        SDL_Rect rect_dst_text;
        // ����Ⱦ��������Ӱ��
        rect_dst_text.x = center_pos.x - width_text / 2 + offset_shadow.x;
        rect_dst_text.y = center_pos.y + height / 2 + offset_shadow.y;
        rect_dst_text.w = width_text;
        rect_dst_text.h = height_text;
        SDL_RenderCopy(renderer, tex_text_background, nullptr, &rect_dst_text);

        // ����Ⱦǰ�������ı���
        rect_dst_text.x -= offset_shadow.x;
        rect_dst_text.y -= offset_shadow.y;
        SDL_RenderCopy(renderer, tex_text_foreground, nullptr, &rect_dst_text);
    }

protected:
    // ��ͣĿ������ö��
    enum class HoveredTarget { None, Top, Left, Right };

protected:
    bool visible = false;               // ���ɼ���
    SDL_Point idx_tile_selected;        // ��������Ƭ����
    SDL_Point center_pos = { 0 };       // �������λ�ã���Ļ���꣩
    SDL_Texture* tex_idle = nullptr;    // Ĭ���������
    SDL_Texture* tex_hovered_top = nullptr;   // ������ͣ����
    SDL_Texture* tex_hovered_left = nullptr;  // �����ͣ����
    SDL_Texture* tex_hovered_right = nullptr; // �Ҳ���ͣ����
    SDL_Texture* tex_select_cursor = nullptr; // ѡ��������
    int val_top = 0, val_left = 0, val_right = 0; // ��������ʾ��ֵ
    HoveredTarget hovered_target = HoveredTarget::None; // ��ǰ��ͣ����

protected:
    // ���麯�� - ��Ҫ����ʵ�־������߼�
    virtual void on_click_top_area() = 0;
    virtual void on_click_left_area() = 0;
    virtual void on_click_right_area() = 0;

private:
    // ���ֳ��������أ�
    const int size_button = 48;     // ��ť�ߴ�
    const int width = 144, height = 144; // ���ߴ�
    const SDL_Point offset_top = { 48, 6 };    // ������ťƫ��
    const SDL_Point offset_left = { 8, 80 };   // ��ఴťƫ��
    const SDL_Point offset_right = { 90, 80 }; // �Ҳఴťƫ��
    const SDL_Point offset_shadow = { 3, 3 };  // �ı���Ӱƫ��

    // �ı���ɫ
    const SDL_Color color_text_background = { 175, 175, 175, 255 }; // ��Ӱɫ���ң�
    const SDL_Color color_text_foreground = { 255, 255, 255, 255 }; // ǰ��ɫ���ף�

    // �ı���Ⱦ���
    int width_text = 0, height_text = 0;
    SDL_Texture* tex_text_background = nullptr; // �ı���Ӱ����
    SDL_Texture* tex_text_foreground = nullptr; // �ı���������
};

#endif // !_PANEL_H_