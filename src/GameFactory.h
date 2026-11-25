#pragma once
#include "Game.h"
#include <memory>
#include <string>

// 简单工厂：按棋种编号与棋盘尺寸创建对局对象
class GameFactory {
public:
    enum GameType { TYPE_GOMOKU = 1, TYPE_GO = 2 };
    static bool typeValid(int type);
    static bool sizeValid(int size);
    // type/size 不合法时返回空指针并给出原因
    static std::unique_ptr<Game> create(int type, int size, std::string& feedback);
};
