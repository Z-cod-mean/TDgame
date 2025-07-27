#ifndef _PLACE_PANEL_H_
#define _PLACE_PANEL_H_

#include "panel.h"               // 基础面板类
#include "coin_manager.h"        // 金币管理系统
#include "tower_manager.h"       // 防御塔管理系统
#include "resources_manager.h"   // 资源管理系统

#include <SDL2_gfxPrimitives.h>  // SDL2图形绘制扩展库

// 放置防御塔的操作面板类（继承自Panel）
class PlacePanel : public Panel
{
public:
	PlacePanel()
	{
		// 从资源管理器中获取面板所需的纹理资源
		const ResourcesManager::TexturePool& texture_pool
			= ResourcesManager::instance()->get_texture_pool();

		// 设置不同状态下的面板纹理
		tex_idle = texture_pool.find(ResID::Tex_UIPlaceIdle)->second;           // 默认状态纹理
		tex_hovered_top = texture_pool.find(ResID::Tex_UIPlaceHoveredTop)->second;   // 顶部悬停纹理
		tex_hovered_left = texture_pool.find(ResID::Tex_UIPlaceHoveredLeft)->second;  // 左侧悬停纹理
		tex_hovered_right = texture_pool.find(ResID::Tex_UIPlaceHoveredRight)->second;// 右侧悬停纹理
	}

	~PlacePanel() = default;  // 默认析构函数

	// 更新面板状态（每帧调用）
	void on_update(SDL_Renderer* renderer) override
	{
		static TowerManager* instance = TowerManager::instance();

		// 获取三种防御塔的建造费用
		val_top = (int)instance->get_place_cost(TowerType::Axeman);   // 斧兵塔费用
		val_left = (int)instance->get_place_cost(TowerType::Archer);  // 弓箭手塔费用
		val_right = (int)instance->get_place_cost(TowerType::Gunner); // 枪手塔费用

		// 计算三种防御塔的攻击范围（转换为像素）
		reg_top = (int)instance->get_damage_range(TowerType::Axeman) * SIZE_TILE;   // 斧兵塔范围
		reg_left = (int)instance->get_damage_range(TowerType::Archer) * SIZE_TILE;  // 弓箭手塔范围
		reg_right = (int)instance->get_damage_range(TowerType::Gunner) * SIZE_TILE; // 枪手塔范围

		// 调用基类更新方法（处理文本渲染）
		Panel::on_update(renderer);
	}

	// 渲染面板（每帧调用）
	void on_render(SDL_Renderer* renderer) override
	{
		if (!visible) return;  // 面板不可见时跳过渲染

		// 根据悬停状态获取对应防御塔的攻击范围
		int reg = 0;
		switch (hovered_target)
		{
		case Panel::HoveredTarget::Top:
			reg = reg_top;    // 顶部区域悬停（斧兵塔）
			break;
		case Panel::HoveredTarget::Left:
			reg = reg_left;   // 左侧区域悬停（弓箭手塔）
			break;
		case Panel::HoveredTarget::Right:
			reg = reg_right;  // 右侧区域悬停（枪手塔）
			break;
		}

		// 绘制防御塔攻击范围指示器
		if (reg > 0)
		{
			// 绘制半透明填充圆（攻击区域）
			filledCircleRGBA(renderer, center_pos.x, center_pos.y, reg,
				color_region_content.r, color_region_content.g, color_region_content.b, color_region_content.a);

			// 绘制圆形边框（范围边界）
			aacircleRGBA(renderer, center_pos.x, center_pos.y, reg,
				color_region_frame.r, color_region_frame.g, color_region_frame.b, color_region_frame.a);
		}

		// 调用基类渲染方法（绘制面板主体和文本）
		Panel::on_render(renderer);
	}

protected:
	// 顶部区域点击事件（放置斧兵塔）
	void on_click_top_area() override
	{
		CoinManager* instance = CoinManager::instance();

		// 检查金币是否足够
		if (val_top <= instance->get_current_coin_num())
		{
			// 放置斧兵塔
			TowerManager::instance()->place_tower(TowerType::Axeman, idx_tile_selected);
			// 扣除相应金币
			instance->decrease_coin(val_top);
		}
	}

	// 左侧区域点击事件（放置弓箭手塔）
	void on_click_left_area() override
	{
		CoinManager* instance = CoinManager::instance();

		// 检查金币是否足够
		if (val_left <= instance->get_current_coin_num())
		{
			// 放置弓箭手塔
			TowerManager::instance()->place_tower(TowerType::Archer, idx_tile_selected);
			// 扣除相应金币
			instance->decrease_coin(val_left);
		}
	}

	// 右侧区域点击事件（放置枪手塔）
	void on_click_right_area() override
	{
		CoinManager* instance = CoinManager::instance();

		// 检查金币是否足够
		if (val_right <= instance->get_current_coin_num())
		{
			// 放置枪手塔
			TowerManager::instance()->place_tower(TowerType::Gunner, idx_tile_selected);
			// 扣除相应金币
			instance->decrease_coin(val_right);
		}
	}

private:
	// 范围指示器颜色配置
	const SDL_Color color_region_frame = { 30, 80, 162, 175 };    // 蓝色边框（RGBA:30,80,162,175）
	const SDL_Color color_region_content = { 0, 149, 217, 75 };   // 浅蓝填充（RGBA:0,149,217,75）

private:
	// 三种防御塔的攻击范围（像素单位）
	int reg_top = 0;     // 斧兵塔攻击范围
	int reg_left = 0;    // 弓箭手塔攻击范围
	int reg_right = 0;   // 枪手塔攻击范围
};

#endif // !_PLACE_PANEL_H_
