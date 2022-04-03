#include "main.h"
//#include "helper.h"

pros::Controller master(pros::E_CONTROLLER_MASTER);

//motor definitions - drive
pros::Motor left_front_mtr(11);
pros::Motor left_back_mtr(18);
pros::Motor right_front_mtr(3,true);
pros::Motor right_back_mtr(10,true);
pros::Motor arm_turntableA(2);
pros::Motor arm_turntableB(9, true);

//motor definitions - trigger and jaw
pros::Motor trigger (19);
pros::Motor crane_rotate (20);


//vision sensor stuff
//define vision sensor and signatures
int sensport = 8;
pros::Vision FrontSensor(sensport);

//signatures
pros::vision_signature_s_t red_target_sig =
FrontSensor.signature_from_utility(1, 8163, 9675, 8918, -595, 177, -208, 3.000, 0);
//neutral sig: 1, 2449, 2703, 2576, -3221, -3007, -3114, 3.000, 0
//red sig: 1, 7301, 7873, 7588, -515, 177, -168, 3.000, 0
//other red sig: 1, 8163, 9675, 8918, -595, 177, -208, 3.000, 0


//objects
pros::vision_object_s_t red_target[3]; //3 is the max amount of detected neutral mogii

//this sets the trigger's firing mode
void setTrigMode(int speedMult, int fireMode){
  pros::lcd::initialize();
    if (fireMode == 1){
      trigger = 127;
      pros::lcd::print(1, "firing");
    }

    else {
      trigger.move_velocity(0);
      pros::lcd::print(1, "standby");
    }
}

//auton vision test to make sure it works
void vision_test () {
 pros::lcd::initialize();

 bool isDoingStuff_RO = false;

 while (true) {
	 pros::lcd::clear();
	 FrontSensor.read_by_sig(0, red_target_sig.id, 3, red_target); //The vision sensor takes a picture, finds the areas with the matching color signature provided, (3 is the max amount of objects) then stores them into an area of those objects
	 pros::screen::set_pen(COLOR_BLUE_VIOLET);
	 pros::screen::print(TEXT_SMALL, 4, "mogus object 0: (%d, %d)", red_target[0].x_middle_coord, red_target[0].y_middle_coord); //prints the details of the first mogii object in array on the screen
	 pros::screen::print(TEXT_SMALL, 5, "object count: %d", FrontSensor.get_object_count()); //prints the amount of objects detected by vision sensor

	 pros::screen::set_pen(COLOR_YELLOW);
	 //move the arm up and down to keep the signature centered
   //x, y so center is 150, 100. Max x = 300, max y = 200
	 //check in y dimension
	 if (red_target[0].y_middle_coord > 110){
     pros::lcd::clear();
     arm_turntableA.move_relative(-100, 127);
		 arm_turntableB.move_relative(-100, 127);
		 pros::screen::print(TEXT_SMALL, 1,"arm moving down");
     pros::delay(5);

	 } else if (red_target[0].y_middle_coord < 90 && red_target[0].y_middle_coord != 0) {
     pros::lcd::clear();
     arm_turntableA.move_relative(100, 127);
		 arm_turntableB.move_relative(100, 127);
		 pros::screen::print(TEXT_SMALL, 1,"arm moving up");
     pros::delay(5);

	 } else {
     pros::lcd::clear();
		 arm_turntableA = 0;
		 arm_turntableB = 0;
		 pros::screen::print(TEXT_SMALL, 1,"centered");
     pros::delay(5);
	 }

	 //check in x dimension
	 if (red_target[0].x_middle_coord > 160){
		 crane_rotate.move_relative(100, 127);
		 pros::screen::print(TEXT_SMALL, 2,"arm going right");
     pros::delay(5);

	 } if (red_target[0].x_middle_coord < 140 && red_target[0].x_middle_coord != 0) {
		 crane_rotate.move_relative(-100, 127);
		 pros::screen::print(TEXT_SMALL, 2,"arm going left");
     pros::delay(5);
	 } else {
		 crane_rotate = 0;
		 pros::screen::print(TEXT_SMALL, 2,"centered");
	 }

	 pros::delay(10);

   }
}


//awp selection
bool pressed = false;

void on_center_button() {
	pressed = !pressed;
	if (pressed) {
		pros::lcd::print(4, "Autonomous Targeting");
	} else {
		pros::lcd::print(4, "Manual Targeting");
	}

	//helper struct
	struct control {
		struct {
			int x = 0;
			int y = 0;
		} left;
		struct {
			int x = 0;
			int y = 0;
		} right;
	} position;

	//define drive motor speed variables by side
	int right_speed = 0;
	int left_speed = 0;

	//define controller variables
	int right_x;
	int right_y;
	int left_x;
	int left_y;
}


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
	//set mtr brake modes
	arm_turntableA.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	arm_turntableB.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

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
  vision_test();
}

//I hate this robot so much. you dont understand.
//and people say I dont comment my code
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
	//define inputs (in this case the sticks)
	int right_x;
	int right_y;
	int left_y;
	int left_x;

  //tare trigger pos for single-shot
  trigger.tare_position();

  //if the trigger is in the act of firing
  bool isFiring = false;

  //initialize lcd for prints
	pros::lcd::initialize();

	//limiter variables
	crane_rotate.set_encoder_units(pros::E_MOTOR_ENCODER_DEGREES);
	arm_turntableA.set_encoder_units(pros::E_MOTOR_ENCODER_DEGREES);


	while(true){

		//get stick inputs
		right_x = master.get_analog(ANALOG_RIGHT_X);
		right_y = master.get_analog(ANALOG_RIGHT_Y);
		left_y = master.get_analog(ANALOG_LEFT_Y);
		left_x = master.get_analog(ANALOG_LEFT_X);

		//get bumper presses
		bool right_front_bumper = master.get_digital(DIGITAL_R1);

		//one-stick tank steer
		float motor_mult = 1.5; //motor speed multiplier

		//right drive
		right_front_mtr.move_velocity((right_y - right_x)*motor_mult);
		right_back_mtr.move_velocity((right_y - right_x)*motor_mult);

		//left drive
		left_front_mtr.move_velocity((right_y + right_x)*motor_mult);
		left_back_mtr.move_velocity((right_y + right_x)*motor_mult);

		//crane rotation code w/ limiters
		if (crane_rotate.get_position() > -1950 && crane_rotate.get_position() < 1950) {
			crane_rotate = 0.8 * left_x;
		} else if (crane_rotate.get_position() < -1950) {
			if (left_x > 0) {
					crane_rotate = 0.8 * left_x;
			} else {
				crane_rotate = 0;
			}
		} else if (crane_rotate.get_position() > 1950) {
			if (left_x < 0) {
				crane_rotate = 0.8 * left_x;
			} else {
				crane_rotate = 0;
			}
		} else {
			crane_rotate = 0;
		}

		//crane up/down code w/ limiters
		arm_turntableA = 0.8 * left_y;
		arm_turntableB = 0.8 * left_y;

    //firing modes
	  if (right_front_bumper && isFiring == false) { //single shot
			trigger.move_relative(-900, 127);
      isFiring = true;
      pros::delay(450);
	  }
    else if (!right_front_bumper && isFiring){
      isFiring = false;
    }
    if (master.get_digital(DIGITAL_A)){
      trigger.move_absolute(0, -127);
    }
		pros::delay(20);
	}
}
