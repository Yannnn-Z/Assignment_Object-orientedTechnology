#pragma once
#include "Game.h"
#include <string>

// 渲染上下文：界面组件渲染所需的数据集合，由客户端组装
struct RenderContext {
    RenderContext(const Game* g, const std::string& msg, bool help)
        : game(g), message(msg), showHelp(help) {}
    const Game* game;          // 可为空（尚未开始游戏）
    const std::string& message;
    bool showHelp;
};

// 抽象建造者：界面由横幅、棋盘、状态、消息、帮助五种组件装配而成。
// 第二阶段若实现图形界面，只需新增一个具体建造者，装配流程（指挥者）不变
class UIBuilder {
public:
    virtual ~UIBuilder() = default;
    virtual void begin() = 0;
    virtual void buildBanner(const RenderContext& ctx) = 0;
    virtual void buildBoard(const RenderContext& ctx) = 0;
    virtual void buildStatus(const RenderContext& ctx) = 0;
    virtual void buildMessage(const RenderContext& ctx) = 0;
    virtual void buildHelp(const RenderContext& ctx) = 0;
    virtual void end() = 0;
};

// 控制台界面建造者：以字符画形式渲染各界面组件
class ConsoleUIBuilder : public UIBuilder {
public:
    void begin() override;
    void buildBanner(const RenderContext& ctx) override;
    void buildBoard(const RenderContext& ctx) override;
    void buildStatus(const RenderContext& ctx) override;
    void buildMessage(const RenderContext& ctx) override;
    void buildHelp(const RenderContext& ctx) override;
    void end() override;
};

// 指挥者：以固定顺序装配界面组件
class UIDirector {
public:
    static void display(UIBuilder& builder, const RenderContext& ctx);
};
