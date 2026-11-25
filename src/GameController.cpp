#include "GameController.h"
#include "GameFactory.h"
#include <fstream>
#include <sstream>

bool GameController::inProgress() const {
    return game_ != 0 && game_->playing();
}

bool GameController::startGame(int type, int size, std::string& feedback) {
    if (inProgress()) {
        feedback = "对局正在进行中，如需重开请先执行 restart。";
        return false;
    }
    std::unique_ptr<Game> game = GameFactory::create(type, size, feedback);
    if (!game) return false;
    game_ = std::move(game);
    undoStack_.clear();
    undoUsed_[0] = undoUsed_[1] = 0;
    lastType_ = type;
    lastSize_ = size;
    std::ostringstream t;
    t << game_->typeName() << "对局开始（" << size << "×" << size << "），黑方先行。";
    feedback = t.str();
    return true;
}

bool GameController::restart(std::string& feedback) {
    if (!inProgress()) {
        feedback = "当前没有进行中的对局，无法重新开始。";
        return false;
    }
    std::unique_ptr<Game> game = GameFactory::create(lastType_, lastSize_, feedback);
    if (!game) return false;
    game_ = std::move(game);
    undoStack_.clear();
    undoUsed_[0] = undoUsed_[1] = 0;
    std::ostringstream t;
    t << game_->typeName() << "对局已重新开始（" << lastSize_ << "×" << lastSize_ << "），黑方先行。";
    feedback = t.str();
    return true;
}

bool GameController::place(int row, int col, std::string& feedback) {
    if (!game_) {
        feedback = "尚未开始游戏，请先执行 start。";
        return false;
    }
    GameMemento snapshot = game_->createMemento();   // 先拍快照，落子成功才入栈
    if (!game_->place(row, col, feedback)) return false;
    undoStack_.push_back(snapshot);
    return true;
}

bool GameController::passTurn(std::string& feedback) {
    if (!game_) {
        feedback = "尚未开始游戏，请先执行 start。";
        return false;
    }
    GameMemento snapshot = game_->createMemento();
    if (!game_->pass(feedback)) return false;
    undoStack_.push_back(snapshot);
    return true;
}

bool GameController::undo(std::string& feedback) {
    if (!inProgress()) {
        feedback = "对局未在进行中，无法悔棋。";
        return false;
    }
    if (undoStack_.empty()) {
        feedback = "无棋可悔。";
        return false;
    }
    const Stone mover = opponentOf(game_->currentPlayer());   // 被撤销一手的行棋方
    const int idx = mover == STONE_BLACK ? 0 : 1;
    if (undoUsed_[idx] >= kUndoLimitPerPlayer) {
        feedback = std::string(stoneName(mover)) + "悔棋次数已达上限（" +
                   std::to_string(kUndoLimitPerPlayer) + " 次/局）。";
        return false;
    }
    game_->restoreMemento(undoStack_.back());
    undoStack_.pop_back();
    ++undoUsed_[idx];
    feedback = "悔棋成功，已撤销" + std::string(stoneName(mover)) +
               "的最近一手，轮到" + stoneName(game_->currentPlayer()) + "行棋。";
    return true;
}

void GameController::resign(std::string& feedback) {
    if (!inProgress()) {
        feedback = "对局未在进行中，无法认负。";
        return;
    }
    game_->resign(game_->currentPlayer(), feedback);
}

bool GameController::saveToFile(const std::string& path, std::string& feedback) {
    if (!game_) {
        feedback = "尚未开始游戏，无局面可保存。";
        return false;
    }
    std::ofstream out(path.c_str());
    if (!out) {
        feedback = "保存局面失败：无法写入文件 " + path + "。";
        return false;
    }
    out << game_->createMemento().serialize();
    if (!out) {
        feedback = "保存局面失败：写入文件 " + path + " 时出错。";
        return false;
    }
    feedback = "当前局面已保存到 " + path + "。";
    return true;
}

bool GameController::loadFromFile(const std::string& path, std::string& feedback) {
    std::ifstream in(path.c_str());
    if (!in) {
        feedback = "读取局面失败：文件 " + path + " 不存在。";
        return false;
    }
    std::ostringstream buf;
    buf << in.rdbuf();
    GameMemento m;
    if (!GameMemento::deserialize(buf.str(), m)) {
        feedback = "读取局面失败：文件 " + path + " 内容不合法。";
        return false;
    }
    std::string reason;
    std::unique_ptr<Game> game = GameFactory::create(m.typeCode(), m.boardSize(), reason);
    if (!game) {
        feedback = "读取局面失败：存档中的对局参数不合法。";
        return false;
    }
    game->restoreMemento(m);
    game_ = std::move(game);
    undoStack_.clear();
    undoUsed_[0] = undoUsed_[1] = 0;      // 读档覆盖当前局面，悔棋记录重新计
    lastType_ = m.typeCode();
    lastSize_ = m.boardSize();
    std::ostringstream t;
    t << "读取成功：已载入" << game_->typeName() << " " << m.boardSize() << "×"
      << m.boardSize() << " 局面（第 " << m.moveCount() << " 手），轮到"
      << stoneName(game_->currentPlayer()) << "行棋。";
    feedback = t.str();
    return true;
}
