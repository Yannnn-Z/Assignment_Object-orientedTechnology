#include "GoGame.h"
#include <sstream>

static const int DR[4] = {-1, 1, 0, 0};
static const int DC[4] = {0, 0, -1, 1};

GoGame::GoGame(int size) : Game(size) {
    const size_t cells = static_cast<size_t>(size) * size;
    scratch_.assign(cells, STONE_EMPTY);
    mark_.assign(cells, 0);
    queue_.reserve(cells);
    groupBuf_.reserve(cells);
}

std::string GoGame::extraStatus() const {
    std::ostringstream out;
    out << " | 黑提子 " << captured_[0] << "，白提子 " << captured_[1];
    return out.str();
}

bool GoGame::collectGroup(const std::vector<Stone>& g, int row, int col,
                          std::vector<int>& groupOut) const {
    const int n = board_.size();
    const int start = row * n + col;
    const Stone color = g[static_cast<size_t>(start)];
    ++stamp_;
    queue_.clear();
    groupOut.clear();
    queue_.push_back(start);
    mark_[static_cast<size_t>(start)] = stamp_;
    groupOut.push_back(start);

    bool hasLiberty = false;
    for (size_t head = 0; head < queue_.size(); ++head) {
        const int idx = queue_[head];
        const int r = idx / n, c = idx % n;
        for (int d = 0; d < 4; ++d) {
            const int rr = r + DR[d], cc = c + DC[d];
            if (rr < 0 || rr >= n || cc < 0 || cc >= n) continue;
            const size_t ni = static_cast<size_t>(rr) * n + cc;
            const Stone v = g[ni];
            if (v == STONE_EMPTY) {
                hasLiberty = true;             // 发现气，继续遍历以收集完整块
            } else if (v == color && mark_[ni] != stamp_) {
                mark_[ni] = stamp_;
                queue_.push_back(static_cast<int>(ni));
                groupOut.push_back(static_cast<int>(ni));
            }
        }
    }
    return hasLiberty;
}

bool GoGame::checkPlaceLegal(int row, int col, std::string& reason) const {
    if (board_.at(row, col) != STONE_EMPTY) {
        reason = "该位置已有棋子，请选择空点落子。";
        return false;
    }

    // 在模拟网格上完成“落子 -> 提对方无气块 -> 检查自身气”
    scratch_ = board_.grid();                       // 赋值复用已有容量
    const int n = board_.size();
    scratch_[static_cast<size_t>(row) * n + col] = current_;
    pendingRemoved_ = 0;
    for (int d = 0; d < 4; ++d) {
        const int r = row + DR[d], c = col + DC[d];
        if (r < 0 || r >= n || c < 0 || c >= n) continue;
        const size_t ni = static_cast<size_t>(r) * n + c;
        if (scratch_[ni] != opponentOf(current_)) continue;
        groupBuf_.clear();
        if (!collectGroup(scratch_, r, c, groupBuf_)) {   // 相邻对方块无气 -> 提掉
            for (size_t i = 0; i < groupBuf_.size(); ++i)
                scratch_[static_cast<size_t>(groupBuf_[i])] = STONE_EMPTY;
            pendingRemoved_ += static_cast<int>(groupBuf_.size());
        }
    }
    groupBuf_.clear();
    if (!collectGroup(scratch_, row, col, groupBuf_)) {
        reason = "落子后己方棋子无气（自杀），该点为禁着点。";
        return false;
    }
    for (size_t i = 0; i < positions_.size(); ++i) {
        if (positions_[i] == scratch_) {
            reason = "落子将重现历史局面（打劫/全局同形），该点为禁着点。";
            return false;
        }
    }
    return true;
}

void GoGame::applyRuleEffect(int row, int col) {
    (void)row;
    (void)col;
    board_.setGrid(scratch_);                       // 应用模拟结果（含提子）
    positions_.push_back(scratch_);                 // 记录新局面
    if (pendingRemoved_ > 0) {
        captured_[current_ == STONE_BLACK ? 0 : 1] += pendingRemoved_;
        lastMoveText_ += "（提子 " + std::to_string(pendingRemoved_) + " 枚）";
    }
}

void GoGame::onDoublePass() {
    // 中国规则数子法：得分 = 子数 + 只邻本方的空点数；公气不计入任何一方
    const int n = board_.size();
    const std::vector<Stone>& g = board_.grid();
    ++stamp_;
    long long blackTerr = 0, whiteTerr = 0;
    for (int i = 0; i < n * n; ++i) {
        const size_t si = static_cast<size_t>(i);
        if (g[si] != STONE_EMPTY || mark_[si] == stamp_) continue;
        queue_.clear();
        queue_.push_back(i);
        mark_[si] = stamp_;
        long long cnt = 0;
        bool touchBlack = false, touchWhite = false;
        for (size_t head = 0; head < queue_.size(); ++head) {
            const int idx = queue_[head];
            ++cnt;
            const int r = idx / n, c = idx % n;
            for (int d = 0; d < 4; ++d) {
                const int rr = r + DR[d], cc = c + DC[d];
                if (rr < 0 || rr >= n || cc < 0 || cc >= n) continue;
                const size_t ni = static_cast<size_t>(rr) * n + cc;
                if (g[ni] == STONE_EMPTY) {
                    if (mark_[ni] != stamp_) {
                        mark_[ni] = stamp_;
                        queue_.push_back(static_cast<int>(ni));
                    }
                } else if (g[ni] == STONE_BLACK) {
                    touchBlack = true;
                } else {
                    touchWhite = true;
                }
            }
        }
        if (touchBlack && !touchWhite)      blackTerr += cnt;
        else if (touchWhite && !touchBlack) whiteTerr += cnt;
    }

    const long long black = board_.count(STONE_BLACK) + blackTerr;
    const long long white = board_.count(STONE_WHITE) + whiteTerr;
    const double diff = static_cast<double>(black - white);   // 黑贴 3.75 子 = 目差 7.5
    std::ostringstream t;
    t << "终局数子（中国规则，黑贴 3.75 子）：黑 " << black << " 子，白 " << white << " 子";
    if (diff > 7.5)
        finish(STATE_BLACK_WIN, t.str() + "，黑方获胜。");
    else if (diff < 7.5)
        finish(STATE_WHITE_WIN, t.str() + "，白方获胜。");
    else
        finish(STATE_DRAW, t.str() + "，双方平局。");
}

void GoGame::writeRuleData(std::vector<int>& data) const {
    data.push_back(captured_[0]);
    data.push_back(captured_[1]);
}

void GoGame::readRuleData(const std::vector<int>& data) {
    if (data.size() >= 2) {
        captured_[0] = data[0];
        captured_[1] = data[1];
    }
}

void GoGame::onRestored() {
    // positions_ 与实际落子数一一对应；读档后历史局面未知，仅保留可确定的部分
    positions_.resize(static_cast<size_t>(placeCount_));
}
