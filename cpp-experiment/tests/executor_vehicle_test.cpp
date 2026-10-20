// 课后综合作业：支持不同的车辆类型（跑车 / 公交车）

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
using executor::VehicleType;
using executor_test::expectPose;

TEST(ExecutorVehicle, 默认车型为普通轿车)
{
    Executor exec;
    EXPECT_EQ(VehicleType::kCar, exec.vehicleType());

    exec.init(0, 0, kNorth);
    exec.execute("M");
    expectPose(exec, 0, 1, kNorth);
}

TEST(ExecutorVehicle, 构造函数指定车型)
{
    Executor car(VehicleType::kCar);
    Executor sportsCar(VehicleType::kSportsCar);
    Executor bus(VehicleType::kBus);

    EXPECT_EQ(VehicleType::kCar, car.vehicleType());
    EXPECT_EQ(VehicleType::kSportsCar, sportsCar.vehicleType());
    EXPECT_EQ(VehicleType::kBus, bus.vehicleType());
}

// ------------------------------ 跑车 ------------------------------

TEST(ExecutorVehicle, 跑车M前进两格)
{
    Executor exec(VehicleType::kSportsCar);
    exec.init(0, 0, kNorth);

    exec.execute("M");
    expectPose(exec, 0, 2, kNorth);

    exec.execute("M");
    expectPose(exec, 0, 4, kNorth);
}

TEST(ExecutorVehicle, 跑车L先左转再前进一格)
{
    Executor exec(VehicleType::kSportsCar);
    exec.init(0, 0, kNorth);

    exec.execute("L");
    expectPose(exec, -1, 0, kWest);
}

TEST(ExecutorVehicle, 跑车R先右转再前进一格)
{
    Executor exec(VehicleType::kSportsCar);
    exec.init(0, 0, kNorth);

    exec.execute("R");
    expectPose(exec, 1, 0, kEast);
}

TEST(ExecutorVehicle, 跑车连续转向)
{
    Executor exec(VehicleType::kSportsCar);

    exec.init(0, 0, kNorth);
    exec.execute("LL");
    expectPose(exec, -1, -1, kSouth);

    exec.init(0, 0, kNorth);
    exec.execute("LR");
    expectPose(exec, -1, 1, kNorth);
}

TEST(ExecutorVehicle, 跑车批量指令)
{
    Executor exec(VehicleType::kSportsCar);
    exec.init(0, 0, kNorth);

    // M(0,2,N) R:右转(E)前进一格(1,2,E) M:前进两格(3,2,E)
    // L:左转(N)前进一格(3,3,N) M:前进两格(3,5,N)
    exec.execute("MRMLM");
    expectPose(exec, 3, 5, kNorth);
}

TEST(ExecutorVehicle, 跑车M展开为两个逐格前进动作)
{
    Executor exec(VehicleType::kSportsCar);
    exec.init(0, 0, kNorth);

    EXPECT_EQ(ActionList({Action::kMoveForward, Action::kMoveForward}), exec.expand("M"));
    EXPECT_EQ(ActionList({Action::kTurnLeft, Action::kMoveForward}), exec.expand("L"));
    EXPECT_EQ(ActionList({Action::kTurnRight, Action::kMoveForward}), exec.expand("R"));
}

// ------------------------------ 公交车 ------------------------------

TEST(ExecutorVehicle, 公交车M前进一格)
{
    Executor exec(VehicleType::kBus);
    exec.init(0, 0, kNorth);

    exec.execute("M");
    expectPose(exec, 0, 1, kNorth);

    exec.execute("M");
    expectPose(exec, 0, 2, kNorth);
}

TEST(ExecutorVehicle, 公交车L先前进一格再左转)
{
    Executor exec(VehicleType::kBus);
    exec.init(0, 0, kNorth);

    exec.execute("L");
    expectPose(exec, 0, 1, kWest);
}

TEST(ExecutorVehicle, 公交车R先前进一格再右转)
{
    Executor exec(VehicleType::kBus);
    exec.init(0, 0, kNorth);

    exec.execute("R");
    expectPose(exec, 0, 1, kEast);
}

TEST(ExecutorVehicle, 公交车连续转向)
{
    Executor exec(VehicleType::kBus);

    exec.init(0, 0, kNorth);
    exec.execute("LL");
    expectPose(exec, -1, 1, kSouth);

    exec.init(0, 0, kNorth);
    exec.execute("LR");
    expectPose(exec, -1, 1, kNorth);
}

TEST(ExecutorVehicle, 公交车批量指令)
{
    Executor exec(VehicleType::kBus);
    exec.init(0, 0, kNorth);

    // M(0,1,N) R:前进一格(0,2,N)右转(E) M(1,2,E)
    // L:前进一格(2,2,E)左转(N) M(2,3,N)
    exec.execute("MRMLM");
    expectPose(exec, 2, 3, kNorth);
}

TEST(ExecutorVehicle, 公交车展开顺序为先前进后转向)
{
    Executor exec(VehicleType::kBus);
    exec.init(0, 0, kNorth);

    EXPECT_EQ(ActionList({Action::kMoveForward}), exec.expand("M"));
    EXPECT_EQ(ActionList({Action::kMoveForward, Action::kTurnLeft}), exec.expand("L"));
    EXPECT_EQ(ActionList({Action::kMoveForward, Action::kTurnRight}), exec.expand("R"));
}

// ------------------------------ 车型对比 ------------------------------

TEST(ExecutorVehicle, 相同指令在不同车型上产生不同结果)
{
    Executor car(VehicleType::kCar);
    Executor sportsCar(VehicleType::kSportsCar);
    Executor bus(VehicleType::kBus);
    car.init(0, 0, kNorth);
    sportsCar.init(0, 0, kNorth);
    bus.init(0, 0, kNorth);

    car.execute("L");
    sportsCar.execute("L");
    bus.execute("L");

    expectPose(car, 0, 0, kWest);       // 原地左转
    expectPose(sportsCar, -1, 0, kWest);  // 左转后前进一格
    expectPose(bus, 0, 1, kWest);       // 前进一格后左转
}

TEST(ExecutorVehicle, 公交车转向行为与轿车加速状态一致)
{
    Executor car(VehicleType::kCar);
    Executor bus(VehicleType::kBus);
    car.init(0, 0, kNorth);
    bus.init(0, 0, kNorth);

    car.execute("FL");  // 轿车加速状态下：前进一格后左转
    bus.execute("L");   // 公交车：前进一格后左转

    expectPose(bus, car.x(), car.y(), car.heading());
    expectPose(bus, 0, 1, kWest);
}

TEST(ExecutorVehicle, 跑车M行为与轿车加速状态一致)
{
    Executor car(VehicleType::kCar);
    Executor sportsCar(VehicleType::kSportsCar);
    car.init(0, 0, kNorth);
    sportsCar.init(0, 0, kNorth);

    car.execute("FM");
    sportsCar.execute("M");

    expectPose(sportsCar, car.x(), car.y(), car.heading());
    expectPose(sportsCar, 0, 2, kNorth);
}

// ------------------ 车型与加速/倒车状态的叠加（需求文档未定义，此处给出统一规则） ------------------

TEST(ExecutorVehicle, 跑车在加速状态下前进四格)
{
    Executor exec(VehicleType::kSportsCar);
    exec.init(0, 0, kNorth);

    // 跑车本身前进 2 格，加速状态再延长一倍 -> 4 个逐格动作
    exec.execute("FM");
    expectPose(exec, 0, 4, kNorth);
    EXPECT_EQ(ActionList({Action::kMoveForward, Action::kMoveForward, Action::kMoveForward,
                          Action::kMoveForward}),
              exec.expand("M"));
}

TEST(ExecutorVehicle, 公交车在加速状态下前进两格)
{
    Executor exec(VehicleType::kBus);
    exec.init(0, 0, kNorth);

    exec.execute("FM");
    expectPose(exec, 0, 2, kNorth);
}

TEST(ExecutorVehicle, 跑车在倒车状态下后退两格且左右转向互换)
{
    Executor exec(VehicleType::kSportsCar);
    exec.init(0, 0, kNorth);

    exec.execute("BM");
    expectPose(exec, 0, -2, kNorth);

    exec.init(0, 0, kNorth);
    exec.execute("BL");
    // 倒车状态下 L 变成右转，随后按跑车配方后退一格
    expectPose(exec, -1, 0, kEast);
}

TEST(ExecutorVehicle, 公交车在倒车状态下后退一格且左右转向互换)
{
    Executor exec(VehicleType::kBus);
    exec.init(0, 0, kNorth);

    exec.execute("BM");
    expectPose(exec, 0, -1, kNorth);

    exec.init(0, 0, kNorth);
    exec.execute("BL");
    // 倒车状态下 L 变成右转：先后退一格(0,-1)再右转(E)
    expectPose(exec, 0, -1, kEast);
}

TEST(ExecutorVehicle, 各车型掉头后朝向均反转)
{
    const VehicleType types[3] = {VehicleType::kCar, VehicleType::kSportsCar, VehicleType::kBus};

    for (const VehicleType type : types) {
        Executor exec(type);
        exec.init(0, 0, kNorth);
        exec.execute("TR");
        EXPECT_EQ(kSouth, exec.heading());
    }
}
