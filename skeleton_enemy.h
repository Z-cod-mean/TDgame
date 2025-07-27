#ifndef _SKELETON_ENEMY_H_
#define _SKELETON_ENEMY_H_

#include "enemy.h"
#include "config_manager.h"
#include "resources_manager.h"

/**
 * @class SkeletonEnemy
 * @brief ���õ������ͣ��̳��Ի���������
 *
 * ���õ�������Ϸ�еĻ�����������֮һ�����б�׼���ƶ��͹�����Ϊ��
 * ����������ˣ����ÿ��ܾ��и��ߵ��ƶ��ٶȵ��ϵ͵�����ֵ��������ٵ������ĵ������͡�
 */
class SkeletonEnemy : public Enemy
{
public:
    /**
     * @brief ���캯������ʼ�����õ��˵����ԺͶ���
     *
     * 1. ��������������Դ
     * 2. ���ö���֡���У���׼5֡������
     * 3. ���õ������ԣ�����ֵ���ٶȡ��˺��ȣ�
     * 4. ��ʼ���ߴ��״̬
     */
    SkeletonEnemy()
    {
        // ��ȡ��Դ������ʵ��
        static const ResourcesManager::TexturePool& texture_pool
            = ResourcesManager::instance()->get_texture_pool();

        // ������������Ͳ�ͼ����
        static SDL_Texture* tex_skeleton = texture_pool.find(ResID::Tex_Skeleton)->second;
        static SDL_Texture* tex_skeleton_sketch = texture_pool.find(ResID::Tex_SkeletonSketch)->second;

        // ��ȡ���ù������е�����ģ������
        static ConfigManager::EnemyTemplate& skeleton_template = ConfigManager::instance()->skeleton_template;

        // ��������򶯻���֡��������
        // ������֣�5��4�У���20֡��
        static const std::vector<int> idx_list_up = { 5, 6, 7, 8, 9 };     // �����ƶ����� (5֡)
        static const std::vector<int> idx_list_down = { 0, 1, 2, 3, 4 };   // �����ƶ����� (5֡)
        static const std::vector<int> idx_list_left = { 15, 16, 17, 18, 19 }; // �����ƶ����� (5֡)
        static const std::vector<int> idx_list_right = { 10, 11, 12, 13, 14 }; // �����ƶ����� (5֡)

        // ===================== �����������ö��� =====================
        anim_up.set_loop(true);
        anim_up.set_interval(0.15); // ��׼֡�ʣ�ÿ֡0.15��
        anim_up.set_frame_data(tex_skeleton, 5, 4, idx_list_up); // ����, 5��, 4��, ֡����

        anim_down.set_loop(true);
        anim_down.set_interval(0.15);
        anim_down.set_frame_data(tex_skeleton, 5, 4, idx_list_down);

        anim_left.set_loop(true);
        anim_left.set_interval(0.15);
        anim_left.set_frame_data(tex_skeleton, 5, 4, idx_list_left);

        anim_right.set_loop(true);
        anim_right.set_interval(0.15);
        anim_right.set_frame_data(tex_skeleton, 5, 4, idx_list_right);

        // ===================== �������ò�ͼ���� =====================
        // ����·��Ԥ����������ʾģʽ
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

        // ===================== ���õ������� =====================
        // ������ģ���ȡ����ֵ
        max_hp = skeleton_template.hp;              // �������ֵ�����ܽϵͣ�
        max_speed = skeleton_template.speed;        // �ƶ��ٶȣ����ܽϸߣ�
        damage = skeleton_template.damage;          // �����˺�ֵ
        reward_ratio = skeleton_template.reward_ratio; // ���ܺ�Ľ���ϵ��

        // �ָ��������ԣ�����ͨ��û�лָ�������������������
        recover_interval = skeleton_template.recover_interval; // �����ָ����
        recover_range = skeleton_template.recover_range;       // �ָ�Ӱ�췶Χ
        recover_intensity = skeleton_template.recover_intensity; // �ָ�ǿ��

        // ��ʼ����ǰ״̬
        size.x = 48, size.y = 48;  // ���˳ߴ� (48x48����)
        hp = max_hp;               // ��ǰ����ֵ = �������ֵ
        speed = max_speed;         // ��ǰ�ٶ� = ����ٶ�
    }

    /// Ĭ����������
    ~SkeletonEnemy() = default;
};

#endif // !_SKELETON_ENEMY_H_
