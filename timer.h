#ifndef _TIMER_H_
#define _TIMER_H_

#include <functional>  // ����std::function�ص�����

// ��ʱ����
// ���ܣ��ṩ��ʱ�����ص����ܣ�֧�ֵ���/ѭ��ģʽ����ͣ/�ָ���
class Timer
{
public:
    Timer() = default;    // Ĭ�Ϲ��캯��
    ~Timer() = default;   // Ĭ����������

    // ���ü�ʱ�������¿�ʼ��ʱ��
    void restart()
    {
        pass_time = 0;    // �����ѹ�ʱ��
        shotted = false;  // ���ô������
    }

    // ���õȴ�ʱ�䣨��λ���룩
    // ������val - �ȴ�ʱ�����룩
    void set_wait_time(double val)
    {
        wait_time = val;
    }

    // �����Ƿ�Ϊ���δ���ģʽ
    // ע�⣺��ǰʵ����bug������flagδ��ʹ�ã���������Ϊfalse
    void set_one_shot(bool flag)
    {
        one_shot = flag;  // Ӧ��Ϊ one_shot = flag;
    }

    // ���ó�ʱ�ص�����
    // ������on_timeout - ��ʱʱ���õĺ�������
    void set_on_timeout(std::function<void()> on_timeout)
    {
        this->on_timeout = on_timeout;
    }

    // ��ͣ��ʱ��
    void pause()
    {
        paused = true;
    }

    // �ָ���ʱ��������ͣ״̬������
    void resume()
    {
        paused = false;
    }

    // ���¼�ʱ��״̬
    // ������delta - �����ϴθ��µ�ʱ�������룩
    void on_update(double delta)
    {
        if (paused) return;  // ��ͣ״̬�²�����

        pass_time += delta;  // �ۼƾ���ʱ��

        // ����Ƿ�ﵽ�ȴ�ʱ��
        if (pass_time >= wait_time)
        {
            // �ж��Ƿ���������
            // 1. �ǵ���ģʽ �� 2. ����ģʽ��δ������
            bool can_shot = (!one_shot || (one_shot && !shotted));
            shotted = true;  // ���Ϊ�Ѵ���

            // ������������лص���������ִ�лص�
            if (can_shot && on_timeout)
                on_timeout();

            // ѭ��ģʽ�£��۳�һ���������ڵ�ʱ��
            pass_time -= wait_time;
        }
    }

private:
    double pass_time = 0;          // ���ۼƵ�ʱ�䣨�룩
    double wait_time = 0;          // �������ʱ�䣨�룩
    bool paused = false;           // �Ƿ���ͣ
    bool shotted = false;          // �Ƿ��Ѵ��������ڵ���ģʽ��
    bool one_shot = false;         // �Ƿ�Ϊ���δ���ģʽ����ǰʵ����bug������false��
    std::function<void()> on_timeout;  // ��ʱ�ص�����
};

#endif // !_TIMER_H_
