#ifndef _COIN_PROP_H_
#define _COIN_PROP_H_

#include "tile.h"              // 瓦片类（位置信息）
#include "timer.h"             // 计时器类
#include "vector2.h"           // 二维向量类
#include "resources_manager.h" // 资源管理器

#include <SDL.h>               // SDL基础库

// 金币道具类（游戏中的可收集金币）
class CoinProp
{
public:
	CoinProp()
	{
		// 跳跃计时器配置（一次性计时器）
		timer_jump.set_one_shot(true);  // 设置为单次触发
		timer_jump.set_wait_time(interval_jump);  // 设置跳跃持续时间
		timer_jump.set_on_timeout(       // 设置超时回调
			[&]() { is_jumping = false; });  // 跳跃结束时标记为false

		// 消失计时器配置（一次性计时器）
		timer_disappear.set_one_shot(true);  // 设置为单次触发
		timer_disappear.set_wait_time(interval_disappear);  // 设置消失时间
		timer_disappear.set_on_timeout(       // 设置超时回调
			[&]() { is_valid = false; });  // 超时后标记为无效

		// 初始化随机水平速度和向上速度
		velocity.x = (rand() % 2 ? 1 : -1) * 2 * SIZE_TILE;  // 随机左右方向
		velocity.y = -3 * SIZE_TILE;  // 向上跳跃的初始速度
	}

	~CoinProp() = default;  // 默认析构函数

	// 设置金币位置
	void set_position(const Vector2& position)
	{
		this->position = position;
	}

	// 获取金币当前位置
	const Vector2& get_position() const
	{
		return position;
	}

	// 获取金币尺寸
	const Vector2& get_size() const
	{
		return size;
	}

	// 标记金币为无效（被收集后调用）
	void make_invalid()
	{
		is_valid = false;
	}

	// 检查金币是否可被移除（是否无效）
	bool can_remove()
	{
		return !is_valid;
	}

	// 每帧更新金币状态
	void on_update(double delta)
	{
		// 更新两个计时器
		timer_jump.on_update(delta);
		timer_disappear.on_update(delta);

		// 跳跃状态下的物理模拟
		if (is_jumping)
		{
			velocity.y += gravity * delta;  // 应用重力加速度
		}
		// 跳跃结束后的漂浮效果
		else
		{
			velocity.x = 0;  // 停止水平移动
			velocity.y = sin(SDL_GetTicks64() / 1000.0 * 4) * 30;  // 垂直漂浮（正弦运动）
		}

		// 根据速度更新位置
		position += velocity * delta;
	}

	// 渲染金币
	void on_render(SDL_Renderer* renderer)
	{
		static SDL_Rect rect = { 0, 0, (int)size.x, (int)size.y };  // 渲染区域
		// 获取金币纹理
		static SDL_Texture* tex_coin = ResourcesManager::instance()
			->get_texture_pool().find(ResID::Tex_Coin)->second;

		// 计算渲染位置（居中）
		rect.x = (int)(position.x - size.x / 2);
		rect.y = (int)(position.y - size.y / 2);

		// 执行渲染
		SDL_RenderCopy(renderer, tex_coin, nullptr, &rect);
	}

private:
	Vector2 position;      // 当前位置
	Vector2 velocity;      // 当前速度

	Timer timer_jump;      // 跳跃状态计时器
	Timer timer_disappear; // 消失计时器

	bool is_valid = true;  // 是否有效（未被收集）
	bool is_jumping = true; // 是否处于跳跃状态

	// 物理参数
	double gravity = 490;  // 重力加速度（像素/秒?）
	double interval_jump = 0.75;  // 跳跃持续时间（秒）
	Vector2 size = { 16, 16 };  // 金币尺寸（像素）
	double interval_disappear = 10;  // 消失前等待时间（秒）

};

#endif // !_COIN_PROP_H_
