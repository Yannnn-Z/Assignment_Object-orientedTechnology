#include "GameMemento.h"
#include <sstream>

GameMemento::GameMemento(int typeCode, int boardSize, const std::vector<Stone>& grid,
                         Stone current, int state, int passStreak,
                         int moveCount, int placeCount, const std::vector<int>& ruleData)
    : typeCode_(typeCode), boardSize_(boardSize), grid_(grid), current_(current),
      state_(state), passStreak_(passStreak), moveCount_(moveCount),
      placeCount_(placeCount), ruleData_(ruleData) {}

std::string GameMemento::serialize() const {
    std::ostringstream out;
    out << "BOARDGAME-SAVE-V1\n";
    out << typeCode_ << " " << boardSize_ << " " << current_ << " " << state_ << " "
        << passStreak_ << " " << moveCount_ << " " << placeCount_ << "\n";
    out << "grid " << grid_.size() << "\n";
    for (size_t i = 0; i < grid_.size(); ++i)
        out << static_cast<int>(grid_[i]);
    out << "\n";
    out << "rule " << ruleData_.size();
    for (size_t i = 0; i < ruleData_.size(); ++i)
        out << " " << ruleData_[i];
    out << "\n";
    return out.str();
}

bool GameMemento::deserialize(const std::string& text, GameMemento& out) {
    std::istringstream in(text);
    std::string header;
    if (!(in >> header) || header != "BOARDGAME-SAVE-V1") return false;

    int type = 0, size = 0, cur = 0, st = 0, ps = 0, mc = 0, pc = 0;
    if (!(in >> type >> size >> cur >> st >> ps >> mc >> pc)) return false;
    if (type != 1 && type != 2) return false;
    if (size < kMinBoardSize || size > kMaxBoardSize) return false;
    if (cur != STONE_BLACK && cur != STONE_WHITE) return false;
    if (st < 0 || st > 3 || ps < 0 || mc < 0 || pc < 0 || pc > mc) return false;

    std::string tag;
    long long gridLen = 0;
    if (!(in >> tag >> gridLen) || tag != "grid") return false;
    if (gridLen != static_cast<long long>(size) * size) return false;
    std::string gridText;
    if (!(in >> gridText) || gridText.size() != static_cast<size_t>(gridLen)) return false;
    std::vector<Stone> grid(static_cast<size_t>(gridLen), STONE_EMPTY);
    for (long long i = 0; i < gridLen; ++i) {
        const char ch = gridText[static_cast<size_t>(i)];
        if (ch < '0' || ch > '2') return false;
        grid[static_cast<size_t>(i)] = static_cast<Stone>(ch - '0');
    }

    int ruleN = 0;
    if (!(in >> tag >> ruleN) || tag != "rule" || ruleN < 0 || ruleN > 16) return false;
    std::vector<int> rule(static_cast<size_t>(ruleN), 0);
    for (int i = 0; i < ruleN; ++i)
        if (!(in >> rule[static_cast<size_t>(i)])) return false;

    out = GameMemento(type, size, grid, static_cast<Stone>(cur), st, ps, mc, pc, rule);
    return true;
}
