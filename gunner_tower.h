#ifndef _GUNNER_TOWER_H_
#define _GUNNER_TOWER_H_

#include "tower.h"
#include "resources_manager.h"

/**
 * @class GunnerTower
 * @brief 枪手塔类，继承自基础塔类，实现枪手塔的特定属性和行为
 */
class GunnerTower : public Tower
{
public:
	/**
	 * @brief 构造函数：初始化枪手塔的纹理、动画帧和属性
	 *
	 * 从资源管理器获取枪手塔纹理，设置各方向动画帧索引，
	 * 配置塔的尺寸、类型、子弹速度和子弹类型
	 */
	GunnerTower()
	{
		// 从资源管理器获取枪手塔的纹理（静态变量避免重复获取）
		static SDL_Texture* tex_gunner = ResourcesManager::instance()
			->get_texture_pool().find(ResID::Tex_Gunner)->second;

		// 定义各方向动画的帧索引列表（精灵表中的位置索引）
		static const std::vector<int> idx_list_idle_up = { 4, 5 };     // 朝上闲置动画帧
		static const std::vector<int> idx_list_idle_down = { 0, 1 };   // 朝下闲置动画帧
		static const std::vector<int> idx_list_idle_left = { 12, 13 }; // 朝左闲置动画帧
		static const std::vector<int> idx_list_idle_right = { 8, 9 }; // 朝右闲置动画帧
		static const std::vector<int> idx_list_fire_up = { 20, 21, 22, 23 };    // 朝上攻击动画帧
		static const std::vector<int> idx_list_fire_down = { 16, 17, 18, 19 };  // 朝下攻击动画帧
		static const std::vector<int> idx_list_fire_left = { 28, 29, 30, 31 };  // 朝左攻击动画帧
		static const std::vector<int> idx_list_fire_right = { 24, 25, 26, 27 }; // 朝右攻击动画帧

		// 设置各方向动画帧数据（纹理、行列数、帧索引列表）
		anim_idle_up.set_frame_data(tex_gunner, 4, 8, idx_list_idle_up);     // 4列8行的精灵表
		anim_idle_down.set_frame_data(tex_gunner, 4, 8, idx_list_idle_down);
		anim_idle_left.set_frame_data(tex_gunner, 4, 8, idx_list_idle_left);
		anim_idle_right.set_frame_data(tex_gunner, 4, 8, idx_list_idle_right);
		anim_fire_up.set_frame_data(tex_gunner, 4, 8, idx_list_fire_up);
		anim_fire_down.set_frame_data(tex_gunner, 4, 8, idx_list_fire_down);
		anim_fire_left.set_frame_data(tex_gunner, 4, 8, idx_list_fire_left);
		anim_fire_right.set_frame_data(tex_gunner, 4, 8, idx_list_fire_right);

		// 设置塔的尺寸（像素）
		size.x = 48, size.y = 48;

		// 设置塔类型为枪手
		tower_type = TowerType::Gunner;

		// 设置子弹属性（速度和类型）
		fire_speed = 6;              // 子弹飞行速度
		bullet_type = BulletType::Shell; // 使用炮弹子弹类型
	}

	/// 默认析构函数
	~GunnerTower() = default;
};

#endif // !_GUNNER_TOWER_H_
