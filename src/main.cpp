// ============================================================
//  面向对象技术及应用大作业：棋类对战平台（第一阶段）
// --------------------------------------------
//  姓名：张南彦
//  学号：2026214282
// ============================================================
#include "ConsoleClient.h"
#include "GameController.h"
#include "UIBuilder.h"

// 平台入口：组合后端控制者与界面建造者，交给客户端驱动（不使用全局变量）
int main() {
    GameController controller;      // 平台后端
    ConsoleUIBuilder builder;       // 控制台界面建造者（第二阶段可替换为 GUI 建造者）
    ConsoleClient client(controller, builder);
    client.run();
    return 0;
}
