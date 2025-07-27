#ifndef _ARROW_BULLET_H_
#define _ARROW_BULLET_H_

#include "bullet.h"
#include "resources_manager.h"

/**
 * @class ArrowBullet
 * @brief ��ʸ�ӵ��࣬�̳��Ի����ӵ���
 *
 * ����ʵ�ּ�ʸ�ӵ����ض���Ϊ��������תЧ������ײ��Ч��
 * ��ʸ�ӵ�ͨ���ɹ����������䣬���д�͸Ч����������Ч��
 */
class ArrowBullet : public Bullet
{
public:
    /**
     * @brief ���캯������ʼ����ʸ�ӵ������ԺͶ���
     *
     * 1. ���ؼ�ʸ������Դ
     * 2. ���ö���֡����
     * 3. �����ӵ����ԣ�����ת���ߴ�ȣ�
     */
    ArrowBullet()
    {
        // ��ȡ��Դ�������еļ�ʸ����
        static SDL_Texture* tex_arrow = ResourcesManager::instance()
            ->get_texture_pool().find(ResID::Tex_BulletArrow)->second;

        // ���嶯��֡���У�������е�֡������
        static const std::vector<int> idx_list = { 0, 1 }; // ʹ��ǰ��֡

        // ���ö�������
        animation.set_loop(true);      // ѭ�����Ŷ���
        animation.set_interval(0.1);   // ÿ֡0.1�루10 FPS��
        animation.set_frame_data(tex_arrow, 2, 1, idx_list); // ����, 2��1��, ֡����

        // �����ӵ�����
        can_rotated = true;   // �����ӵ����ݷ��з�����ת
        size.x = 48;          // �ӵ����
        size.y = 48;          // �ӵ��߶�
    }

    /// Ĭ����������
    ~ArrowBullet() = default;

    /**
     * @brief ��ײ����ʱ�Ĵ�����д���෽����
     * @param enemy �����еĵ���ָ��
     *
     * 1. ����������ּ�ʸ������Ч֮һ
     * 2. ���û������ײ�����߼�
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
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ArrowHit_1)->second, 0);
            break;
        case 1:
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ArrowHit_2)->second, 0);
            break;
        case 2:
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ArrowHit_3)->second, 0);
            break;
        }

        // ���û������ײ��������ӵ�Ϊ��Ч�ȣ�
        Bullet::on_collide(enemy);
    }
};

#endif // !_ARROW_BULLET_H_
