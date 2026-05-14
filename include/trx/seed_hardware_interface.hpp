#pragma once

#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include "rclcpp/rclcpp.hpp"
#include "trx/seed_command.hpp" 
#include <memory>
#include <string>
#include <vector>

namespace trx
{

class SeedHardwareInterface : public hardware_interface::SystemInterface
{
public:
  RCLCPP_SHARED_PTR_DEFINITIONS(SeedHardwareInterface)

  hardware_interface::CallbackReturn on_init(const hardware_interface::HardwareInfo& info) override;
  std::vector<hardware_interface::StateInterface> export_state_interfaces() override;
  std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;
  hardware_interface::CallbackReturn on_configure(const rclcpp_lifecycle::State&) override;
  hardware_interface::CallbackReturn on_activate(const rclcpp_lifecycle::State&) override;
  hardware_interface::CallbackReturn on_deactivate(const rclcpp_lifecycle::State&) override;
  hardware_interface::return_type read(const rclcpp::Time&, const rclcpp::Duration&) override;
  hardware_interface::return_type write(const rclcpp::Time&, const rclcpp::Duration&) override;

private:
  std::string serial_port_;
  int can_id1_;
  int can_id2_;
  double controller_rate_;

  std::vector<double> position_;
  std::vector<double> command_;
  std::vector<double> time_;

  hardware_interface::HardwareInfo info_;
  std::unique_ptr<SeedCommand> seed_;
};

}  // namespace trx
