#ifndef _SHELL_BULLET_H_
#define _SHELL_BULLET_H_

#include "bullet.h"
#include "resources_manager.h"

/**
 * @class ShellBullet
 * @brief �ڵ��ӵ��࣬�̳��Ի����ӵ���
 *
 * ����ʵ���ڵ��ӵ����ض���Ϊ��������ըЧ���ͷ�Χ�˺���
 * �ڵ��ӵ�ͨ����ǹ�������䣬����Ŀ���������ը����������Χ��������˺���
 */
class ShellBullet : public Bullet
{
public:
    /**
     * @brief ���캯������ʼ���ڵ��ӵ������ԺͶ���
     *
     * 1. �����ڵ��ͱ�ը������Դ
     * 2. ���÷��кͱ�ը����
     * 3. ���ñ�ը��Χ�ͳߴ�
     */
    ShellBullet()
    {
        // ��ȡ��Դ�������е��ڵ��ͱ�ը����
        static SDL_Texture* tex_shell = ResourcesManager::instance()
            ->get_texture_pool().find(ResID::Tex_BulletShell)->second;
        static SDL_Texture* tex_explode = ResourcesManager::instance()
            ->get_texture_pool().find(ResID::Tex_EffectExplode)->second;

        // ���嶯��֡����
        static const std::vector<int> idx_list = { 0, 1 };         // �ڵ�����֡
        static const std::vector<int> idx_explode_list = { 0, 1, 2, 3, 4 }; // ��ըЧ��֡

        // ===================== �����ڵ����ж��� =====================
        animation.set_loop(true);             // ѭ������
        animation.set_interval(0.1);          // ÿ֡0.1��
        animation.set_frame_data(tex_shell, 2, 1, idx_list); // ����, 2��1��, ֡����

        // ===================== ���ñ�ըЧ������ =====================
        animation_explode.set_loop(false);    // ��ѭ�������β��ţ�
        animation_explode.set_interval(0.1);   // ÿ֡0.1��
        animation_explode.set_frame_data(tex_explode, 5, 1, idx_explode_list); // ����, 5��1��, ֡����

        // ��ը��������ʱ�Ļص�������ӵ���Ч
        animation_explode.set_on_finished([&]() { make_invalid(); });

        // ���ñ�ը��Χ��96���ذ뾶��
        damage_range = 96;

        // �����ӵ��ߴ�
        size.x = 48, size.y = 48;
    }

    /// Ĭ����������
    ~ShellBullet() = default;

    /**
     * @brief ÿ֡�����ӵ�״̬
     * @param delta ֡ʱ������
     *
     * 1. ����ӵ��Կ���ײ�������У������û������
     * 2. �����ѱ�ը�������±�ը����
     */
    void on_update(double delta) override
    {
        if (can_collide()) // ���ڷ�����
        {
            Bullet::on_update(delta); // ������£��ƶ�λ�ã�
            return;
        }

        // �ѱ�ը״̬�����±�ը����
        animation_explode.on_update(delta);
    }

    /**
     * @brief ��Ⱦ�ӵ�
     * @param renderer SDL��Ⱦ��ָ��
     *
     * 1. ����ӵ��Կ���ײ�������У�����Ⱦ�ڵ�
     * 2. �����ѱ�ը������Ⱦ��ը����
     */
    void on_render(SDL_Renderer* renderer) override
    {
        if (can_collide()) // ���ڷ�����
        {
            Bullet::on_render(renderer); // ������Ⱦ���ڵ���
            return;
        }

        // ��ըЧ����Ⱦ
        static SDL_Point point;

        // ���㱬ը����λ�ã���ըЧ�����ڵ���
        point.x = (int)(position.x - 96 / 2); // 96Ϊ��ըЧ���ߴ�
        point.y = (int)(position.y - 96 / 2);

        // ��Ⱦ��ը����
        animation_explode.on_render(renderer, point);
    }

    /**
     * @brief ��ײ����ʱ�Ĵ���
     * @param enemy �����еĵ���ָ��
     *
     * 1. �����ڵ�������Ч
     * 2. ������ײ��⣨���뱬ը״̬��
     * 3. ע�⣺�����û��෽������Ϊ��Ҫ��ʾ��ը����
     */
    void on_collide(Enemy* enemy) override
    {
        // ��ȡ��Ч��Դ��
        static const ResourcesManager::SoundPool& sound_pool
            = ResourcesManager::instance()->get_sound_pool();

        // �����ڵ�������Ч
        Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ShellHit)->second, 0);

        // ������ײ��⣨���뱬ը״̬��
        disable_collide();

        // ע�⣺�����û����on_collide����Ϊ������Ҫ��ʾ��ը����
        // �������������Ϊ��Ч
    }

private:
    Animation animation_explode; ///< ��ը��Ч����
};

#endif // !_SHELL_BULLET_H_
