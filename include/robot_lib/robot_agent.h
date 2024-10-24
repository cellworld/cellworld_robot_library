#pragma once
#include <controller/agent.h>
#include <robot_lib/gamepad_wrapper.h>

namespace robot {
    struct Robot_agent : controller::Agent {
        explicit Robot_agent(const controller::Agent_operational_limits &limits, bool &reset_robot_agent);
        explicit Robot_agent(const controller::Agent_operational_limits &limits);
        explicit Robot_agent(const controller::Agent_operational_limits &limits, int game_pad_port);
        explicit Robot_agent(const controller::Agent_operational_limits &limits, std::string device_path);
        bool connect();
        bool connect(const std::string &);
        bool connect(const std::string &, int);
        virtual void set_left(double) override;
        virtual void set_right(double) override;
        virtual void capture() override;
        virtual bool update() override;
        virtual void end_capture() override;
        virtual bool stop() override;
        void set_led(int, bool);
        void set_leds(bool);
        void increase_brightness();
        void decrease_brightness();
        char message[3];
        ~Robot_agent();
        int port();
        controller::Agent_operational_limits limits;
        Gamepad_wrapper gamepad;
        std::string ip_address {"192.168.137.155"};
        int ip_port=4500;
        bool &reset_robot_agent;
        bool reset_step_one = true;
    private:
        easy_tcp::Connection connection{-1};
        bool need_update = false;
    };
}