#include "trx/seed_hardware_interface.hpp"
#include <cmath>

// パルス値→ラジアン
double pulse_to_rad(int pulse) {
    const double pulse_max = 59913.0;
    return -(2/pulse_max) * pulse;
}

// ラジアン→パルス値
int rad_to_pulse(double rad) {
    const double rad_min = -2.0;
    const double rad_max = 0.0;
    const double pulse_max = 59913.0;
    if (rad < rad_min) rad = rad_min;
    if (rad > rad_max) rad = rad_max;
    return static_cast<int> (-(pulse_max/2) * rad);
}

namespace trx
{

hardware_interface::CallbackReturn SeedHardwareInterface::on_init(const hardware_interface::HardwareInfo& info)
{
  if (info.joints.size() != 4) {
    RCLCPP_FATAL(rclcpp::get_logger("SeedHW"), "Expected 4 joints, got %zu", info.joints.size());
    return hardware_interface::CallbackReturn::FAILURE;
  }
  info_ = info;
  position_.resize(info.joints.size(), 0.0);
  command_.resize(info.joints.size(), 0.0);
  time_.resize(info.joints.size(), 0.0);
  info_ = info;

  serial_port_ = info_.hardware_parameters["serial_port"];
  can_id_ = std::stoi(info_.hardware_parameters["can_id"]);
  controller_rate_ = std::stod(info_.hardware_parameters["controller_rate"]);

  RCLCPP_INFO(rclcpp::get_logger("SeedHW"), "on_init called. serial_port=%s, can_id=%d", serial_port_.c_str(), can_id_);
  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn SeedHardwareInterface::on_configure(const rclcpp_lifecycle::State&)
{
  try {
    seed_ = std::make_unique<SeedCommand>(serial_port_, 115200);
    seed_->COM_Open();
    RCLCPP_INFO(rclcpp::get_logger("SeedHW"), "Serial connection opened!");
  } catch (const std::exception& e) {
    RCLCPP_ERROR(rclcpp::get_logger("SeedHW"), "Serial init failed: %s", e.what());
    return hardware_interface::CallbackReturn::FAILURE;
  }
  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn SeedHardwareInterface::on_activate(const rclcpp_lifecycle::State&)
{
  RCLCPP_INFO(rclcpp::get_logger("SeedHW"), "on_activate called");
  if (seed_) {
    auto pos_result = seed_->getPosition(can_id_);
    position_[3] = pulse_to_rad(pos_result[2]);
    command_[0] = position_[3];
  }
  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn SeedHardwareInterface::on_deactivate(const rclcpp_lifecycle::State&)
{
  RCLCPP_INFO(rclcpp::get_logger("SeedHW"), "on_deactivate called");
  return hardware_interface::CallbackReturn::SUCCESS;
}

std::vector<hardware_interface::StateInterface> SeedHardwareInterface::export_state_interfaces()
{
  std::vector<hardware_interface::StateInterface> interfaces;
  for (size_t i = 0; i < info_.joints.size(); ++i) {
      interfaces.emplace_back(
      info_.joints[i].name,
      hardware_interface::HW_IF_POSITION,
      &position_[i]);
  }
  return interfaces;
}

std::vector<hardware_interface::CommandInterface> SeedHardwareInterface::export_command_interfaces()
{
  std::vector<hardware_interface::CommandInterface> interfaces;
    for (size_t i = 0; i < info_.joints.size(); ++i) {
      if(!info_.joints[i].command_interfaces.empty()){
        interfaces.emplace_back(
        info_.joints[i].name,
        hardware_interface::HW_IF_POSITION,
        &command_[i]);

        interfaces.emplace_back(
        info_.joints[i].name,
        "time",
        &time_[i]);
      }
    }
  return interfaces;
}


hardware_interface::return_type SeedHardwareInterface::read(const rclcpp::Time&, const rclcpp::Duration&)
{
    auto pos_result = seed_->getPosition(can_id_);
    int pulse = pos_result[2];
    if (pos_result[0] == 1) {
        if (pulse <= 0) {
            position_[0] = 0.0;
            position_[1] = 0.0;
            position_[2] = 0.0;
            position_[3] = 0.0;
        } else if (pulse <= 52000) {
            position_[0] = pulse * 1.2 / 52000.0; 
            position_[1] = 0.0;
            position_[2] = 0.0;
            position_[3] = pulse_to_rad(pulse);
        } else if (pulse <= 61000) {
            position_[0] = 1.2;
            position_[1] = (pulse - 52000) * 0.8 / 8000.0; 
            position_[2] = (pulse - 52000) * 0.8 / 8000.0;
            position_[3] = pulse_to_rad(pulse); 
        } else {
            position_[0] = 0.0;
            position_[1] = 0.0;
            position_[2] = 0.0;
            position_[3] = 0.0;
        }
    }
    return hardware_interface::return_type::OK;
}

hardware_interface::return_type SeedHardwareInterface::write(const rclcpp::Time&, const rclcpp::Duration&)
{
    double lower = -2.0;
    double upper = 0.0;
    double cmd_saturated = std::max(lower, std::min(command_[3], upper));
    static int cmd_val_prev = 0;
    int cmd_val = rad_to_pulse(cmd_saturated);
    if(cmd_val != cmd_val_prev){
        seed_->setPositionPulse(time_[3]*3000, cmd_val);
        cmd_val_prev = cmd_val;
    }
    return hardware_interface::return_type::OK;
}

}  // namespace trx

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(trx::SeedHardwareInterface, hardware_interface::SystemInterface)
