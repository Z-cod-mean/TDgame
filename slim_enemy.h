#ifndef _SLIM_ENEMY_H_
#define _SLIM_ENEMY_H_

#include "enemy.h"
#include "config_manager.h"
#include "resources_manager.h"

/**
 * @class SlimEnemy
 * @brief ʷ��ķ�������ͣ��̳��Ի���������
 *
 * ʷ��ķ����Ϸ�еĻ����������ͣ������������Ե����ԡ�
 * ����������ˣ�ʷ��ķ�Ķ�������������6֡/���򣩣��ƶ��ٶȽ��������ܾ�������������
 */
class SlimEnemy : public Enemy
{
public:
    /**
     * @brief ���캯������ʼ��ʷ��ķ�����ԺͶ���
     *
     * 1. ����ʷ��ķ������Դ
     * 2. ���ö���֡���У�6֡�����������ƶ�Ч����
     * 3. ���õ������ԣ�����ֵ���ٶȡ��˺��ȣ�
     * 4. ��ʼ���ߴ��״̬
     */
    SlimEnemy()
    {
        // ��ȡ��Դ������ʵ��
        static const ResourcesManager::TexturePool& texture_pool
            = ResourcesManager::instance()->get_texture_pool();

        // ����ʷ��ķ����Ͳ�ͼ����
        static SDL_Texture* tex_slime = texture_pool.find(ResID::Tex_Slime)->second;
        static SDL_Texture* tex_slime_sketch = texture_pool.find(ResID::Tex_SlimeSketch)->second;

        // ��ȡ���ù������е�ʷ��ķģ������
        static ConfigManager::EnemyTemplate& slim_template = ConfigManager::instance()->slim_template;

        // ��������򶯻���֡��������
        // ������֣�6��4�У���24֡��
        static const std::vector<int> idx_list_up = { 6, 7, 8, 9, 10, 11 };   // �����ƶ����� (6֡)
        static const std::vector<int> idx_list_down = { 0, 1, 2, 3, 4, 5 };   // �����ƶ����� (6֡)
        static const std::vector<int> idx_list_left = { 18, 19, 20, 21, 22, 23 }; // �����ƶ����� (6֡)
        static const std::vector<int> idx_list_right = { 12, 13, 14, 15, 16, 17 }; // �����ƶ����� (6֡)

        // ===================== ��������ʷ��ķ���� =====================
        anim_up.set_loop(true);
        anim_up.set_interval(0.1); // �Ͽ��֡�ʣ�ÿ֡0.1��
        anim_up.set_frame_data(tex_slime, 6, 4, idx_list_up); // ����, 6��, 4��, ֡����

        anim_down.set_loop(true);
        anim_down.set_interval(0.1);
        anim_down.set_frame_data(tex_slime, 6, 4, idx_list_down);

        anim_left.set_loop(true);
        anim_left.set_interval(0.1);
        anim_left.set_frame_data(tex_slime, 6, 4, idx_list_left);

        anim_right.set_loop(true);
        anim_right.set_interval(0.1);
        anim_right.set_frame_data(tex_slime, 6, 4, idx_list_right);

        // ===================== ����ʷ��ķ��ͼ���� =====================
        // ����·��Ԥ����������ʾģʽ
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

        // ===================== ���õ������� =====================
        // ������ģ���ȡ����ֵ
        max_hp = slim_template.hp;              // �������ֵ
        max_speed = slim_template.speed;        // �ƶ��ٶȣ�ͨ��������
        damage = slim_template.damage;          // �����˺�ֵ
        reward_ratio = slim_template.reward_ratio; // ���ܺ�Ľ���ϵ��

        // �ָ��������ԣ�ʷ��ķ�����з��ѻ�����������
        recover_interval = slim_template.recover_interval; // �����ָ����
        recover_range = slim_template.recover_range;       // �ָ�Ӱ�췶Χ
        recover_intensity = slim_template.recover_intensity; // �ָ�ǿ��

        // ��ʼ����ǰ״̬
        size.x = 48, size.y = 48;  // ���˳ߴ� (48x48����)
        hp = max_hp;               // ��ǰ����ֵ = �������ֵ
        speed = max_speed;         // ��ǰ�ٶ� = ����ٶ�
    }

    /// Ĭ����������
    ~SlimEnemy() = default;
};

#endif // !_SLIM_ENEMY_H_
