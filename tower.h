#ifndef _TOWER_H_
#define _TOWER_H_

#include "facing.h"
#include "vector2.h"
#include "animation.h"
#include "tower_type.h"
#include "enemy_manager.h"
#include "bullet_manager.h"

/**
 * @class Tower
 * @brief ��ʾ��Ϸ�еķ����������𹥻����ˡ����Ŷ����͹�������ȴ
 */
class Tower
{
public:
	/**
	 * @brief ���캯������ʼ�������������ʱ��
	 */
	Tower()
	{
		// ����һ���������ʱ�����������������������
		timer_fire.set_one_shot(true);
		timer_fire.set_on_timeout([&]() { can_fire = true; });

		// ��ʼ�����ö�����ѭ�����ţ�
		anim_idle_up.set_loop(true);
		anim_idle_up.set_interval(0.2);
		anim_idle_down.set_loop(true);
		anim_idle_down.set_interval(0.2);
		anim_idle_left.set_loop(true);
		anim_idle_left.set_interval(0.2);
		anim_idle_right.set_loop(true);
		anim_idle_right.set_interval(0.2);

		// ��ʼ���������������β��ţ������󷵻�����״̬��
		anim_fire_up.set_loop(false);
		anim_fire_up.set_interval(0.2);
		anim_fire_up.set_on_finished([&]() { update_idle_animation(); });

		anim_fire_down.set_loop(false);
		anim_fire_down.set_interval(0.2);
		anim_fire_down.set_on_finished([&]() { update_idle_animation(); });

		anim_fire_left.set_loop(false);
		anim_fire_left.set_interval(0.2);
		anim_fire_left.set_on_finished([&]() { update_idle_animation(); });

		anim_fire_right.set_loop(false);
		anim_fire_right.set_interval(0.2);
		anim_fire_right.set_on_finished([&]() { update_idle_animation(); });
	}

	~Tower() = default;

	/// ��������λ�ã����ĵ����꣩
	void set_position(const Vector2& position) { this->position = position; }

	/// ��ȡ���ĳߴ�
	const Vector2& get_size() const { return size; }

	/// ��ȡ����λ�ã�ע�⣺ԭʵ�ַ��سߴ磬�����Ǵ���
	const Vector2& get_position() const { return size; } // ���ƴ���ӦΪ����position

	/**
	 * @brief ÿ֡������״̬
	 * @param delta ֡ʱ���������룩
	 */
	void on_update(double delta)
	{
		timer_fire.on_update(delta);   // ���������ȴ��ʱ��
		anim_current->on_update(delta); // ���µ�ǰ����

		if (can_fire) on_fire();       // ������������ִ������߼�
	}

	/**
	 * @brief ��Ⱦ��
	 * @param renderer SDL��Ⱦ��ָ��
	 */
	void on_render(SDL_Renderer* renderer)
	{
		static SDL_Point point;
		// ����������ת��Ϊ���Ͻ���Ⱦ����
		point.x = (int)(position.x - size.x / 2);
		point.y = (int)(position.y - size.y / 2);

		anim_current->on_render(renderer, point); // ��Ⱦ��ǰ����֡
	}

protected:
	Vector2 size;  ///< ���ĳߴ磨��ߣ�

	// ��ͬ����Ķ���״̬
	Animation anim_idle_up;     ///< �������ö���
	Animation anim_idle_down;   ///< �������ö���
	Animation anim_idle_left;   ///< �������ö���
	Animation anim_idle_right;  ///< �������ö���
	Animation anim_fire_up;     ///< ���Ϲ�������
	Animation anim_fire_down;   ///< ���¹�������
	Animation anim_fire_left;   ///< ���󹥻�����
	Animation anim_fire_right;  ///< ���ҹ�������

	TowerType tower_type = TowerType::Archer; ///< �������ͣ�������/����/ǹ�֣�

	double fire_speed = 0;          ///< �ӵ������ٶ�
	BulletType bullet_type = BulletType::Arrow; ///< ������ӵ�����

private:
	Timer timer_fire;           ///< ������ȴ��ʱ��
	Vector2 position;           ///< ��������λ������
	bool can_fire = true;       ///< �Ƿ���Է����ӵ�
	Facing facing = Facing::Right; ///< ��ǰ����Ĭ�����ң�
	Animation* anim_current = &anim_idle_right; ///< ��ǰ���ŵĶ���ָ��

private:
	/**
	 * @brief ���µ���Ӧ��������ö���
	 */
	void update_idle_animation()
	{
		switch (facing)
		{
		case Left:  anim_current = &anim_idle_left;  break;
		case Right: anim_current = &anim_idle_right; break;
		case Up:    anim_current = &anim_idle_up;    break;
		case Down:  anim_current = &anim_idle_down;  break;
		}
	}

	/**
	 * @brief ���µ���Ӧ����Ĺ�������
	 */
	void update_fire_animation()
	{
		switch (facing)
		{
		case Left:  anim_current = &anim_fire_left;  break;
		case Right: anim_current = &anim_fire_right; break;
		case Up:    anim_current = &anim_fire_up;    break;
		case Down:  anim_current = &anim_fire_down;  break;
		}
	}

	/**
	 * @brief �ڹ�����Χ��Ѱ��Ŀ�����
	 * @return �ҵ��ĵ���ָ�루δ�ҵ�����nullptr��
	 */
	Enemy* find_target_enemy()
	{
		double max_process = -1;  // ��¼���˵�����н�����
		double view_range = 0;    // ��ǰ������Ұ��Χ
		Enemy* enemy_target = nullptr;

		// �����ù������л�ȡ��ǰ������Ұ��Χ�����������ͺ͵ȼ���
		static ConfigManager* instance = ConfigManager::instance();
		switch (tower_type)
		{
		case Archer:
			view_range = instance->archer_template.view_range[instance->level_archer];
			break;
		case Axeman:
			view_range = instance->axeman_template.view_range[instance->level_axeman];
			break;
		case Gunner:
			view_range = instance->gunner_template.view_range[instance->level_gunner];
			break;
		}

		// ��ȡ���е����б�
		EnemyManager::EnemyList& enemy_list = EnemyManager::instance()->get_enemy_list();

		// �������ˣ�Ѱ������Ұ��Χ�����н��������ĵ���
		for (Enemy* enemy : enemy_list)
		{
			// ������������ľ��루ʹ�õ�ͼ���ӳߴ�SIZE_TILE��Ϊ��λ��
			double distance = (enemy->get_position() - position).length();
			if (distance <= view_range * SIZE_TILE)
			{
				double enemy_progress = enemy->get_route_process();
				// ѡ���н��������ĵ��ˣ���ӽ��յ㣩
				if (enemy_progress > max_process)
				{
					enemy_target = enemy;
					max_process = enemy_progress;
				}
			}
		}

		return enemy_target;
	}

	/**
	 * @brief ִ�й����߼��������ӵ���������Ч�Ͷ���
	 */
	void on_fire()
	{
		// Ѱ�ҹ���Ŀ��
		Enemy* target_enemy = find_target_enemy();
		if (!target_enemy) return; // û��Ŀ���򷵻�

		can_fire = false; // �������״̬

		// ��ȡ���ú���Ч��Դ
		static ConfigManager* instance = ConfigManager::instance();
		static const ResourcesManager::SoundPool& sound_pool =
			ResourcesManager::instance()->get_sound_pool();

		double interval = 0; // �������
		double damage = 0;   // �����˺�

		// �������������ù���������������Ч
		switch (tower_type)
		{
		case Archer:
			interval = instance->archer_template.interval[instance->level_archer];
			damage = instance->archer_template.damage[instance->level_archer];
			// ���ѡ�����ּ�ʸ��Ч֮һ
			switch (rand() % 2) {
			case 0: Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ArrowFire_1)->second, 0); break;
			case 1: Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ArrowFire_2)->second, 0); break;
			}
			break;
		case Axeman:
			interval = instance->axeman_template.interval[instance->level_axeman];
			damage = instance->axeman_template.damage[instance->level_axeman];
			Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_AxeFire)->second, 0);
			break;
		case Gunner:
			interval = instance->gunner_template.interval[instance->level_gunner];
			damage = instance->gunner_template.damage[instance->level_gunner];
			Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ShellFire)->second, 0);
			break;
		}

		// ���ò�����������ȴ��ʱ��
		timer_fire.set_wait_time(interval);
		timer_fire.restart();

		// �����ӵ�����ָ��Ŀ����ˣ�
		Vector2 direction = target_enemy->get_position() - position;
		// ͨ���ӵ������������ӵ�����׼�����������������ٶȣ�
		BulletManager::instance()->fire_bullet(
			bullet_type,
			position,
			direction.normalize() * fire_speed * SIZE_TILE,
			damage
		);

		// ����Ŀ�귽��������ĳ�������ˮƽ����
		bool horizontal_dominant = abs(direction.x) >= abs(direction.y);
		if (horizontal_dominant)
			facing = (direction.x > 0) ? Facing::Right : Facing::Left;
		else
			facing = (direction.y > 0) ? Facing::Down : Facing::Up;

		// �л����������������ò���״̬
		update_fire_animation();
		anim_current->reset();
	}
};

#endif // !_TOWER_H_