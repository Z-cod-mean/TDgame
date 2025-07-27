#ifndef _HOME_MANAGER_H_
#define _HOME_MANAGER_H_

#include "manager.h"         // ����������
#include "config_manager.h"  // ���ù�����
#include "resources_manager.h" // ��Դ������

/**
 * @class HomeManager
 * @brief ���ع������࣬���������һ��ص�����ֵ״̬
 *
 * ����ʹ�õ���ģʽʵ�֣�ȫ��Ψһ����Ҫ���ܰ�����
 * 1. ������صĵ�ǰ����ֵ
 * 2. ��������ܵ����˺�
 * 3. ���Ż���������Ч
 * 4. �ṩ����ֵ��ѯ�ӿ�
 */
class HomeManager : public Manager<HomeManager>
{
    friend class Manager<HomeManager>; // ����������˽�й��캯��

public:
    /**
     * @brief ��ȡ���ص�ǰ����ֵ
     * @return ���ص�ǰ������ֵ
     */
    double get_current_hp_num()
    {
        return num_hp;
    }

    /**
     * @brief ���ٻ�������ֵ
     * @param val ���ٵ�����ֵ��
     *
     * 1. ���ٻ�������ֵ
     * 2. ȷ������ֵ������0
     * 3. ���Ż���������Ч
     */
    void decrease_hp(double val)
    {
        // ��������ֵ
        num_hp -= val;

        // ȷ������ֵ������0
        if (num_hp < 0)
            num_hp = 0;

        // ��ȡ��Ч��Դ��
        static const ResourcesManager::SoundPool& sound_pool
            = ResourcesManager::instance()->get_sound_pool();

        // ���Ż���������Ч
        Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_HomeHurt)->second, 0);
    }

protected:
    /**
     * @brief ���캯������ʼ����������ֵ
     *
     * �����ù�������ȡ��ʼ����ֵ
     */
    HomeManager()
    {
        // �����û�ȡ��ʼ����ֵ
        num_hp = ConfigManager::instance()->num_initial_hp;
    }

    /// Ĭ����������
    ~HomeManager() = default;

private:
    double num_hp = 0; ///< ���ص�ǰ������ֵ
};

#endif // !_HOME_MANAGER_H_
