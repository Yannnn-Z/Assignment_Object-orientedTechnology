#pragma once
#include "Board.h"
#include "GameMemento.h"
#include "Stone.h"
#include <string>
#include <vector>

// 对局阶段
enum GameState {
    STATE_PLAYING = 0,
    STATE_BLACK_WIN = 1,
    STATE_WHITE_WIN = 2,
    STATE_DRAW = 3,
};

// 棋类对局抽象基类。
// place()/pass() 是模板方法：流程固定为“合法性校验 -> 落子 -> 规则效果 ->
// 记录 -> 终局判定 -> 交换行棋方”，各类棋的规则差异通过钩子函数由子类实现，
// 客户端因此可以以同样的形式对待不同棋种
class Game {
public:
    virtual ~Game() = default;

    // ---- 模板方法 ----
    bool place(int row, int col, std::string& feedback);   // 0-based 坐标
    bool pass(std::string& feedback);                      // 虚着（仅围棋）

    // ---- 直接操作 ----
    void resign(Stone who, std::string& feedback);         // 投子认负

    // ---- 状态查询 ----
    bool playing() const { return state_ == STATE_PLAYING; }
    bool finished() const { return state_ != STATE_PLAYING; }
    Stone currentPlayer() const { return current_; }
    const Board& board() const { return board_; }
    const std::string& resultText() const { return resultText_; }
    const std::string& lastMoveText() const { return lastMoveText_; }
    int moveCount() const { return moveCount_; }
    int placeCount() const { return placeCount_; }
    int passStreak() const { return passStreak_; }
    virtual const char* typeName() const = 0;
    virtual int typeCode() const = 0;                  // 1 五子棋 2 围棋
    virtual bool supportsPass() const = 0;
    virtual std::string extraStatus() const { return ""; }   // 状态栏附加信息

    // ---- 快照（供悔棋与存档）----
    GameMemento createMemento() const;
    void restoreMemento(const GameMemento& m);

protected:
    explicit Game(int size);

    // ---- 规则钩子 ----
    virtual bool checkPlaceLegal(int row, int col, std::string& reason) const = 0;
    virtual void applyRuleEffect(int row, int col) = 0;       // 围棋提子等
    virtual void checkEndCondition(int row, int col) = 0;     // 判定胜负或继续
    virtual void onDoublePass() {}                            // 连续两次虚着（围棋终局）
    virtual void writeRuleData(std::vector<int>& data) const { (void)data; }
    virtual void readRuleData(const std::vector<int>& data) { (void)data; }
    virtual void onRestored() {}                              // 快照恢复后的善后

    void finish(GameState st, const std::string& text);

    Board board_;
    Stone current_ = STONE_BLACK;
    GameState state_ = STATE_PLAYING;
    int passStreak_ = 0;
    int moveCount_ = 0;      // 落子 + 虚着
    int placeCount_ = 0;     // 仅实际落子
    std::string lastMoveText_ = "（无）";
    std::string resultText_;
};
