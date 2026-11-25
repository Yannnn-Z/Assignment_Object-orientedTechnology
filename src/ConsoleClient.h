#pragma once
#include "GameController.h"
#include "UIBuilder.h"
#include <string>

// 命令行客户端：负责接收并解析用户指令、转发给后端执行、渲染界面并反馈结果。
// 指令格式类异常（参数缺失、非数字、未知指令）在客户端判断；
// 业务规则类异常（位置占用/越界、棋盘尺寸、悔棋限制、存档合法性）由后端判断
class ConsoleClient {
public:
    ConsoleClient(GameController& controller, UIBuilder& builder);
    void run();
private:
    // 返回 false 表示退出程序
    bool handleCommand(const std::string& line, std::string& message);
    bool handleStart(const std::string& args, std::string& message);
    static bool parseInt(const std::string& text, int& value);
    GameController& controller_;
    UIBuilder& builder_;
    bool showHelp_ = true;
};
