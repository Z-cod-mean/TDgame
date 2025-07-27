#ifndef _SLIM_ENEMY_H_
#define _SLIM_ENEMY_H_

#include "enemy.h"
#include "config_manager.h"
#include "resources_manager.h"

/**
 * @class SlimEnemy
 * @brief 史莱姆敌人类型，继承自基础敌人类
 *
 * 史莱姆是游戏中的基础敌人类型，具有柔软、弹性的特性。
 * 相比其他敌人，史莱姆的动画更加流畅（6帧/方向），移动速度较慢但可能具有特殊能力。
 */
class SlimEnemy : public Enemy
{
public:
    /**
     * @brief 构造函数：初始化史莱姆的属性和动画
     *
     * 1. 加载史莱姆纹理资源
     * 2. 配置动画帧序列（6帧动画，流畅移动效果）
     * 3. 设置敌人属性（生命值、速度、伤害等）
     * 4. 初始化尺寸和状态
     */
    SlimEnemy()
    {
        // 获取资源管理器实例
        static const ResourcesManager::TexturePool& texture_pool
            = ResourcesManager::instance()->get_texture_pool();

        // 加载史莱姆纹理和草图纹理
        static SDL_Texture* tex_slime = texture_pool.find(ResID::Tex_Slime)->second;
        static SDL_Texture* tex_slime_sketch = texture_pool.find(ResID::Tex_SlimeSketch)->second;

        // 获取配置管理器中的史莱姆模板数据
        static ConfigManager::EnemyTemplate& slim_template = ConfigManager::instance()->slim_template;

        // 定义各方向动画的帧索引序列
        // 精灵表布局：6列4行（共24帧）
        static const std::vector<int> idx_list_up = { 6, 7, 8, 9, 10, 11 };   // 朝上移动动画 (6帧)
        static const std::vector<int> idx_list_down = { 0, 1, 2, 3, 4, 5 };   // 朝下移动动画 (6帧)
        static const std::vector<int> idx_list_left = { 18, 19, 20, 21, 22, 23 }; // 朝左移动动画 (6帧)
        static const std::vector<int> idx_list_right = { 12, 13, 14, 15, 16, 17 }; // 朝右移动动画 (6帧)

        // ===================== 设置完整史莱姆动画 =====================
        anim_up.set_loop(true);
        anim_up.set_interval(0.1); // 较快的帧率：每帧0.1秒
        anim_up.set_frame_data(tex_slime, 6, 4, idx_list_up); // 纹理, 6列, 4行, 帧序列

        anim_down.set_loop(true);
        anim_down.set_interval(0.1);
        anim_down.set_frame_data(tex_slime, 6, 4, idx_list_down);

        anim_left.set_loop(true);
        anim_left.set_interval(0.1);
        anim_left.set_frame_data(tex_slime, 6, 4, idx_list_left);

        anim_right.set_loop(true);
        anim_right.set_interval(0.1);
        anim_right.set_frame_data(tex_slime, 6, 4, idx_list_right);

        // ===================== 设置史莱姆草图动画 =====================
        // 用于路径预览或特殊显示模式
        anim_up_sketch.set_loop(true);
        anim_up_sketch.set_interval(0.1);
        anim_up_sketch.set_frame_data(tex_slime_sketch, 6, 4, idx_list_up);

        anim_down_sketch.set_loop(true);
        anim_down_sketch.set_interval(0.1);
        anim_down_sketch.set_frame_data(tex_slime_sketch, 6, 4, idx_list_down);

        anim_left_sketch.set_loop(true);
        anim_left_sketch.set_interval(0.1);
        anim_left_sketch.set_frame_data(tex_slime_sketch, 6, 4, idx_list_left);

        anim_right_sketch.set_loop(true);
        anim_right_sketch.set_interval(0.1);
        anim_right_sketch.set_frame_data(tex_slime_sketch, 6, 4, idx_list_right);

        // ===================== 设置敌人属性 =====================
        // 从配置模板获取属性值
        max_hp = slim_template.hp;              // 最大生命值
        max_speed = slim_template.speed;        // 移动速度（通常较慢）
        damage = slim_template.damage;          // 攻击伤害值
        reward_ratio = slim_template.reward_ratio; // 击败后的奖励系数

        // 恢复能力属性（史莱姆可能有分裂或再生能力）
        recover_interval = slim_template.recover_interval; // 生命恢复间隔
        recover_range = slim_template.recover_range;       // 恢复影响范围
        recover_intensity = slim_template.recover_intensity; // 恢复强度

        // 初始化当前状态
        size.x = 48, size.y = 48;  // 敌人尺寸 (48x48像素)
        hp = max_hp;               // 当前生命值 = 最大生命值
        speed = max_speed;         // 当前速度 = 最大速度
    }

    /// 默认析构函数
    ~SlimEnemy() = default;
};

#endif // !_SLIM_ENEMY_H_
