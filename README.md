# 使用DirectComposition显示一张图片
![截图](DComposition.png)


## 一、概述
&#160; &#160; &#160; &#160;根据MSDN的文档描述，对于一个特定的窗口，可以调用`IDCompositionDevice::CreateTargetForHwnd`方法创建针对于
这个窗口的target。其中的`topmost`参数可以设置为TRUE或者FALSE。当为TRUE时，这个target的视觉树就会呈现在子
窗口的视觉树上面。当为FALSE时，相应地位于子窗口视觉树的下方。这个项目就是为了演示显示这个关系。

## 二、结构
&#160; &#160; &#160; &#160;主窗口上层的视觉树显示的是QQ.ico图片，下层的视觉树显示的是github.jpg图片。子窗口仅创建了一个上层的视觉树，为了
便于查看结果，这个视觉树仅仅是将背景刷为一个Alpha值为0.5的透明颜色。

## 三、结果
&#160; &#160; &#160; &#160;可以明显看到，淡紫色的背景位于github之上，QQ之下。