#pragma once

#include <gtest/gtest.h>

#include <cstdint>

#include "executor/direction.h"
#include "executor/executor.h"

namespace executor_test
{

/// 断言执行器当前的位置与朝向
inline void expectPose(const executor::Executor& exec, std::int32_t x, std::int32_t y, char heading)
{
    EXPECT_EQ(x, exec.x());
    EXPECT_EQ(y, exec.y());
    EXPECT_EQ(heading, exec.heading());
}

/// 与给定朝向相反的方向（掉头后应当朝向它）
inline char opposite(char heading)
{
    return executor::turnLeft(executor::turnLeft(heading));
}

}  // namespace executor_test
