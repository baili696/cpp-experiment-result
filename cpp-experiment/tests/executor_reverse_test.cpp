// 需求 3：倒车指令 B（含 B 与 F 叠加）

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

TEST(ExecutorReverse, B指令本身不移动只切换状态)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("B");
    expectPose(exec, 0, 0, kNorth);
    EXPECT_TRUE(exec.reversing());
}

TEST(ExecutorReverse, 再收到一次B取消倒车状态)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("B");
    EXPECT_TRUE(exec.reversing());

    exec.execute("B");
    EXPECT_FALSE(exec.reversing());
}

TEST(ExecutorReverse, 倒车状态下M后退一格且朝向不变)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("BM");
    expectPose(exec, 0, -1, kNorth);
}

TEST(ExecutorReverse, 倒车状态下各朝向均后退一格)
{
    Executor exec;

    exec.init(0, 0, kEast);
    exec.execute("BM");
    expectPose(exec, -1, 0, kEast);

    exec.init(0, 0, kSouth);
    exec.execute("BM");
    expectPose(exec, 0, 1, kSouth);

    exec.init(0, 0, kWest);
    exec.execute("BM");
    expectPose(exec, 1, 0, kWest);
}

TEST(ExecutorReverse, 倒车状态下L变为右转且位置不变)
{
    Executor exec;
    exec.init(2, 3, kNorth);

    exec.execute("BL");
    expectPose(exec, 2, 3, kEast);
}

TEST(ExecutorReverse, 倒车状态下R变为左转且位置不变)
{
    Executor exec;
    exec.init(2, 3, kNorth);

    exec.execute("BR");
    expectPose(exec, 2, 3, kWest);
}

TEST(ExecutorReverse, 取消倒车后M恢复前进)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    // B 开启后再收到 B 即取消倒车状态，M 恢复为前进一格
    exec.execute("BBM");
    expectPose(exec, 0, 1, kNorth);
}

TEST(ExecutorReverse, 倒车与加速叠加时M后退两格)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("BFM");
    expectPose(exec, 0, -2, kNorth);
}

TEST(ExecutorReverse, 倒车与加速叠加时L先后退一格再右转)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("BFL");
    expectPose(exec, 0, -1, kEast);
}

TEST(ExecutorReverse, 倒车与加速叠加时R先后退一格再左转)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("BFR");
    expectPose(exec, 0, -1, kWest);
}

TEST(ExecutorReverse, 叠加状态下各朝向均后退两格)
{
    Executor exec;

    exec.init(0, 0, kEast);
    exec.execute("BFM");
    expectPose(exec, -2, 0, kEast);

    exec.init(0, 0, kSouth);
    exec.execute("BFM");
    expectPose(exec, 0, 2, kSouth);

    exec.init(0, 0, kWest);
    exec.execute("BFM");
    expectPose(exec, 2, 0, kWest);
}

TEST(ExecutorReverse, 先F后B与先B后F结果一致)
{
    Executor boostFirst;
    Executor reverseFirst;
    boostFirst.init(0, 0, kNorth);
    reverseFirst.init(0, 0, kNorth);

    boostFirst.execute("BFM");
    reverseFirst.execute("FBM");

    expectPose(boostFirst, reverseFirst.x(), reverseFirst.y(), reverseFirst.heading());
}

TEST(ExecutorReverse, 叠加状态下取消加速)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("BF");
    EXPECT_TRUE(exec.boosted());
    EXPECT_TRUE(exec.reversing());

    exec.execute("F");
    EXPECT_FALSE(exec.boosted());
    EXPECT_TRUE(exec.reversing());

    exec.execute("M");
    expectPose(exec, 0, -1, kNorth);
}

TEST(ExecutorReverse, 叠加状态下取消倒车)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("BF");
    exec.execute("B");
    EXPECT_TRUE(exec.boosted());
    EXPECT_FALSE(exec.reversing());

    exec.execute("M");
    expectPose(exec, 0, 2, kNorth);
}

TEST(ExecutorReverse, 叠加状态下批量指令)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    // M:后退两格(0,-2,N) M:后退两格(0,-4,N)
    // L:后退一格(0,-5,N) 右转(E)
    exec.execute("BFMML");
    expectPose(exec, 0, -5, kEast);
}

TEST(ExecutorReverse, 倒车状态下M展开为逐格后退动作)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("B");
    EXPECT_EQ(ActionList({Action::kMoveBackward}), exec.expand("M"));

    exec.execute("F");
    EXPECT_EQ(ActionList({Action::kMoveBackward, Action::kMoveBackward}), exec.expand("M"));
}

TEST(ExecutorReverse, 倒车状态下转向动作展开)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("B");
    EXPECT_EQ(ActionList({Action::kTurnRight}), exec.expand("L"));
    EXPECT_EQ(ActionList({Action::kTurnLeft}), exec.expand("R"));

    exec.execute("F");
    EXPECT_EQ(ActionList({Action::kMoveBackward, Action::kTurnRight}), exec.expand("L"));
    EXPECT_EQ(ActionList({Action::kMoveBackward, Action::kTurnLeft}), exec.expand("R"));
}
