#ifndef _ARCHER_TOWER_H_
#define _ARCHER_TOWER_H_

#include "tower.h"           // 基础塔类
#include "resources_manager.h" // 资源管理器

/**
 * @class ArcherTower
 * @brief 弓箭手塔类，继承自基础塔类
 *
 * 该类实现弓箭手塔的特定属性和动画配置。弓箭手塔是游戏中的远程攻击单位，
 * 发射箭矢攻击敌人，具有中等射程和攻击速度。
 */
class ArcherTower : public Tower
{
public:
    /**
     * @brief 构造函数：初始化弓箭手塔的属性和动画
     *
     * 1. 加载弓箭手纹理资源
     * 2. 配置各方向动画帧序列
     * 3. 设置塔属性和子弹属性
     */
    ArcherTower()
    {
        // 获取资源管理器中的弓箭手纹理（静态变量避免重复加载）
        static SDL_Texture* tex_archer = ResourcesManager::instance()
            ->get_texture_pool().find(ResID::Tex_Archer)->second;

        // 定义各方向动画的帧索引序列
        // 精灵表布局：3列8行（共24帧）
        static const std::vector<int> idx_list_idle_up = { 3, 4 };     // 朝上闲置动画（2帧）
        static const std::vector<int> idx_list_idle_down = { 0, 1 };   // 朝下闲置动画（2帧）
        static const std::vector<int> idx_list_idle_left = { 6, 7 };   // 朝左闲置动画（2帧）
        static const std::vector<int> idx_list_idle_right = { 9, 10 }; // 朝右闲置动画（2帧）

        static const std::vector<int> idx_list_fire_up = { 15, 16, 17 };   // 朝上攻击动画（3帧）
        static const std::vector<int> idx_list_fire_down = { 12, 13, 14 }; // 朝下攻击动画（3帧）
        static const std::vector<int> idx_list_fire_left = { 18, 19, 20 }; // 朝左攻击动画（3帧）
        static const std::vector<int> idx_list_fire_right = { 21, 22, 23 };// 朝右攻击动画（3帧）

        // ===================== 设置闲置动画 =====================
        anim_idle_up.set_frame_data(tex_archer, 3, 8, idx_list_idle_up);     // 纹理, 3列, 8行, 帧序列
        anim_idle_down.set_frame_data(tex_archer, 3, 8, idx_list_idle_down);
        anim_idle_left.set_frame_data(tex_archer, 3, 8, idx_list_idle_left);
        anim_idle_right.set_frame_data(tex_archer, 3, 8, idx_list_idle_right);

        // ===================== 设置攻击动画 =====================
        anim_fire_up.set_frame_data(tex_archer, 3, 8, idx_list_fire_up);
        anim_fire_down.set_frame_data(tex_archer, 3, 8, idx_list_fire_down);
        anim_fire_left.set_frame_data(tex_archer, 3, 8, idx_list_fire_left);
        anim_fire_right.set_frame_data(tex_archer, 3, 8, idx_list_fire_right);

        // 设置塔的尺寸（像素）
        size.x = 48, size.y = 48;

        // 设置塔类型为弓箭手
        tower_type = TowerType::Archer;

        // 设置子弹属性（速度和类型）
        fire_speed = 6;              // 子弹飞行速度
        bullet_type = BulletType::Arrow; // 使用箭矢子弹类型
    }

    /// 默认析构函数
    ~ArcherTower() = default;
};

#endif // !_ARCHER_TOWER_H_