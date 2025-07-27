#ifndef _STATUS_BAR_H_
#define _STATUS_BAR_H_

#include "coin_manager.h"       // 金币管理系统
#include "home_manager.h"       // 家园（基地）管理系统
#include "resources_manager.h"  // 资源管理系统
#include "player_manager.h"     // 玩家状态管理系统

#include <SDL.h>                // SDL基础库
#include <string>               // 字符串处理
#include <SDL2_gfxPrimitives.h> // SDL2图形绘制扩展库

// 游戏状态栏类（显示玩家、基地和资源信息）
class StatusBar
{
public:
	StatusBar() = default;    // 默认构造函数
	~StatusBar() = default;   // 默认析构函数

	// 设置状态栏左上角位置
	void set_position(int x, int y)
	{
		position.x = x, position.y = y;  // 设置状态栏左上角坐标
	}

	// 更新状态栏状态（每帧调用）
	void on_update(SDL_Renderer* renderer)
	{
		// 获取主字体资源
		static TTF_Font* font = ResourcesManager::instance()
			->get_font_pool().find(ResID::Font_Main)->second;

		// 清理旧文本纹理
		SDL_DestroyTexture(tex_text_background);
		tex_text_background = nullptr;
		SDL_DestroyTexture(tex_text_foreground);
		tex_text_foreground = nullptr;

		// 获取当前金币数量并转换为字符串
		std::string str_val = std::to_string(
			(int)CoinManager::instance()->get_current_coin_num());

		// 创建文本表面（带阴影效果）
		SDL_Surface* suf_text_background = TTF_RenderText_Blended(
			font, str_val.c_str(), color_text_background); // 阴影文本（灰色）
		SDL_Surface* suf_text_foreground = TTF_RenderText_Blended(
			font, str_val.c_str(), color_text_foreground); // 前景文本（白色）

		// 保存文本尺寸
		width_text = suf_text_background->w;
		height_text = suf_text_background->h;

		// 创建纹理
		tex_text_background = SDL_CreateTextureFromSurface(renderer, suf_text_background);
		tex_text_foreground = SDL_CreateTextureFromSurface(renderer, suf_text_foreground);

		// 释放临时表面
		SDL_FreeSurface(suf_text_background);
		SDL_FreeSurface(suf_text_foreground);
	}

	// 渲染状态栏（每帧调用）
	void on_render(SDL_Renderer* renderer)
	{
		static SDL_Rect rect_dst;  // 可重用的目标矩形
		// 获取纹理资源
		static const auto& tex_pool = ResourcesManager::instance()->get_texture_pool();
		static SDL_Texture* tex_coin = tex_pool.find(ResID::Tex_UICoin)->second;           // 金币图标
		static SDL_Texture* tex_heart = tex_pool.find(ResID::Tex_UIHeart)->second;         // 心形图标（代表生命值）
		static SDL_Texture* tex_home_avatar = tex_pool.find(ResID::Tex_UIHomeAvatar)->second; // 基地头像
		static SDL_Texture* tex_player_avatar = tex_pool.find(ResID::Tex_UIPlayerAvatar)->second; // 玩家头像

		// 1. 渲染基地头像（左上角）
		rect_dst.x = position.x, rect_dst.y = position.y;
		rect_dst.w = 78, rect_dst.h = 78;
		SDL_RenderCopy(renderer, tex_home_avatar, nullptr, &rect_dst);

		// 2. 渲染基地生命值（心形图标）
		int heart_count = (int)HomeManager::instance()->get_current_hp_num();  // 获取当前生命值数量
		for (int i = 0; i < heart_count; i++)
		{
			// 水平排列心形图标
			rect_dst.x = position.x + 78 + 15 + i * (32 + 2);  // 间距计算
			rect_dst.y = position.y;  // 垂直位置
			rect_dst.w = 32, rect_dst.h = 32;  // 图标尺寸
			SDL_RenderCopy(renderer, tex_heart, nullptr, &rect_dst);
		}

		// 3. 渲染金币图标（基地头像右侧）
		rect_dst.x = position.x + 78 + 15;  // 水平位置
		rect_dst.y = position.y + 78 - 32;  // 垂直位置（底部对齐）
		rect_dst.w = 32, rect_dst.h = 32;
		SDL_RenderCopy(renderer, tex_coin, nullptr, &rect_dst);

		// 4. 渲染金币数量（带阴影效果）
		// 先渲染阴影
		rect_dst.x += 32 + 10 + offset_shadow.x;  // 位置调整（金币图标右侧）
		rect_dst.y = rect_dst.y + (32 - height_text) / 2 + offset_shadow.y;  // 垂直居中
		rect_dst.w = width_text, rect_dst.h = height_text;
		SDL_RenderCopy(renderer, tex_text_background, nullptr, &rect_dst);

		// 再渲染前景文本
		rect_dst.x -= offset_shadow.x;  // 调整回原始位置
		rect_dst.y -= offset_shadow.y;
		SDL_RenderCopy(renderer, tex_text_foreground, nullptr, &rect_dst);

		// 5. 渲染玩家头像（基地头像下方）
		rect_dst.x = position.x + (78 - 65) / 2;  // 水平居中
		rect_dst.y = position.y + 78 + 5;         // 垂直位置（基地头像下方）
		rect_dst.w = 65, rect_dst.h = 65;
		SDL_RenderCopy(renderer, tex_player_avatar, nullptr, &rect_dst);

		// 6. 渲染玩家魔法值进度条
		// 背景框（圆角矩形）
		rect_dst.x = position.x + 78 + 15;  // 水平位置
		rect_dst.y += 10;  // 垂直位置（玩家头像下方）
		roundedBoxRGBA(renderer,
			rect_dst.x, rect_dst.y,
			rect_dst.x + width_mp_bar, rect_dst.y + height_mp_bar,
			4,  // 圆角半径
			color_mp_bar_background.r, color_mp_bar_background.g,
			color_mp_bar_background.b, color_mp_bar_background.a);

		// 前景进度（根据当前魔法值比例）
		rect_dst.x += width_border_mp_bar;  // 内边距调整
		rect_dst.y += width_border_mp_bar;
		rect_dst.w = width_mp_bar - 2 * width_border_mp_bar;  // 内部宽度
		rect_dst.h = height_mp_bar - 2 * width_border_mp_bar; // 内部高度

		double process = PlayerManager::instance()->get_current_mp() / 100;  // 魔法值比例
		roundedBoxRGBA(renderer,
			rect_dst.x, rect_dst.y,
			rect_dst.x + (int)(rect_dst.w * process), rect_dst.y + rect_dst.h,
			2,  // 圆角半径
			color_mp_bar_foredground.r, color_mp_bar_foredground.g,
			color_mp_bar_foredground.b, color_mp_bar_foredground.a);
	}

private:
	// 常量配置
	const int size_heart = 32;                // 心形图标尺寸
	const int width_mp_bar = 200;             // 魔法条总宽度
	const int height_mp_bar = 20;             // 魔法条总高度
	const int width_border_mp_bar = 4;        // 魔法条边框宽度
	const SDL_Point offset_shadow = { 2, 2 }; // 文本阴影偏移量

	// 颜色配置
	const SDL_Color color_text_background = { 175, 175, 175, 255 }; // 文本阴影颜色（灰色）
	const SDL_Color color_text_foreground = { 255, 255, 255, 255 }; // 文本前景颜色（白色）
	const SDL_Color color_mp_bar_background = { 48, 40, 51, 255 };  // 魔法条背景颜色（深紫色）
	const SDL_Color color_mp_bar_foredground = { 144, 121, 173, 255 }; // 魔法条前景颜色（浅紫色）

private:
	SDL_Point position = { 0 };          // 状态栏左上角位置
	int width_text = 0, height_text = 0; // 文本尺寸
	SDL_Texture* tex_text_background = nullptr; // 文本阴影纹理
	SDL_Texture* tex_text_foreground = nullptr; // 文本前景纹理
};

#endif // !_STATUS_BAR_H_
