#ifndef _PLAYER_MANAGER_H_
#define _PLAYER_MANAGER_H_

#include "tile.h"
#include "facing.h"
#include "vector2.h"
#include "manager.h"
#include "animation.h"
#include "coin_manager.h"
#include "enemy_manager.h"
#include "resources_manager.h"

#include <SDL.h>

// ��ҹ����࣬�̳���Managerģ����
// ��������ҽ�ɫ��ص������߼������롢�ƶ���������������
class PlayerManager : public Manager<PlayerManager>
{
    friend class Manager<PlayerManager>;  // ������Ԫ������Managerģ�������˽�г�Ա

public:
    // ���������¼�
    // ������event - SDL�¼�����
    void on_input(const SDL_Event& event)
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:  // ���������¼�
            switch (event.key.keysym.sym)
            {
            case SDLK_a:    // A�� - �����ƶ�
                is_move_left = true;
                break;
            case SDLK_d:    // D�� - �����ƶ�
                is_move_right = true;
                break;
            case SDLK_w:    // W�� - �����ƶ�
                is_move_up = true;
                break;
            case SDLK_s:    // S�� - �����ƶ�
                is_move_down = true;
                break;
            case SDLK_j:    // J�� - �ͷ����⼼��
                on_release_flash();
                break;
            case SDLK_k:    // K�� - �ͷų������
                on_release_impact();
                break;
            default:
                break;
            }
            break;
        case SDL_KEYUP:    // �����ͷ��¼�
            switch (event.key.keysym.sym)
            {
            case SDLK_a:    // ֹͣ�����ƶ�
                is_move_left = false;
                break;
            case SDLK_d:    // ֹͣ�����ƶ�
                is_move_right = false;
                break;
            case SDLK_w:    // ֹͣ�����ƶ�
                is_move_up = false;
                break;
            case SDLK_s:    // ֹͣ�����ƶ�
                is_move_down = false;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    // ������Ϸ�߼�
    // ������delta - ������һ֡��ʱ����(��)
    void on_update(double delta)
    {
        // ���¼�ʱ��
        timer_auto_increase_mp.on_update(delta);       // MP�Զ��ָ���ʱ��
        timer_release_flash_cd.on_update(delta);        // ���⼼����ȴ��ʱ��

        // �����ƶ�������������һ��
        Vector2 direction =
            Vector2(is_move_right - is_move_left,
                is_move_down - is_move_up).normalize();
        // ����ʵ���ٶ�(����/��)
        velocity = direction * speed * SIZE_TILE;

        // ������������ͷż��ܣ������ƶ��߼�
        if (!is_releasing_flash && !is_releasing_impact)
        {
            // ����λ��
            position += velocity * delta;

            // ȷ����Ҳ����Ƴ���ͼ�߽�
            const SDL_Rect& rect_map = ConfigManager::instance()->rect_tile_map;
            if (position.x < rect_map.x) position.x = rect_map.x;
            if (position.x > rect_map.x + rect_map.w) position.x = rect_map.x + rect_map.w;
            if (position.y < rect_map.y) position.y = rect_map.y;
            if (position.y > rect_map.y + rect_map.h) position.y = rect_map.y + rect_map.h;

            // �����ƶ�������½�ɫ����
            if (velocity.y > 0) facing = Facing::Down;
            if (velocity.y < 0) facing = Facing::Up;
            if (velocity.x > 0) facing = Facing::Right;
            if (velocity.x < 0) facing = Facing::Left;

            // ���ݳ������ö�Ӧ��վ������
            switch (facing)
            {
            case Facing::Left:
                anim_current = &anim_idle_left;
                break;
            case Facing::Right:
                anim_current = &anim_idle_right;
                break;
            case Facing::Up:
                anim_current = &anim_idle_up;
                break;
            case Facing::Down:
                anim_current = &anim_idle_down;
                break;
            }
        }
        else
        {
            // ��������ͷż��ܣ������ù�������
            switch (facing)
            {
            case Facing::Left:
                anim_current = &anim_attack_left;
                break;
            case Facing::Right:
                anim_current = &anim_attack_right;
                break;
            case Facing::Up:
                anim_current = &anim_attack_up;
                break;
            case Facing::Down:
                anim_current = &anim_attack_down;
                break;
            }
        }

        // ���µ�ǰ����
        anim_current->on_update(delta);

        // �������⼼��Ч��
        if (is_releasing_flash)
        {
            // ����������Ч����
            anim_effect_flash_current->on_update(delta);

            // ��ȡ�����б������ײ
            EnemyManager::EnemyList& enemy_list
                = EnemyManager::instance()->get_enemy_list();
            for (Enemy* enemy : enemy_list)
            {
                if (enemy->can_remove())  // �����ѱ���Ƴ��ĵ���
                    continue;

                // �������Ƿ������⼼�ܵĹ�����Χ��
                const Vector2& position = enemy->get_position();
                if (position.x >= rect_hitbox_flash.x
                    && position.x <= rect_hitbox_flash.x + rect_hitbox_flash.w
                    && position.y >= rect_hitbox_flash.y
                    && position.y <= rect_hitbox_flash.y + rect_hitbox_flash.h)
                {
                    // �Ե�������˺�(�˺�ֵ��ʱ���ۼ�)
                    enemy->decrease_hp(ConfigManager::instance()->player_template.normal_attack_damage * delta);
                }
            }
        }

        // ����������Ч��
        if (is_releasing_impact)
        {
            // ���³����Ч����
            anim_effect_impact_current->on_update(delta);

            // ��ȡ�����б������ײ
            EnemyManager::EnemyList& enemy_list
                = EnemyManager::instance()->get_enemy_list();
            for (Enemy* enemy : enemy_list)
            {
                if (enemy->can_remove())  // �����ѱ���Ƴ��ĵ���
                    continue;

                // �������Ƿ��ڳ�����ܵĹ�����Χ��
                const Vector2& size = enemy->get_size();
                const Vector2& position = enemy->get_position();
                if (position.x >= rect_hitbox_impact.x
                    && position.x <= rect_hitbox_impact.x + rect_hitbox_impact.w
                    && position.y >= rect_hitbox_impact.y
                    && position.y <= rect_hitbox_impact.y + rect_hitbox_impact.h)
                {
                    // �Ե�������˺�������
                    enemy->decrease_hp(ConfigManager::instance()->player_template.skill_damage * delta);
                    enemy->slow_down();
                }
            }
        }

        // ������ҵ��ߵ���ײ
        CoinManager::CoinPropList& coin_prop_list = CoinManager::instance()->get_coin_prop_list();
        static const ResourcesManager::SoundPool& sound_pool = ResourcesManager::instance()->get_sound_pool();

        for (CoinProp* coin_prop : coin_prop_list)
        {
            if (coin_prop->can_remove())  // �������ռ��Ľ��
                continue;

            // �������Ƿ�������ײ
            const Vector2& pos_coin_prop = coin_prop->get_position();
            if (pos_coin_prop.x >= position.x - size.x / 2
                && pos_coin_prop.x <= position.x + size.x / 2
                && pos_coin_prop.y >= position.y - size.y / 2
                && pos_coin_prop.y <= position.y + size.y / 2)
            {
                // �ռ����
                coin_prop->make_invalid();
                CoinManager::instance()->increase_coin(10);

                // �����ռ���Ч
                Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_Coin)->second, 0);
            }
        }
    }

    // ��Ⱦ��ҽ�ɫ�ͼ�����Ч
    // ������renderer - SDL��Ⱦ��
    void on_render(SDL_Renderer* renderer)
    {
        static SDL_Point point;  // ���ڴ洢��Ⱦλ��

        // ��Ⱦ��ҽ�ɫ
        point.x = (int)(position.x - size.x / 2);
        point.y = (int)(position.y - size.y / 2);
        anim_current->on_render(renderer, point);

        // ��������ͷ����⼼�ܣ���Ⱦ������Ч
        if (is_releasing_flash)
        {
            point.x = rect_hitbox_flash.x;
            point.y = rect_hitbox_flash.y;
            anim_effect_flash_current->on_render(renderer, point);
        }

        // ��������ͷų�����ܣ���Ⱦ�����Ч
        if (is_releasing_impact)
        {
            point.x = rect_hitbox_impact.x;
            point.y = rect_hitbox_impact.y;
            anim_effect_impact_current->on_render(renderer, point);
        }
    }

    // ��ȡ��ǰMPֵ
    double get_current_mp() const
    {
        return mp;
    }

protected:
    // ���캯��(protected����Ϊ���ǵ�����)
    PlayerManager()
    {
        // ����MP�Զ��ָ���ʱ��
        timer_auto_increase_mp.set_one_shot(false);  // ѭ����ʱ��
        timer_auto_increase_mp.set_wait_time(0.1);   // ÿ0.1�봥��һ��
        timer_auto_increase_mp.set_on_timeout(
            [&]()  // ��ʱ���ص�����
            {
                // �������õĻָ��ٶ�����MP
                double interval = ConfigManager::instance()->player_template.skill_interval;
                mp = std::min(mp + 100 / (interval / 0.1), 100.0);  // ȷ�����������ֵ100
            });

        // �������⼼����ȴ��ʱ��
        timer_release_flash_cd.set_one_shot(true);  // ���μ�ʱ��
        timer_release_flash_cd.set_wait_time(
            ConfigManager::instance()->player_template.normal_attack_interval);  // �����ö�ȡ��ȴʱ��
        timer_release_flash_cd.set_on_timeout(
            [&]()  // ��ʱ���ص�����
            {
                can_release_flash = true;  // ��ȴ����������ٴ��ͷż���
            });

        // ����Դ��������ȡ����
        const ResourcesManager::TexturePool& tex_pool
            = ResourcesManager::instance()->get_texture_pool();

        SDL_Texture* tex_player = tex_pool.find(ResID::Tex_Player)->second;

        // ��ʼ��վ������(�ϡ��¡�����)
        anim_idle_up.set_loop(true); anim_idle_up.set_interval(0.1);
        anim_idle_up.set_frame_data(tex_player, 4, 8, { 4, 5, 6, 7 });
        anim_idle_down.set_loop(true); anim_idle_down.set_interval(0.1);
        anim_idle_down.set_frame_data(tex_player, 4, 8, { 0, 1, 2, 3 });
        anim_idle_left.set_loop(true); anim_idle_left.set_interval(0.1);
        anim_idle_left.set_frame_data(tex_player, 4, 8, { 8, 9, 10, 11 });
        anim_idle_right.set_loop(true); anim_idle_right.set_interval(0.1);
        anim_idle_right.set_frame_data(tex_player, 4, 8, { 12, 13, 14, 15 });

        // ��ʼ����������(�ϡ��¡�����)
        anim_attack_up.set_loop(true); anim_attack_up.set_interval(0.1);
        anim_attack_up.set_frame_data(tex_player, 4, 8, { 20, 21 });
        anim_attack_down.set_loop(true); anim_attack_down.set_interval(0.1);
        anim_attack_down.set_frame_data(tex_player, 4, 8, { 16, 17 });
        anim_attack_left.set_loop(true); anim_attack_left.set_interval(0.1);
        anim_attack_left.set_frame_data(tex_player, 4, 8, { 24, 25 });
        anim_attack_right.set_loop(true); anim_attack_right.set_interval(0.1);
        anim_attack_right.set_frame_data(tex_player, 4, 8, { 28, 29 });

        // ��ʼ�����⼼����Ч����(�ϡ��¡�����)
        anim_effect_flash_up.set_loop(false); anim_effect_flash_up.set_interval(0.1);
        anim_effect_flash_up.set_frame_data(tex_pool.find(ResID::Tex_EffectFlash_Up)->second, 5, 1, { 0, 1, 2, 3, 4 });
        anim_effect_flash_up.set_on_finished([&]() { is_releasing_flash = false; });  // ��������ʱ��Ǽ��ܽ���
        anim_effect_flash_down.set_loop(false); anim_effect_flash_down.set_interval(0.1);
        anim_effect_flash_down.set_frame_data(tex_pool.find(ResID::Tex_EffectFlash_Down)->second, 5, 1, { 4, 3, 2, 1, 0 });
        anim_effect_flash_down.set_on_finished([&]() { is_releasing_flash = false; });
        anim_effect_flash_left.set_loop(false); anim_effect_flash_left.set_interval(0.1);
        anim_effect_flash_left.set_frame_data(tex_pool.find(ResID::Tex_EffectFlash_Left)->second, 1, 5, { 4, 3, 2, 1, 0 });
        anim_effect_flash_left.set_on_finished([&]() { is_releasing_flash = false; });
        anim_effect_flash_right.set_loop(false); anim_effect_flash_right.set_interval(0.1);
        anim_effect_flash_right.set_frame_data(tex_pool.find(ResID::Tex_EffectFlash_Right)->second, 1, 5, { 0, 1, 2, 3, 4 });
        anim_effect_flash_right.set_on_finished([&]() { is_releasing_flash = false; });

        // ��ʼ�����������Ч����(�ϡ��¡�����)
        anim_effect_impact_up.set_loop(false); anim_effect_impact_up.set_interval(0.1);
        anim_effect_impact_up.set_frame_data(tex_pool.find(ResID::Tex_EffectImpact_Up)->second, 5, 1, { 0, 1, 2, 3, 4 });
        anim_effect_impact_up.set_on_finished([&]() { is_releasing_impact = false; });  // ��������ʱ��Ǽ��ܽ���
        anim_effect_impact_down.set_loop(false); anim_effect_impact_down.set_interval(0.1);
        anim_effect_impact_down.set_frame_data(tex_pool.find(ResID::Tex_EffectImpact_Down)->second, 5, 1, { 4, 3, 2, 1, 0 });
        anim_effect_impact_down.set_on_finished([&]() { is_releasing_impact = false; });
        anim_effect_impact_left.set_loop(false); anim_effect_impact_left.set_interval(0.1);
        anim_effect_impact_left.set_frame_data(tex_pool.find(ResID::Tex_EffectImpact_Left)->second, 1, 5, { 4, 3, 2, 1, 0 });
        anim_effect_impact_left.set_on_finished([&]() { is_releasing_impact = false; });
        anim_effect_impact_right.set_loop(false); anim_effect_impact_right.set_interval(0.1);
        anim_effect_impact_right.set_frame_data(tex_pool.find(ResID::Tex_EffectImpact_Right)->second, 1, 5, { 0, 1, 2, 3, 4 });
        anim_effect_impact_right.set_on_finished([&]() { is_releasing_impact = false; });

        // ��ʼ�����λ��(��ͼ����)
        const SDL_Rect& rect_map = ConfigManager::instance()->rect_tile_map;
        position.x = rect_map.x + rect_map.w / 2;
        position.y = rect_map.y + rect_map.h / 2;

        // �����ö�ȡ����ƶ��ٶ�
        speed = ConfigManager::instance()->player_template.speed;

        // ������ҽ�ɫ��С
        size.x = 96, size.y = 96;
    }

    // ��������
    ~PlayerManager() = default;

private:
    // �������
    Vector2 size;        // ��ҳߴ�(����)
    Vector2 position;    // ���λ��(��������)
    Vector2 velocity;    // ����ٶ�(����/��)

    // ���ܹ�����Χ
    SDL_Rect rect_hitbox_flash = { 0 };    // ���⼼�ܹ�����Χ
    SDL_Rect rect_hitbox_impact = { 0 };   // ������ܹ�����Χ

    double mp = 100;     // ħ��ֵ(0-100)
    double speed = 0;    // �ƶ��ٶ�

    // ����״̬��־
    bool can_release_flash = true;    // �Ƿ�����ͷ����⼼��
    bool is_releasing_flash = false;  // �Ƿ������ͷ����⼼��
    bool is_releasing_impact = false; // �Ƿ������ͷų������

    // �ƶ�״̬��־
    bool is_move_up = false;    // �Ƿ������ƶ�
    bool is_move_down = false;  // �Ƿ������ƶ�
    bool is_move_left = false;  // �Ƿ������ƶ�
    bool is_move_right = false; // �Ƿ������ƶ�

    // ������Դ
    Animation anim_idle_up;      // վ��(��)����
    Animation anim_idle_down;    // վ��(��)����
    Animation anim_idle_left;    // վ��(��)����
    Animation anim_idle_right;   // վ��(��)����
    Animation anim_attack_up;    // ����(��)����
    Animation anim_attack_down;  // ����(��)����
    Animation anim_attack_left;  // ����(��)����
    Animation anim_attack_right; // ����(��)����
    Animation* anim_current = &anim_idle_right;  // ��ǰ���ŵĶ���

    // ������Ч����
    Animation anim_effect_flash_up;     // ������Ч(��)
    Animation anim_effect_flash_down;   // ������Ч(��)
    Animation anim_effect_flash_left;   // ������Ч(��)
    Animation anim_effect_flash_right;  // ������Ч(��)
    Animation* anim_effect_flash_current = nullptr;  // ��ǰ������Ч����

    Animation anim_effect_impact_up;    // �����Ч(��)
    Animation anim_effect_impact_down;  // �����Ч(��)
    Animation anim_effect_impact_left;  // �����Ч(��)
    Animation anim_effect_impact_right; // �����Ч(��)
    Animation* anim_effect_impact_current = nullptr;  // ��ǰ�����Ч����

    // ��ʱ��
    Timer timer_release_flash_cd;    // ���⼼����ȴ��ʱ��
    Timer timer_auto_increase_mp;   // MP�Զ��ָ���ʱ��

    Facing facing = Facing::Left;   // ��ҵ�ǰ����(Ĭ��Ϊ��)

private:
    // �ͷ����⼼��
    void on_release_flash()
    {
        // ����Ƿ�����ͷż���
        if (!can_release_flash || is_releasing_flash)
            return;

        // ������ҳ������ò�ͬ�Ĺ�����Χ�Ͷ���
        switch (facing)
        {
        case Facing::Left:  // ���󹥻�
            anim_effect_flash_current = &anim_effect_flash_left;
            rect_hitbox_flash.x = (int)(position.x - size.x / 2 - 300);  // ������Χ�����300����
            rect_hitbox_flash.y = (int)(position.y - 68 / 2);           // �߶�68����
            rect_hitbox_flash.w = 300, rect_hitbox_flash.h = 68;
            break;
        case Facing::Right:  // ���ҹ���
            anim_effect_flash_current = &anim_effect_flash_right;
            rect_hitbox_flash.x = (int)(position.x + size.x / 2);  // ������Χ���Ҳ�300����
            rect_hitbox_flash.y = (int)(position.y - 68 / 2);
            rect_hitbox_flash.w = 300, rect_hitbox_flash.h = 68;
            break;
        case Facing::Up:  // ���Ϲ���
            anim_effect_flash_current = &anim_effect_flash_up;
            rect_hitbox_flash.x = (int)(position.x - 68 / 2);  // ���68����
            rect_hitbox_flash.y = (int)(position.y - size.x / 2 - 300);  // ������Χ���Ϸ�300����
            rect_hitbox_flash.w = 68, rect_hitbox_flash.h = 300;
            break;
        case Facing::Down:  // ���¹���
            anim_effect_flash_current = &anim_effect_flash_down;
            rect_hitbox_flash.x = (int)(position.x - 68 / 2);
            rect_hitbox_flash.y = (int)(position.y + size.x / 2);  // ������Χ���·�300����
            rect_hitbox_flash.w = 68, rect_hitbox_flash.h = 300;
            break;
        }

        // ���ü���״̬
        is_releasing_flash = true;
        anim_effect_flash_current->reset();  // ���ö���
        timer_release_flash_cd.restart();    // ��ʼ��ȴ��ʱ
        can_release_flash = false;           // ������ȴ״̬

        // ���ż�����Ч
        static const ResourcesManager::SoundPool& sound_pool
            = ResourcesManager::instance()->get_sound_pool();
        Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_Flash)->second, 0);
    }

    // �ͷų������(����ȫ��MP)
    void on_release_impact()
    {
        // ���MP�Ƿ��㹻��û�������ͷż���
        if (mp < 100 || is_releasing_impact)
            return;

        // ������ҳ������ò�ͬ�Ĺ�����Χ�Ͷ���
        switch (facing)
        {
        case Facing::Left:  // ���󹥻�
            anim_effect_impact_current = &anim_effect_impact_left;
            rect_hitbox_impact.x = (int)(position.x - size.x / 2 - 60);  // ������Χ�����60����
            rect_hitbox_impact.y = (int)(position.y - 140 / 2);         // �߶�140����
            rect_hitbox_impact.w = 60, rect_hitbox_impact.h = 140;
            break;
        case Facing::Right:  // ���ҹ���
            anim_effect_impact_current = &anim_effect_impact_right;
            rect_hitbox_impact.x = (int)(position.x + size.x / 2);  // ������Χ���Ҳ�60����
            rect_hitbox_impact.y = (int)(position.y - 140 / 2);
            rect_hitbox_impact.w = 60, rect_hitbox_impact.h = 140;
            break;
        case Facing::Up:  // ���Ϲ���
            anim_effect_impact_current = &anim_effect_impact_up;
            rect_hitbox_impact.x = (int)(position.x - 140 / 2);  // ���140����
            rect_hitbox_impact.y = (int)(position.y - size.x / 2 - 60);  // ������Χ���Ϸ�60����
            rect_hitbox_impact.w = 140, rect_hitbox_impact.h = 60;
            break;
        case Facing::Down:  // ���¹���
            anim_effect_impact_current = &anim_effect_impact_down;
            rect_hitbox_impact.x = (int)(position.x - 140 / 2);
            rect_hitbox_impact.y = (int)(position.y + size.x / 2);  // ������Χ���·�60����
            rect_hitbox_impact.w = 140, rect_hitbox_impact.h = 60;
            break;
        }

        // ����MP�����ü���״̬
        mp = 0;
        is_releasing_impact = true;
        anim_effect_impact_current->reset();  // ���ö���

        // ���ż�����Ч
        static const ResourcesManager::SoundPool& sound_pool
            = ResourcesManager::instance()->get_sound_pool();
        Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_Impact)->second, 0);
    }
};

#endif // !_PLAYER_MANAGER_H_
