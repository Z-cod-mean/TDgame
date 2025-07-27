#ifndef _STATUS_BAR_H_
#define _STATUS_BAR_H_

#include "coin_manager.h"       // ��ҹ���ϵͳ
#include "home_manager.h"       // ��԰�����أ�����ϵͳ
#include "resources_manager.h"  // ��Դ����ϵͳ
#include "player_manager.h"     // ���״̬����ϵͳ

#include <SDL.h>                // SDL������
#include <string>               // �ַ�������
#include <SDL2_gfxPrimitives.h> // SDL2ͼ�λ�����չ��

// ��Ϸ״̬���ࣨ��ʾ��ҡ����غ���Դ��Ϣ��
class StatusBar
{
public:
	StatusBar() = default;    // Ĭ�Ϲ��캯��
	~StatusBar() = default;   // Ĭ����������

	// ����״̬�����Ͻ�λ��
	void set_position(int x, int y)
	{
		position.x = x, position.y = y;  // ����״̬�����Ͻ�����
	}

	// ����״̬��״̬��ÿ֡���ã�
	void on_update(SDL_Renderer* renderer)
	{
		// ��ȡ��������Դ
		static TTF_Font* font = ResourcesManager::instance()
			->get_font_pool().find(ResID::Font_Main)->second;

		// ������ı�����
		SDL_DestroyTexture(tex_text_background);
		tex_text_background = nullptr;
		SDL_DestroyTexture(tex_text_foreground);
		tex_text_foreground = nullptr;

		// ��ȡ��ǰ���������ת��Ϊ�ַ���
		std::string str_val = std::to_string(
			(int)CoinManager::instance()->get_current_coin_num());

		// �����ı����棨����ӰЧ����
		SDL_Surface* suf_text_background = TTF_RenderText_Blended(
			font, str_val.c_str(), color_text_background); // ��Ӱ�ı�����ɫ��
		SDL_Surface* suf_text_foreground = TTF_RenderText_Blended(
			font, str_val.c_str(), color_text_foreground); // ǰ���ı�����ɫ��

		// �����ı��ߴ�
		width_text = suf_text_background->w;
		height_text = suf_text_background->h;

		// ��������
		tex_text_background = SDL_CreateTextureFromSurface(renderer, suf_text_background);
		tex_text_foreground = SDL_CreateTextureFromSurface(renderer, suf_text_foreground);

		// �ͷ���ʱ����
		SDL_FreeSurface(suf_text_background);
		SDL_FreeSurface(suf_text_foreground);
	}

	// ��Ⱦ״̬����ÿ֡���ã�
	void on_render(SDL_Renderer* renderer)
	{
		static SDL_Rect rect_dst;  // �����õ�Ŀ�����
		// ��ȡ������Դ
		static const auto& tex_pool = ResourcesManager::instance()->get_texture_pool();
		static SDL_Texture* tex_coin = tex_pool.find(ResID::Tex_UICoin)->second;           // ���ͼ��
		static SDL_Texture* tex_heart = tex_pool.find(ResID::Tex_UIHeart)->second;         // ����ͼ�꣨��������ֵ��
		static SDL_Texture* tex_home_avatar = tex_pool.find(ResID::Tex_UIHomeAvatar)->second; // ����ͷ��
		static SDL_Texture* tex_player_avatar = tex_pool.find(ResID::Tex_UIPlayerAvatar)->second; // ���ͷ��

		// 1. ��Ⱦ����ͷ�����Ͻǣ�
		rect_dst.x = position.x, rect_dst.y = position.y;
		rect_dst.w = 78, rect_dst.h = 78;
		SDL_RenderCopy(renderer, tex_home_avatar, nullptr, &rect_dst);

		// 2. ��Ⱦ��������ֵ������ͼ�꣩
		int heart_count = (int)HomeManager::instance()->get_current_hp_num();  // ��ȡ��ǰ����ֵ����
		for (int i = 0; i < heart_count; i++)
		{
			// ˮƽ��������ͼ��
			rect_dst.x = position.x + 78 + 15 + i * (32 + 2);  // ������
			rect_dst.y = position.y;  // ��ֱλ��
			rect_dst.w = 32, rect_dst.h = 32;  // ͼ��ߴ�
			SDL_RenderCopy(renderer, tex_heart, nullptr, &rect_dst);
		}

		// 3. ��Ⱦ���ͼ�꣨����ͷ���Ҳࣩ
		rect_dst.x = position.x + 78 + 15;  // ˮƽλ��
		rect_dst.y = position.y + 78 - 32;  // ��ֱλ�ã��ײ����룩
		rect_dst.w = 32, rect_dst.h = 32;
		SDL_RenderCopy(renderer, tex_coin, nullptr, &rect_dst);

		// 4. ��Ⱦ�������������ӰЧ����
		// ����Ⱦ��Ӱ
		rect_dst.x += 32 + 10 + offset_shadow.x;  // λ�õ��������ͼ���Ҳࣩ
		rect_dst.y = rect_dst.y + (32 - height_text) / 2 + offset_shadow.y;  // ��ֱ����
		rect_dst.w = width_text, rect_dst.h = height_text;
		SDL_RenderCopy(renderer, tex_text_background, nullptr, &rect_dst);

		// ����Ⱦǰ���ı�
		rect_dst.x -= offset_shadow.x;  // ������ԭʼλ��
		rect_dst.y -= offset_shadow.y;
		SDL_RenderCopy(renderer, tex_text_foreground, nullptr, &rect_dst);

		// 5. ��Ⱦ���ͷ�񣨻���ͷ���·���
		rect_dst.x = position.x + (78 - 65) / 2;  // ˮƽ����
		rect_dst.y = position.y + 78 + 5;         // ��ֱλ�ã�����ͷ���·���
		rect_dst.w = 65, rect_dst.h = 65;
		SDL_RenderCopy(renderer, tex_player_avatar, nullptr, &rect_dst);

		// 6. ��Ⱦ���ħ��ֵ������
		// ������Բ�Ǿ��Σ�
		rect_dst.x = position.x + 78 + 15;  // ˮƽλ��
		rect_dst.y += 10;  // ��ֱλ�ã����ͷ���·���
		roundedBoxRGBA(renderer,
			rect_dst.x, rect_dst.y,
			rect_dst.x + width_mp_bar, rect_dst.y + height_mp_bar,
			4,  // Բ�ǰ뾶
			color_mp_bar_background.r, color_mp_bar_background.g,
			color_mp_bar_background.b, color_mp_bar_background.a);

		// ǰ�����ȣ����ݵ�ǰħ��ֵ������
		rect_dst.x += width_border_mp_bar;  // �ڱ߾����
		rect_dst.y += width_border_mp_bar;
		rect_dst.w = width_mp_bar - 2 * width_border_mp_bar;  // �ڲ����
		rect_dst.h = height_mp_bar - 2 * width_border_mp_bar; // �ڲ��߶�

		double process = PlayerManager::instance()->get_current_mp() / 100;  // ħ��ֵ����
		roundedBoxRGBA(renderer,
			rect_dst.x, rect_dst.y,
			rect_dst.x + (int)(rect_dst.w * process), rect_dst.y + rect_dst.h,
			2,  // Բ�ǰ뾶
			color_mp_bar_foredground.r, color_mp_bar_foredground.g,
			color_mp_bar_foredground.b, color_mp_bar_foredground.a);
	}

private:
	// ��������
	const int size_heart = 32;                // ����ͼ��ߴ�
	const int width_mp_bar = 200;             // ħ�����ܿ��
	const int height_mp_bar = 20;             // ħ�����ܸ߶�
	const int width_border_mp_bar = 4;        // ħ�����߿���
	const SDL_Point offset_shadow = { 2, 2 }; // �ı���Ӱƫ����

	// ��ɫ����
	const SDL_Color color_text_background = { 175, 175, 175, 255 }; // �ı���Ӱ��ɫ����ɫ��
	const SDL_Color color_text_foreground = { 255, 255, 255, 255 }; // �ı�ǰ����ɫ����ɫ��
	const SDL_Color color_mp_bar_background = { 48, 40, 51, 255 };  // ħ����������ɫ������ɫ��
	const SDL_Color color_mp_bar_foredground = { 144, 121, 173, 255 }; // ħ����ǰ����ɫ��ǳ��ɫ��

private:
	SDL_Point position = { 0 };          // ״̬�����Ͻ�λ��
	int width_text = 0, height_text = 0; // �ı��ߴ�
	SDL_Texture* tex_text_background = nullptr; // �ı���Ӱ����
	SDL_Texture* tex_text_foreground = nullptr; // �ı�ǰ������
};

#endif // !_STATUS_BAR_H_
