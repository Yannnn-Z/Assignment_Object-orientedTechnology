#pragma once
#include "Game.h"
#include "GameMemento.h"
#include <memory>
#include <string>
#include <vector>

// 对局控制者：平台后端的外观，同时是备忘录的管理者。
// 客户端只与本类交互，统一处理开局、悔棋栈、存档读写与认负，
// 不接触任何具体棋种类——由此以同样的形式对待不同游戏类型
class GameController {
public:
    static const int kUndoLimitPerPlayer = 3;   // 每方每局悔棋上限

    bool startGame(int type, int size, std::string& feedback);   // 未开始或已结束时
    bool restart(std::string& feedback);                          // 对局进行中重开
    bool started() const { return game_ != 0; }
    bool inProgress() const;
    Game* game() { return game_.get(); }
    const Game* game() const { return game_.get(); }

    bool place(int row, int col, std::string& feedback);
    bool passTurn(std::string& feedback);
    bool undo(std::string& feedback);
    void resign(std::string& feedback);
    bool saveToFile(const std::string& path, std::string& feedback);
    bool loadFromFile(const std::string& path, std::string& feedback);
private:
    std::unique_ptr<Game> game_;
    std::vector<GameMemento> undoStack_;   // 悔棋快照栈
    int undoUsed_[2] = {0, 0};             // [0] 黑方已悔次数 [1] 白方已悔次数
    int lastType_ = 0, lastSize_ = 0;
};
