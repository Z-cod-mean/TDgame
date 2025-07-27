#ifndef _GOBLIN_PRIEST_ENEMY_H_
#define _GOBLIN_PRIEST_ENEMY_H_

#include "enemy.h"
#include "config_manager.h"
#include "resources_manager.h"

/**
 * @class GoblinPriestEnemy
 * @brief 哥布林祭司敌人类型，继承自基础敌人类
 *
 * 哥布林祭司是具有治疗能力的特殊敌人，能够为周围其他敌人恢复生命值。
 * 相比普通哥布林，它具有更复杂的技能系统和行为逻辑。
 */
class GoblinPriestEnemy : public Enemy
{
public:
    /**
     * @brief 构造函数：初始化哥布林祭司的属性和动画
     *
     * 1. 加载纹理资源
     * 2. 配置动画帧序列
     * 3. 设置敌人属性（生命值、速度、伤害等）
     * 4. 初始化技能计时器
     */
    GoblinPriestEnemy()
    {
        // 获取资源管理器实例
        static const ResourcesManager::TexturePool& texture_pool
            = ResourcesManager::instance()->get_texture_pool();

        // 加载哥布林祭司纹理和草图纹理
        static SDL_Texture* tex_goblin_priest = texture_pool.find(ResID::Tex_GoblinPriest)->second;
        static SDL_Texture* tex_goblin_priest_sketch = texture_pool.find(ResID::Tex_GoblinPriestSketch)->second;

        // 获取配置管理器中的哥布林祭司模板数据
        static ConfigManager::EnemyTemplate& goblin_priest_template = ConfigManager::instance()->goblin_priest_template;

        // 定义各方向动画的帧索引序列
        // 精灵表布局：5列4行（共20帧）
        static const std::vector<int> idx_list_up = { 5, 6, 7, 8, 9 };     // 朝上移动动画 (5帧)
        static const std::vector<int> idx_list_down = { 0, 1, 2, 3, 4 };   // 朝下移动动画 (5帧)
        static const std::vector<int> idx_list_left = { 15, 16, 17, 18, 19 }; // 朝左移动动画 (5帧)
        static const std::vector<int> idx_list_right = { 10, 11, 12, 13, 14 }; // 朝右移动动画 (5帧)

        // ===================== 设置完整哥布林祭司动画 =====================
        anim_up.set_loop(true);
        anim_up.set_interval(0.15); // 每帧0.15秒
        anim_up.set_frame_data(tex_goblin_priest, 5, 4, idx_list_up); // 纹理, 5列, 4行, 帧序列

        anim_down.set_loop(true);
        anim_down.set_interval(0.15);
        anim_down.set_frame_data(tex_goblin_priest, 5, 4, idx_list_down);

        anim_left.set_loop(true);
        anim_left.set_interval(0.15);
        anim_left.set_frame_data(tex_goblin_priest, 5, 4, idx_list_left);

        anim_right.set_loop(true);
        anim_right.set_interval(0.15);
        anim_right.set_frame_data(tex_goblin_priest, 5, 4, idx_list_right);

        // ===================== 设置哥布林祭司草图动画 =====================
        // 用于路径预览或特殊显示模式
        anim_up_sketch.set_loop(true);
        anim_up_sketch.set_interval(0.15);
        anim_up_sketch.set_frame_data(tex_goblin_priest_sketch, 5, 4, idx_list_up);

        anim_down_sketch.set_loop(true);
        anim_down_sketch.set_interval(0.15);
        anim_down_sketch.set_frame_data(tex_goblin_priest_sketch, 5, 4, idx_list_down);

        anim_left_sketch.set_loop(true);
        anim_left_sketch.set_interval(0.15);
        anim_left_sketch.set_frame_data(tex_goblin_priest_sketch, 5, 4, idx_list_left);

        anim_right_sketch.set_loop(true);
        anim_right_sketch.set_interval(0.15);
        anim_right_sketch.set_frame_data(tex_goblin_priest_sketch, 5, 4, idx_list_right);

        // ===================== 设置敌人属性 =====================
        // 从配置模板获取属性值
        max_hp = goblin_priest_template.hp;              // 最大生命值
        max_speed = goblin_priest_template.speed;        // 最大移动速度
        damage = goblin_priest_template.damage;          // 攻击伤害值
        reward_ratio = goblin_priest_template.reward_ratio; // 击败后的奖励系数

        // 治疗相关属性
        recover_interval = goblin_priest_template.recover_interval; // 治疗技能冷却时间
        recover_range = goblin_priest_template.recover_range;       // 治疗影响范围
        recover_intensity = goblin_priest_template.recover_intensity; // 治疗强度

        // 初始化当前状态
        size.x = 48, size.y = 48;  // 敌人尺寸 (48x48像素)
        hp = max_hp;               // 当前生命值 = 最大生命值
        speed = max_speed;         // 当前速度 = 最大速度

        // 初始化治疗技能计时器
        timer_skill.set_wait_time(recover_interval); // 设置冷却时间为配置值
    }

    /// 默认析构函数
    ~GoblinPriestEnemy() = default;
};

#endif // !_GOBLIN_PRIEST_ENEMY_H_
