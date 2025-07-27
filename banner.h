#ifndef _BANNER_H_
#define _BANNER_H_

#include "timer.h"              // 计时器工具类
#include "vector2.h"            // 二维向量工具类
#include "config_manager.h"     // 游戏配置管理器
#include "resources_manager.h"  // 资源管理器

#include <SDL.h>                // SDL基础库

// 游戏结束横幅类（显示胜利/失败信息）
class Banner
{
public:
	// 构造函数：初始化横幅尺寸和计时器
	Banner()
	{
		// 设置前景文字和背景条带的尺寸
		size_foreground = { 646, 215 };  // 前景文字纹理尺寸
		size_background = { 1282, 209 }; // 背景条带纹理尺寸

		// 配置显示计时器（5秒后自动隐藏）
		timer_display.set_one_shot(true);      // 设置为一次性计时器
		timer_display.set_wait_time(5);        // 设置等待时间为5秒
		timer_display.set_on_timeout(           // 设置超时回调函数
			[&]() { is_end_display = true; }); // 超时时将结束标志设为true
	}

	// 默认析构函数
	~Banner() = default;

	// 设置横幅在屏幕上的中心位置
	void set_center_position(const Vector2& pos)
	{
		pos_center = pos;  // 保存中心位置坐标
	}

	// 每帧更新横幅状态
	void on_update(double delta)
	{
		timer_display.on_update(delta);  // 更新计时器状态

		// 从资源管理器获取纹理池
		const ResourcesManager::TexturePool& tex_pool
			= ResourcesManager::instance()->get_texture_pool();

		// 获取配置管理器实例
		const ConfigManager* instance = ConfigManager::instance();

		// 根据游戏胜利状态选择前景纹理
		tex_foreground = tex_pool.find(
			instance->is_game_win ? ResID::Tex_UIWinText : ResID::Tex_UILossText
		)->second;

		// 获取背景条带纹理
		tex_background = tex_pool.find(ResID::Tex_UIGameOverBar)->second;
	}

	// 渲染横幅到屏幕
	void on_render(SDL_Renderer* renderer)
	{
		static SDL_Rect rect_dst;  // 目标渲染区域（复用提高效率）

		// 渲染背景条带（居中显示）
		rect_dst.x = (int)(pos_center.x - size_background.x / 2);  // 水平居中计算
		rect_dst.y = (int)(pos_center.y - size_background.y / 2);  // 垂直居中计算
		rect_dst.w = (int)size_background.x;  // 设置宽度
		rect_dst.h = (int)size_background.y;  // 设置高度
		SDL_RenderCopy(renderer, tex_background, nullptr, &rect_dst);  // 执行渲染

		// 渲染前景文字（居中显示在背景上）
		rect_dst.x = (int)(pos_center.x - size_foreground.x / 2);  // 水平居中计算
		rect_dst.y = (int)(pos_center.y - size_foreground.y / 2);  // 垂直居中计算
		rect_dst.w = (int)size_foreground.x;  // 设置宽度
		rect_dst.h = (int)size_foreground.y;  // 设置高度
		SDL_RenderCopy(renderer, tex_foreground, nullptr, &rect_dst);  // 执行渲染
	}

	// 检查横幅显示是否结束（注意：函数名有拼写错误，但保持原样）
	bool check_end_dispaly()
	{
		return is_end_display;  // 返回显示结束标志
	}

private:
	Vector2 pos_center;          // 横幅中心位置（屏幕坐标）

	Vector2 size_foreground;     // 前景文字尺寸
	Vector2 size_background;     // 背景条带尺寸

	SDL_Texture* tex_foreground = nullptr;  // 前景文字纹理（胜利/失败）
	SDL_Texture* tex_background = nullptr;  // 背景条带纹理

	Timer timer_display;         // 显示时长计时器
	bool is_end_display = false; // 显示结束标志（true表示应结束显示）

};

#endif // !_BANNER_H_
