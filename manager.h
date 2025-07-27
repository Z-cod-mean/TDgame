#ifndef _MANAGER_H_
#define _MANAGER_H_

/**
 * @class Manager
 * @brief ͨ�õ���������ģ����
 *
 * �����ṩ�˵���ģʽ��ͨ��ʵ�֣�ȷ���κμ̳��Ը�ģ����඼ֻ��һ��ȫ��ʵ����
 * ʹ��CRTP��Curiously Recurring Template Pattern�����ģʽʵ�֡�
 *
 * @tparam T Ҫ����ľ������ͣ�����̳���Manager<T>��
 */
template <typename T>
class Manager
{
public:
    /**
     * @brief ��ȡ����ʵ��
     * @return ����T�ĵ���ʵ��ָ��
     *
     * ���ʵ�������ڣ��򴴽�һ����ʵ���������أ�
     * �̲߳���ȫ�����ⲿȷ���̰߳�ȫ��
     */
    static T* instance()
    {
        if (!manager)  // ���ʵ���Ƿ��Ѵ���
            manager = new T();  // ������ʵ��

        return manager;  // ���ص���ʵ��
    }

private:
    static T* manager;  ///< ����ʵ��ָ��

protected:
    /**
     * @brief �������캯��
     *
     * ��ֹ�ⲿֱ��ʵ����
     */
    Manager() = default;

    /**
     * @brief ������������
     */
    ~Manager() = default;

    /**
     * @brief ɾ���������캯��
     * ��ֹͨ������������ʵ��
     */
    Manager(const Manager&) = delete;

    /**
     * @brief ɾ����ֵ�����
     * ��ֹͨ����ֵ����ʵ��
     */
    Manager& operator=(const Manager&) = delete;
};

// ��̬��Ա������ʼ����ÿ��ģ���ػ����ж���ʵ����
template <typename T>
T* Manager<T>::manager = nullptr;  ///< ��ʼ��Ϊ��ָ��

#endif // !_MANAGER_H_