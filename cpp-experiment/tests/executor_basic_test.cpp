// 需求 1：基本控制指令（初始化 / M / L / R / 批量 / 查询）

#include <gtest/gtest.h>

#include "executor/executor.h"
#include "test_support.h"

using executor::Executor;
using executor::kEast;
using executor::kNorth;
using executor::kSouth;
using executor::kWest;
using executor_test::expectPose;

TEST(ExecutorBasic, 默认初始化为原点朝向N)
{
    Executor exec;
    expectPose(exec, 0, 0, kNorth);
}

TEST(ExecutorBasic, 使用指定位置与朝向初始化)
{
    Executor exec;

    exec.init(3, -4, kWest);
    expectPose(exec, 3, -4, kWest);

    exec.init(-2147483647, 2147483647, kSouth);
    expectPose(exec, -2147483647, 2147483647, kSouth);
}

TEST(ExecutorBasic, 重新初始化回到默认位置)
{
    Executor exec;
    exec.init(5, 6, kEast);
    exec.execute("MM");

    exec.init();
    expectPose(exec, 0, 0, kNorth);
}

TEST(ExecutorBasic, M指令按朝向前进一格)
{
    Executor exec;

    exec.init(0, 0, kNorth);
    exec.execute("M");
    expectPose(exec, 0, 1, kNorth);

    exec.init(0, 0, kSouth);
    exec.execute("M");
    expectPose(exec, 0, -1, kSouth);

    exec.init(0, 0, kEast);
    exec.execute("M");
    expectPose(exec, 1, 0, kEast);

    exec.init(0, 0, kWest);
    exec.execute("M");
    expectPose(exec, -1, 0, kWest);
}

TEST(ExecutorBasic, 连续M指令逐格累加)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("MMMMM");
    expectPose(exec, 0, 5, kNorth);
}

TEST(ExecutorBasic, 朝向X轴方向移动使用X坐标)
{
    Executor exec;
    exec.init(-5, -5, kWest);

    exec.execute("MM");
    expectPose(exec, -7, -5, kWest);
}

TEST(ExecutorBasic, L指令左转九十度且位置不变)
{
    Executor exec;
    exec.init(2, 3, kNorth);

    exec.execute("L");
    expectPose(exec, 2, 3, kWest);

    exec.execute("L");
    expectPose(exec, 2, 3, kSouth);

    exec.execute("L");
    expectPose(exec, 2, 3, kEast);

    exec.execute("L");
    expectPose(exec, 2, 3, kNorth);
}

TEST(ExecutorBasic, R指令右转九十度且位置不变)
{
    Executor exec;
    exec.init(2, 3, kNorth);

    exec.execute("R");
    expectPose(exec, 2, 3, kEast);

    exec.execute("R");
    expectPose(exec, 2, 3, kSouth);

    exec.execute("R");
    expectPose(exec, 2, 3, kWest);

    exec.execute("R");
    expectPose(exec, 2, 3, kNorth);
}

TEST(ExecutorBasic, 左右转互相抵消)
{
    Executor exec;
    exec.init(1, 1, kNorth);

    exec.execute("LR");
    expectPose(exec, 1, 1, kNorth);

    exec.execute("RL");
    expectPose(exec, 1, 1, kNorth);
}

TEST(ExecutorBasic, 单条指令接口与批量指令接口等价)
{
    Executor batch;
    Executor single;
    batch.init(0, 0, kNorth);
    single.init(0, 0, kNorth);

    batch.execute("MMLMRM");
    single.execute('M');
    single.execute('M');
    single.execute('L');
    single.execute('M');
    single.execute('R');
    single.execute('M');

    expectPose(batch, single.x(), single.y(), single.heading());
}

TEST(ExecutorBasic, 批量指令按顺序依次执行)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    // M(0,1,N) M(0,2,N) L(W) M(-1,2,W) R(N) M(-1,3,N)
    exec.execute("MMLMRM");
    expectPose(exec, -1, 3, kNorth);
}

TEST(ExecutorBasic, 移动与转向组合的批量指令)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    // M(0,1,N) R(E) M(1,1,E) L(N) M(1,2,N)
    exec.execute("MRMLM");
    expectPose(exec, 1, 2, kNorth);
}

TEST(ExecutorBasic, 绕正方形一圈回到出发点)
{
    Executor exec;
    exec.init(0, 0, kNorth);

    exec.execute("MMRMMRMM");
    expectPose(exec, 2, 0, kSouth);

    exec.execute("MMRMMRMM");
    expectPose(exec, 0, 0, kNorth);
}

TEST(ExecutorBasic, 批量指令可包含空格便于阅读)
{
    Executor spaced;
    Executor compact;
    spaced.init(0, 0, kNorth);
    compact.init(0, 0, kNorth);

    spaced.execute("M M L M R M");
    compact.execute("MMLMRM");

    expectPose(spaced, compact.x(), compact.y(), compact.heading());
}

TEST(ExecutorBasic, 查询接口返回最新的位置与朝向)
{
    Executor exec;
    exec.init(7, 8, kEast);

    EXPECT_EQ(7, exec.x());
    EXPECT_EQ(8, exec.y());
    EXPECT_EQ(kEast, exec.heading());

    const executor::Pose pose = exec.pose();
    EXPECT_EQ(7, pose.x);
    EXPECT_EQ(8, pose.y);
    EXPECT_EQ(kEast, pose.heading);

    exec.execute("MRM");
    // M(8,8,E) R(S) M(8,7,S)
    expectPose(exec, 8, 7, kSouth);
}
