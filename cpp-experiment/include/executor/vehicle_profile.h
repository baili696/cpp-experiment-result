#pragma once

#include <memory>

#include "executor/action.h"

namespace executor
{

/// 车辆类型
enum class VehicleType {
    kCar,        ///< 普通轿车：需求 1~4 描述的默认车型
    kSportsCar,  ///< 跑车：速度很快
    kBus,        ///< 公交车：车长 2 格，坐标记录的是车头位置
};

/// 车型的“基准动作配方”。
///
/// 每个车型只需要描述自己在【正常前进、未加速、未倒车】状态下，
/// M / L / R 三条指令分别对应哪些原子动作；
/// 加速(F)、倒车(B)、掉头(TR) 对配方的修饰由 Executor 统一处理。
/// 这样新增车型时只给出三条配方即可，不必重复实现整套状态机，
/// 也保证了所有车型的状态语义完全一致。
class VehicleProfile
{
public:
    VehicleProfile() = default;
    VehicleProfile(const VehicleProfile&) = delete;
    VehicleProfile& operator=(const VehicleProfile&) = delete;
    virtual ~VehicleProfile() = default;

    virtual VehicleType type() const = 0;
    virtual const char* name() const = 0;

    /// M：基准状态下的前进动作（必须一格一格，不允许跳跃）
    virtual ActionList moveRecipe() const = 0;
    /// L：基准状态下的左转动作
    virtual ActionList leftRecipe() const = 0;
    /// R：基准状态下的右转动作
    virtual ActionList rightRecipe() const = 0;
};

/// 普通轿车：
///   M -> 前进 1 格；L -> 左转 90 度；R -> 右转 90 度
class CarProfile final : public VehicleProfile
{
public:
    VehicleType type() const override
    {
        return VehicleType::kCar;
    }
    const char* name() const override
    {
        return "Car";
    }

    ActionList moveRecipe() const override
    {
        return {Action::kMoveForward};
    }
    ActionList leftRecipe() const override
    {
        return {Action::kTurnLeft};
    }
    ActionList rightRecipe() const override
    {
        return {Action::kTurnRight};
    }
};

/// 跑车：
///   M -> 前进 2 格（先前进 1 格再前进 1 格）
///   L -> 先左转 90 度，再前进 1 格
///   R -> 先右转 90 度，再前进 1 格
class SportsCarProfile final : public VehicleProfile
{
public:
    VehicleType type() const override
    {
        return VehicleType::kSportsCar;
    }
    const char* name() const override
    {
        return "SportsCar";
    }

    ActionList moveRecipe() const override
    {
        return {Action::kMoveForward, Action::kMoveForward};
    }
    ActionList leftRecipe() const override
    {
        return {Action::kTurnLeft, Action::kMoveForward};
    }
    ActionList rightRecipe() const override
    {
        return {Action::kTurnRight, Action::kMoveForward};
    }
};

/// 公交车（车头坐标）：
///   M -> 前进 1 格
///   L -> 先前进 1 格，再左转 90 度
///   R -> 先前进 1 格，再右转 90 度
class BusProfile final : public VehicleProfile
{
public:
    VehicleType type() const override
    {
        return VehicleType::kBus;
    }
    const char* name() const override
    {
        return "Bus";
    }

    ActionList moveRecipe() const override
    {
        return {Action::kMoveForward};
    }
    ActionList leftRecipe() const override
    {
        return {Action::kMoveForward, Action::kTurnLeft};
    }
    ActionList rightRecipe() const override
    {
        return {Action::kMoveForward, Action::kTurnRight};
    }
};

/// 按车辆类型创建对应的配方对象
std::unique_ptr<VehicleProfile> makeVehicleProfile(VehicleType type);

}  // namespace executor
