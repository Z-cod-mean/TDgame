#ifndef _ENEMY_H_ // ��ֹͷ�ļ��ظ�����
#define _ENEMY_H_ // ����ͷ�ļ���ʶ��

// ���붨ʱ���࣬������/״̬����ȴʱ��
#include "timer.h"
// ����·���࣬��������ƶ�·��
#include "route.h"
// �����ά�����࣬����λ�á������ٶȵȼ���
#include "vector2.h"
// ���붯���࣬���ڲ��ŵ�������/�ܻ�����
#include "animation.h"
// �������ù����࣬���ڻ�ȡ��ͼ���ӳߴ�Ȳ���
#include "config_manager.h"

#include <functional> // ���뺯����װ��std::function�����ڻص�����

// �����࣬���������ƶ���������״̬���߼�
class Enemy
{
public:
	//����һ������callback����Ӧ�Բ�ͬ�ļ���Ч��
	typedef std::function<void(Enemy* enemy)> SkillCallback; // ���弼�ܻص����ͣ�����һ��Enemyָ�����

public:
	// ���캯������ʼ�����ּ�ʱ��
	Enemy()
	{
		//����ÿ���೤ʱ�䴥��һ��
		timer_skill.set_one_shot(false); // ����Ϊѭ������
		//����ʱ����ʱʱ��ִ�а󶨵ļ����ͷŻص�����
		timer_skill.set_on_timeout([&]() { on_skill_released(this); });

		// �����ܻ����׵ļ�ʱ��ֻ����һ��
		timer_sketch.set_one_shot(true);
		// �������׵ĳ���ʱ��Ϊ0.075��
		timer_sketch.set_wait_time(0.075);
		// ���׽����󣬹ر��ܻ�״̬
		timer_sketch.set_on_timeout([&]() { is_show_sketch = false; });

		// �����ٶȻָ��ļ�ʱ��ֻ����һ��
		timer_restore_speed.set_one_shot(true);
		// �ٶȻָ��ص����ָ�Ϊ����ٶ�
		timer_restore_speed.set_on_timeout([&]() { speed = max_speed; });
	}

	// ����������ʹ��Ĭ����Ϊ
	~Enemy() = default;

	// ÿ֡���º�����deltaΪ��ǰ֡ʱ�䣨�룩
	void on_update(double delta)
	{
		// ���¼��ܡ��ܻ����ָ��ٶȵȼ�ʱ��״̬
		timer_skill.on_update(delta);
		timer_sketch.on_update(delta);
		timer_restore_speed.on_update(delta);

		// ���㱾֡���ƶ����루�ٶ� * ʱ�䣩
		Vector2 move_distance = velocity * delta;
		// ��ǰĿ����뵱ǰλ��֮�������������;��룩
		Vector2 target_distance = position_target - position;
		// �����֡���ƶ��ľ���С��Ŀ����룬���߱�֡���ߵģ�����ֱ�ӵ�Ŀ���
		position += move_distance < target_distance ? move_distance : target_distance;

		// ����Ѿ��ӽ�Ŀ��㣨���Ƚ���Ϊ0��
		if (target_distance.approx_zero())
		{
			// �л�����һ��·����
			idx_target++;
			refresh_position_target();
			// ���¼��㷽������������һ��
			direction = (position_target - position).normalize();
		}

		// ���ݷ�������ٶ������������ٶȺ͸��Ӵ�С��
		velocity.x = direction.x * speed * SIZE_TILE;
		velocity.y = direction.y * speed * SIZE_TILE;

		// �ж��Ǻ����ƶ����Ȼ�������
		bool is_show_x_amin = abs(velocity.x) >= abs(velocity.y);

		// �����ܻ�״̬ѡ���Ӧ���������׻�������
		if (is_show_sketch) // ����������ף��ܵ�������
		{
			if (is_show_x_amin) // ����Ǻ����ƶ�������
				// ���ݺ����ٶȷ����ж�ʹ���������׻����������׶���
				anim_current = velocity.x > 0 ? &anim_right_sketch : &anim_left_sketch;
			else // �����������ƶ�������
				// ���������ٶȷ����ж�ʹ���������׻����������׶���
				anim_current = velocity.y > 0 ? &anim_down_sketch : &anim_up_sketch;
		}
		else // ���û������״̬��ʹ����ͨ����
		{
			if (is_show_x_amin) // ����Ǻ����ƶ�������
				// ʹ�����һ��������ͨ����
				anim_current = velocity.x > 0 ? &anim_right : &anim_left;
			else // �����������ƶ�������
				// ʹ�����»����ϵ���ͨ����
				anim_current = velocity.y > 0 ? &anim_down : &anim_up;
		}


		// ���µ�ǰ����֡
		anim_current->on_update(delta);
	}

	// ��Ⱦ������������Ƶ��˶�����Ѫ��
	void on_render(SDL_Renderer* renderer)
	{
		static SDL_Rect rect; // ���ڻ���Ѫ��
		static SDL_Point point; // ���ڻ��ƶ���λ��
		static const int offset_y = 2; // Ѫ������˼��
		static const Vector2 size_hp_bar = { 40, 8 }; // Ѫ���ߴ�
		static const SDL_Color color_border = { 116, 185, 124, 255 }; // Ѫ���߿���ɫ
		static const SDL_Color color_content = { 226, 255, 194, 255 }; // Ѫ�������ɫ

		// ������Ⱦλ�ã��Ե������ĵ�Ϊ�ο���
		point.x = (int)(position.x - size.x / 2);
		point.y = (int)(position.y - size.y / 2);

		// ��Ⱦ����
		anim_current->on_render(renderer, point);

		// ���Ѫ����������ʾѪ��
		if (hp < max_hp)
		{
			rect.x = (int)(position.x - size_hp_bar.x / 2);
			rect.y = (int)(position.y - size.y / 2 - size_hp_bar.y - offset_y);
			rect.w = (int)(size_hp_bar.x * (hp / max_hp)); // ��ǰѪ������
			rect.h = (int)size_hp_bar.y;
			SDL_SetRenderDrawColor(renderer, color_content.r, color_content.g, color_content.b, color_content.a);
			SDL_RenderFillRect(renderer, &rect); // ��Ѫ������

			rect.w = (int)size_hp_bar.x;
			SDL_SetRenderDrawColor(renderer, color_border.r, color_border.g, color_border.b, color_border.a);
			SDL_RenderDrawRect(renderer, &rect); // ��Ѫ���߿�
		}
	}

	// ���ü����ͷŻص�����
	void set_on_skill_released(SkillCallback on_skill_released)
	{
		this->on_skill_released = on_skill_released;
	}

	// ����Ѫ�������ܳ������ֵ��
	void increase_hp(double val)
	{
		hp += val;
		if (hp > max_hp)
			hp = max_hp;
	}

	// ����Ѫ������Ѫ��<=0�����Ϊ��Ч����������
	void decrease_hp(double val)
	{
		hp -= val;
		if (hp <= 0)
		{
			hp = 0;
			is_valid = false;
		}
		is_show_sketch = true;
		timer_sketch.restart();
	}

	// ����1���ָ��ٶ�
	void slow_down()
	{
		speed = max_speed - 0.5;
		timer_restore_speed.set_wait_time(1);
		timer_restore_speed.restart();
	}

	// ���õ�ǰλ��
	void set_position(const Vector2& position)
	{
		this->position = position;
	}

	// ����·������ʼ��Ŀ���
	void set_route(const Route* route)
	{
		this->route = route;
		refresh_position_target();
	}

	// ���õ���Ϊ��Ч������ɱ����ʧ��
	void make_invalid()
	{
		is_valid = false;
	}

	// ��ȡ��ǰѪ��
	double get_hp() const { return hp; }
	// ��ȡ���˳ߴ�
	const Vector2& get_size() const { return size; }
	// ��ȡ��ǰλ��
	const Vector2& get_position() const { return position; }
	// ��ȡ��ǰ�ٶ�
	const Vector2& get_velocity() const { return velocity; }
	// ��ȡ�����˺�
	double get_damage() const { return damage; }
	// ��ȡ��ɱ��������
	double get_reward_ratio() const { return reward_ratio; }
	// ��ȡ��Ѫ��Χ�����أ�
	double get_recover_radius() const { return SIZE_TILE * recover_range; }
	// ��ȡ��Ѫǿ��
	double get_recover_intensity() const { return recover_intensity; }
	// �Ƿ�������/��Ч
	bool can_remove() const { return !is_valid; }
	// ��ȡ��ǰ��·���еĽ��ȣ�0~1��
	double get_route_process() const
	{
		if (route->get_idx_list().size() == 1)
			return 1;
		return (double)idx_target / (route->get_idx_list().size() - 1);
	}

protected:
	Vector2 size; // ���˳ߴ磨���أ�
	Timer timer_skill; // ������ȴ��ʱ��
	Animation anim_up, anim_down, anim_left, anim_right; // ��ͨ�ƶ�����
	Animation anim_up_sketch, anim_down_sketch, anim_left_sketch, anim_right_sketch; // �ܻ����׶���
	double hp = 0, max_hp = 0; // ��ǰ�����Ѫ��
	double speed = 0, max_speed = 0; // ��ǰ������ٶ�
	double damage = 0; // �����˺�
	double reward_ratio = 0; // ��ɱ��������
	double recover_interval = 0, recover_range = 0, recover_intensity = 0; // ��Ѫ�������Χ��ǿ��

private:
	Vector2 position; // ��ǰλ��
	Vector2 velocity; // ��ǰ�ٶ�����
	Vector2 direction; // ��ǰ�ƶ�����
	bool is_valid = true; // �Ƿ���Ч���Ƿ��
	Timer timer_sketch; // ���׼�ʱ��
	bool is_show_sketch = false; // �Ƿ����ܻ�״̬
	Animation* anim_current = nullptr; // ��ǰ���Ŷ���ָ��
	SkillCallback on_skill_released; // �����ͷŻص�����
	Timer timer_restore_speed; // �ٶȻָ���ʱ��
	const Route* route = nullptr; // ·������ָ��
	int idx_target = 0; // ��ǰĿ���������
	Vector2 position_target; // ��ǰĿ��λ��

	// ����·������Ŀ��λ��
	void refresh_position_target()
	{
		const Route::IdxList& idx_list = route->get_idx_list();
		if (idx_target < idx_list.size())
		{
			const SDL_Point& point = idx_list[idx_target];
			static const SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;
			position_target.x = rect_tile_map.x + point.x * SIZE_TILE + SIZE_TILE / 2; // ��������x
			position_target.y = rect_tile_map.y + point.y * SIZE_TILE + SIZE_TILE / 2; // ��������y
		}
	}
};

#endif // !_ENEMY_H_ // ������ֹ�ظ�����
