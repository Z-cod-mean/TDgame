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
 * @brief 表示游戏中的防御塔，负责攻击敌人、播放动画和管理攻击冷却
 */
class Tower
{
public:
	/**
	 * @brief 构造函数：初始化动画和射击计时器
	 */
	Tower()
	{
		// 设置一次性射击计时器（触发后重置射击能力）
		timer_fire.set_one_shot(true);
		timer_fire.set_on_timeout([&]() { can_fire = true; });

		// 初始化闲置动画（循环播放）
		anim_idle_up.set_loop(true);
		anim_idle_up.set_interval(0.2);
		anim_idle_down.set_loop(true);
		anim_idle_down.set_interval(0.2);
		anim_idle_left.set_loop(true);
		anim_idle_left.set_interval(0.2);
		anim_idle_right.set_loop(true);
		anim_idle_right.set_interval(0.2);

		// 初始化攻击动画（单次播放，结束后返回闲置状态）
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

	/// 设置塔的位置（中心点坐标）
	void set_position(const Vector2& position) { this->position = position; }

	/// 获取塔的尺寸
	const Vector2& get_size() const { return size; }

	/// 获取塔的位置（注意：原实现返回尺寸，可能是错误）
	const Vector2& get_position() const { return size; } // 疑似错误，应为返回position

	/**
	 * @brief 每帧更新塔状态
	 * @param delta 帧时间增量（秒）
	 */
	void on_update(double delta)
	{
		timer_fire.on_update(delta);   // 更新射击冷却计时器
		anim_current->on_update(delta); // 更新当前动画

		if (can_fire) on_fire();       // 如果可以射击，执行射击逻辑
	}

	/**
	 * @brief 渲染塔
	 * @param renderer SDL渲染器指针
	 */
	void on_render(SDL_Renderer* renderer)
	{
		static SDL_Point point;
		// 将中心坐标转换为左上角渲染坐标
		point.x = (int)(position.x - size.x / 2);
		point.y = (int)(position.y - size.y / 2);

		anim_current->on_render(renderer, point); // 渲染当前动画帧
	}

protected:
	Vector2 size;  ///< 塔的尺寸（宽高）

	// 不同方向的动画状态
	Animation anim_idle_up;     ///< 朝上闲置动画
	Animation anim_idle_down;   ///< 朝下闲置动画
	Animation anim_idle_left;   ///< 朝左闲置动画
	Animation anim_idle_right;  ///< 朝右闲置动画
	Animation anim_fire_up;     ///< 朝上攻击动画
	Animation anim_fire_down;   ///< 朝下攻击动画
	Animation anim_fire_left;   ///< 朝左攻击动画
	Animation anim_fire_right;  ///< 朝右攻击动画

	TowerType tower_type = TowerType::Archer; ///< 塔的类型（弓箭手/斧兵/枪手）

	double fire_speed = 0;          ///< 子弹飞行速度
	BulletType bullet_type = BulletType::Arrow; ///< 发射的子弹类型

private:
	Timer timer_fire;           ///< 攻击冷却计时器
	Vector2 position;           ///< 塔的中心位置坐标
	bool can_fire = true;       ///< 是否可以发射子弹
	Facing facing = Facing::Right; ///< 当前朝向（默认向右）
	Animation* anim_current = &anim_idle_right; ///< 当前播放的动画指针

private:
	/**
	 * @brief 更新到对应方向的闲置动画
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
	 * @brief 更新到对应方向的攻击动画
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
	 * @brief 在攻击范围内寻找目标敌人
	 * @return 找到的敌人指针（未找到返回nullptr）
	 */
	Enemy* find_target_enemy()
	{
		double max_process = -1;  // 记录敌人的最大行进进度
		double view_range = 0;    // 当前塔的视野范围
		Enemy* enemy_target = nullptr;

		// 从配置管理器中获取当前塔的视野范围（根据塔类型和等级）
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

		// 获取所有敌人列表
		EnemyManager::EnemyList& enemy_list = EnemyManager::instance()->get_enemy_list();

		// 遍历敌人，寻找在视野范围内且行进进度最大的敌人
		for (Enemy* enemy : enemy_list)
		{
			// 计算敌人与塔的距离（使用地图格子尺寸SIZE_TILE作为单位）
			double distance = (enemy->get_position() - position).length();
			if (distance <= view_range * SIZE_TILE)
			{
				double enemy_progress = enemy->get_route_process();
				// 选择行进进度最大的敌人（最接近终点）
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
	 * @brief 执行攻击逻辑：发射子弹、播放音效和动画
	 */
	void on_fire()
	{
		// 寻找攻击目标
		Enemy* target_enemy = find_target_enemy();
		if (!target_enemy) return; // 没有目标则返回

		can_fire = false; // 重置射击状态

		// 获取配置和音效资源
		static ConfigManager* instance = ConfigManager::instance();
		static const ResourcesManager::SoundPool& sound_pool =
			ResourcesManager::instance()->get_sound_pool();

		double interval = 0; // 攻击间隔
		double damage = 0;   // 攻击伤害

		// 根据塔类型设置攻击参数并播放音效
		switch (tower_type)
		{
		case Archer:
			interval = instance->archer_template.interval[instance->level_archer];
			damage = instance->archer_template.damage[instance->level_archer];
			// 随机选择两种箭矢音效之一
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

		// 设置并启动攻击冷却计时器
		timer_fire.set_wait_time(interval);
		timer_fire.restart();

		// 计算子弹方向（指向目标敌人）
		Vector2 direction = target_enemy->get_position() - position;
		// 通过子弹管理器发射子弹（标准化方向向量并乘以速度）
		BulletManager::instance()->fire_bullet(
			bullet_type,
			position,
			direction.normalize() * fire_speed * SIZE_TILE,
			damage
		);

		// 根据目标方向更新塔的朝向（优先水平方向）
		bool horizontal_dominant = abs(direction.x) >= abs(direction.y);
		if (horizontal_dominant)
			facing = (direction.x > 0) ? Facing::Right : Facing::Left;
		else
			facing = (direction.y > 0) ? Facing::Down : Facing::Up;

		// 切换到攻击动画并重置播放状态
		update_fire_animation();
		anim_current->reset();
	}
};

#endif // !_TOWER_H_