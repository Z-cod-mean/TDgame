#ifndef _BANNER_H_
#define _BANNER_H_

#include "timer.h"              // ��ʱ��������
#include "vector2.h"            // ��ά����������
#include "config_manager.h"     // ��Ϸ���ù�����
#include "resources_manager.h"  // ��Դ������

#include <SDL.h>                // SDL������

// ��Ϸ��������ࣨ��ʾʤ��/ʧ����Ϣ��
class Banner
{
public:
	// ���캯������ʼ������ߴ�ͼ�ʱ��
	Banner()
	{
		// ����ǰ�����ֺͱ��������ĳߴ�
		size_foreground = { 646, 215 };  // ǰ����������ߴ�
		size_background = { 1282, 209 }; // ������������ߴ�

		// ������ʾ��ʱ����5����Զ����أ�
		timer_display.set_one_shot(true);      // ����Ϊһ���Լ�ʱ��
		timer_display.set_wait_time(5);        // ���õȴ�ʱ��Ϊ5��
		timer_display.set_on_timeout(           // ���ó�ʱ�ص�����
			[&]() { is_end_display = true; }); // ��ʱʱ��������־��Ϊtrue
	}

	// Ĭ����������
	~Banner() = default;

	// ���ú������Ļ�ϵ�����λ��
	void set_center_position(const Vector2& pos)
	{
		pos_center = pos;  // ��������λ������
	}

	// ÿ֡���º��״̬
	void on_update(double delta)
	{
		timer_display.on_update(delta);  // ���¼�ʱ��״̬

		// ����Դ��������ȡ�����
		const ResourcesManager::TexturePool& tex_pool
			= ResourcesManager::instance()->get_texture_pool();

		// ��ȡ���ù�����ʵ��
		const ConfigManager* instance = ConfigManager::instance();

		// ������Ϸʤ��״̬ѡ��ǰ������
		tex_foreground = tex_pool.find(
			instance->is_game_win ? ResID::Tex_UIWinText : ResID::Tex_UILossText
		)->second;

		// ��ȡ������������
		tex_background = tex_pool.find(ResID::Tex_UIGameOverBar)->second;
	}

	// ��Ⱦ�������Ļ
	void on_render(SDL_Renderer* renderer)
	{
		static SDL_Rect rect_dst;  // Ŀ����Ⱦ���򣨸������Ч�ʣ�

		// ��Ⱦ����������������ʾ��
		rect_dst.x = (int)(pos_center.x - size_background.x / 2);  // ˮƽ���м���
		rect_dst.y = (int)(pos_center.y - size_background.y / 2);  // ��ֱ���м���
		rect_dst.w = (int)size_background.x;  // ���ÿ��
		rect_dst.h = (int)size_background.y;  // ���ø߶�
		SDL_RenderCopy(renderer, tex_background, nullptr, &rect_dst);  // ִ����Ⱦ

		// ��Ⱦǰ�����֣�������ʾ�ڱ����ϣ�
		rect_dst.x = (int)(pos_center.x - size_foreground.x / 2);  // ˮƽ���м���
		rect_dst.y = (int)(pos_center.y - size_foreground.y / 2);  // ��ֱ���м���
		rect_dst.w = (int)size_foreground.x;  // ���ÿ��
		rect_dst.h = (int)size_foreground.y;  // ���ø߶�
		SDL_RenderCopy(renderer, tex_foreground, nullptr, &rect_dst);  // ִ����Ⱦ
	}

	// �������ʾ�Ƿ������ע�⣺��������ƴд���󣬵�����ԭ����
	bool check_end_dispaly()
	{
		return is_end_display;  // ������ʾ������־
	}

private:
	Vector2 pos_center;          // �������λ�ã���Ļ���꣩

	Vector2 size_foreground;     // ǰ�����ֳߴ�
	Vector2 size_background;     // ���������ߴ�

	SDL_Texture* tex_foreground = nullptr;  // ǰ����������ʤ��/ʧ�ܣ�
	SDL_Texture* tex_background = nullptr;  // ������������

	Timer timer_display;         // ��ʾʱ����ʱ��
	bool is_end_display = false; // ��ʾ������־��true��ʾӦ������ʾ��

};

#endif // !_BANNER_H_
