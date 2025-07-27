#ifndef _GOBLIN_PRIEST_ENEMY_H_
#define _GOBLIN_PRIEST_ENEMY_H_

#include "enemy.h"
#include "config_manager.h"
#include "resources_manager.h"

/**
 * @class GoblinPriestEnemy
 * @brief �粼�ּ�˾�������ͣ��̳��Ի���������
 *
 * �粼�ּ�˾�Ǿ�������������������ˣ��ܹ�Ϊ��Χ�������˻ָ�����ֵ��
 * �����ͨ�粼�֣������и����ӵļ���ϵͳ����Ϊ�߼���
 */
class GoblinPriestEnemy : public Enemy
{
public:
    /**
     * @brief ���캯������ʼ���粼�ּ�˾�����ԺͶ���
     *
     * 1. ����������Դ
     * 2. ���ö���֡����
     * 3. ���õ������ԣ�����ֵ���ٶȡ��˺��ȣ�
     * 4. ��ʼ�����ܼ�ʱ��
     */
    GoblinPriestEnemy()
    {
        // ��ȡ��Դ������ʵ��
        static const ResourcesManager::TexturePool& texture_pool
            = ResourcesManager::instance()->get_texture_pool();

        // ���ظ粼�ּ�˾����Ͳ�ͼ����
        static SDL_Texture* tex_goblin_priest = texture_pool.find(ResID::Tex_GoblinPriest)->second;
        static SDL_Texture* tex_goblin_priest_sketch = texture_pool.find(ResID::Tex_GoblinPriestSketch)->second;

        // ��ȡ���ù������еĸ粼�ּ�˾ģ������
        static ConfigManager::EnemyTemplate& goblin_priest_template = ConfigManager::instance()->goblin_priest_template;

        // ��������򶯻���֡��������
        // ������֣�5��4�У���20֡��
        static const std::vector<int> idx_list_up = { 5, 6, 7, 8, 9 };     // �����ƶ����� (5֡)
        static const std::vector<int> idx_list_down = { 0, 1, 2, 3, 4 };   // �����ƶ����� (5֡)
        static const std::vector<int> idx_list_left = { 15, 16, 17, 18, 19 }; // �����ƶ����� (5֡)
        static const std::vector<int> idx_list_right = { 10, 11, 12, 13, 14 }; // �����ƶ����� (5֡)

        // ===================== ���������粼�ּ�˾���� =====================
        anim_up.set_loop(true);
        anim_up.set_interval(0.15); // ÿ֡0.15��
        anim_up.set_frame_data(tex_goblin_priest, 5, 4, idx_list_up); // ����, 5��, 4��, ֡����

        anim_down.set_loop(true);
        anim_down.set_interval(0.15);
        anim_down.set_frame_data(tex_goblin_priest, 5, 4, idx_list_down);

        anim_left.set_loop(true);
        anim_left.set_interval(0.15);
        anim_left.set_frame_data(tex_goblin_priest, 5, 4, idx_list_left);

        anim_right.set_loop(true);
        anim_right.set_interval(0.15);
        anim_right.set_frame_data(tex_goblin_priest, 5, 4, idx_list_right);

        // ===================== ���ø粼�ּ�˾��ͼ���� =====================
        // ����·��Ԥ����������ʾģʽ
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

        // ===================== ���õ������� =====================
        // ������ģ���ȡ����ֵ
        max_hp = goblin_priest_template.hp;              // �������ֵ
        max_speed = goblin_priest_template.speed;        // ����ƶ��ٶ�
        damage = goblin_priest_template.damage;          // �����˺�ֵ
        reward_ratio = goblin_priest_template.reward_ratio; // ���ܺ�Ľ���ϵ��

        // �����������
        recover_interval = goblin_priest_template.recover_interval; // ���Ƽ�����ȴʱ��
        recover_range = goblin_priest_template.recover_range;       // ����Ӱ�췶Χ
        recover_intensity = goblin_priest_template.recover_intensity; // ����ǿ��

        // ��ʼ����ǰ״̬
        size.x = 48, size.y = 48;  // ���˳ߴ� (48x48����)
        hp = max_hp;               // ��ǰ����ֵ = �������ֵ
        speed = max_speed;         // ��ǰ�ٶ� = ����ٶ�

        // ��ʼ�����Ƽ��ܼ�ʱ��
        timer_skill.set_wait_time(recover_interval); // ������ȴʱ��Ϊ����ֵ
    }

    /// Ĭ����������
    ~GoblinPriestEnemy() = default;
};

#endif // !_GOBLIN_PRIEST_ENEMY_H_
