#include "GomokuGame.h"

GomokuGame::GomokuGame(int size) : Game(size) {}

bool GomokuGame::checkPlaceLegal(int row, int col, std::string& reason) const {
    if (board_.at(row, col) != STONE_EMPTY) {
        reason = "该位置已有棋子，请选择空点落子。";
        return false;
    }
    return true;
}

void GomokuGame::applyRuleEffect(int, int) {}

void GomokuGame::checkEndCondition(int row, int col) {
    static const int DR[4] = {0, 1, 1, 1};
    static const int DC[4] = {1, 0, 1, -1};
    for (int d = 0; d < 4; ++d) {
        int cnt = 1 + countLine(row, col, DR[d], DC[d]) + countLine(row, col, -DR[d], -DC[d]);
        if (cnt >= 5) {
            finish(current_ == STONE_BLACK ? STATE_BLACK_WIN : STATE_WHITE_WIN,
                   std::string(stoneName(current_)) + "连成五子，" + stoneName(current_) + "获胜！");
            return;
        }
    }
    if (board_.isFull())
        finish(STATE_DRAW, "棋盘已满，双方平局。");
}

int GomokuGame::countLine(int row, int col, int dr, int dc) const {
    int n = 0;
    int r = row + dr, c = col + dc;
    while (board_.inBounds(r, c) && board_.at(r, c) == current_) {
        ++n;
        r += dr;
        c += dc;
    }
    return n;
}
