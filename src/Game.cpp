#include "Game.h"
#include <sstream>

Game::Game(int size) {
    board_.reset(size);
}

bool Game::place(int row, int col, std::string& feedback) {
    if (finished()) {
        feedback = "对局已结束：" + resultText_;
        return false;
    }
    if (!board_.inBounds(row, col)) {
        feedback = "落子位置越界，行、列需在 1~" + std::to_string(board_.size()) + " 之间。";
        return false;
    }
    std::string reason;
    if (!checkPlaceLegal(row, col, reason)) {
        feedback = reason;
        return false;
    }

    std::ostringstream mv;
    mv << stoneName(current_) << "落子 " << (row + 1) << "," << (col + 1);
    lastMoveText_ = mv.str();

    board_.set(row, col, current_);
    ++placeCount_;
    ++moveCount_;
    passStreak_ = 0;
    applyRuleEffect(row, col);
    checkEndCondition(row, col);

    if (playing()) {
        Stone next = opponentOf(current_);
        current_ = next;
        feedback = lastMoveText_ + "，轮到" + stoneName(next) + "行棋。";
    } else {
        feedback = lastMoveText_ + "。" + resultText_;
    }
    return true;
}

bool Game::pass(std::string& feedback) {
    if (finished()) {
        feedback = "对局已结束：" + resultText_;
        return false;
    }
    if (!supportsPass()) {
        feedback = std::string(typeName()) + "不支持虚着，请落子。";
        return false;
    }
    ++passStreak_;
    ++moveCount_;
    lastMoveText_ = std::string(stoneName(current_)) + "虚着";
    if (passStreak_ >= 2) {
        onDoublePass();
        feedback = "双方连续虚着，开始终局计数。" + resultText_;
    } else {
        Stone next = opponentOf(current_);
        current_ = next;
        feedback = std::string(stoneName(opponentOf(next))) + "虚着，轮到" + stoneName(next) + "行棋。";
    }
    return true;
}

void Game::resign(Stone who, std::string& feedback) {
    if (finished()) {
        feedback = "对局已结束：" + resultText_;
        return;
    }
    finish(who == STONE_BLACK ? STATE_WHITE_WIN : STATE_BLACK_WIN,
           std::string(stoneName(who)) + "投子认负，" +
           stoneName(opponentOf(who)) + "获胜。");
    feedback = resultText_;
}

GameMemento Game::createMemento() const {
    std::vector<int> rule;
    writeRuleData(rule);
    return GameMemento(typeCode(), board_.size(), board_.grid(), current_,
                       static_cast<int>(state_), passStreak_, moveCount_,
                       placeCount_, rule);
}

void Game::restoreMemento(const GameMemento& m) {
    board_.setGrid(m.grid());
    current_ = m.current();
    state_ = static_cast<GameState>(m.state());
    passStreak_ = m.passStreak();
    moveCount_ = m.moveCount();
    placeCount_ = m.placeCount();
    readRuleData(m.ruleData());
    onRestored();
    lastMoveText_ = "（悔棋或读档恢复）";
}

void Game::finish(GameState st, const std::string& text) {
    state_ = st;
    resultText_ = text;
}
