#include "executor/executor.h"

#include "executor/direction.h"

namespace executor {
namespace {

/// 需求 2：加速状态下前进/后退 2 格，且“不能跳跃，只能一格一格走”，
/// 因此把配方里每一段前进动作直接延长一倍，得到的是两个独立的单格动作。
void doubleForwardSteps(ActionList& actions)
{
    ActionList doubled;
    doubled.reserve(actions.size() * 2);
    for (const Action& action : actions) {
        doubled.push_back(action);
        if (action == Action::kMoveForward) {
            doubled.push_back(Action::kMoveForward);
        }
    }
    actions.swap(doubled);
}

/// 需求 3：倒车状态下用“后退”代替“前进”，并且左右转向互换。
void mirrorForReverse(ActionList& actions)
{
    for (Action& action : actions) {
        switch (action) {
            case Action::kMoveForward:
                action = Action::kMoveBackward;
                break;
            case Action::kMoveBackward:
                action = Action::kMoveForward;
                break;
            case Action::kTurnLeft:
                action = Action::kTurnRight;
                break;
            case Action::kTurnRight:
                action = Action::kTurnLeft;
                break;
        }
    }
}

bool isSpace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

}  // namespace

Executor::Executor(VehicleType type) : profile_(makeVehicleProfile(type)) {}

void Executor::init()
{
    init(0, 0, kNorth);
}

void Executor::init(std::int32_t x, std::int32_t y, char heading)
{
    pose_ = Pose{x, y, heading};
    boost_ = false;
    reverse_ = false;
}

void Executor::execute(char command)
{
    applyCommand(std::string(1, command));
}

void Executor::execute(const std::string& commands)
{
    for (std::size_t i = 0; i < commands.size(); ++i) {
        const char c = commands[i];
        if (isSpace(c)) {
            continue;  // 允许用空格分隔，便于阅读批量指令
        }
        if (c == 'T') {
            // TR 一起出现时才有效
            if (i + 1 < commands.size() && commands[i + 1] == 'R') {
                ++i;
                applyCommand("TR");
            }
            continue;
        }
        applyCommand(std::string(1, c));
    }
}

ActionList Executor::expand(const std::string& command) const
{
    if (command == "F" || command == "B") {
        return ActionList{};  // 状态切换指令本身不产生动作
    }

    if (command == "TR") {
        // 需求 4：TR 只受加速状态影响，不受倒车状态影响
        if (boost_) {
            return ActionList{Action::kMoveForward, Action::kTurnLeft,
                              Action::kMoveForward, Action::kTurnLeft};
        }
        return ActionList{Action::kTurnLeft, Action::kMoveForward, Action::kTurnLeft};
    }

    ActionList actions;
    bool isTurnCommand = false;
    if (command == "M") {
        actions = profile_->moveRecipe();
    } else if (command == "L") {
        actions = profile_->leftRecipe();
        isTurnCommand = true;
    } else if (command == "R") {
        actions = profile_->rightRecipe();
        isTurnCommand = true;
    } else {
        return ActionList{};  // 约束保证不会出现非法指令
    }

    if (boost_) {
        doubleForwardSteps(actions);
        if (isTurnCommand) {
            // 需求 2：加速状态下 L/R 都要“先前进 1 格”
            actions.insert(actions.begin(), Action::kMoveForward);
        }
    }
    if (reverse_) {
        mirrorForReverse(actions);
    }
    return actions;
}

void Executor::applyCommand(const std::string& command)
{
    if (command == "F") {
        boost_ = !boost_;
        return;
    }
    if (command == "B") {
        reverse_ = !reverse_;
        return;
    }

    for (const Action& action : expand(command)) {
        apply(action);
    }
}

void Executor::apply(const Action& action)
{
    const Offset step = forwardOffset(pose_.heading);
    switch (action) {
        case Action::kMoveForward:
            pose_.x += step.dx;
            pose_.y += step.dy;
            break;
        case Action::kMoveBackward:
            pose_.x -= step.dx;
            pose_.y -= step.dy;
            break;
        case Action::kTurnLeft:
            pose_.heading = turnLeft(pose_.heading);
            break;
        case Action::kTurnRight:
            pose_.heading = turnRight(pose_.heading);
            break;
    }
}

}  // namespace executor
