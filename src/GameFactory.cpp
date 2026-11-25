#include "GameFactory.h"
#include "GameMemento.h"
#include "GoGame.h"
#include "GomokuGame.h"

bool GameFactory::typeValid(int type) {
    return type == TYPE_GOMOKU || type == TYPE_GO;
}

bool GameFactory::sizeValid(int size) {
    return size >= kMinBoardSize && size <= kMaxBoardSize;
}

std::unique_ptr<Game> GameFactory::create(int type, int size, std::string& feedback) {
    if (!typeValid(type)) {
        feedback = "游戏类型编号需为 1（五子棋）或 2（围棋）。";
        return std::unique_ptr<Game>();
    }
    if (!sizeValid(size)) {
        feedback = "棋盘大小需在 " + std::to_string(kMinBoardSize) + "~" +
                   std::to_string(kMaxBoardSize) + " 之间。";
        return std::unique_ptr<Game>();
    }
    if (type == TYPE_GOMOKU)
        return std::unique_ptr<Game>(new GomokuGame(size));
    return std::unique_ptr<Game>(new GoGame(size));
}
