// 需求 4：掉头指令 TR

#include <gtest/gtest.h>

#include "executor/action.h"
#include "executor/executor.h"
#include "test_support.h"

using executor::Action;
using executor::ActionList;
using executor::Executor;
using executor::kEast;
using executor::kNorth;
using executor::kSouth;
using executor::kWest;
using executor_test::expectPose;
using executor_test::opposite;

TEST(ExecutorTurnRound, 掉头后朝向反转并左移一格)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    // 左转(W) -> 前进一格(-1,0) -> 左转(S)
    exec.execute("TR");
    expectPose(exec, -1, 0, kSouth);
}

TEST(ExecutorTurnRound, 各朝向下的掉头结果)
{
    Executor exec;

    exec.init(0, 0, kEast);
    exec.execute("TR");
    expectPose(exec, 0, 1, kWest);

    exec.init(0, 0, kSouth);
    exec.execute("TR");
    expectPose(exec, 1, 0, kNorth);

    exec.init(0, 0, kWest);
    exec.execute("TR");
    expectPose(exec, 0, -1, kEast);
}

TEST(ExecutorTurnRound, 掉头后朝向总是反转且移到左侧一格)
{
    Executor exec;

    // 掉头 = 向当前朝向的左侧横移一格，同时朝向反转：
    //   N -> 左移一格到西侧；E -> 左移一格到北侧；
    //   S -> 左移一格到东侧；W -> 左移一格到南侧。
    exec.init(1, 1, kNorth);
    exec.execute("TR");
    expectPose(exec, 0, 1, opposite(kNorth));

    exec.init(1, 1, kEast);
    exec.execute("TR");
    expectPose(exec, 1, 2, opposite(kEast));

    exec.init(1, 1, kSouth);
    exec.execute("TR");
    expectPose(exec, 2, 1, opposite(kSouth));

    exec.init(1, 1, kWest);
    exec.execute("TR");
    expectPose(exec, 1, 0, opposite(kWest));
}

TEST(ExecutorTurnRound, 连续两次掉头回到出发点)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("TRTR");
    expectPose(exec, 0, 0, kNorth);
}

TEST(ExecutorTurnRound, 掉头不受倒车状态影响)
{
    Executor plain;
    Executor reversing;
    plain.init(0, 0, kNorth);
    reversing.init(0, 0, kNorth);

    plain.execute("TR");
    reversing.execute("BTR");

    expectPose(reversing, plain.x(), plain.y(), plain.heading());
    expectPose(reversing, -1, 0, kSouth);
}

TEST(ExecutorTurnRound, 掉头不受倒车状态影响_批量中含B)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("BBTR");
    expectPose(exec, -1, 0, kSouth);
}

TEST(ExecutorTurnRound, 加速状态下掉头为先前进再左转再前进再左转)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    // 前进一格(0,1,N) 左转(W) 前进一格(-1,1,W) 左转(S)
    exec.execute("FTR");
    expectPose(exec, -1, 1, kSouth);
}

TEST(ExecutorTurnRound, 加速状态下连续两次掉头回到出发点)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("FTRTR");
    expectPose(exec, 0, 0, kNorth);
}

TEST(ExecutorTurnRound, 加速与倒车叠加时掉头仍按加速规则执行)
{
    Executor boosted;
    Executor boostedAndReversing;
    boosted.init(0, 0, kNorth);
    boostedAndReversing.init(0, 0, kNorth);

    boosted.execute("FTR");
    boostedAndReversing.execute("BFTR");

    expectPose(boostedAndReversing, boosted.x(), boosted.y(), boosted.heading());
    expectPose(boostedAndReversing, -1, 1, kSouth);
}

TEST(ExecutorTurnRound, 掉头的展开顺序)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    EXPECT_EQ(ActionList({Action::kTurnLeft, Action::kMoveForward, Action::kTurnLeft}),
              exec.expand("TR"));

    exec.execute("F");
    EXPECT_EQ(ActionList({Action::kMoveForward, Action::kTurnLeft, Action::kMoveForward,
                          Action::kTurnLeft}),
              exec.expand("TR"));
}

TEST(ExecutorTurnRound, 掉头可与其他指令混合批量执行)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    // M(0,1,N) TR:左转(W) 前进(-1,1) 左转(S)
    exec.execute("MTR");
    expectPose(exec, -1, 1, kSouth);

    // 再执行 M 应沿 S 方向前进一格
    exec.execute("M");
    expectPose(exec, -1, 0, kSouth);

    // L:朝向东
    exec.execute("L");
    expectPose(exec, -1, 0, kEast);
}
