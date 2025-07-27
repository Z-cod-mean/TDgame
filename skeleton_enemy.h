#ifndef _SKELETON_ENEMY_H_
#define _SKELETON_ENEMY_H_

#include "enemy.h"
#include "config_manager.h"
#include "resources_manager.h"

/**
 * @class SkeletonEnemy
 * @brief 骷髅敌人类型，继承自基础敌人类
 *
 * 骷髅敌人是游戏中的基础敌人类型之一，具有标准的移动和攻击行为。
 * 相比其他敌人，骷髅可能具有更高的移动速度但较低的生命值，代表快速但脆弱的敌人类型。
 */
class SkeletonEnemy : public Enemy
{
public:
    /**
     * @brief 构造函数：初始化骷髅敌人的属性和动画
     *
     * 1. 加载骷髅纹理资源
     * 2. 配置动画帧序列（标准5帧动画）
     * 3. 设置敌人属性（生命值、速度、伤害等）
     * 4. 初始化尺寸和状态
     */
    SkeletonEnemy()
    {
        // 获取资源管理器实例
        static const ResourcesManager::TexturePool& texture_pool
            = ResourcesManager::instance()->get_texture_pool();

        // 加载骷髅纹理和草图纹理
        static SDL_Texture* tex_skeleton = texture_pool.find(ResID::Tex_Skeleton)->second;
        static SDL_Texture* tex_skeleton_sketch = texture_pool.find(ResID::Tex_SkeletonSketch)->second;

        // 获取配置管理器中的骷髅模板数据
        static ConfigManager::EnemyTemplate& skeleton_template = ConfigManager::instance()->skeleton_template;

        // 定义各方向动画的帧索引序列
        // 精灵表布局：5列4行（共20帧）
        static const std::vector<int> idx_list_up = { 5, 6, 7, 8, 9 };     // 朝上移动动画 (5帧)
        static const std::vector<int> idx_list_down = { 0, 1, 2, 3, 4 };   // 朝下移动动画 (5帧)
        static const std::vector<int> idx_list_left = { 15, 16, 17, 18, 19 }; // 朝左移动动画 (5帧)
        static const std::vector<int> idx_list_right = { 10, 11, 12, 13, 14 }; // 朝右移动动画 (5帧)

        // ===================== 设置完整骷髅动画 =====================
        anim_up.set_loop(true);
        anim_up.set_interval(0.15); // 标准帧率：每帧0.15秒
        anim_up.set_frame_data(tex_skeleton, 5, 4, idx_list_up); // 纹理, 5列, 4行, 帧序列

        anim_down.set_loop(true);
        anim_down.set_interval(0.15);
        anim_down.set_frame_data(tex_skeleton, 5, 4, idx_list_down);

        anim_left.set_loop(true);
        anim_left.set_interval(0.15);
        anim_left.set_frame_data(tex_skeleton, 5, 4, idx_list_left);

        anim_right.set_loop(true);
        anim_right.set_interval(0.15);
        anim_right.set_frame_data(tex_skeleton, 5, 4, idx_list_right);

        // ===================== 设置骷髅草图动画 =====================
        // 用于路径预览或特殊显示模式
        anim_up_sketch.set_loop(true);
        anim_up_sketch.set_interval(0.15);
        anim_up_sketch.set_frame_data(tex_skeleton_sketch, 5, 4, idx_list_up);

        anim_down_sketch.set_loop(true);
        anim_down_sketch.set_interval(0.15);
        anim_down_sketch.set_frame_data(tex_skeleton_sketch, 5, 4, idx_list_down);

        anim_left_sketch.set_loop(true);
        anim_left_sketch.set_interval(0.15);
        anim_left_sketch.set_frame_data(tex_skeleton_sketch, 5, 4, idx_list_left);

        anim_right_sketch.set_loop(true);
        anim_right_sketch.set_interval(0.15);
        anim_right_sketch.set_frame_data(tex_skeleton_sketch, 5, 4, idx_list_right);

        // ===================== 设置敌人属性 =====================
        // 从配置模板获取属性值
        max_hp = skeleton_template.hp;              // 最大生命值（可能较低）
        max_speed = skeleton_template.speed;        // 移动速度（可能较高）
        damage = skeleton_template.damage;          // 攻击伤害值
        reward_ratio = skeleton_template.reward_ratio; // 击败后的奖励系数

        // 恢复能力属性（骷髅通常没有恢复能力，但保留参数）
        recover_interval = skeleton_template.recover_interval; // 生命恢复间隔
        recover_range = skeleton_template.recover_range;       // 恢复影响范围
        recover_intensity = skeleton_template.recover_intensity; // 恢复强度

        // 初始化当前状态
        size.x = 48, size.y = 48;  // 敌人尺寸 (48x48像素)
        hp = max_hp;               // 当前生命值 = 最大生命值
        speed = max_speed;         // 当前速度 = 最大速度
    }

    /// 默认析构函数
    ~SkeletonEnemy() = default;
};

#endif // !_SKELETON_ENEMY_H_
