#pragma once

#include <string>
#include <mutex>
#include <vector>
#include <array>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/system/error_code.hpp>
class SeedCommand {
public:
    SeedCommand(const std::string& port = "/dev/trx_s", int baudrate = 115200);
    ~SeedCommand();

    bool COM_Open();
    void COM_Close();
    void Script_Go(int id_num, int s_num);
    void setPosition(double pos);
    void SCM(int id_num, int d3, int d4, int d5, int d6, int d7, int d8);
    void writeSerialCommand(uint8_t _id, uint8_t *_data);
    void setPositionPulse(int target_pulse, int time_ms , int id);
    void setPosition(uint8_t id, uint8_t cmd, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5);
    int str2int(std::string _data);
    std::array<int, 3> getPosition(uint8_t _id);
    bool readSerialCommand(std::vector<uint8_t>& _receive_data, uint16_t _timeout);
    double pulse_to_rad_thumb(int pulse);
    int rad_to_pulse_thumb(double rad);
    
private:
    std::string port_;
    int baudrate_;
    int check_sum_;
    int count_;
    int length_;
    int fd_;
    int count_limit = 10;

    std::mutex mtx_;
    std::vector<uint8_t> send_data_;
    

};
