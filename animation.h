#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "timer.h"

#include <SDL.h>
#include <vector>
#include <functional>

class Animation
{
public:
	//它定义了一个**可以存储“无参数、无返回值函数”**的变量类型，名字叫 PlayCallback
	typedef std::function<void()> PlayCallback;

public:
	Animation()
	{
		// 设置定时器为循环触发（不是只触发一次）
		timer.set_one_shot(false);

		// 设置定时器每次触发时要执行的动作（lambda 表达式）
		timer.set_on_timeout(
			[&]() // 捕获外部变量的引用，方便在 lambda 中访问类的成员
			{
				// 播放下一帧
				idx_frame++;

				// 如果已经超过最后一帧
				if (idx_frame >= rect_src_list.size())
				{
					// 如果是循环播放则回到第0帧；否则停在最后一帧
					idx_frame = is_loop ? 0 : rect_src_list.size() - 1;

					// 如果不是循环播放，并且设置了动画结束后的回调，就执行回调
					if (!is_loop && on_finished)
						on_finished();
				}
			}
		);
	}

	~Animation() = default;

	//重置动画状态
	void reset()
	{
		timer.restart();

		idx_frame = 0;
	}

	//把一张包含多帧的精灵图（Spritesheet）根据给定的分割方式，提取出你想播放的帧，并保存下来
	//给动画添加帧(要使用的纹理，纹理格子的水平数量,数值数量,使用了哪几个索引的格子)
	void set_frame_data(
		SDL_Texture* texture,      // 精灵表纹理
		int num_h,                 // 水平方向帧数（列数）
		int num_v,                 // 垂直方向帧数（行数）
		const std::vector<int>& idx_list // 需要使用的帧索引列表
	)
	{
		// 1. 获取整张贴图的宽高
		int width_tex, height_tex;
		this->texture = texture;
		SDL_QueryTexture(texture, nullptr, nullptr, &width_tex, &height_tex);
		//--------------------------------------------------------------------------------------

		// 2. 计算单帧宽高
		width_frame = width_tex / num_h;  // 水平方向单帧宽度
		height_frame = height_tex / num_v; // 垂直方向单帧高度
		//--------------------------------------------------------------------------------------

		// 3. 为每个索引分配一个裁剪区域（SDL_Rect）
		rect_src_list.resize(idx_list.size()); // 调整容器大小以容纳所有帧
		//--------------------------------------------------------------------------------------

		// 4. 遍历所有需要播放的帧索引
		for (size_t i = 0; i < idx_list.size(); i++)
		{
			int idx = idx_list[i]; // 当前帧的索引值
			SDL_Rect& rect_src = rect_src_list[i]; // 获取对应的矩形引用

			// 5. 计算帧在精灵表中的位置：
			rect_src.x = (idx % num_h) * width_frame;  // X坐标 = (索引 % 列数) * 单帧宽度
			rect_src.y = (idx / num_h) * height_frame; // Y坐标 = (索引 / 列数) * 单帧高度

			// 6. 设置帧的尺寸：
			rect_src.w = width_frame;  // 帧宽度
			rect_src.h = height_frame; // 帧高度
		}
	}

	//是否循环播放
	void set_loop(bool is_loop)
	{
		this->is_loop = is_loop;
	}
	//帧间隔多长时间
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
	// 渲染当前帧到指定位置，并可选择旋转角度
	void on_render(SDL_Renderer* renderer, const SDL_Point& pos_dst, double angle = 0) const
	{
		// 静态目标矩形，用于存储要渲染到屏幕上的区域
		static SDL_Rect rect_dst;

		// 设置目标矩形的位置（左上角坐标）
		rect_dst.x = pos_dst.x, rect_dst.y = pos_dst.y;
		rect_dst.w = width_frame, rect_dst.h = height_frame;
		// 渲染纹理的当前帧到目标位置，并按给定角度旋转，不进行翻转
		SDL_RenderCopyEx(
			renderer,                                    // 渲染器
			texture,                                     // 纹理对象
			&rect_src_list[idx_frame],                   // 纹理中当前帧的源矩形区域
			&rect_dst,                                   // 屏幕上的目标区域
			angle,                                       // 旋转角度（以中心为轴）
			nullptr,                                     // 旋转中心（nullptr 表示默认中心）
			SDL_RendererFlip::SDL_FLIP_NONE);            // 不进行镜像翻转
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
