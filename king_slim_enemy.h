#ifndef _KINNG_SLIM_ENEMY_H_
#define _KINNG_SLIM_ENEMY_H_

#include "enemy.h"
#include "config_manager.h"
#include "resources_manager.h"

/**
 * @class KingSlimeEnemy
 * @brief ʷ��ķ���������ͣ��̳��Ի���������
 *
 * ʷ��ķ������Ϸ�еľ�Ӣ��Boss�����ˣ������ͨ���˾��и��ߵ�����ֵ���˺�������������
 * �䶯��֡�����࣬�����ٶȸ��죬չ�ֳ����������ƶ�Ч����
 */
class KingSlimeEnemy : public Enemy
{
public:
    /**
     * @brief ���캯������ʼ��ʷ��ķ�������ԺͶ���
     *
     * 1. ����������Դ
     * 2. ���ö���֡���У�����֡���������ٶȣ�
     * 3. ���õ������ԣ����ߵ�����ֵ���˺��ȣ�
     * 4. ��ʼ���ߴ��״̬
     */
    KingSlimeEnemy()
    {
        // ��ȡ��Դ������ʵ��
        static const ResourcesManager::TexturePool& texture_pool
            = ResourcesManager::instance()->get_texture_pool();

        // ����ʷ��ķ������Ͳ�ͼ����
        static SDL_Texture* tex_king_slime = texture_pool.find(ResID::Tex_KingSlime)->second;
        static SDL_Texture* tex_king_slime_sketch = texture_pool.find(ResID::Tex_KingSlimeSketch)->second;

        // ��ȡ���ù������е�ʷ��ķ��ģ������
        static ConfigManager::EnemyTemplate& king_slim_template = ConfigManager::instance()->king_slim_template;

        // ��������򶯻���֡��������
        // ������֣�6��4�У���24֡��
        static const std::vector<int> idx_list_up = { 18, 19, 20, 21, 22, 23 };   // �����ƶ����� (6֡)
        static const std::vector<int> idx_list_down = { 0, 1, 2, 3, 4, 5 };       // �����ƶ����� (6֡)
        static const std::vector<int> idx_list_left = { 6, 7, 8, 9, 10, 11 };     // �����ƶ����� (6֡)
        static const std::vector<int> idx_list_right = { 12, 13, 14, 15, 16, 17 }; // �����ƶ����� (6֡)

        // ===================== ��������ʷ��ķ������ =====================
        anim_up.set_loop(true);
        anim_up.set_interval(0.1); // �����֡�ʣ�ÿ֡0.1��
        anim_up.set_frame_data(tex_king_slime, 6, 4, idx_list_up); // ����, 6��, 4��, ֡����

        anim_down.set_loop(true);
        anim_down.set_interval(0.1);
        anim_down.set_frame_data(tex_king_slime, 6, 4, idx_list_down);

        anim_left.set_loop(true);
        anim_left.set_interval(0.1);
        anim_left.set_frame_data(tex_king_slime, 6, 4, idx_list_left);

        anim_right.set_loop(true);
        anim_right.set_interval(0.1);
        anim_right.set_frame_data(tex_king_slime, 6, 4, idx_list_right);

        // ===================== ����ʷ��ķ����ͼ���� =====================
        // ����·��Ԥ����������ʾģʽ
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

        // ===================== ���õ������� =====================
        // ������ģ���ȡ����ֵ����ΪBoss�����ˣ���ֵ���ߣ�
        max_hp = king_slim_template.hp;              // ���ߵ��������ֵ
        max_speed = king_slim_template.speed;        // �ƶ��ٶȣ����ܽ����������ͣ�
        damage = king_slim_template.damage;          // ���ߵĹ����˺�ֵ
        reward_ratio = king_slim_template.reward_ratio; // ���ܺ�ķ����ϵ��

        // �ָ��������ԣ���ΪBoss���������һָ�������
        recover_interval = king_slim_template.recover_interval; // �����ָ����
        recover_range = king_slim_template.recover_range;       // �ָ�Ӱ�췶Χ
        recover_intensity = king_slim_template.recover_intensity; // �ָ�ǿ��

        // ��ʼ����ǰ״̬
        size.x = 48, size.y = 48;  // ���˳ߴ� (48x48����) - ���ܱ���ͨ���˴�
        hp = max_hp;               // ��ǰ����ֵ = �������ֵ
        speed = max_speed;         // ��ǰ�ٶ� = ����ٶ�
    }

    /// Ĭ����������
    ~KingSlimeEnemy() = default;
};

#endif // !_KINNG_SLIM_ENEMY_H_