#ifndef _BULLET_H_
#define _BULLET_H_

#include "vector2.h"
#include "enemy.h"
#include "animation.h"
#include "config_manager.h"

/**
 * @class Bullet
 * @brief ��ʾ��Ϸ�е��ӵ����󣬸������ӵ����ƶ�����ײ����Ⱦ��״̬����
 */
class Bullet
{
public:
	Bullet() = default;      // Ĭ�Ϲ��캯��
	~Bullet() = default;     // Ĭ����������

	/**
	 * @brief �����ӵ��ٶȲ��Զ�������ת�Ƕȣ����֧����ת��
	 * @param velocity ��ά�ٶ�����
	 */
	void set_velocity(const Vector2& velocity)
	{
		this->velocity = velocity;

		// ����ӵ�֧����ת�������ٶȷ�����㶯����ת�Ƕȣ�����ת�Ƕȣ�
		if (can_rotated)
		{
			double randian = std::atan2(velocity.y, velocity.x);
			angle_anim_rotated = randian * 180 / 3.14159265;
		}
	}

	/// �����ӵ�λ��
	void set_position(const Vector2& position) { this->position = position; }

	/// �����ӵ��˺�ֵ
	void set_damage(double damage) { this->damage = damage; }

	/// ��ȡ�ӵ��ߴ�
	const Vector2& get_size() const { return size; }

	/// ��ȡ�ӵ���ǰλ��
	const Vector2& get_position() const { return position; }

	/// ��ȡ�ӵ��˺�ֵ
	double get_damage() const { return damage; }

	/// ��ȡ�ӵ��˺���Χ����ֵ��ʾ�޷�Χ�˺���
	double get_damage_range() const { return damage_range; }

	/// �����ӵ���ײ���
	void disable_collide() { is_collisional = false; }

	/// ����ӵ��Ƿ����ײ
	bool can_collide() const { return is_collisional; }

	/// ����ӵ�Ϊ��Ч��׼���Ƴ���
	void make_invalid()
	{
		is_valid = false;
		is_collisional = false;  // ͬʱ������ײ
	}

	/// ����ӵ��Ƿ�ɱ��Ƴ�
	bool can_remove() const { return !is_valid; }

	/**
	 * @brief ÿ֡�����ӵ�״̬
	 * @param delta ֡ʱ���������룩
	 */
	virtual void on_update(double delta)
	{
		animation.on_update(delta);         // ���¶���״̬
		position += velocity * delta;       // �����ٶȸ���λ��

		// ��ȡ��ͼ�߽���Σ�ͨ�����ù�������
		static const SDL_Rect& rect_map = ConfigManager::instance()->rect_tile_map;

		// ����ӵ��Ƿ񳬳���ͼ�߽磨�����ӵ��ߴ磩
		if (position.x - size.x / 2 <= rect_map.x ||
			position.x + size.x / 2 >= rect_map.x + rect_map.w ||
			position.y - size.y / 2 <= rect_map.y ||
			position.y + size.y / 2 >= rect_map.y + rect_map.h)
		{
			is_valid = false;  // �����߽�����Ϊ��Ч
		}
	}

	/**
	 * @brief ��Ⱦ�ӵ�
	 * @param renderer SDL��Ⱦ��ָ��
	 */
	virtual void on_render(SDL_Renderer* renderer)
	{
		static SDL_Point point;  // ���õ��������ظ�����

		// ������Ⱦλ�ã����ĵ�ת���Ͻ����꣩
		point.x = (int)(position.x - size.x / 2);
		point.y = (int)(position.y - size.y / 2);

		// ���ö�����Ⱦ������ת�Ƕȣ�
		animation.on_render(renderer, point, angle_anim_rotated);
	}

	/**
	 * @brief ��ײ����ʱ�Ĵ����麯�����ɱ�������д��
	 * @param enemy ��ײ���ĵ���ָ��
	 */
	virtual void on_collide(Enemy* enemy)
	{
		// Ĭ����Ϊ����ײ���ӵ�ʧЧ
		is_valid = false;
		is_collisional = false;
	}

protected:
	Vector2 size;         ///< �ӵ��ĳߴ磨��ߣ�
	Vector2 velocity;     ///< �ӵ����ƶ��ٶ�����
	Vector2 position;     ///< �ӵ�������λ������

	Animation animation;  ///< �ӵ��Ķ���������
	bool can_rotated = false; ///< ��ʶ�ӵ��Ƿ�����ٶȷ�����ת

	double damage = 0;    ///< �ӵ��Ļ����˺�ֵ
	double damage_range = -1; ///< �˺����÷�Χ����ֵ��ʾ�޷�Χ�˺���

private:
	bool is_valid = true;       ///< �ӵ��Ƿ���Ч��falseʱ�����Ƴ���
	bool is_collisional = true; ///< �Ƿ������ײ���
	double angle_anim_rotated = 0; ///< ������ת�Ƕȣ��ȣ�
};

#endif // !_BULLET_H_
