#ifndef _AXEMAX_H_       // ��ֹͷ�ļ��ظ�����
#define _AXEMAX_H_

#include "tower.h"              // �������ඨ��
#include "resources_manager.h"  // ��Դ������

class AxemanTower : public Tower  // �̳��Ի�������
{
public:
    AxemanTower()  // ���캯��
    {
        // ���ظ���������̬��Դֻ��ʼ��һ�Σ�
        static SDL_Texture* tex_axeman = ResourcesManager::instance()
            ->get_texture_pool().find(ResID::Tex_Axeman)->second;

        // ��������򶯻�֡��������̬������
        static const std::vector<int> idx_list_idle_up = { 3, 4 };     // �ϴ���֡
        static const std::vector<int> idx_list_idle_down = { 0, 1 };   // �´���֡
        static const std::vector<int> idx_list_idle_left = { 9, 10 };  // �����֡
        static const std::vector<int> idx_list_idle_right = { 6, 7 };   // �Ҵ���֡
        static const std::vector<int> idx_list_fire_up = { 15, 16, 17 };    // �Ϲ���֡
        static const std::vector<int> idx_list_fire_down = { 12, 13, 14 };  // �¹���֡
        static const std::vector<int> idx_list_fire_left = { 21, 22, 23 };  // �󹥻�֡
        static const std::vector<int> idx_list_fire_right = { 18, 19, 20 }; // �ҹ���֡

        // ���ø����򶯻�
        anim_idle_up.set_frame_data(tex_axeman, 3, 8, idx_list_idle_up);     // �ϴ�������
        anim_idle_down.set_frame_data(tex_axeman, 3, 8, idx_list_idle_down);  // �´�������
        anim_idle_left.set_frame_data(tex_axeman, 3, 8, idx_list_idle_left); // ���������
        anim_idle_right.set_frame_data(tex_axeman, 3, 8, idx_list_idle_right);// �Ҵ�������
        anim_fire_up.set_frame_data(tex_axeman, 3, 8, idx_list_fire_up);      // �Ϲ�������
        anim_fire_down.set_frame_data(tex_axeman, 3, 8, idx_list_fire_down);  // �¹�������
        anim_fire_left.set_frame_data(tex_axeman, 3, 8, idx_list_fire_left);  // �󹥻�����
        anim_fire_right.set_frame_data(tex_axeman, 3, 8, idx_list_fire_right);// �ҹ�������

        // �������ߴ磨���أ�
        size.x = 48;
        size.y = 48;

        // ���������ͱ�ʶ
        tower_type = TowerType::Axeman;

        // ���ù�������
        fire_speed = 5;                 // �����ٶȣ�ֵԽ��Խ�죩
        bullet_type = BulletType::Axe;   // ���丫ͷ�����ӵ�
    }

    ~AxemanTower() = default;  // ʹ��Ĭ����������
};

#endif // !_AXEMAX_H_