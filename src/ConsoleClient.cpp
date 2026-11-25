#include "ConsoleClient.h"
#include <cctype>
#include <iostream>
#include <sstream>

ConsoleClient::ConsoleClient(GameController& controller, UIBuilder& builder)
    : controller_(controller), builder_(builder) {}

bool ConsoleClient::parseInt(const std::string& text, int& value) {
    std::istringstream in(text);
    return (in >> value) && in.eof();
}

void ConsoleClient::run() {
    std::string message = "欢迎使用棋类对战平台！输入 start 开始对局。";
    std::string line;
    while (true) {
        RenderContext ctx(controller_.game(), message, showHelp_);
        UIDirector::display(builder_, ctx);
        std::cout << "\n请输入指令：";
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) {
            message = "请输入指令（hint on 可显示操作提示）。";
            continue;
        }
        if (!handleCommand(line, message)) break;
    }
    std::cout << "\n已退出棋类对战平台，再见！\n";
}

bool ConsoleClient::handleCommand(const std::string& line, std::string& message) {
    std::istringstream in(line);
    std::string cmd, rest;
    in >> cmd;
    std::getline(in, rest);
    while (!rest.empty() && rest[0] == ' ') rest.erase(rest.begin());
    for (size_t i = 0; i < cmd.size(); ++i)
        cmd[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(cmd[i])));

    if (cmd == "quit" || cmd == "exit") {
        message = "正在退出程序。";
        return false;
    }
    if (cmd == "start") {
        handleStart(rest, message);
        return true;
    }
    if (cmd == "restart")
        return controller_.restart(message), true;
    if (cmd == "move") {
        std::istringstream a(rest);
        int r = 0, c = 0;
        std::string extra;
        if (!(a >> r >> c) || (a >> extra)) {
            message = "指令格式错误，正确用法：move 行 列（1 起始的整数坐标）。";
            return true;
        }
        return controller_.place(r - 1, c - 1, message), true;
    }
    if (cmd == "pass")
        return controller_.passTurn(message), true;
    if (cmd == "undo")
        return controller_.undo(message), true;
    if (cmd == "resign") {
        controller_.resign(message);
        return true;
    }
    if (cmd == "save" || cmd == "load") {
        std::istringstream a(rest);
        std::string file, extra;
        if (!(a >> file) || (a >> extra) || file.empty()) {
            message = "指令格式错误，正确用法：" + cmd + " 文件名。";
            return true;
        }
        if (cmd == "save") controller_.saveToFile(file, message);
        else               controller_.loadFromFile(file, message);
        return true;
    }
    if (cmd == "hint") {
        std::istringstream a(rest);
        std::string arg;
        a >> arg;
        if (arg == "on")       showHelp_ = true;
        else if (arg == "off") showHelp_ = false;
        else if (arg.empty())  showHelp_ = !showHelp_;
        else {
            message = "指令格式错误，正确用法：hint on|off。";
            return true;
        }
        message = showHelp_ ? "已显示操作提示。" : "已隐藏操作提示（hint on 可重新显示）。";
        return true;
    }
    message = "未知指令：“" + cmd + "”。可执行 hint on 查看全部指令。";
    return true;
}

bool ConsoleClient::handleStart(const std::string& args, std::string& message) {
    int type = 0, size = 0;
    std::istringstream a(args);
    const bool hasArgs = static_cast<bool>(a >> type >> size);

    if (!hasArgs) {
        std::string line;
        std::cout << "请选择游戏类型（1-五子棋  2-围棋）：";
        if (!std::getline(std::cin, line) || !parseInt(line, type)) {
            message = "游戏类型需为整数 1 或 2，开局已取消。";
            return false;
        }
        std::cout << "请输入棋盘大小（8~19）：";
        if (!std::getline(std::cin, line) || !parseInt(line, size)) {
            message = "棋盘大小需为整数，开局已取消。";
            return false;
        }
    }
    controller_.startGame(type, size, message);
    return true;
}
