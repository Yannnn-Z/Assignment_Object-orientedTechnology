#pragma once
#include "Game.h"

// 围棋：支持提子、自杀禁着、全局同形禁着（打劫）、虚着与数子法终局。
// 合法性检查在复用的模拟网格上完成，搜索用的标记/队列缓冲均为成员变量，
// 避免每次落子反复创建对象
class GoGame : public Game {
public:
    explicit GoGame(int size);
    const char* typeName() const override { return "围棋"; }
    int typeCode() const override { return 2; }
    bool supportsPass() const override { return true; }
    std::string extraStatus() const override;      // 提子统计
protected:
    bool checkPlaceLegal(int row, int col, std::string& reason) const override;
    void applyRuleEffect(int row, int col) override;
    void checkEndCondition(int, int) override {}   // 围棋落子不直接终局
    void onDoublePass() override;                  // 双虚着 -> 数子法
    void writeRuleData(std::vector<int>& data) const override;
    void readRuleData(const std::vector<int>& data) override;
    void onRestored() override;
private:
    // 在网格 g 上收集 (row,col) 所在同色连通块：返回该块是否有气
    bool collectGroup(const std::vector<Stone>& g, int row, int col,
                      std::vector<int>& groupOut) const;

    mutable std::vector<Stone> scratch_;          // 模拟落子结果网格（复用）
    mutable std::vector<int> mark_;               // 访问标记（时间戳法，免清零）
    mutable std::vector<int> queue_;              // BFS 队列缓冲
    mutable std::vector<int> groupBuf_;           // 连通块收集缓冲
    mutable int stamp_ = 0;
    mutable int pendingRemoved_ = 0;              // 模拟出的本次提子数
    std::vector<std::vector<Stone>> positions_;   // 每次落子后的局面（同形禁着）
    int captured_[2] = {0, 0};                    // [0] 黑方提子数 [1] 白方提子数
};
