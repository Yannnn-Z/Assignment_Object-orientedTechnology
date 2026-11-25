#include "UIBuilder.h"
#include <iomanip>
#include <iostream>

void ConsoleUIBuilder::begin() {
    std::cout << "\n==============================================\n";
}

void ConsoleUIBuilder::buildBanner(const RenderContext&) {
    std::cout << "  棋类对战平台（五子棋 / 围棋）\n";
}

void ConsoleUIBuilder::buildBoard(const RenderContext& ctx) {
    if (!ctx.game) {
        std::cout << "  [棋盘] 尚未开始游戏，执行 start 开局\n";
        return;
    }
    const Board& b = ctx.game->board();
    const int n = b.size();
    std::cout << "  [棋盘]  ";
    for (int c = 0; c < n; ++c) std::cout << " " << std::setw(2) << (c + 1);
    std::cout << "\n";
    for (int r = 0; r < n; ++r) {
        std::cout << "          " << std::setw(2) << (r + 1);
        for (int c = 0; c < n; ++c)
            std::cout << "  " << stoneSymbol(b.at(r, c));
        std::cout << "\n";
    }
}

void ConsoleUIBuilder::buildStatus(const RenderContext& ctx) {
    if (!ctx.game) {
        std::cout << "  [状态] 尚未开始游戏\n";
        return;
    }
    const Game* g = ctx.game;
    std::cout << "  [状态] " << g->typeName() << " " << g->board().size() << "×"
              << g->board().size() << " | 第 " << g->moveCount() << " 手 | 最近一手："
              << g->lastMoveText() << g->extraStatus();
    if (g->playing())
        std::cout << " | 轮到" << stoneName(g->currentPlayer()) << "行棋\n";
    else
        std::cout << " | " << g->resultText() << "\n";
}

void ConsoleUIBuilder::buildMessage(const RenderContext& ctx) {
    std::cout << "  [反馈] " << ctx.message << "\n";
}

void ConsoleUIBuilder::buildHelp(const RenderContext& ctx) {
    if (!ctx.showHelp) return;
    std::cout <<
        "  [提示] start [类型 尺寸]  开始对局（类型 1-五子棋 2-围棋，尺寸 8~19）\n"
        "         move 行 列        落子          pass   虚着（仅围棋）\n"
        "         undo              悔棋一步      resign 投子认负\n"
        "         restart           重新开始      quit   退出程序\n"
        "         save 文件 / load 文件           保存 / 读取局面\n"
        "         hint on|off       显示 / 隐藏本提示\n";
}

void ConsoleUIBuilder::end() {
    std::cout << "==============================================";
}

void UIDirector::display(UIBuilder& builder, const RenderContext& ctx) {
    builder.begin();
    builder.buildBanner(ctx);
    builder.buildBoard(ctx);
    builder.buildStatus(ctx);
    builder.buildMessage(ctx);
    builder.buildHelp(ctx);
    builder.end();
}
