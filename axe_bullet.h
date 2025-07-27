#ifndef _AXE_BULLET_H_
#define _AXE_BULLET_H_

#include "bullet.h"
#include "resources_manager.h"

/**
 * @class AxeBullet
 * @brief ��ͷ�ӵ��࣬�̳��Ի����ӵ���
 *
 * ����ʵ�ָ�ͷ�ӵ����ض���Ϊ��������ת������������ײЧ����
 * ��ͷ�ӵ�ͨ���ɸ��������䣬����ʹ���˼��ٵ�����Ч����
 */
class AxeBullet : public Bullet
{
public:
    /**
     * @brief ���캯������ʼ����ͷ�ӵ������ԺͶ���
     *
     * 1. ���ظ�ͷ������Դ
     * 2. ���ö���֡���У�9֡��ת������
     * 3. �����ӵ��ߴ�
     */
    AxeBullet()
    {
        // ��ȡ��Դ�������еĸ�ͷ����
        static SDL_Texture* tex_axe = ResourcesManager::instance()
            ->get_texture_pool().find(ResID::Tex_BulletAxe)->second;

        // ���嶯��֡���У���ͷ��ת������9��֡��
        static const std::vector<int> idx_list = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

        // ���ö�������
        animation.set_loop(true);      // ѭ�����Ŷ���
        animation.set_interval(0.1);   // ÿ֡0.1�루10 FPS��
        animation.set_frame_data(tex_axe, 4, 2, idx_list); // ����, 4��2��, ֡����

        // �����ӵ��ߴ�
        size.x = 48;          // �ӵ����
        size.y = 48;          // �ӵ��߶�
    }

    /// Ĭ����������
    ~AxeBullet() = default;

    /**
     * @brief ��ײ����ʱ�Ĵ�����д���෽����
     * @param enemy �����еĵ���ָ��
     *
     * 1. ����������ָ�ͷ������Ч֮һ
     * 2. �Ե���ʩ�Ӽ���Ч��
     * 3. ���û������ײ�����߼�
     */
    void on_collide(Enemy* enemy) override
    {
        // ��ȡ��Ч��Դ��
        static const ResourcesManager::SoundPool& sound_pool
            = ResourcesManager::instance()->get_sound_pool();

        // ���ѡ������������Ч֮һ����
        switch (rand() % 3)
        {
        case 0:
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_AxeHit_1)->second, 0);
            break;
        case 1:
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_AxeHit_2)->second, 0);
            break;
        case 2:
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_AxeHit_3)->second, 0);
            break;
        }

        // �Ե���ʩ�Ӽ���Ч��
        enemy->slow_down();

        // ���û������ײ��������ӵ�Ϊ��Ч�ȣ�
        Bullet::on_collide(enemy);
    }
};

#endif // !_AXE_BULLET_H_
