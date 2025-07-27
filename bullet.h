#ifndef _BULLET_H_
#define _BULLET_H_

#include "vector2.h"
#include "enemy.h"
#include "animation.h"
#include "config_manager.h"

/**
 * @class Bullet
 * @brief 表示游戏中的子弹对象，负责处理子弹的移动、碰撞、渲染和状态管理
 */
class Bullet
{
public:
	Bullet() = default;      // 默认构造函数
	~Bullet() = default;     // 默认析构函数

	/**
	 * @brief 设置子弹速度并自动计算旋转角度（如果支持旋转）
	 * @param velocity 二维速度向量
	 */
	void set_velocity(const Vector2& velocity)
	{
		this->velocity = velocity;

		// 如果子弹支持旋转，根据速度方向计算动画旋转角度（弧度转角度）
		if (can_rotated)
		{
			double randian = std::atan2(velocity.y, velocity.x);
			angle_anim_rotated = randian * 180 / 3.14159265;
		}
	}

	/// 设置子弹位置
	void set_position(const Vector2& position) { this->position = position; }

	/// 设置子弹伤害值
	void set_damage(double damage) { this->damage = damage; }

	/// 获取子弹尺寸
	const Vector2& get_size() const { return size; }

	/// 获取子弹当前位置
	const Vector2& get_position() const { return position; }

	/// 获取子弹伤害值
	double get_damage() const { return damage; }

	/// 获取子弹伤害范围（负值表示无范围伤害）
	double get_damage_range() const { return damage_range; }

	/// 禁用子弹碰撞检测
	void disable_collide() { is_collisional = false; }

	/// 检查子弹是否可碰撞
	bool can_collide() const { return is_collisional; }

	/// 标记子弹为无效（准备移除）
	void make_invalid()
	{
		is_valid = false;
		is_collisional = false;  // 同时禁用碰撞
	}

	/// 检查子弹是否可被移除
	bool can_remove() const { return !is_valid; }

	/**
	 * @brief 每帧更新子弹状态
	 * @param delta 帧时间增量（秒）
	 */
	virtual void on_update(double delta)
	{
		animation.on_update(delta);         // 更新动画状态
		position += velocity * delta;       // 根据速度更新位置

		// 获取地图边界矩形（通过配置管理器）
		static const SDL_Rect& rect_map = ConfigManager::instance()->rect_tile_map;

		// 检测子弹是否超出地图边界（考虑子弹尺寸）
		if (position.x - size.x / 2 <= rect_map.x ||
			position.x + size.x / 2 >= rect_map.x + rect_map.w ||
			position.y - size.y / 2 <= rect_map.y ||
			position.y + size.y / 2 >= rect_map.y + rect_map.h)
		{
			is_valid = false;  // 超出边界则标记为无效
		}
	}

	/**
	 * @brief 渲染子弹
	 * @param renderer SDL渲染器指针
	 */
	virtual void on_render(SDL_Renderer* renderer)
	{
		static SDL_Point point;  // 复用点对象避免重复创建

		// 计算渲染位置（中心点转左上角坐标）
		point.x = (int)(position.x - size.x / 2);
		point.y = (int)(position.y - size.y / 2);

		// 调用动画渲染（带旋转角度）
		animation.on_render(renderer, point, angle_anim_rotated);
	}

	/**
	 * @brief 碰撞敌人时的处理（虚函数，可被子类重写）
	 * @param enemy 碰撞到的敌人指针
	 */
	virtual void on_collide(Enemy* enemy)
	{
		// 默认行为：碰撞后子弹失效
		is_valid = false;
		is_collisional = false;
	}

protected:
	Vector2 size;         ///< 子弹的尺寸（宽高）
	Vector2 velocity;     ///< 子弹的移动速度向量
	Vector2 position;     ///< 子弹的中心位置坐标

	Animation animation;  ///< 子弹的动画控制器
	bool can_rotated = false; ///< 标识子弹是否根据速度方向旋转

	double damage = 0;    ///< 子弹的基础伤害值
	double damage_range = -1; ///< 伤害作用范围（负值表示无范围伤害）

private:
	bool is_valid = true;       ///< 子弹是否有效（false时将被移除）
	bool is_collisional = true; ///< 是否参与碰撞检测
	double angle_anim_rotated = 0; ///< 动画旋转角度（度）
};

#endif // !_BULLET_H_
