#ifndef _UPGRADE_PANEL_H_
#define _UPGRADE_PANEL_H_

#include "panel.h"               // 基础面板类
#include "tower_manager.h"       // 防御塔管理系统
#include "resources_manager.h"   // 资源管理系统

// 防御塔升级操作面板类（继承自Panel）
class UpgradePanel : public Panel
{
public:
	UpgradePanel()
	{
		// 从资源管理器中获取升级面板所需的纹理资源
		const ResourcesManager::TexturePool& texture_pool
			= ResourcesManager::instance()->get_texture_pool();

		// 设置不同状态下的升级面板纹理
		tex_idle = texture_pool.find(ResID::Tex_UIUpgradeIdle)->second;           // 默认状态纹理
		tex_hovered_top = texture_pool.find(ResID::Tex_UIUpgradeHoveredTop)->second;   // 顶部悬停纹理
		tex_hovered_left = texture_pool.find(ResID::Tex_UIUpgradeHoveredLeft)->second;  // 左侧悬停纹理
		tex_hovered_right = texture_pool.find(ResID::Tex_UIUpgradeHoveredRight)->second;// 右侧悬停纹理
	}

	~UpgradePanel() = default;  // 默认析构函数

	// 更新面板状态（每帧调用）
	void on_update(SDL_Renderer* renderer)
	{
		static TowerManager* instance = TowerManager::instance();

		// 获取三种防御塔的升级费用
		val_top = (int)instance->get_upgrade_cost(TowerType::Axeman);   // 斧兵塔升级费用
		val_left = (int)instance->get_upgrade_cost(TowerType::Archer);  // 弓箭手塔升级费用
		val_right = (int)instance->get_upgrade_cost(TowerType::Gunner); // 枪手塔升级费用

		// 调用基类更新方法（处理文本渲染）
		Panel::on_update(renderer);
	}

protected:
	// 顶部区域点击事件（升级斧兵塔）
	void on_click_top_area() override
	{
		CoinManager* instance = CoinManager::instance();

		// 检查金币是否足够且升级费用有效
		if (val_top > 0 && val_top <= instance->get_current_coin_num())
		{
			// 升级斧兵塔
			TowerManager::instance()->upgrade_tower(TowerType::Axeman);
			// 扣除升级费用
			instance->decrease_coin(val_top);
		}
	}

	// 左侧区域点击事件（升级弓箭手塔）
	void on_click_left_area() override
	{
		CoinManager* instance = CoinManager::instance();

		// 检查金币是否足够且升级费用有效
		if (val_left > 0 && val_left <= instance->get_current_coin_num())
		{
			// 升级弓箭手塔
			TowerManager::instance()->upgrade_tower(TowerType::Axeman);
			// 扣除升级费用
			instance->decrease_coin(val_left);
		}
	}

	// 右侧区域点击事件（升级枪手塔）
	void on_click_right_area() override
	{
		CoinManager* instance = CoinManager::instance();

		// 检查金币是否足够且升级费用有效
		if (val_right > 0 && val_right <= instance->get_current_coin_num())
		{
			// 升级枪手塔
			TowerManager::instance()->upgrade_tower(TowerType::Axeman);
			// 扣除升级费用
			instance->decrease_coin(val_right);
		}
	}

};

#endif // !_UPGRADE_PANEL_H_
