#pragma once
#include "Stone.h"
#include <string>
#include <vector>

// 棋盘尺寸限制
const int kMinBoardSize = 8;
const int kMaxBoardSize = 19;

// 备忘录：某一时刻对局的完整状态快照（供悔棋与存档共用）
// 棋子网格与规则附加数据（如围棋提子数）全部封在内部，外界只能整体传递
class GameMemento {
public:
    GameMemento() = default;

    GameMemento(int typeCode, int boardSize, const std::vector<Stone>& grid,
                Stone current, int state, int passStreak,
                int moveCount, int placeCount, const std::vector<int>& ruleData);

    int typeCode() const { return typeCode_; }
    int boardSize() const { return boardSize_; }
    const std::vector<Stone>& grid() const { return grid_; }
    Stone current() const { return current_; }
    int state() const { return state_; }
    int passStreak() const { return passStreak_; }
    int moveCount() const { return moveCount_; }
    int placeCount() const { return placeCount_; }
    const std::vector<int>& ruleData() const { return ruleData_; }

    // 存档序列化 / 反序列化，内容不合法返回 false
    std::string serialize() const;
    static bool deserialize(const std::string& text, GameMemento& out);

private:
    int typeCode_ = 0;        // 1 五子棋 2 围棋
    int boardSize_ = 0;
    std::vector<Stone> grid_;
    Stone current_ = STONE_BLACK;
    int state_ = 0;           // GameState
    int passStreak_ = 0;
    int moveCount_ = 0;
    int placeCount_ = 0;
    std::vector<int> ruleData_;   // 子类规则附加状态
};
