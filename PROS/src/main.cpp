#include "main.h"
#include "autolib/api.hpp"
#include "autolib/auto/purePursuit.hpp"
#include "okapi/api/chassis/controller/chassisController.hpp"
#include "okapi/api/chassis/controller/chassisControllerPid.hpp"
#include "okapi/api/chassis/controller/defaultOdomChassisController.hpp"
#include "okapi/api/chassis/controller/odomChassisController.hpp"
#include "okapi/api/chassis/model/chassisModel.hpp"
#include "okapi/api/chassis/model/skidSteerModel.hpp"
#include "okapi/api/odometry/odomState.hpp"
#include "okapi/api/odometry/odometry.hpp"
#include "okapi/api/odometry/twoEncoderOdometry.hpp"
#include "okapi/api/units/QAngle.hpp"
#include "okapi/api/units/QLength.hpp"
#include "okapi/api/util/timeUtil.hpp"
#include <initializer_list>

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");

	pros::lcd::register_btn1_cb(on_center_button);
	
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
	pros::Motor left_mtr(1);
	pros::Motor right_mtr(2); 
	okapi::SkidSteerModel model{left_mtr,right_mtr,left_mtr.get_encoder_units(), right_mtr.get_encoder_units(), 200,10};
	okapi::TimeUtil timeUtil();
	okapi::TwoEncoderOdometry odom();
	okapi::ChassisControllerPID controller();
	okapi::DefaultOdomChassisController control{timeUtil(),odom,};
	okapi::PathfinderLimits lims {5.0,3.0,4.0};
	autolib::PathGenerator pathGen(lims);
	std::initializer_list<autolib::Pose> list{autolib::Pose{okapi::QLength(5.0), okapi::QLength(1.0), okapi::QAngle(1.0)}};
	pathGen.generatePath(list, "fucking idk");
	autolib::PurePursuit pp(pathGen.getPaths(), okapi::QLength(1.0));
	pp.updateChassis(const double &reqVelocity, pp.run(control.getState(), "fucking idk"), control);//how do we put this in a loop
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	pros::Motor left_mtr(1);
	pros::Motor right_mtr(2);
	

	while (true) {
		
		
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);
		int left = master.get_analog(ANALOG_LEFT_Y);
		int right = master.get_analog(ANALOG_RIGHT_Y);

		left_mtr = left;
		right_mtr = right;
		pros::delay(20);
	}
}
