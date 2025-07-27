#ifndef _COIN_PROP_H_
#define _COIN_PROP_H_

#include "tile.h"              // ��Ƭ�ࣨλ����Ϣ��
#include "timer.h"             // ��ʱ����
#include "vector2.h"           // ��ά������
#include "resources_manager.h" // ��Դ������

#include <SDL.h>               // SDL������

// ��ҵ����ࣨ��Ϸ�еĿ��ռ���ң�
class CoinProp
{
public:
	CoinProp()
	{
		// ��Ծ��ʱ�����ã�һ���Լ�ʱ����
		timer_jump.set_one_shot(true);  // ����Ϊ���δ���
		timer_jump.set_wait_time(interval_jump);  // ������Ծ����ʱ��
		timer_jump.set_on_timeout(       // ���ó�ʱ�ص�
			[&]() { is_jumping = false; });  // ��Ծ����ʱ���Ϊfalse

		// ��ʧ��ʱ�����ã�һ���Լ�ʱ����
		timer_disappear.set_one_shot(true);  // ����Ϊ���δ���
		timer_disappear.set_wait_time(interval_disappear);  // ������ʧʱ��
		timer_disappear.set_on_timeout(       // ���ó�ʱ�ص�
			[&]() { is_valid = false; });  // ��ʱ����Ϊ��Ч

		// ��ʼ�����ˮƽ�ٶȺ������ٶ�
		velocity.x = (rand() % 2 ? 1 : -1) * 2 * SIZE_TILE;  // ������ҷ���
		velocity.y = -3 * SIZE_TILE;  // ������Ծ�ĳ�ʼ�ٶ�
	}

	~CoinProp() = default;  // Ĭ����������

	// ���ý��λ��
	void set_position(const Vector2& position)
	{
		this->position = position;
	}

	// ��ȡ��ҵ�ǰλ��
	const Vector2& get_position() const
	{
		return position;
	}

	// ��ȡ��ҳߴ�
	const Vector2& get_size() const
	{
		return size;
	}

	// ��ǽ��Ϊ��Ч�����ռ�����ã�
	void make_invalid()
	{
		is_valid = false;
	}

	// ������Ƿ�ɱ��Ƴ����Ƿ���Ч��
	bool can_remove()
	{
		return !is_valid;
	}

	// ÿ֡���½��״̬
	void on_update(double delta)
	{
		// ����������ʱ��
		timer_jump.on_update(delta);
		timer_disappear.on_update(delta);

		// ��Ծ״̬�µ�����ģ��
		if (is_jumping)
		{
			velocity.y += gravity * delta;  // Ӧ���������ٶ�
		}
		// ��Ծ�������Ư��Ч��
		else
		{
			velocity.x = 0;  // ֹͣˮƽ�ƶ�
			velocity.y = sin(SDL_GetTicks64() / 1000.0 * 4) * 30;  // ��ֱƯ���������˶���
		}

		// �����ٶȸ���λ��
		position += velocity * delta;
	}

	// ��Ⱦ���
	void on_render(SDL_Renderer* renderer)
	{
		static SDL_Rect rect = { 0, 0, (int)size.x, (int)size.y };  // ��Ⱦ����
		// ��ȡ�������
		static SDL_Texture* tex_coin = ResourcesManager::instance()
			->get_texture_pool().find(ResID::Tex_Coin)->second;

		// ������Ⱦλ�ã����У�
		rect.x = (int)(position.x - size.x / 2);
		rect.y = (int)(position.y - size.y / 2);

		// ִ����Ⱦ
		SDL_RenderCopy(renderer, tex_coin, nullptr, &rect);
	}

private:
	Vector2 position;      // ��ǰλ��
	Vector2 velocity;      // ��ǰ�ٶ�

	Timer timer_jump;      // ��Ծ״̬��ʱ��
	Timer timer_disappear; // ��ʧ��ʱ��

	bool is_valid = true;  // �Ƿ���Ч��δ���ռ���
	bool is_jumping = true; // �Ƿ�����Ծ״̬

	// �������
	double gravity = 490;  // �������ٶȣ�����/��?��
	double interval_jump = 0.75;  // ��Ծ����ʱ�䣨�룩
	Vector2 size = { 16, 16 };  // ��ҳߴ磨���أ�
	double interval_disappear = 10;  // ��ʧǰ�ȴ�ʱ�䣨�룩

};

#endif // !_COIN_PROP_H_
