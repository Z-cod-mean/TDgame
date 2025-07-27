#ifndef _ENEMY_H_ // 防止头文件重复包含
#define _ENEMY_H_ // 定义头文件标识符

// 引入定时器类，处理技能/状态的冷却时间
#include "timer.h"
// 引入路径类，管理敌人移动路径
#include "route.h"
// 引入二维向量类，用于位置、方向、速度等计算
#include "vector2.h"
// 引入动画类，用于播放敌人行走/受击动画
#include "animation.h"
// 引入配置管理类，用于获取地图格子尺寸等参数
#include "config_manager.h"

#include <functional> // 引入函数封装器std::function，用于回调机制

// 敌人类，包含敌人移动、动画、状态等逻辑
class Enemy
{
public:
	//定义一个技能callback用于应对不同的技能效果
	typedef std::function<void(Enemy* enemy)> SkillCallback; // 定义技能回调类型，接受一个Enemy指针参数

public:
	// 构造函数，初始化各种计时器
	Enemy()
	{
		//技能每隔多长时间触发一次
		timer_skill.set_one_shot(false); // 设置为循环触发
		//当计时器超时时，执行绑定的技能释放回调函数
		timer_skill.set_on_timeout([&]() { on_skill_released(this); });

		// 设置受击闪白的计时器只触发一次
		timer_sketch.set_one_shot(true);
		// 设置闪白的持续时间为0.075秒
		timer_sketch.set_wait_time(0.075);
		// 闪白结束后，关闭受击状态
		timer_sketch.set_on_timeout([&]() { is_show_sketch = false; });

		// 设置速度恢复的计时器只触发一次
		timer_restore_speed.set_one_shot(true);
		// 速度恢复回调：恢复为最大速度
		timer_restore_speed.set_on_timeout([&]() { speed = max_speed; });
	}

	// 析构函数，使用默认行为
	~Enemy() = default;

	// 每帧更新函数，delta为当前帧时间（秒）
	void on_update(double delta)
	{
		// 更新技能、受击、恢复速度等计时器状态
		timer_skill.on_update(delta);
		timer_sketch.on_update(delta);
		timer_restore_speed.on_update(delta);

		// 计算本帧的移动距离（速度 * 时间）
		Vector2 move_distance = velocity * delta;
		// 当前目标点与当前位置之间的向量（方向和距离）
		Vector2 target_distance = position_target - position;
		// 如果本帧能移动的距离小于目标距离，就走本帧能走的；否则直接到目标点
		position += move_distance < target_distance ? move_distance : target_distance;

		// 如果已经接近目标点（长度近似为0）
		if (target_distance.approx_zero())
		{
			// 切换到下一个路径点
			idx_target++;
			refresh_position_target();
			// 重新计算方向向量，并归一化
			direction = (position_target - position).normalize();
		}

		// 根据方向计算速度向量（乘以速度和格子大小）
		velocity.x = direction.x * speed * SIZE_TILE;
		velocity.y = direction.y * speed * SIZE_TILE;

		// 判断是横向移动优先还是纵向
		bool is_show_x_amin = abs(velocity.x) >= abs(velocity.y);

		// 根据受击状态选择对应动画（闪白或正常）
		if (is_show_sketch) // 如果正在闪白（受到攻击）
		{
			if (is_show_x_amin) // 如果是横向移动更明显
				// 根据横向速度方向判断使用向右闪白还是向左闪白动画
				anim_current = velocity.x > 0 ? &anim_right_sketch : &anim_left_sketch;
			else // 否则是纵向移动更明显
				// 根据纵向速度方向判断使用向下闪白还是向上闪白动画
				anim_current = velocity.y > 0 ? &anim_down_sketch : &anim_up_sketch;
		}
		else // 如果没有闪白状态，使用普通动画
		{
			if (is_show_x_amin) // 如果是横向移动更明显
				// 使用向右或向左的普通动画
				anim_current = velocity.x > 0 ? &anim_right : &anim_left;
			else // 否则是纵向移动更明显
				// 使用向下或向上的普通动画
				anim_current = velocity.y > 0 ? &anim_down : &anim_up;
		}


		// 更新当前动画帧
		anim_current->on_update(delta);
	}

	// 渲染函数，负责绘制敌人动画和血条
	void on_render(SDL_Renderer* renderer)
	{
		static SDL_Rect rect; // 用于绘制血条
		static SDL_Point point; // 用于绘制动画位置
		static const int offset_y = 2; // 血条与敌人间距
		static const Vector2 size_hp_bar = { 40, 8 }; // 血条尺寸
		static const SDL_Color color_border = { 116, 185, 124, 255 }; // 血条边框颜色
		static const SDL_Color color_content = { 226, 255, 194, 255 }; // 血条填充颜色

		// 计算渲染位置（以敌人中心点为参考）
		point.x = (int)(position.x - size.x / 2);
		point.y = (int)(position.y - size.y / 2);

		// 渲染动画
		anim_current->on_render(renderer, point);

		// 如果血量不满，显示血条
		if (hp < max_hp)
		{
			rect.x = (int)(position.x - size_hp_bar.x / 2);
			rect.y = (int)(position.y - size.y / 2 - size_hp_bar.y - offset_y);
			rect.w = (int)(size_hp_bar.x * (hp / max_hp)); // 当前血量比例
			rect.h = (int)size_hp_bar.y;
			SDL_SetRenderDrawColor(renderer, color_content.r, color_content.g, color_content.b, color_content.a);
			SDL_RenderFillRect(renderer, &rect); // 画血条背景

			rect.w = (int)size_hp_bar.x;
			SDL_SetRenderDrawColor(renderer, color_border.r, color_border.g, color_border.b, color_border.a);
			SDL_RenderDrawRect(renderer, &rect); // 画血条边框
		}
	}

	// 设置技能释放回调函数
	void set_on_skill_released(SkillCallback on_skill_released)
	{
		this->on_skill_released = on_skill_released;
	}

	// 增加血量（不能超过最大值）
	void increase_hp(double val)
	{
		hp += val;
		if (hp > max_hp)
			hp = max_hp;
	}

	// 减少血量，若血量<=0，标记为无效并播放闪白
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

	// 减速1秒后恢复速度
	void slow_down()
	{
		speed = max_speed - 0.5;
		timer_restore_speed.set_wait_time(1);
		timer_restore_speed.restart();
	}

	// 设置当前位置
	void set_position(const Vector2& position)
	{
		this->position = position;
	}

	// 设置路径并初始化目标点
	void set_route(const Route* route)
	{
		this->route = route;
		refresh_position_target();
	}

	// 设置敌人为无效（被击杀或消失）
	void make_invalid()
	{
		is_valid = false;
	}

	// 获取当前血量
	double get_hp() const { return hp; }
	// 获取敌人尺寸
	const Vector2& get_size() const { return size; }
	// 获取当前位置
	const Vector2& get_position() const { return position; }
	// 获取当前速度
	const Vector2& get_velocity() const { return velocity; }
	// 获取攻击伤害
	double get_damage() const { return damage; }
	// 获取击杀奖励比例
	double get_reward_ratio() const { return reward_ratio; }
	// 获取回血范围（像素）
	double get_recover_radius() const { return SIZE_TILE * recover_range; }
	// 获取回血强度
	double get_recover_intensity() const { return recover_intensity; }
	// 是否已死亡/无效
	bool can_remove() const { return !is_valid; }
	// 获取当前在路径中的进度（0~1）
	double get_route_process() const
	{
		if (route->get_idx_list().size() == 1)
			return 1;
		return (double)idx_target / (route->get_idx_list().size() - 1);
	}

protected:
	Vector2 size; // 敌人尺寸（像素）
	Timer timer_skill; // 技能冷却计时器
	Animation anim_up, anim_down, anim_left, anim_right; // 普通移动动画
	Animation anim_up_sketch, anim_down_sketch, anim_left_sketch, anim_right_sketch; // 受击闪白动画
	double hp = 0, max_hp = 0; // 当前与最大血量
	double speed = 0, max_speed = 0; // 当前与最大速度
	double damage = 0; // 攻击伤害
	double reward_ratio = 0; // 击杀奖励比例
	double recover_interval = 0, recover_range = 0, recover_intensity = 0; // 回血间隔、范围、强度

private:
	Vector2 position; // 当前位置
	Vector2 velocity; // 当前速度向量
	Vector2 direction; // 当前移动方向
	bool is_valid = true; // 是否有效（是否存活）
	Timer timer_sketch; // 闪白计时器
	bool is_show_sketch = false; // 是否处于受击状态
	Animation* anim_current = nullptr; // 当前播放动画指针
	SkillCallback on_skill_released; // 技能释放回调函数
	Timer timer_restore_speed; // 速度恢复定时器
	const Route* route = nullptr; // 路径对象指针
	int idx_target = 0; // 当前目标格子索引
	Vector2 position_target; // 当前目标位置

	// 根据路径更新目标位置
	void refresh_position_target()
	{
		const Route::IdxList& idx_list = route->get_idx_list();
		if (idx_target < idx_list.size())
		{
			const SDL_Point& point = idx_list[idx_target];
			static const SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;
			position_target.x = rect_tile_map.x + point.x * SIZE_TILE + SIZE_TILE / 2; // 格子中心x
			position_target.y = rect_tile_map.y + point.y * SIZE_TILE + SIZE_TILE / 2; // 格子中心y
		}
	}
};

#endif // !_ENEMY_H_ // 结束防止重复包含
