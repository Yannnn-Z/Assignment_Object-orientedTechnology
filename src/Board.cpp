#include "Board.h"

void Board::reset(int size) {
    size_ = size;
    grid_.assign(static_cast<size_t>(size) * size, STONE_EMPTY);   // 复用已有容量
}

bool Board::inBounds(int row, int col) const {
    return row >= 0 && row < size_ && col >= 0 && col < size_;
}

Stone Board::at(int row, int col) const {
    return grid_[static_cast<size_t>(row) * size_ + col];
}

void Board::set(int row, int col, Stone s) {
    grid_[static_cast<size_t>(row) * size_ + col] = s;
}

bool Board::isFull() const {
    for (size_t i = 0; i < grid_.size(); ++i)
        if (grid_[i] == STONE_EMPTY) return false;
    return true;
}

int Board::count(Stone s) const {
    int n = 0;
    for (size_t i = 0; i < grid_.size(); ++i)
        if (grid_[i] == s) ++n;
    return n;
}

void Board::setGrid(const std::vector<Stone>& grid) {
    grid_ = grid;   // 尺寸由调用方保证一致，赋值复用容量
}
