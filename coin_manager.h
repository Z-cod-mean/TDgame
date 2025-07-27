#ifndef _COIN_MANAGER_H_
#define _COIN_MANAGER_H_

#include "coin_prop.h"      // ��ҵ�����
#include "manager.h"        // ����������
#include "config_manager.h" // ���ù�����

#include <vector>           // ��������

/**
 * @class CoinManager
 * @brief ��ҹ������࣬������Ϸ�н��ϵͳ�Ĺ���
 *
 * ����ʹ�õ���ģʽʵ�֣�ȫ��Ψһ����Ҫ���ܰ�����
 * 1. �������ӵ�еĽ������
 * 2. �����ͼ�ϵ���Ľ�ҵ���
 * 3. �����ҵ������߼�
 * 4. ���º���Ⱦ��ҵ���
 */
class CoinManager : public Manager<CoinManager>
{
    friend class Manager<CoinManager>; // ����������˽�й��캯��

public:
    typedef std::vector<CoinProp*> CoinPropList; ///< ��ҵ����б�����

public:
    /**
     * @brief ������ҽ������
     * @param val ���ӵĽ������
     */
    void increase_coin(double val)
    {
        num_coin += val; // ֱ�����ӽ������
    }

    /**
     * @brief ������ҽ������
     * @param val ���ٵĽ������
     *
     * ������ٺ�������С��0��������Ϊ0
     */
    void decrease_coin(double val)
    {
        num_coin -= val; // ���ٽ������

        // ��ֹ���������Ϊ����
        if (num_coin < 0)
            num_coin = 0;
    }

    /**
     * @brief ÿ֡���½�ҵ���״̬
     * @param delta ֡ʱ���������룩
     *
     * 1. �������н�ҵ��߲�����״̬
     * 2. ������Ϊ���Ƴ��ĵ���
     */
    void on_update(double delta)
    {
        // �������н�ҵ���
        for (CoinProp* coin_prop : coin_prop_list)
            coin_prop->on_update(delta);

        // ������Ƴ��Ľ�ҵ��ߣ�ʹ��remove-erase���÷���
        coin_prop_list.erase(std::remove_if(coin_prop_list.begin(), coin_prop_list.end(),
            [](CoinProp* coin_prop) {
                bool deletable = coin_prop->can_remove(); // ����Ƿ���Ƴ�
                if (deletable) delete coin_prop;          // �ͷ��ڴ�
                return deletable;
            }),
            coin_prop_list.end());
    }

    /**
     * @brief ��Ⱦ���н�ҵ���
     * @param renderer SDL��Ⱦ��ָ��
     */
    void on_render(SDL_Renderer* renderer)
    {
        for (CoinProp* coin_prop : coin_prop_list)
            coin_prop->on_render(renderer);
    }

    /**
     * @brief ��ȡ��ǰ��ҽ������
     * @return ��ҵ�ǰӵ�еĽ������
     */
    double get_current_coin_num()
    {
        return num_coin;
    }

    /**
     * @brief ��ȡ��ҵ����б�����
     * @return ��ǰ���л�Ծ��ҵ��ߵ��б�
     */
    CoinPropList& get_coin_prop_list()
    {
        return coin_prop_list;
    }

    /**
     * @brief ���ɽ�ҵ���
     * @param position �������λ�ã���ά������
     *
     * ��ָ��λ�ô���һ���µĽ�ҵ��ߣ�����ӵ������б�
     */
    void spawn_coin_prop(const Vector2& position)
    {
        CoinProp* coin_prop = new CoinProp(); // �����½�ҵ���
        coin_prop->set_position(position);    // ����λ��

        // ��ӵ���ҵ����б�
        coin_prop_list.push_back(coin_prop);
    }

protected:
    /**
     * @brief ���캯������ʼ���������
     *
     * �����ù�������ȡ��ʼ�������
     */
    CoinManager()
    {
        // �����û�ȡ��ʼ�������
        num_coin = ConfigManager::instance()->num_initial_coin;
    }

    /**
     * @brief �����������������н�ҵ���
     */
    ~CoinManager()
    {
        // �ͷ����н�ҵ����ڴ�
        for (CoinProp* coin_prop : coin_prop_list)
            delete coin_prop;
    }

private:
    double num_coin = 0;       ///< ��ҵ�ǰӵ�еĽ������
    CoinPropList coin_prop_list; ///< �洢���л�Ծ��ҵ��ߵ��б�
};

#endif // !_COIN_MANAGER_H_
