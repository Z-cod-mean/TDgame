#ifndef _AXEMAX_H_       // 防止头文件重复包含
#define _AXEMAX_H_

#include "tower.h"              // 基础塔类定义
#include "resources_manager.h"  // 资源管理类

class AxemanTower : public Tower  // 继承自基础塔类
{
public:
    AxemanTower()  // 构造函数
    {
        // 加载斧兵纹理（静态资源只初始化一次）
        static SDL_Texture* tex_axeman = ResourcesManager::instance()
            ->get_texture_pool().find(ResID::Tex_Axeman)->second;

        // 定义各方向动画帧索引（静态常量）
        static const std::vector<int> idx_list_idle_up = { 3, 4 };     // 上待机帧
        static const std::vector<int> idx_list_idle_down = { 0, 1 };   // 下待机帧
        static const std::vector<int> idx_list_idle_left = { 9, 10 };  // 左待机帧
        static const std::vector<int> idx_list_idle_right = { 6, 7 };   // 右待机帧
        static const std::vector<int> idx_list_fire_up = { 15, 16, 17 };    // 上攻击帧
        static const std::vector<int> idx_list_fire_down = { 12, 13, 14 };  // 下攻击帧
        static const std::vector<int> idx_list_fire_left = { 21, 22, 23 };  // 左攻击帧
        static const std::vector<int> idx_list_fire_right = { 18, 19, 20 }; // 右攻击帧

        // 配置各方向动画
        anim_idle_up.set_frame_data(tex_axeman, 3, 8, idx_list_idle_up);     // 上待机动画
        anim_idle_down.set_frame_data(tex_axeman, 3, 8, idx_list_idle_down);  // 下待机动画
        anim_idle_left.set_frame_data(tex_axeman, 3, 8, idx_list_idle_left); // 左待机动画
        anim_idle_right.set_frame_data(tex_axeman, 3, 8, idx_list_idle_right);// 右待机动画
        anim_fire_up.set_frame_data(tex_axeman, 3, 8, idx_list_fire_up);      // 上攻击动画
        anim_fire_down.set_frame_data(tex_axeman, 3, 8, idx_list_fire_down);  // 下攻击动画
        anim_fire_left.set_frame_data(tex_axeman, 3, 8, idx_list_fire_left);  // 左攻击动画
        anim_fire_right.set_frame_data(tex_axeman, 3, 8, idx_list_fire_right);// 右攻击动画

        // 设置塔尺寸（像素）
        size.x = 48;
        size.y = 48;

        // 设置塔类型标识
        tower_type = TowerType::Axeman;

        // 配置攻击属性
        fire_speed = 5;                 // 攻击速度（值越大越快）
        bullet_type = BulletType::Axe;   // 发射斧头类型子弹
    }

    ~AxemanTower() = default;  // 使用默认析构函数
};

#endif // !_AXEMAX_H_