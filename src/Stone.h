#pragma once

// 棋子颜色：空点/黑/白。棋子用枚举值表示，落子只修改格子数值，
// 不为每颗棋子创建对象，避免频繁的对象创建与销毁开销
enum Stone {
    STONE_EMPTY = 0,
    STONE_BLACK = 1,
    STONE_WHITE = 2,
};

// 对手颜色
inline Stone opponentOf(Stone s) { return s == STONE_BLACK ? STONE_WHITE : STONE_BLACK; }

// 控制台显示符号：空点 · 黑 ● 白 ○
inline const char* stoneSymbol(Stone s) {
    switch (s) {
        case STONE_BLACK: return "●";
        case STONE_WHITE: return "○";
        default:          return "·";
    }
}

// 颜色中文名
inline const char* stoneName(Stone s) {
    switch (s) {
        case STONE_BLACK: return "黑方";
        case STONE_WHITE: return "白方";
        default:          return "空";
    }
}
