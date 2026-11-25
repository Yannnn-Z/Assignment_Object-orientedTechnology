#pragma once
#include "Game.h"

// 五子棋：任一方向连成五子（含以上）即胜，棋盘下满则平局
class GomokuGame : public Game {
public:
    explicit GomokuGame(int size);
    const char* typeName() const override { return "五子棋"; }
    int typeCode() const override { return 1; }
    bool supportsPass() const override { return false; }
protected:
    bool checkPlaceLegal(int row, int col, std::string& reason) const override;
    void applyRuleEffect(int row, int col) override;          // 无额外规则
    void checkEndCondition(int row, int col) override;
private:
    // 沿 (dr,dc) 方向统计与 (row,col) 同色的连子数（不含自身）
    int countLine(int row, int col, int dr, int dc) const;
};
