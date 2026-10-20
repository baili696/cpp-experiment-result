// 需求 2：加速指令 F

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

TEST(ExecutorBoost, F指令本身不移动只切换状态)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("F");
    expectPose(exec, 0, 0, kNorth);
    EXPECT_TRUE(exec.boosted());
}

TEST(ExecutorBoost, 再收到一次F取消加速状态)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("F");
    EXPECT_TRUE(exec.boosted());

    exec.execute("F");
    EXPECT_FALSE(exec.boosted());
}

TEST(ExecutorBoost, 加速状态下M前进两格)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("FM");
    expectPose(exec, 0, 2, kNorth);
}

TEST(ExecutorBoost, 加速状态下多次M每次前进两格)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("FMM");
    expectPose(exec, 0, 4, kNorth);
}

TEST(ExecutorBoost, 取消加速后M恢复前进一格)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("FFM");
    expectPose(exec, 0, 1, kNorth);

    exec.execute("FFFM");
    expectPose(exec, 0, 3, kNorth);
}

TEST(ExecutorBoost, 加速状态下L先前进一格再左转)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("FL");
    expectPose(exec, 0, 1, kWest);
}

TEST(ExecutorBoost, 加速状态下R先前进一格再右转)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("FR");
    expectPose(exec, 0, 1, kEast);
}

TEST(ExecutorBoost, 加速状态对批量指令中每条指令都生效)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    // M(0,2,N) M(0,4,N) M(0,6,N) R:前进一格(0,7,N)右转(E)
    // R:前进一格(1,7,E)右转(S)
    exec.execute("FMMMRR");
    expectPose(exec, 1, 7, kSouth);
}

TEST(ExecutorBoost, 加速状态下连续两次L)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    // L:(0,1,N) 左转(W) -> L:前进一格(-1,1,W) 左转(S)
    exec.execute("FLL");
    expectPose(exec, -1, 1, kSouth);
}

TEST(ExecutorBoost, 加速只影响处于加速状态下的指令)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    // M(0,1,N) F 加速 M(0,3,N) R:前进一格(0,4,N)右转(E)
    exec.execute("MFMR");
    expectPose(exec, 0, 4, kEast);
}

TEST(ExecutorBoost, 加速状态下M展开为两个逐格前进动作)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    EXPECT_EQ(ActionList({Action::kMoveForward}), exec.expand("M"));

    exec.execute("F");
    EXPECT_EQ(ActionList({Action::kMoveForward, Action::kMoveForward}), exec.expand("M"));
}

TEST(ExecutorBoost, 加速状态下L的展开顺序为先前进后左转)
{
    Executor exec;
    exec.init(0, 0, kNorth);
    exec.execute("F");

    EXPECT_EQ(ActionList({Action::kMoveForward, Action::kTurnLeft}), exec.expand("L"));
}

TEST(ExecutorBoost, 加速状态下R的展开顺序为先前进后右转)
{
    Executor exec;
    exec.init(0, 0, kNorth);
    exec.execute("F");

    EXPECT_EQ(ActionList({Action::kMoveForward, Action::kTurnRight}), exec.expand("R"));
}

TEST(ExecutorBoost, 加速状态下不产生跳跃式移动)
{
    Executor exec;
    exec.init(0, 0, kNorth);
    exec.execute("F");

    // 每一步动作都只改变一格，不存在跨格
    const ActionList actions = {Action::kMoveForward, Action::kMoveForward};
    exec.execute("M");
    expectPose(exec, 0, 2, kNorth);
    EXPECT_EQ(actions, exec.expand("M"));
}
