#ifndef _GAME_MANAGER_H_
#define _GAME_MANAGER_H_

// 游戏核心模块头文件
#include "banner.h"          // 游戏结束横幅
#include "manager.h"         // 单例管理器基类
#include "config_manager.h"  // 配置管理
#include "enemy_manager.h"   // 敌人管理
#include "wave_manager.h"    // 波次管理
#include "resources_manager.h" // 资源管理
#include "tower_manager.h"   // 防御塔管理
#include "bullet_manager.h"  // 子弹管理
#include "status_bar.h"      // 状态栏UI
#include "panel.h"          // 面板基类
#include "place_panel.h"    // 放置防御塔面板
#include "upgrade_panel.h"  // 升级面板
#include "player_manager.h"  // 玩家管理

// SDL库
#include <SDL.h>
#include <SDL_ttf.h>    // 字体渲染
#include <SDL_image.h>  // 图像加载
#include <SDL_mixer.h>  // 音频播放

/**
 * @class GameManager
 * @brief 游戏主控制器，继承自单例模板类
 *
 * 负责：
 * 1. SDL系统初始化/销毁
 * 2. 游戏主循环控制
 * 3. 输入/更新/渲染调度
 * 4. 游戏状态管理
 */
class GameManager : public Manager<GameManager>
{
    friend class Manager<GameManager>;  // 允许单例模板访问私有构造函数

public:
    /**
     * @brief 启动游戏主循环
     * @param argc 命令行参数计数
     * @param argv 命令行参数数组
     * @return 程序退出码
     */
    int run(int argc, char** argv)
    {
        // 淡入背景音乐（循环播放，淡入时间1500ms）
        Mix_FadeInMusic(ResourcesManager::instance()->get_music_pool().find(ResID::Music_BGM)->second, -1, 1500);

        // 初始化性能计数器（用于计算帧间隔）
        Uint64 last_counter = SDL_GetPerformanceCounter();
        const Uint64 counter_freq = SDL_GetPerformanceFrequency();

        // 主游戏循环
        while (!is_quit)
        {
            // 处理输入事件
            while (SDL_PollEvent(&event))
                on_input();

            // 计算帧间隔时间（delta time）
            Uint64 current_counter = SDL_GetPerformanceCounter();
            double delta = (double)(current_counter - last_counter) / counter_freq;
            last_counter = current_counter;

            // 帧率控制（锁定60FPS）
            if (delta * 1000 < 1000.0 / 60)
                SDL_Delay((Uint32)(1000.0 / 60 - delta * 1000));

            // 游戏逻辑更新
            on_update(delta);

            // 渲染准备（清屏为黑色）
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // 渲染游戏内容
            on_render();

            // 提交渲染
            SDL_RenderPresent(renderer);
        }

        return 0;
    }

protected:
    /**
     * @brief 构造函数（单例模式，仅内部可调用）
     *
     * 初始化顺序：
     * 1. SDL子系统
     * 2. 游戏配置
     * 3. 窗口和渲染器
     * 4. 游戏资源
     * 5. UI组件
     */
    GameManager()
    {
        // 初始化SDL子系统（带错误检查）
        init_assert(!SDL_Init(SDL_INIT_EVERYTHING), u8"SDL2 初始化失败！");
        init_assert(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG), u8"SDL_imgae 初始化失败！");
        init_assert(Mix_Init(MIX_INIT_MP3), u8"SDL_mixer 初始化失败！");
        init_assert(!TTF_Init(), u8"SDL_ttf 初始化失败！");

        // 音频设备设置（44.1kHz，立体声，2048样本缓冲区）
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");  // 启用输入法UI

        // 加载游戏配置
        ConfigManager* config = ConfigManager::instance();
        init_assert(config->map.load("map.csv"), u8"加载游戏地图失败！");
        init_assert(config->load_level_config("level.json"), u8"加载关卡配置失败！");
        init_assert(config->load_game_config("config.json"), u8"加载游戏配置失败！");

        // 创建游戏窗口（居中显示）
        window = SDL_CreateWindow(
            config->basic_template.window_title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            config->basic_template.window_width,
            config->basic_template.window_height,
            SDL_WINDOW_SHOWN
        );
        init_assert(window, u8"创建游戏窗口失败！");

        // 创建硬件加速渲染器（开启垂直同步）
        renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE
        );
        init_assert(renderer, u8"创建渲染器失败！");

        // 加载游戏资源（纹理、音效等）
        init_assert(ResourcesManager::instance()->load_from_file(renderer), u8"加载游戏资源失败！");
        init_assert(generate_tile_map_texture(), u8"生成瓦片地图纹理失败！");

        // 初始化UI组件
        status_bar.set_position(15, 15);
        banner = new Banner();
        place_panel = new PlacePanel();
        upgrade_panel = new UpgradePanel();
    }

    /**
     * @brief 析构函数
     *
     * 销毁顺序：
     * 1. SDL资源
     * 2. 子系统
     * 3. UI组件
     */
    ~GameManager()
    {
        // 销毁渲染器和窗口
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        // 关闭SDL子系统
        TTF_Quit();
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();

        // 释放UI组件
        delete banner;
        delete place_panel;
        delete upgrade_panel;
    }

private:
    // SDL事件对象
    SDL_Event event;
    bool is_quit = false;  // 游戏退出标志

    // UI组件
    StatusBar status_bar;  // 顶部状态栏

    // SDL图形资源
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* tex_tile_map = nullptr;  // 瓦片地图纹理

    // 游戏面板
    Panel* place_panel = nullptr;    // 防御塔放置面板
    Panel* upgrade_panel = nullptr;  // 防御塔升级面板
    Banner* banner = nullptr;        // 游戏结束横幅

private:
    /**
     * @brief 初始化断言检查
     * @param flag 检查条件，为false时触发错误
     * @param err_msg 错误提示信息（UTF-8编码）
     */
    void init_assert(bool flag, const char* err_msg)
    {
        if (flag) return;

        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"游戏启动失败", err_msg, window);
        exit(-1);
    }

    /**
     * @brief 处理输入事件
     *
     * 主要逻辑：
     * 1. 处理退出事件
     * 2. 鼠标点击时显示对应面板
     *   - 点击基地：显示升级面板
     *   - 点击空地：显示防御塔放置面板
     */
    void on_input()
    {
        static SDL_Point pos_center;          // 瓦片中心坐标（像素）
        static SDL_Point idx_tile_selected;   // 选中瓦片索引（网格坐标）
        static ConfigManager* instance = ConfigManager::instance();

        switch (event.type)
        {
        case SDL_QUIT:  // 窗口关闭事件
            is_quit = true;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (instance->is_game_over) break;  // 游戏结束时不响应

            // 获取鼠标点击位置的瓦片索引
            if (get_cursor_idx_tile(idx_tile_selected, event.motion.x, event.motion.y))
            {
                get_selected_tile_center_pos(pos_center, idx_tile_selected);

                if (check_home(idx_tile_selected))  // 点击的是基地
                {
                    upgrade_panel->set_idx_tile(idx_tile_selected);
                    upgrade_panel->set_center_pos(pos_center);
                    upgrade_panel->show();
                }
                else if (can_place_tower(idx_tile_selected))  // 点击的是可放置区域
                {
                    place_panel->set_idx_tile(idx_tile_selected);
                    place_panel->set_center_pos(pos_center);
                    place_panel->show();
                }
            }
            break;
        default:
            break;
        }

        // 游戏进行中时传递事件给各面板
        if (!instance->is_game_over)
        {
            place_panel->on_input(event);
            upgrade_panel->on_input(event);
            PlayerManager::instance()->on_input(event);
        }
    }

    /**
     * @brief 游戏逻辑更新
     * @param delta 帧间隔时间（秒）
     */
    void on_update(double delta)
    {
        static bool is_game_over_last_tick = false;
        static ConfigManager* instance = ConfigManager::instance();

        if (!instance->is_game_over)  // 游戏进行中
        {
            // 更新各子系统
            status_bar.on_update(renderer);
            place_panel->on_update(renderer);
            upgrade_panel->on_update(renderer);
            WaveManager::instance()->on_update(delta);
            EnemyManager::instance()->on_update(delta);
            CoinManager::instance()->on_update(delta);
            BulletManager::instance()->on_update(delta);
            TowerManager::instance()->on_update(delta);
            PlayerManager::instance()->on_update(delta);
            return;
        }

        // 游戏刚结束时触发音效
        if (!is_game_over_last_tick && instance->is_game_over)
        {
            static const ResourcesManager::SoundPool& sound_pool
                = ResourcesManager::instance()->get_sound_pool();

            Mix_FadeOutMusic(1500);  // 淡出背景音乐
            // 播放胜利/失败音效
            Mix_PlayChannel(-1, sound_pool.find(
                instance->is_game_win ? ResID::Sound_Win : ResID::Sound_Loss)->second, 0);
        }

        is_game_over_last_tick = instance->is_game_over;

        // 更新结束横幅
        banner->on_update(delta);
        if (banner->check_end_dispaly())  // 横幅显示完成后退出
            is_quit = true;
    }

    /**
     * @brief 渲染游戏画面
     */
    void on_render()
    {
        static ConfigManager* instance = ConfigManager::instance();
        static SDL_Rect& rect_dst = instance->rect_tile_map;

        // 1. 渲染瓦片地图背景
        SDL_RenderCopy(renderer, tex_tile_map, nullptr, &rect_dst);

        // 2. 渲染游戏实体（按从后到前顺序）
        EnemyManager::instance()->on_render(renderer);
        CoinManager::instance()->on_render(renderer);
        BulletManager::instance()->on_render(renderer);
        TowerManager::instance()->on_render(renderer);
        PlayerManager::instance()->on_render(renderer);

        if (!instance->is_game_over)  // 游戏进行中UI
        {
            place_panel->on_render(renderer);
            upgrade_panel->on_render(renderer);
            status_bar.on_render(renderer);
            return;
        }

        // 游戏结束时的横幅居中显示
        int width_screen, height_screen;
        SDL_GetWindowSizeInPixels(window, &width_screen, &height_screen);
        banner->set_center_position({ (double)width_screen / 2, (double)height_screen / 2 });
        banner->on_render(renderer);
    }

    /**
     * @brief 生成瓦片地图纹理
     * @return 是否生成成功
     *
     * 步骤：
     * 1. 创建空白纹理作为绘制目标
     * 2. 根据地图数据拼接瓦片集
     * 3. 标记基地位置
     */
    bool generate_tile_map_texture()
    {
        const Map& map = ConfigManager::instance()->map;
        const TileMap& tile_map = map.get_tile_map();
        SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;
        SDL_Texture* tex_tile_set = ResourcesManager::instance()->get_texture_pool().find(ResID::Tex_Tileset)->second;

        // 获取瓦片集纹理信息
        int width_tex_tile_set, height_tex_tile_set;
        SDL_QueryTexture(tex_tile_set, nullptr, nullptr, &width_tex_tile_set, &height_tex_tile_set);
        int num_tile_single_line = (int)std::ceil((double)width_tex_tile_set / SIZE_TILE);

        // 计算地图纹理尺寸（瓦片数量 × 单个瓦片尺寸）
        int width_tex_tile_map = (int)map.get_width() * SIZE_TILE;
        int height_tex_tile_map = (int)map.get_height() * SIZE_TILE;

        // 创建目标纹理（ARGB8888格式，支持透明度）
        tex_tile_map = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_TARGET, width_tex_tile_map, height_tex_tile_map);
        if (!tex_tile_map) return false;

        // 设置地图在窗口中的显示位置（居中）
        ConfigManager* config = ConfigManager::instance();
        rect_tile_map.x = (config->basic_template.window_width - width_tex_tile_map) / 2;
        rect_tile_map.y = (config->basic_template.window_height - height_tex_tile_map) / 2;
        rect_tile_map.w = width_tex_tile_map;
        rect_tile_map.h = height_tex_tile_map;

        // 准备渲染到纹理
        SDL_SetTextureBlendMode(tex_tile_map, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, tex_tile_map);

        // 绘制每个瓦片
        for (int y = 0; y < map.get_height(); y++)
        {
            for (int x = 0; x < map.get_width(); x++)
            {
                SDL_Rect rect_src;
                const Tile& tile = tile_map[y][x];

                // 目标位置（网格坐标转像素坐标）
                const SDL_Rect& rect_dst =
                {
                    x * SIZE_TILE, y * SIZE_TILE,
                    SIZE_TILE, SIZE_TILE
                };

                // 绘制地形（从瓦片集中选取对应部分）
                rect_src =
                {
                    (tile.terrian % num_tile_single_line) * SIZE_TILE,
                    (tile.terrian / num_tile_single_line) * SIZE_TILE,
                    SIZE_TILE, SIZE_TILE
                };
                SDL_RenderCopy(renderer, tex_tile_set, &rect_src, &rect_dst);

                // 绘制装饰物（如果有）
                if (tile.decoration >= 0)
                {
                    rect_src =
                    {
                        (tile.decoration % num_tile_single_line) * SIZE_TILE,
                        (tile.decoration / num_tile_single_line) * SIZE_TILE,
                        SIZE_TILE, SIZE_TILE
                    };
                    SDL_RenderCopy(renderer, tex_tile_set, &rect_src, &rect_dst);
                }
            }
        }

        // 标记基地位置（特殊纹理）
        const SDL_Point& idx_home = map.get_idx_home();
        const SDL_Rect rect_dst =
        {
            idx_home.x * SIZE_TILE, idx_home.y * SIZE_TILE,
            SIZE_TILE, SIZE_TILE
        };
        SDL_RenderCopy(renderer,
            ResourcesManager::instance()->get_texture_pool().find(ResID::Tex_Home)->second,
            nullptr,
            &rect_dst);

        // 恢复默认渲染目标
        SDL_SetRenderTarget(renderer, nullptr);

        return true;
    }

    /**
     * @brief 检查是否为基地瓦片
     * @param idx_tile_selected 待检查的瓦片索引
     */
    bool check_home(const SDL_Point& idx_tile_selected)
    {
        static const Map& map = ConfigManager::instance()->map;
        static const SDL_Point& idx_home = map.get_idx_home();

        return (idx_home.x == idx_tile_selected.x && idx_home.y == idx_tile_selected.y);
    }

    /**
     * @brief 获取鼠标位置对应的瓦片索引
     * @param idx_tile_selected 输出参数，返回瓦片索引
     * @param screen_x 鼠标X坐标（像素）
     * @param screen_y 鼠标Y坐标（像素）
     * @return 是否在有效地图区域内
     */
    bool get_cursor_idx_tile(SDL_Point& idx_tile_selected, int screen_x, int screen_y) const
    {
        static const Map& map = ConfigManager::instance()->map;
        static const SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;

        // 检查是否在地图显示区域内
        if (screen_x < rect_tile_map.x || screen_x > rect_tile_map.x + rect_tile_map.w
            || screen_y < rect_tile_map.y || screen_y > rect_tile_map.y + rect_tile_map.h)
            return false;

        // 计算网格坐标（限制在有效范围内）
        idx_tile_selected.x = std::min((screen_x - rect_tile_map.x) / SIZE_TILE, (int)map.get_width() - 1);
        idx_tile_selected.y = std::min((screen_y - rect_tile_map.y) / SIZE_TILE, (int)map.get_height() - 1);

        return true;
    }

    /**
     * @brief 检查瓦片是否可放置防御塔
     * @param idx_tile_selected 待检查的瓦片索引
     * @return 是否可放置
     *
     * 条件：
     * 1. 无装饰物
     * 2. 非路径方向
     * 3. 无现有防御塔
     */
    bool can_place_tower(const SDL_Point& idx_tile_selected) const
    {
        static const Map& map = ConfigManager::instance()->map;
        const Tile& tile = map.get_tile_map()[idx_tile_selected.y][idx_tile_selected.x];

        return (tile.decoration < 0 && tile.direction == Tile::Direction::None && !tile.has_tower);
    }

    /**
     * @brief 获取瓦片中心点的屏幕坐标
     * @param pos 输出参数，返回中心坐标
     * @param idx_tile_selected 瓦片索引
     */
    void get_selected_tile_center_pos(SDL_Point& pos, const SDL_Point& idx_tile_selected) const
    {
        static const SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;

        pos.x = rect_tile_map.x + idx_tile_selected.x * SIZE_TILE + SIZE_TILE / 2;
        pos.y = rect_tile_map.y + idx_tile_selected.y * SIZE_TILE + SIZE_TILE / 2;
    }
};

#endif // !_GAME_MANAGER_H_