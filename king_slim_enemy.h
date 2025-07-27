#ifndef _KINNG_SLIM_ENEMY_H_
#define _KINNG_SLIM_ENEMY_H_

#include "enemy.h"
#include "config_manager.h"
#include "resources_manager.h"

/**
 * @class KingSlimeEnemy
 * @brief 史莱姆王敌人类型，继承自基础敌人类
 *
 * 史莱姆王是游戏中的精英或Boss级敌人，相比普通敌人具有更高的生命值、伤害和特殊能力。
 * 其动画帧数更多，动画速度更快，展现出更流畅的移动效果。
 */
class KingSlimeEnemy : public Enemy
{
public:
    /**
     * @brief 构造函数：初始化史莱姆王的属性和动画
     *
     * 1. 加载纹理资源
     * 2. 配置动画帧序列（更多帧数，更快速度）
     * 3. 设置敌人属性（更高的生命值、伤害等）
     * 4. 初始化尺寸和状态
     */
    KingSlimeEnemy()
    {
        // 获取资源管理器实例
        static const ResourcesManager::TexturePool& texture_pool
            = ResourcesManager::instance()->get_texture_pool();

        // 加载史莱姆王纹理和草图纹理
        static SDL_Texture* tex_king_slime = texture_pool.find(ResID::Tex_KingSlime)->second;
        static SDL_Texture* tex_king_slime_sketch = texture_pool.find(ResID::Tex_KingSlimeSketch)->second;

        // 获取配置管理器中的史莱姆王模板数据
        static ConfigManager::EnemyTemplate& king_slim_template = ConfigManager::instance()->king_slim_template;

        // 定义各方向动画的帧索引序列
        // 精灵表布局：6列4行（共24帧）
        static const std::vector<int> idx_list_up = { 18, 19, 20, 21, 22, 23 };   // 朝上移动动画 (6帧)
        static const std::vector<int> idx_list_down = { 0, 1, 2, 3, 4, 5 };       // 朝下移动动画 (6帧)
        static const std::vector<int> idx_list_left = { 6, 7, 8, 9, 10, 11 };     // 朝左移动动画 (6帧)
        static const std::vector<int> idx_list_right = { 12, 13, 14, 15, 16, 17 }; // 朝右移动动画 (6帧)

        // ===================== 设置完整史莱姆王动画 =====================
        anim_up.set_loop(true);
        anim_up.set_interval(0.1); // 更快的帧率：每帧0.1秒
        anim_up.set_frame_data(tex_king_slime, 6, 4, idx_list_up); // 纹理, 6列, 4行, 帧序列

        anim_down.set_loop(true);
        anim_down.set_interval(0.1);
        anim_down.set_frame_data(tex_king_slime, 6, 4, idx_list_down);

        anim_left.set_loop(true);
        anim_left.set_interval(0.1);
        anim_left.set_frame_data(tex_king_slime, 6, 4, idx_list_left);

        anim_right.set_loop(true);
        anim_right.set_interval(0.1);
        anim_right.set_frame_data(tex_king_slime, 6, 4, idx_list_right);

        // ===================== 设置史莱姆王草图动画 =====================
        // 用于路径预览或特殊显示模式
        anim_up_sketch.set_loop(true);
        anim_up_sketch.set_interval(0.1);
        anim_up_sketch.set_frame_data(tex_king_slime_sketch, 6, 4, idx_list_up);

        anim_down_sketch.set_loop(true);
        anim_down_sketch.set_interval(0.1);
        anim_down_sketch.set_frame_data(tex_king_slime_sketch, 6, 4, idx_list_down);

        anim_left_sketch.set_loop(true);
        anim_left_sketch.set_interval(0.1);
        anim_left_sketch.set_frame_data(tex_king_slime_sketch, 6, 4, idx_list_left);

        anim_right_sketch.set_loop(true);
        anim_right_sketch.set_interval(0.1);
        anim_right_sketch.set_frame_data(tex_king_slime_sketch, 6, 4, idx_list_right);

        // ===================== 设置敌人属性 =====================
        // 从配置模板获取属性值（作为Boss级敌人，数值更高）
        max_hp = king_slim_template.hp;              // 更高的最大生命值
        max_speed = king_slim_template.speed;        // 移动速度（可能较慢但更坚韧）
        damage = king_slim_template.damage;          // 更高的攻击伤害值
        reward_ratio = king_slim_template.reward_ratio; // 击败后的丰厚奖励系数

        // 恢复能力属性（作为Boss可能有自我恢复能力）
        recover_interval = king_slim_template.recover_interval; // 生命恢复间隔
        recover_range = king_slim_template.recover_range;       // 恢复影响范围
        recover_intensity = king_slim_template.recover_intensity; // 恢复强度

        // 初始化当前状态
        size.x = 48, size.y = 48;  // 敌人尺寸 (48x48像素) - 可能比普通敌人大
        hp = max_hp;               // 当前生命值 = 最大生命值
        speed = max_speed;         // 当前速度 = 最大速度
    }

    /// 默认析构函数
    ~KingSlimeEnemy() = default;
};

#endif // !_KINNG_SLIM_ENEMY_H_