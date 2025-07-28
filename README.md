源自于B站UP主 voidmatrix的项目进行的二次创作
已完成代码注释 注意配置环境：SDL全家桶。

SDL2（核心库）
├── Video（图形渲染、窗口）
├── Audio（音频播放）
├── Events（事件系统）
├── Input（键盘、鼠标、手柄）
├── Timer（计时器）
├── Filesystem（文件路径工具）
├── Threads（多线程支持）
├── Joystick / GameController（手柄）
├── Hints（运行时配置）
└── Platform（平台封装，如 Windows, Linux, macOS）

| 模块名称                                        | 功能                             |
| ------------------------------------------- | ------------------------------ |
| **SDL\_video.h**                            | 创建窗口、渲染器、管理 OpenGL 上下文等        |
| **SDL\_render.h**                           | 渲染 2D 图像（基于 GPU）               |
| **SDL\_audio.h**                            | 播放音效、音乐，配置音频设备                 |
| **SDL\_events.h**                           | 捕捉事件（键盘、鼠标、窗口等）                |
| **SDL\_keyboard.h / SDL\_mouse.h**          | 输入设备处理                         |
| **SDL\_timer.h**                            | 获取时间戳，创建定时器                    |
| **SDL\_joystick.h / SDL\_gamecontroller.h** | 手柄和控制器支持                       |
| **SDL\_image.h**                            | 图片加载库（SDL\_image 扩展）           |
| **SDL\_ttf.h**                              | 字体渲染（SDL\_ttf 扩展）              |
| **SDL\_mixer.h**                            | 音效播放混音（SDL\_mixer 扩展）          |
| **SDL\_net.h**                              | 网络支持（UDP、TCP 简单封装，SDL\_net 扩展） |
