#include <robot_lib/robot_agent.h>

using namespace std;
// temp constants
#define MAX_J 30
#define MIN_J 0
#define JOYSTICK 32767

namespace robot{
    Robot_agent::Robot_agent(const controller::Agent_operational_limits &limits):
        //Robot_agent(limits,4690){  // port for joystick 4690
        Robot_agent(limits,"/dev/input/js0"){  // joystick device
        set_leds(true);
    }

    void Robot_agent::set_left(double left_value) {
        char left = limits.convert(left_value);
        // for joystick control press R2
        if (!gamepad.buttons.empty() && gamepad.buttons[5].state == 1){
            human_intervention = true;
            float joystick_left = (float)-gamepad.axes[1]/JOYSTICK; // normalize this to config file
            if (joystick_left > 0){
                joystick_left = abs(joystick_left) * (MAX_J - MIN_J) + MIN_J;
            } else if (joystick_left < 0){
                joystick_left = -(abs(joystick_left) * (MAX_J - MIN_J) + MIN_J);
            }
            // drive straight
            if (gamepad.axes[7] == -32767){
                joystick_left = joystick_left / 2 + 30; // max value for char 127
            }
            else if (gamepad.axes[7] == 32767){
                joystick_left = joystick_left / 2 - 30;
            }
            left = (char) joystick_left;
        }
        // autonomous
        if (message[0] != left)
            need_update = true;

        message[0] = left;
    }

    void Robot_agent::set_right(double right_value) {
        char right = limits.convert(right_value);
        if (!gamepad.buttons.empty() && gamepad.buttons[5].state == 1){
            human_intervention = true;
            float joystick_right = (float)-gamepad.axes[4]/JOYSTICK;
            if (joystick_right > 0){
                joystick_right = abs(joystick_right) * (MAX_J - MIN_J) + MIN_J;
            } else if (joystick_right < 0){
                joystick_right = -(abs(joystick_right) * (MAX_J - MIN_J) + MIN_J);
            }
            // drive straight
            if (gamepad.axes[7] == -32767){
                joystick_right = joystick_right/2 + 30; // 20
            }
            if (gamepad.axes[7] == 32767){
                joystick_right = joystick_right/2 - 30;
            }
            right = (char) joystick_right;
        }
        if (message[1] != right)
            need_update = true;
        message[1] = right;
    }

    void Robot_agent::capture() {
        message[2] |= 1UL << 3; //puff
        message[2] |= 1UL << 6; // todo: add braking back to robot 
        need_update = true;
    }

    void Robot_agent::end_capture() {
        need_update = true;
    }

    void Robot_agent::set_led(int led_number, bool val) {
        if (val)
            message[2] |= 1UL << led_number;
        else
            message[2] &=~(1UL << led_number);
    }

    // if reset reconnect and hardware reboot
    bool Robot_agent::update() {

        if (reset_robot_agent) {
            need_update = true;
            message[0] = (char) 0;
            message[1] = (char) 0;
            message[2] |= 1UL << 4;
            cout << "reset robot" << endl;
        };

        if (!gamepad.buttons.empty() && gamepad.buttons[5].state == 1) human_intervention = true;
        else human_intervention = false;


        if (!need_update) return true;
        else need_update = false; // make false once realize same value
//        cout << "SPEED " << (int) message[0] << " " << (int) message[1] << endl;


        bool res = connection.send_data(message,3);
        message[2] &=~(1UL << 3);
        message[2] &=~(1UL << 4);
        message[2] &=~(1UL << 5);
        message[2] &=~(1UL << 6);
//        human_intervention = false;

        if (reset_robot_agent) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // TODO: ask German if this sleep is an issue
            if (connect()) {
                reset_robot_agent = false;
                cout << "reconnect to robot" << endl;
            } else {
                return false;
            }
        }


        return res;
    }

    Robot_agent::~Robot_agent() {
        message[0] = 0;
        message[1] = 0;
        message[2] = 0;
        need_update = true;
        update();
    }

    int Robot_agent::port() {
        string port_str (std::getenv("ROBOT_PORT")?std::getenv("ROBOT_PORT"):"4500");   // 4500
        return atoi(port_str.c_str());
    }

    void Robot_agent::set_leds(bool val) {
        for (int i=0; i<3;i++) set_led(i,val);
    }

    void Robot_agent::increase_brightness() {
        set_led(4,true);
    }

    void Robot_agent::decrease_brightness() {
        set_led(5,true);
    }

    bool Robot_agent::connect(const string &ip, int port) {
        try {
            connection = connection.connect_remote(ip, port);
            ip_address = ip;
            return true;
        } catch(...) {
            return false;
        }
    }

    bool Robot_agent::connect(const string &ip) {
        return connect(ip, port());
    }

    bool Robot_agent::connect() {
        //return connect("192.168.137.155");
        return connect(ip_address);
    }

    bool no_reset_robot_agent = false;

    Robot_agent::Robot_agent(const controller::Agent_operational_limits &limits, int game_pad_port):
            message{0,0,0},
            limits(limits),
            gamepad(game_pad_port),
            reset_robot_agent(no_reset_robot_agent){
        set_leds(true);
    }

    Robot_agent::Robot_agent(const controller::Agent_operational_limits &limits, std::string device_path):
            message{0,0,0},
            limits(limits),
            gamepad(device_path),
            reset_robot_agent(no_reset_robot_agent){
        set_leds(true);
    }

    bool Robot_agent::stop() {
        message[2] |= 1UL << 6;
        need_update = true;
        return true;
    }

    Robot_agent::Robot_agent(const controller::Agent_operational_limits &limits, bool &reset_robot_agent) :
            message{0,0,0},
            limits(limits),
            gamepad("/dev/input/js0"),
            reset_robot_agent(reset_robot_agent){  // joystick device
        set_leds(true);
    }
}