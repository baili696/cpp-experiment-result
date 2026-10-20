#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "executor/action.h"
#include "executor/vehicle_profile.h"

namespace executor
{

/// 车的位置与朝向
struct Pose {
    std::int32_t x{0};  ///< X 轴（东西方向）坐标
    std::int32_t y{0};  ///< Y 轴（南北方向）坐标
    char heading{'N'};  ///< 朝向：'N' / 'S' / 'E' / 'W'
};

/// 智能车执行器组件。
///
/// ============================ 支持的指令 ============================
///   M  前进（加速状态下前进 2 格；倒车状态下后退 1 格）
///   L  左转 90 度（倒车状态下等价于右转 90 度）
///   R  右转 90 度（倒车状态下等价于左转 90 度）
///   F  加速指令：切换加速状态，重复收到则取消
///   B  倒车指令：切换倒车状态，重复收到则取消
///   TR 掉头指令
/// 支持批量下发，例如 execute("MMLMF")。
///
/// ========================= 状态机（需求 2/3/4） =========================
///
///   状态      M                L                  R
///   --------|----------------|------------------|------------------
///   正常     | 前进 1 格      | 左转 90 度       | 右转 90 度
///   加速 F   | 前进 2 格      | 先前进 1 格,左转 | 先前进 1 格,右转
///   倒车 B   | 后退 1 格      | 右转 90 度       | 左转 90 度
///   加速+倒车| 后退 2 格      | 先后退 1 格,右转 | 先后退 1 格,左转
///
///   TR 掉头：左转 90 -> 前进 1 格 -> 左转 90
///           加速状态下：前进 1 格 -> 左转 90 -> 前进 1 格 -> 左转 90
///           TR 不受倒车状态影响。
///
/// “前进/后退 2 格”都是逐格执行的（见 Action / expand），不存在跨越格子。
///
/// ========================= 不同车型（课后综合作业） =========================
///
/// 车型只决定“正常前进状态下 M/L/R 的基准动作”，状态修饰规则对所有车型一致，
/// 因此无参构造（默认 Car）时本组件的行为与需求 1~4 完全一致。
///   跑车 SportsCar：M 前进 2 格；L 左转后前进 1 格；R 右转后前进 1 格
///   公交 Bus      ：M 前进 1 格；L 前进 1 格后左转；R 前进 1 格后右转
class Executor
{
public:
    /// 默认车型为普通轿车，默认位置与朝向由 init() 决定
    explicit Executor(VehicleType type = VehicleType::kCar);

    Executor(const Executor&) = delete;
    Executor& operator=(const Executor&) = delete;
    ~Executor() = default;

    /// 初始化：默认位置与朝向 (0, 0, N)，并清除加速/倒车状态
    void init();

    /// 初始化：指定位置 (x, y) 与朝向 heading（'N'/'S'/'E'/'W'），
    /// 同时清除加速/倒车状态
    void init(std::int32_t x, std::int32_t y, char heading);

    /// 执行单条指令
    void execute(char command);

    /// 批量执行指令，例如 execute("MMLMFTR")
    void execute(const std::string& commands);

    /// 查询：当前的位置与朝向
    Pose pose() const
    {
        return pose_;
    }
    std::int32_t x() const
    {
        return pose_.x;
    }
    std::int32_t y() const
    {
        return pose_.y;
    }
    char heading() const
    {
        return pose_.heading;
    }

    /// 查询：当前是否处于加速 / 倒车状态
    bool boosted() const
    {
        return boost_;
    }
    bool reversing() const
    {
        return reverse_;
    }

    /// 查询：当前车辆类型
    VehicleType vehicleType() const
    {
        return profile_->type();
    }

    /// 把一条指令（"M" / "L" / "R" / "F" / "B" / "TR"）展开成原子动作序列。
    /// 展开时会考虑当前的加速、倒车状态；F/B 本身不产生动作。
    /// 暴露该接口便于对“逐格移动、动作顺序”进行白盒验证。
    ActionList expand(const std::string& command) const;

private:
    /// 执行一条指令，并更新状态
    void applyCommand(const std::string& command);

    /// 执行一个原子动作
    void apply(const Action& action);

    Pose pose_{};
    bool boost_{false};    ///< 加速状态 F
    bool reverse_{false};  ///< 倒车状态 B
    std::unique_ptr<VehicleProfile> profile_;
};

}  // namespace executor
