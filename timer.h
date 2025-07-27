#ifndef _TIMER_H_
#define _TIMER_H_

#include <functional>  // 用于std::function回调函数

// 计时器类
// 功能：提供定时触发回调功能，支持单次/循环模式、暂停/恢复等
class Timer
{
public:
    Timer() = default;    // 默认构造函数
    ~Timer() = default;   // 默认析构函数

    // 重置计时器（重新开始计时）
    void restart()
    {
        pass_time = 0;    // 重置已过时间
        shotted = false;  // 重置触发标记
    }

    // 设置等待时间（单位：秒）
    // 参数：val - 等待时长（秒）
    void set_wait_time(double val)
    {
        wait_time = val;
    }

    // 设置是否为单次触发模式
    // 注意：当前实现有bug，参数flag未被使用，总是设置为false
    void set_one_shot(bool flag)
    {
        one_shot = flag;  // 应改为 one_shot = flag;
    }

    // 设置超时回调函数
    // 参数：on_timeout - 超时时调用的函数对象
    void set_on_timeout(std::function<void()> on_timeout)
    {
        this->on_timeout = on_timeout;
    }

    // 暂停计时器
    void pause()
    {
        paused = true;
    }

    // 恢复计时器（从暂停状态继续）
    void resume()
    {
        paused = false;
    }

    // 更新计时器状态
    // 参数：delta - 距离上次更新的时间间隔（秒）
    void on_update(double delta)
    {
        if (paused) return;  // 暂停状态下不更新

        pass_time += delta;  // 累计经过时间

        // 检查是否达到等待时间
        if (pass_time >= wait_time)
        {
            // 判断是否允许触发：
            // 1. 非单次模式 或 2. 单次模式且未触发过
            bool can_shot = (!one_shot || (one_shot && !shotted));
            shotted = true;  // 标记为已触发

            // 如果允许触发且有回调函数，则执行回调
            if (can_shot && on_timeout)
                on_timeout();

            // 循环模式下，扣除一个完整周期的时间
            pass_time -= wait_time;
        }
    }

private:
    double pass_time = 0;          // 已累计的时间（秒）
    double wait_time = 0;          // 触发间隔时间（秒）
    bool paused = false;           // 是否暂停
    bool shotted = false;          // 是否已触发（用于单次模式）
    bool one_shot = false;         // 是否为单次触发模式（当前实现有bug，总是false）
    std::function<void()> on_timeout;  // 超时回调函数
};

#endif // !_TIMER_H_
