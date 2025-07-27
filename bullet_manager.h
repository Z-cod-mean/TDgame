#ifndef _BULLET_MANAGER_H_
#define _BULLET_MANAGER_H_

#include "bullet.h"
#include "manager.h"          // ����������
#include "arrow_bullet.h"     // ��ʸ�ӵ�
#include "axe_bullet.h"       // ��ͷ�ӵ�
#include "shell_bullet.h"     // �ڵ��ӵ�
#include "bullet_type.h"      // �ӵ�����ö��

#include <vector>             // ��������

/**
 * @class BulletManager
 * @brief �ӵ��������࣬������Ϸ�������ӵ����������ڹ���
 *
 * ����ʹ�õ���ģʽʵ�֣�ȫ��Ψһ����Ҫ���ܰ�����
 * 1. �����ʹ洢�����ӵ�����
 * 2. ÿ֡�����ӵ�״̬
 * 3. ÿ֡��Ⱦ�ӵ�
 * 4. �Զ�������Ƴ����ӵ�
 */
class BulletManager : public Manager<BulletManager>
{
    friend class Manager<BulletManager>; // ����������˽�й��캯��

public:
    typedef std::vector<Bullet*> BulletList; ///< �ӵ��б�����

public:
    /**
     * @brief ÿ֡���������ӵ�״̬
     * @param delta ֡ʱ���������룩
     *
     * 1. ���������ӵ�������״̬
     * 2. ������Ϊ���Ƴ����ӵ�
     */
    void on_update(double delta)
    {
        // ���������ӵ�
        for (Bullet* bullet : bullet_list)
            bullet->on_update(delta);

        // ������Ƴ����ӵ���ʹ��remove-erase���÷���
        bullet_list.erase(std::remove_if(
            bullet_list.begin(),
            bullet_list.end(),
            [](const Bullet* bullet) {
                bool deletable = bullet->can_remove();
                if (deletable) delete bullet; // �ͷ��ڴ�
                return deletable;
            }),
            bullet_list.end());
    }

    /**
     * @brief ��Ⱦ�����ӵ�
     * @param renderer SDL��Ⱦ��ָ��
     */
    void on_render(SDL_Renderer* renderer)
    {
        for (Bullet* bullet : bullet_list)
            bullet->on_render(renderer);
    }

    /**
     * @brief ��ȡ�ӵ��б�����
     * @return ��ǰ���л�Ծ�ӵ����б�
     */
    BulletList& get_bullet_list()
    {
        return bullet_list;
    }

    /**
     * @brief �������ӵ�
     * @param type �ӵ����ͣ���ͷ/��ͷ/�ڵ���
     * @param position ����λ�ã���ά������
     * @param velocity �ӵ��ٶȣ���ά������
     * @param damage �ӵ��˺�ֵ
     *
     * �����ӵ����ʹ�����Ӧ���ӵ��������ó�ʼ���ԣ�����ӵ������б�
     */
    void fire_bullet(BulletType type, const Vector2& position, const Vector2& velocity, double damage)
    {
        Bullet* bullet = nullptr; // ���ӵ�ָ��

        // �����ӵ����ʹ��������ӵ�����
        switch (type)
        {
        case Arrow: // ��ʸ�ӵ�
            bullet = new ArrowBullet();
            break;
        case Axe:   // ��ͷ�ӵ�
            bullet = new AxeBullet();
            break;
        case Shell: // �ڵ��ӵ�
            bullet = new ShellBullet();
            break;
        default:    // Ĭ��ʹ�ü�ʸ�ӵ�
            bullet = new ArrowBullet();
            break;
        }

        // �����ӵ���ʼ����
        bullet->set_position(position); // λ��
        bullet->set_velocity(velocity); // �ٶ�
        bullet->set_damage(damage);     // �˺�ֵ

        // ��ӵ��ӵ��б�
        bullet_list.push_back(bullet);
    }

protected:
    /// �������캯��������ģʽ��
    BulletManager() = default;

    /// ������������������ʣ���ӵ�
    ~BulletManager()
    {
        for (Bullet* bullet : bullet_list)
            delete bullet; // �ͷ��ڴ�
    }

private:
    BulletList bullet_list; ///< �洢���л�Ծ�ӵ����б�
};

#endif // !_BULLET_MANAGER_H_
