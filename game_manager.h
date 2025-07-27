#ifndef _GAME_MANAGER_H_
#define _GAME_MANAGER_H_

// ��Ϸ����ģ��ͷ�ļ�
#include "banner.h"          // ��Ϸ�������
#include "manager.h"         // ��������������
#include "config_manager.h"  // ���ù���
#include "enemy_manager.h"   // ���˹���
#include "wave_manager.h"    // ���ι���
#include "resources_manager.h" // ��Դ����
#include "tower_manager.h"   // ����������
#include "bullet_manager.h"  // �ӵ�����
#include "status_bar.h"      // ״̬��UI
#include "panel.h"          // ������
#include "place_panel.h"    // ���÷��������
#include "upgrade_panel.h"  // �������
#include "player_manager.h"  // ��ҹ���

// SDL��
#include <SDL.h>
#include <SDL_ttf.h>    // ������Ⱦ
#include <SDL_image.h>  // ͼ�����
#include <SDL_mixer.h>  // ��Ƶ����

/**
 * @class GameManager
 * @brief ��Ϸ�����������̳��Ե���ģ����
 *
 * ����
 * 1. SDLϵͳ��ʼ��/����
 * 2. ��Ϸ��ѭ������
 * 3. ����/����/��Ⱦ����
 * 4. ��Ϸ״̬����
 */
class GameManager : public Manager<GameManager>
{
    friend class Manager<GameManager>;  // ������ģ�����˽�й��캯��

public:
    /**
     * @brief ������Ϸ��ѭ��
     * @param argc �����в�������
     * @param argv �����в�������
     * @return �����˳���
     */
    int run(int argc, char** argv)
    {
        // ���뱳�����֣�ѭ�����ţ�����ʱ��1500ms��
        Mix_FadeInMusic(ResourcesManager::instance()->get_music_pool().find(ResID::Music_BGM)->second, -1, 1500);

        // ��ʼ�����ܼ����������ڼ���֡�����
        Uint64 last_counter = SDL_GetPerformanceCounter();
        const Uint64 counter_freq = SDL_GetPerformanceFrequency();

        // ����Ϸѭ��
        while (!is_quit)
        {
            // ���������¼�
            while (SDL_PollEvent(&event))
                on_input();

            // ����֡���ʱ�䣨delta time��
            Uint64 current_counter = SDL_GetPerformanceCounter();
            double delta = (double)(current_counter - last_counter) / counter_freq;
            last_counter = current_counter;

            // ֡�ʿ��ƣ�����60FPS��
            if (delta * 1000 < 1000.0 / 60)
                SDL_Delay((Uint32)(1000.0 / 60 - delta * 1000));

            // ��Ϸ�߼�����
            on_update(delta);

            // ��Ⱦ׼��������Ϊ��ɫ��
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // ��Ⱦ��Ϸ����
            on_render();

            // �ύ��Ⱦ
            SDL_RenderPresent(renderer);
        }

        return 0;
    }

protected:
    /**
     * @brief ���캯��������ģʽ�����ڲ��ɵ��ã�
     *
     * ��ʼ��˳��
     * 1. SDL��ϵͳ
     * 2. ��Ϸ����
     * 3. ���ں���Ⱦ��
     * 4. ��Ϸ��Դ
     * 5. UI���
     */
    GameManager()
    {
        // ��ʼ��SDL��ϵͳ���������飩
        init_assert(!SDL_Init(SDL_INIT_EVERYTHING), u8"SDL2 ��ʼ��ʧ�ܣ�");
        init_assert(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG), u8"SDL_imgae ��ʼ��ʧ�ܣ�");
        init_assert(Mix_Init(MIX_INIT_MP3), u8"SDL_mixer ��ʼ��ʧ�ܣ�");
        init_assert(!TTF_Init(), u8"SDL_ttf ��ʼ��ʧ�ܣ�");

        // ��Ƶ�豸���ã�44.1kHz����������2048������������
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");  // �������뷨UI

        // ������Ϸ����
        ConfigManager* config = ConfigManager::instance();
        init_assert(config->map.load("map.csv"), u8"������Ϸ��ͼʧ�ܣ�");
        init_assert(config->load_level_config("level.json"), u8"���عؿ�����ʧ�ܣ�");
        init_assert(config->load_game_config("config.json"), u8"������Ϸ����ʧ�ܣ�");

        // ������Ϸ���ڣ�������ʾ��
        window = SDL_CreateWindow(
            config->basic_template.window_title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            config->basic_template.window_width,
            config->basic_template.window_height,
            SDL_WINDOW_SHOWN
        );
        init_assert(window, u8"������Ϸ����ʧ�ܣ�");

        // ����Ӳ��������Ⱦ����������ֱͬ����
        renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE
        );
        init_assert(renderer, u8"������Ⱦ��ʧ�ܣ�");

        // ������Ϸ��Դ��������Ч�ȣ�
        init_assert(ResourcesManager::instance()->load_from_file(renderer), u8"������Ϸ��Դʧ�ܣ�");
        init_assert(generate_tile_map_texture(), u8"������Ƭ��ͼ����ʧ�ܣ�");

        // ��ʼ��UI���
        status_bar.set_position(15, 15);
        banner = new Banner();
        place_panel = new PlacePanel();
        upgrade_panel = new UpgradePanel();
    }

    /**
     * @brief ��������
     *
     * ����˳��
     * 1. SDL��Դ
     * 2. ��ϵͳ
     * 3. UI���
     */
    ~GameManager()
    {
        // ������Ⱦ���ʹ���
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        // �ر�SDL��ϵͳ
        TTF_Quit();
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();

        // �ͷ�UI���
        delete banner;
        delete place_panel;
        delete upgrade_panel;
    }

private:
    // SDL�¼�����
    SDL_Event event;
    bool is_quit = false;  // ��Ϸ�˳���־

    // UI���
    StatusBar status_bar;  // ����״̬��

    // SDLͼ����Դ
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* tex_tile_map = nullptr;  // ��Ƭ��ͼ����

    // ��Ϸ���
    Panel* place_panel = nullptr;    // �������������
    Panel* upgrade_panel = nullptr;  // �������������
    Banner* banner = nullptr;        // ��Ϸ�������

private:
    /**
     * @brief ��ʼ�����Լ��
     * @param flag ���������Ϊfalseʱ��������
     * @param err_msg ������ʾ��Ϣ��UTF-8���룩
     */
    void init_assert(bool flag, const char* err_msg)
    {
        if (flag) return;

        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"��Ϸ����ʧ��", err_msg, window);
        exit(-1);
    }

    /**
     * @brief ���������¼�
     *
     * ��Ҫ�߼���
     * 1. �����˳��¼�
     * 2. �����ʱ��ʾ��Ӧ���
     *   - ������أ���ʾ�������
     *   - ����յأ���ʾ�������������
     */
    void on_input()
    {
        static SDL_Point pos_center;          // ��Ƭ�������꣨���أ�
        static SDL_Point idx_tile_selected;   // ѡ����Ƭ�������������꣩
        static ConfigManager* instance = ConfigManager::instance();

        switch (event.type)
        {
        case SDL_QUIT:  // ���ڹر��¼�
            is_quit = true;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (instance->is_game_over) break;  // ��Ϸ����ʱ����Ӧ

            // ��ȡ�����λ�õ���Ƭ����
            if (get_cursor_idx_tile(idx_tile_selected, event.motion.x, event.motion.y))
            {
                get_selected_tile_center_pos(pos_center, idx_tile_selected);

                if (check_home(idx_tile_selected))  // ������ǻ���
                {
                    upgrade_panel->set_idx_tile(idx_tile_selected);
                    upgrade_panel->set_center_pos(pos_center);
                    upgrade_panel->show();
                }
                else if (can_place_tower(idx_tile_selected))  // ������ǿɷ�������
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

        // ��Ϸ������ʱ�����¼��������
        if (!instance->is_game_over)
        {
            place_panel->on_input(event);
            upgrade_panel->on_input(event);
            PlayerManager::instance()->on_input(event);
        }
    }

    /**
     * @brief ��Ϸ�߼�����
     * @param delta ֡���ʱ�䣨�룩
     */
    void on_update(double delta)
    {
        static bool is_game_over_last_tick = false;
        static ConfigManager* instance = ConfigManager::instance();

        if (!instance->is_game_over)  // ��Ϸ������
        {
            // ���¸���ϵͳ
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

        // ��Ϸ�ս���ʱ������Ч
        if (!is_game_over_last_tick && instance->is_game_over)
        {
            static const ResourcesManager::SoundPool& sound_pool
                = ResourcesManager::instance()->get_sound_pool();

            Mix_FadeOutMusic(1500);  // ������������
            // ����ʤ��/ʧ����Ч
            Mix_PlayChannel(-1, sound_pool.find(
                instance->is_game_win ? ResID::Sound_Win : ResID::Sound_Loss)->second, 0);
        }

        is_game_over_last_tick = instance->is_game_over;

        // ���½������
        banner->on_update(delta);
        if (banner->check_end_dispaly())  // �����ʾ��ɺ��˳�
            is_quit = true;
    }

    /**
     * @brief ��Ⱦ��Ϸ����
     */
    void on_render()
    {
        static ConfigManager* instance = ConfigManager::instance();
        static SDL_Rect& rect_dst = instance->rect_tile_map;

        // 1. ��Ⱦ��Ƭ��ͼ����
        SDL_RenderCopy(renderer, tex_tile_map, nullptr, &rect_dst);

        // 2. ��Ⱦ��Ϸʵ�壨���Ӻ�ǰ˳��
        EnemyManager::instance()->on_render(renderer);
        CoinManager::instance()->on_render(renderer);
        BulletManager::instance()->on_render(renderer);
        TowerManager::instance()->on_render(renderer);
        PlayerManager::instance()->on_render(renderer);

        if (!instance->is_game_over)  // ��Ϸ������UI
        {
            place_panel->on_render(renderer);
            upgrade_panel->on_render(renderer);
            status_bar.on_render(renderer);
            return;
        }

        // ��Ϸ����ʱ�ĺ��������ʾ
        int width_screen, height_screen;
        SDL_GetWindowSizeInPixels(window, &width_screen, &height_screen);
        banner->set_center_position({ (double)width_screen / 2, (double)height_screen / 2 });
        banner->on_render(renderer);
    }

    /**
     * @brief ������Ƭ��ͼ����
     * @return �Ƿ����ɳɹ�
     *
     * ���裺
     * 1. �����հ�������Ϊ����Ŀ��
     * 2. ���ݵ�ͼ����ƴ����Ƭ��
     * 3. ��ǻ���λ��
     */
    bool generate_tile_map_texture()
    {
        const Map& map = ConfigManager::instance()->map;
        const TileMap& tile_map = map.get_tile_map();
        SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;
        SDL_Texture* tex_tile_set = ResourcesManager::instance()->get_texture_pool().find(ResID::Tex_Tileset)->second;

        // ��ȡ��Ƭ��������Ϣ
        int width_tex_tile_set, height_tex_tile_set;
        SDL_QueryTexture(tex_tile_set, nullptr, nullptr, &width_tex_tile_set, &height_tex_tile_set);
        int num_tile_single_line = (int)std::ceil((double)width_tex_tile_set / SIZE_TILE);

        // �����ͼ����ߴ磨��Ƭ���� �� ������Ƭ�ߴ磩
        int width_tex_tile_map = (int)map.get_width() * SIZE_TILE;
        int height_tex_tile_map = (int)map.get_height() * SIZE_TILE;

        // ����Ŀ������ARGB8888��ʽ��֧��͸���ȣ�
        tex_tile_map = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_TARGET, width_tex_tile_map, height_tex_tile_map);
        if (!tex_tile_map) return false;

        // ���õ�ͼ�ڴ����е���ʾλ�ã����У�
        ConfigManager* config = ConfigManager::instance();
        rect_tile_map.x = (config->basic_template.window_width - width_tex_tile_map) / 2;
        rect_tile_map.y = (config->basic_template.window_height - height_tex_tile_map) / 2;
        rect_tile_map.w = width_tex_tile_map;
        rect_tile_map.h = height_tex_tile_map;

        // ׼����Ⱦ������
        SDL_SetTextureBlendMode(tex_tile_map, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, tex_tile_map);

        // ����ÿ����Ƭ
        for (int y = 0; y < map.get_height(); y++)
        {
            for (int x = 0; x < map.get_width(); x++)
            {
                SDL_Rect rect_src;
                const Tile& tile = tile_map[y][x];

                // Ŀ��λ�ã���������ת�������꣩
                const SDL_Rect& rect_dst =
                {
                    x * SIZE_TILE, y * SIZE_TILE,
                    SIZE_TILE, SIZE_TILE
                };

                // ���Ƶ��Σ�����Ƭ����ѡȡ��Ӧ���֣�
                rect_src =
                {
                    (tile.terrian % num_tile_single_line) * SIZE_TILE,
                    (tile.terrian / num_tile_single_line) * SIZE_TILE,
                    SIZE_TILE, SIZE_TILE
                };
                SDL_RenderCopy(renderer, tex_tile_set, &rect_src, &rect_dst);

                // ����װ�������У�
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

        // ��ǻ���λ�ã���������
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

        // �ָ�Ĭ����ȾĿ��
        SDL_SetRenderTarget(renderer, nullptr);

        return true;
    }

    /**
     * @brief ����Ƿ�Ϊ������Ƭ
     * @param idx_tile_selected ��������Ƭ����
     */
    bool check_home(const SDL_Point& idx_tile_selected)
    {
        static const Map& map = ConfigManager::instance()->map;
        static const SDL_Point& idx_home = map.get_idx_home();

        return (idx_home.x == idx_tile_selected.x && idx_home.y == idx_tile_selected.y);
    }

    /**
     * @brief ��ȡ���λ�ö�Ӧ����Ƭ����
     * @param idx_tile_selected ���������������Ƭ����
     * @param screen_x ���X���꣨���أ�
     * @param screen_y ���Y���꣨���أ�
     * @return �Ƿ�����Ч��ͼ������
     */
    bool get_cursor_idx_tile(SDL_Point& idx_tile_selected, int screen_x, int screen_y) const
    {
        static const Map& map = ConfigManager::instance()->map;
        static const SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;

        // ����Ƿ��ڵ�ͼ��ʾ������
        if (screen_x < rect_tile_map.x || screen_x > rect_tile_map.x + rect_tile_map.w
            || screen_y < rect_tile_map.y || screen_y > rect_tile_map.y + rect_tile_map.h)
            return false;

        // �����������꣨��������Ч��Χ�ڣ�
        idx_tile_selected.x = std::min((screen_x - rect_tile_map.x) / SIZE_TILE, (int)map.get_width() - 1);
        idx_tile_selected.y = std::min((screen_y - rect_tile_map.y) / SIZE_TILE, (int)map.get_height() - 1);

        return true;
    }

    /**
     * @brief �����Ƭ�Ƿ�ɷ��÷�����
     * @param idx_tile_selected ��������Ƭ����
     * @return �Ƿ�ɷ���
     *
     * ������
     * 1. ��װ����
     * 2. ��·������
     * 3. �����з�����
     */
    bool can_place_tower(const SDL_Point& idx_tile_selected) const
    {
        static const Map& map = ConfigManager::instance()->map;
        const Tile& tile = map.get_tile_map()[idx_tile_selected.y][idx_tile_selected.x];

        return (tile.decoration < 0 && tile.direction == Tile::Direction::None && !tile.has_tower);
    }

    /**
     * @brief ��ȡ��Ƭ���ĵ����Ļ����
     * @param pos ���������������������
     * @param idx_tile_selected ��Ƭ����
     */
    void get_selected_tile_center_pos(SDL_Point& pos, const SDL_Point& idx_tile_selected) const
    {
        static const SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;

        pos.x = rect_tile_map.x + idx_tile_selected.x * SIZE_TILE + SIZE_TILE / 2;
        pos.y = rect_tile_map.y + idx_tile_selected.y * SIZE_TILE + SIZE_TILE / 2;
    }
};

#endif // !_GAME_MANAGER_H_