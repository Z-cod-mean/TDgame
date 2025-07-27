#ifndef _MANAGER_H_
#define _MANAGER_H_

/**
 * @class Manager
 * @brief 通用单例管理器模板类
 *
 * 该类提供了单例模式的通用实现，确保任何继承自该模板的类都只有一个全局实例。
 * 使用CRTP（Curiously Recurring Template Pattern）设计模式实现。
 *
 * @tparam T 要管理的具体类型（必须继承自Manager<T>）
 */
template <typename T>
class Manager
{
public:
    /**
     * @brief 获取单例实例
     * @return 类型T的单例实例指针
     *
     * 如果实例不存在，则创建一个新实例（懒加载）
     * 线程不安全（需外部确保线程安全）
     */
    static T* instance()
    {
        if (!manager)  // 检查实例是否已创建
            manager = new T();  // 创建新实例

        return manager;  // 返回单例实例
    }

private:
    static T* manager;  ///< 单例实例指针

protected:
    /**
     * @brief 保护构造函数
     *
     * 防止外部直接实例化
     */
    Manager() = default;

    /**
     * @brief 保护析构函数
     */
    ~Manager() = default;

    /**
     * @brief 删除拷贝构造函数
     * 防止通过拷贝创建新实例
     */
    Manager(const Manager&) = delete;

    /**
     * @brief 删除赋值运算符
     * 防止通过赋值复制实例
     */
    Manager& operator=(const Manager&) = delete;
};

// 静态成员变量初始化（每个模板特化都有独立实例）
template <typename T>
T* Manager<T>::manager = nullptr;  ///< 初始化为空指针

#endif // !_MANAGER_H_