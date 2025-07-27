#ifndef _GOBLIN_ENEMY_H_
#define _GOBLIN_ENEMY_H_

#include "enemy.h"
#include "config_manager.h"
#include "resources_manager.h"

/**
 * @class GoblinEnemy
 * @brief 哥布林敌人类型，继承自基础敌人类
 *
 * 该类实现了哥布林敌人的特定属性、动画和行为。
 * 包含完整动画和草图动画（可能用于路径规划或预览模式）。
 */
class GoblinEnemy : public Enemy
{
public:
    /**
     * @brief 构造函数：初始化哥布林敌人的属性和动画
     *
     * 1. 加载纹理资源
     * 2. 配置动画帧序列
     * 3. 设置敌人属性（生命值、速度、伤害等）
     * 4. 初始化尺寸和状态
     */
    GoblinEnemy()
    {
        // 获取资源管理器实例
        static const ResourcesManager::TexturePool& texture_pool
            = ResourcesManager::instance()->get_texture_pool();

        // 加载哥布林纹理和草图纹理
        static SDL_Texture* tex_goblin = texture_pool.find(ResID::Tex_Goblin)->second;
        static SDL_Texture* tex_goblin_sketch = texture_pool.find(ResID::Tex_GoblinSketch)->second;

        // 获取配置管理器中的哥布林模板数据
        static ConfigManager::EnemyTemplate& goblin_template = ConfigManager::instance()->goblin_template;

        // 定义各方向动画的帧索引序列
        // 精灵表布局：5列4行（共20帧）
        static const std::vector<int> idx_list_up = { 5, 6, 7, 8, 9 };     // 朝上移动动画 (5帧)
        static const std::vector<int> idx_list_down = { 0, 1, 2, 3, 4 };   // 朝下移动动画 (5帧)
        static const std::vector<int> idx_list_left = { 15, 16, 17, 18, 19 }; // 朝左移动动画 (5帧)
        static const std::vector<int> idx_list_right = { 10, 11, 12, 13, 14 }; // 朝右移动动画 (5帧)

        // ===================== 设置完整哥布林动画 =====================
        anim_up.set_loop(true);
        anim_up.set_interval(0.15); // 每帧0.15秒
        anim_up.set_frame_data(tex_goblin, 5, 4, idx_list_up); // 纹理, 5列, 4行, 帧序列

        anim_down.set_loop(true);
        anim_down.set_interval(0.15);
        anim_down.set_frame_data(tex_goblin, 5, 4, idx_list_down);

        anim_left.set_loop(true);
        anim_left.set_interval(0.15);
        anim_left.set_frame_data(tex_goblin, 5, 4, idx_list_left);

        anim_right.set_loop(true);
        anim_right.set_interval(0.15);
        anim_right.set_frame_data(tex_goblin, 5, 4, idx_list_right);

        // ===================== 设置哥布林草图动画 =====================
        // 用于路径预览或特殊显示模式
        anim_up_sketch.set_loop(true);
        anim_up_sketch.set_interval(0.15);
        anim_up_sketch.set_frame_data(tex_goblin_sketch, 5, 4, idx_list_up);

        anim_down_sketch.set_loop(true);
        anim_down_sketch.set_interval(0.15);
        anim_down_sketch.set_frame_data(tex_goblin_sketch, 5, 4, idx_list_down);

        anim_left_sketch.set_loop(true);
        anim_left_sketch.set_interval(0.15);
        anim_left_sketch.set_frame_data(tex_goblin_sketch, 5, 4, idx_list_left);

        anim_right_sketch.set_loop(true);
        anim_right_sketch.set_interval(0.15);
        anim_right_sketch.set_frame_data(tex_goblin_sketch, 5, 4, idx_list_right);

        // ===================== 设置敌人属性 =====================
        // 从配置模板获取属性值
        max_hp = goblin_template.hp;              // 最大生命值
        max_speed = goblin_template.speed;        // 最大移动速度
        damage = goblin_template.damage;          // 攻击伤害值
        reward_ratio = goblin_template.reward_ratio; // 击败后的奖励系数
        recover_interval = goblin_template.recover_interval; // 生命恢复间隔
        recover_range = goblin_template.recover_range;       // 恢复影响范围
        recover_intensity = goblin_template.recover_intensity; // 恢复强度

        // 初始化当前状态
        size.x = 48, size.y = 48;  // 敌人尺寸 (48x48像素)
        hp = max_hp;               // 当前生命值 = 最大生命值
        speed = max_speed;         // 当前速度 = 最大速度
    }

    /// 默认析构函数
    ~GoblinEnemy() = default;
};

#endif // !_GOBLIN_ENEMY_H_
