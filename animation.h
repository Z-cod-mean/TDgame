#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "timer.h"

#include <SDL.h>
#include <vector>
#include <functional>

class Animation
{
public:
	//��������һ��**���Դ洢���޲������޷���ֵ������**�ı������ͣ����ֽ� PlayCallback
	typedef std::function<void()> PlayCallback;

public:
	Animation()
	{
		// ���ö�ʱ��Ϊѭ������������ֻ����һ�Σ�
		timer.set_one_shot(false);

		// ���ö�ʱ��ÿ�δ���ʱҪִ�еĶ�����lambda ���ʽ��
		timer.set_on_timeout(
			[&]() // �����ⲿ���������ã������� lambda �з�����ĳ�Ա
			{
				// ������һ֡
				idx_frame++;

				// ����Ѿ��������һ֡
				if (idx_frame >= rect_src_list.size())
				{
					// �����ѭ��������ص���0֡������ͣ�����һ֡
					idx_frame = is_loop ? 0 : rect_src_list.size() - 1;

					// �������ѭ�����ţ����������˶���������Ļص�����ִ�лص�
					if (!is_loop && on_finished)
						on_finished();
				}
			}
		);
	}

	~Animation() = default;

	//���ö���״̬
	void reset()
	{
		timer.restart();

		idx_frame = 0;
	}

	//��һ�Ű�����֡�ľ���ͼ��Spritesheet�����ݸ����ķָʽ����ȡ�����벥�ŵ�֡������������
	//���������֡(Ҫʹ�õ�����������ӵ�ˮƽ����,��ֵ����,ʹ�����ļ��������ĸ���)
	void set_frame_data(
		SDL_Texture* texture,      // ���������
		int num_h,                 // ˮƽ����֡����������
		int num_v,                 // ��ֱ����֡����������
		const std::vector<int>& idx_list // ��Ҫʹ�õ�֡�����б�
	)
	{
		// 1. ��ȡ������ͼ�Ŀ��
		int width_tex, height_tex;
		this->texture = texture;
		SDL_QueryTexture(texture, nullptr, nullptr, &width_tex, &height_tex);
		//--------------------------------------------------------------------------------------

		// 2. ���㵥֡���
		width_frame = width_tex / num_h;  // ˮƽ����֡���
		height_frame = height_tex / num_v; // ��ֱ����֡�߶�
		//--------------------------------------------------------------------------------------

		// 3. Ϊÿ����������һ���ü�����SDL_Rect��
		rect_src_list.resize(idx_list.size()); // ����������С����������֡
		//--------------------------------------------------------------------------------------

		// 4. ����������Ҫ���ŵ�֡����
		for (size_t i = 0; i < idx_list.size(); i++)
		{
			int idx = idx_list[i]; // ��ǰ֡������ֵ
			SDL_Rect& rect_src = rect_src_list[i]; // ��ȡ��Ӧ�ľ�������

			// 5. ����֡�ھ�����е�λ�ã�
			rect_src.x = (idx % num_h) * width_frame;  // X���� = (���� % ����) * ��֡���
			rect_src.y = (idx / num_h) * height_frame; // Y���� = (���� / ����) * ��֡�߶�

			// 6. ����֡�ĳߴ磺
			rect_src.w = width_frame;  // ֡���
			rect_src.h = height_frame; // ֡�߶�
		}
	}

	//�Ƿ�ѭ������
	void set_loop(bool is_loop)
	{
		this->is_loop = is_loop;
	}
	//֡����೤ʱ��
	void set_interval(double interval)
	{
		timer.set_wait_time(interval);
	}

	void set_on_finished(PlayCallback on_finished)
	{
		this->on_finished = on_finished;
	}

	void on_update(double delta)
	{
		timer.on_update(delta);
	}
	// ��Ⱦ��ǰ֡��ָ��λ�ã�����ѡ����ת�Ƕ�
	void on_render(SDL_Renderer* renderer, const SDL_Point& pos_dst, double angle = 0) const
	{
		// ��̬Ŀ����Σ����ڴ洢Ҫ��Ⱦ����Ļ�ϵ�����
		static SDL_Rect rect_dst;

		// ����Ŀ����ε�λ�ã����Ͻ����꣩
		rect_dst.x = pos_dst.x, rect_dst.y = pos_dst.y;
		rect_dst.w = width_frame, rect_dst.h = height_frame;
		// ��Ⱦ����ĵ�ǰ֡��Ŀ��λ�ã����������Ƕ���ת�������з�ת
		SDL_RenderCopyEx(
			renderer,                                    // ��Ⱦ��
			texture,                                     // �������
			&rect_src_list[idx_frame],                   // �����е�ǰ֡��Դ��������
			&rect_dst,                                   // ��Ļ�ϵ�Ŀ������
			angle,                                       // ��ת�Ƕȣ�������Ϊ�ᣩ
			nullptr,                                     // ��ת���ģ�nullptr ��ʾĬ�����ģ�
			SDL_RendererFlip::SDL_FLIP_NONE);            // �����о���ת
	}

private:
	Timer timer;
	bool is_loop = true;
	size_t idx_frame = 0;
	PlayCallback on_finished;
	SDL_Texture* texture = nullptr;
	std::vector<SDL_Rect> rect_src_list;
	int width_frame = 0, height_frame = 0;

};


#endif // !_ANIMATION_H_
