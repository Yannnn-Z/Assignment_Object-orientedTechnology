#pragma once
#include "Stone.h"
#include <vector>

// 棋盘：维护 size*size 的格子数组，网格缓冲在多局之间复用，避免重复分配
class Board {
public:
    void reset(int size);
    int size() const { return size_; }
    bool inBounds(int row, int col) const;          // 0-based 坐标
    Stone at(int row, int col) const;
    void set(int row, int col, Stone s);
    bool isFull() const;
    int count(Stone s) const;                        // 某色棋子总数（数子法用）
    const std::vector<Stone>& grid() const { return grid_; }
    void setGrid(const std::vector<Stone>& grid);    // 从快照整体恢复
private:
    int size_ = 0;
    std::vector<Stone> grid_;
};
