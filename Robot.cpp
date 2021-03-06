#include "Robot.h"

Robot::Robot() :
		gyro(SPI::Port::kMXP),
		pid(),
		robotDrive(Constants::frontLeftDriveChannel,Constants::frontRightDriveChannel,Constants::rearLeftDriveChannel,Constants::rearRightDriveChannel,gyro,pid),
		driveStick(Constants::driveStickChannel),
		operatorStick(Constants::operatorStickChannel),
		compressor(),
		pdp(),
		lift(Constants::liftChannel,Constants::helpChannel),
		climber(Constants::climbChannel1,Constants::climbChannel2),
		grip(Constants::gripperLeftMotorChannel,Constants::gripperRightMotorChannel,Constants::gripperLeftPneum1Channel,Constants::gripperLeftPneum2Channel,Constants::gripperRightPneum1Channel,Constants::gripperRightPneum2Channel),
		aim(Constants::distanceBetweenCameras),
		i2c(I2C::kOnboard,Constants::ledAddress),
		shifter(Constants::shifterChannel1,Constants::shifterChannel2),
		upLim(8),
		downLim(9),
		rIR(0),
		lIR(1)
{
	gyro.ZeroYaw();
}

void Robot::colorSend(uint8_t numToSend)
{
	uint8_t *toSend = new uint8_t[10];
	uint8_t *toReceive = new uint8_t[10];
	toSend[0] = 1;
	toReceive[0] = 0;
	i2c.Transaction(toSend, numToSend, toReceive, 0);
}

void Robot::RobotInit()
{
	NetworkTable::SetUpdateRate(.01);
	if (!IsEnabled())
	{
		colorSend(1);
	}
	CameraServer::GetInstance()->StartAutomaticCapture();
}

void Robot::OperatorControl()
{
	robotDrive.TankDrive(0,0);

	SmartDashboard::PutNumber("xpos", 0);
	SmartDashboard::PutNumber("ypos", 0);

	SmartDashboard::PutBoolean("Opened", true);

	gyro.Reset();
	SmartDashboard::PutBoolean("in op ctrl", true);

	pid.setAngle(SmartDashboard::GetNumber("angle p val", .005), SmartDashboard::GetNumber("angle i val", .001), SmartDashboard::GetNumber("angle d val", .001));
	pid.setMinTurnSpeed(SmartDashboard::GetNumber("angle min turn val", .25));
	pid.setMaxTurnSpeed(SmartDashboard::GetNumber("angle max turn val", .8));

	Positionator pos;
	Position p;

	float angle = 0;
	int driveMode = 1;

	bool togglingDriveMode = false;
	bool firstLoopThroughSinceButtonPressed = true;
	float cube_angle = 0.0;
	bool firstLifterPosition = true;

	bool togglingDropper = false;
	bool togglingHolder = false;
	bool togglingIntake = false;
	bool togglingShooty = false;

	float shootyPower = 0.0;

	float cube_ang = 0.0;

	int drivePowHeight = 1;

	lift.Reset();
	lift.Zero();

	if (IsEnabled())
	{
		colorSend(2);
	}

	while(IsOperatorControl() && IsEnabled())
	{

//		SmartDashboard::PutNumber("TJ's thing",aim.GetHorizontalAngles());

//		if (lift.GetEncoder() > 0)
//		{fr5555555t5ttttttTT%T%TTTtttttttttttt5tt%
//			Constants::percentDrivePower = (Constants::normPower/3) * (Constants::lifterMaxHeight - (Constants::lifterHeightPerRev * lift.GetEncoder() / 4096)) / Constants::lifterMaxHeight + 2*(Constants::normPower/3);
//		}

//		drivePowHeight = (160000.0 - lift.GetEncoder())/(160000.0*1.5)+0.33333;
//		drivePowHeight = drivePowHeight > 1 ? 1 : drivePowHeight;

		//////////////////////////////////////////////////////////////////////////////
		//								DRIVING CODE								//
		//////////////////////////////////////////////////////////////////////////////


		angle = robotDrive.AngleConvert(gyro.GetYaw());

//		if (driveStick.GetRawButton(Constants::driveStraightButtonFor12Inches))
//		{
//			Constants::percentDrivePower *= Constants::highPower / Constants::normPower;
//		}

		if(driveStick.GetPOV() != -1 && gyro.IsConnected())
		{ //turn to angle 0, 90, 180, 270
			float left = pid.PIDAngle(angle, driveStick.GetPOV()); //call pid loop
			float right = -1*left;
			robotDrive.TankDrive(left*drivePowHeight,right*drivePowHeight);
			SmartDashboard::PutNumber("POV", driveStick.GetPOV());
		}
		else
		{
			firstLoopThroughSinceButtonPressed = true;
			pid.resetPIDAngle();
			if (driveMode == 0)
			{
				SmartDashboard::PutNumber("drivemode", 0.0);

				float left = Constants::percentDrivePower*driveStick.GetRawAxis(Constants::tankDriveLeftAxis);
				float right = Constants::percentDrivePower*driveStick.GetRawAxis(Constants::tankDriveRightAxis);
				if (driveStick.GetRawButton(Constants::driveStraightButton))
				{
					robotDrive.DriveStraight(left*drivePowHeight,right*drivePowHeight);
					SmartDashboard::PutBoolean("Driving Straight", true);
				}
				else
				{
					robotDrive.TankDrive(left*drivePowHeight,right*drivePowHeight);
					robotDrive.DriveStraightReset();
					SmartDashboard::PutBoolean("Driving Straight", false);
				}
			}
			else if (driveMode == 1)
			{
				SmartDashboard::PutNumber("drivemode", 1.0);

				float fwd = fabs(driveStick.GetRawAxis(Constants::arcadeDriveForwardAxis)) > 0.05 ? Constants::percentDrivePower*driveStick.GetRawAxis(Constants::arcadeDriveForwardAxis) : 0.0;
				float turn = fabs(driveStick.GetRawAxis(Constants::arcadeDriveTurnAxis)) > 0.05 ? -1 * Constants::percentDrivePower*driveStick.GetRawAxis(Constants::arcadeDriveTurnAxis) : 0.0;
				if (driveStick.GetRawButton(Constants::driveStraightButton))
				{
					robotDrive.DriveStraightArcade(fwd*drivePowHeight);
					SmartDashboard::PutBoolean("Driving Straight", true);
				}
				else
				{
					robotDrive.ArcadeDrive(fwd*drivePowHeight,turn*drivePowHeight);
					robotDrive.DriveStraightReset();
					SmartDashboard::PutBoolean("Driving Straight", false);
				}
			}
			else if (driveMode == 2)
			{
				float up = -1*Constants::percentDrivePower*driveStick.GetRawAxis(Constants::fieldOrientedDriveVerticalAxis);
				float right = Constants::percentDrivePower*driveStick.GetRawAxis(Constants::fieldOrientedDriveHorizontalAxis);
				robotDrive.FieldOrientedDrive(up*drivePowHeight,right*drivePowHeight);
			}
		}


		//////////////////////////////////////////////////////////////////////////////
		//								LIFT CODE									//
		//////////////////////////////////////////////////////////////////////////////

//		if (lift.GetEncoder() <= 14880)// && !upLim.Get() )//&& downLim.Get())
//		{
//			if (operatorStick.GetRawButton(9))
//			{
//				colorSend(5);
//				lift.SetPosition(0);
//			}
//			else if (operatorStick.GetRawButton(10))
//			{
//				colorSend(5);
//				lift.SetPosition(33);
//			}
//			else if (operatorStick.GetRawButton(10))
//			{
//				colorSend(5);
//				lift.SetPosition(100);
//			}
//			else if (operatorStick.GetRawButton(10))
//			{
//				colorSend(5);
//				lift.SetPosition(120);
//			}

			if (driveStick.GetRawButton(Constants::liftUpButton))
			{
				colorSend(5);
				lift.Lift(((driveStick.GetRawAxis(Constants::liftUpAxis))/2.0 + 0.5));
				SmartDashboard::PutString("lifting", "up");
				SmartDashboard::PutNumber("liftpow",(driveStick.GetRawAxis(Constants::liftUpAxis))/2.0 + 0.5);
			}
			else if (driveStick.GetRawButton(Constants::liftDownButton))
			{
				colorSend(7);
				lift.Lift(-1*((driveStick.GetRawAxis(Constants::liftDownAxis))/2.0 + 0.5));
				SmartDashboard::PutString("lifting", "down");
				SmartDashboard::PutNumber("liftpow",-1*((driveStick.GetRawAxis(Constants::liftDownAxis))/2.0 + 0.5));
			}
//			else if (driveStick.GetRawButton(4))
//			{
//				if (firstLifterPosition)
//				{
//					firstLifterPosition =   false;
//					lift.SetPosition(2);
//				}
//				lift.Position();
//			}
//			else if (driveStick.GetRawButton(3))
//			{
//				if (firstLifterPosition)
//				{
//					firstLifterPosition = false;
//					lift.SetPosition(-2);
//				}
//				lift.Position();
//			}
			else
			{
				lift.Brake();
				firstLifterPosition = true;
				SmartDashboard::PutString("lifting", "brake");
			}

//			if(lift.GetEncoder() <= -100)
//			{
//				lift.Lift(-0.2);
//			}
//			else if (lift.GetEncoder() >= 14800)
//			{
//				lift.Lift(+0.1);
//			}
//			lift.Position();
//		}
//		else if (upLim.Get())
//		{
//			lift.Lift(-0.1);
//			firstLifterPosition = true;
//		}
//		else if (!downLim.Get())
//		{
//			lift.Lift(-0.1);
//			firstLifterPosition = true;
//		}
//		else
//		{
//			lift.Lift(-0.1);
//			firstLifterPosition = true;
//		}
//		std::cout << -1 * (driveStick.GetRawAxis(Constants::liftUpAxis)/2.0 + 0.5);


		//////////////////////////////////////////////////////////////////////////////
		//							SMART DASHBOARD STUFF							//
		//////////////////////////////////////////////////////////////////////////////

			SmartDashboard::PutBoolean("Opened", grip.getHolder());
			SmartDashboard::PutBoolean("Spinning", shootyPower != 0);
			SmartDashboard::PutNumber("fightstickPOV", operatorStick.GetPOV());
		SmartDashboard::PutNumber("drive axis 1", driveStick.GetRawAxis(Constants::tankDriveLeftAxis));
		SmartDashboard::PutNumber("drive axis 5", driveStick.GetRawAxis(Constants::tankDriveRightAxis));
		SmartDashboard::PutNumber("Left Encoder", robotDrive.GetLeftEncoder());
		SmartDashboard::PutNumber("Left Encoder Inches", robotDrive.GetLeftEncoder() / Constants::ticksPerInch);
		SmartDashboard::PutNumber("Right Encoder", robotDrive.GetRightEncoder());
		SmartDashboard::PutNumber("Right Encoder Inches", robotDrive.GetRightEncoder() / Constants::ticksPerInch);
		SmartDashboard::PutNumber("gyro", angle);

		SmartDashboard::PutNumber("RawAccelX", gyro.GetRawAccelX());
		SmartDashboard::PutNumber("RawAccelY", gyro.GetRawAccelY());
		SmartDashboard::PutNumber("RawAccelZ", gyro.GetRawAccelZ());
		SmartDashboard::PutNumber("WorldLinearAccelX", gyro.GetWorldLinearAccelX());
		SmartDashboard::PutNumber("WorldLinearAccelY", gyro.GetWorldLinearAccelY());
		SmartDashboard::PutNumber("WorldLinearAccelZ", gyro.GetWorldLinearAccelZ());
		SmartDashboard::PutNumber("Lifter Current", lift.GetCurrent());
		SmartDashboard::PutNumber("Lifter Encoder", lift.GetEncoder());

		//////////////////////////////////////////////////////////////////////////////
		//							TOGGLE CERTAIN THINGS							//
		//////////////////////////////////////////////////////////////////////////////

		if (driveStick.GetRawButton(Constants::toggleDriveModeButton))
		{
			if (!togglingDriveMode)
			{
				if (driveMode == 0)
					driveMode = 1;
				else if (driveMode == 1)
					driveMode = 0;
			}
			togglingDriveMode = true;
		}
		else
			togglingDriveMode = false;

		if (operatorStick.GetRawButton(Constants::dropperButton))
		{
			if (!togglingDropper)
			{
				if (grip.getDropper())
					grip.setDropper(false);
				else
					grip.setDropper(true);
			}
			togglingDropper = true;
		}
		else
			togglingDropper = false;

		if (operatorStick.GetRawButton(Constants::holderButton))
		{
			if (!togglingHolder)
			{
				if (grip.getHolder())
					grip.setHolder(false);
				else
					grip.setHolder(true);
			}
			togglingHolder = true;
		}
		else
			togglingHolder = false;

		if (operatorStick.GetRawAxis(Constants::intakeButton)!=0)
		{
			grip.setMotors(operatorStick.GetRawAxis(Constants::intakeButton));
			grip.setHolder(true);
			togglingIntake = true;
		}
		else if (togglingIntake == true)
		{
			togglingIntake = false;
			grip.setHolder(false);
		}
		else
			togglingIntake = false;

		if (operatorStick.GetRawAxis(Constants::shootyButton)!=0)
		{
			grip.setMotors(-1*operatorStick.GetRawAxis(Constants::shootyButton));
			togglingShooty = true;
		}
		else
			togglingShooty = false;

//		if (rIR.Get() && lIR.Get())
//		{
//			grip.setMotors(0.0);
//		}

		//////////////////////////////////////////////////////////////////////////////
		//							SUPER SHIFTER STUFFS							//
		//////////////////////////////////////////////////////////////////////////////

		if(driveStick.GetRawButton(Constants::shifterButton))
			shifter.set(true);
		else
			shifter.set(false);

		//////////////////////////////////////////////////////////////////////////////
		//								POSITIONATOR								//
		//////////////////////////////////////////////////////////////////////////////

		p = pos.findPosition(angle,robotDrive.GetLeftEncoder(),robotDrive.GetRightEncoder());
//		std::cout << p.xPosition << "       " << p.yPosition << "\n";
		SmartDashboard::PutNumber("xpos", p.xPosition);
		SmartDashboard::PutNumber("ypos", p.yPosition);

		frc::Wait(0.005);

	}

}

void setPoints(Waypoint* points, float x, float y, float degree, int point)
{
	if(x!=0&&y!=0)
	{
		x = x*0.0254 - 1+0.7366;
		y = y*0.0254 - 0.7366;
		x *= 0.85;
		y *= 1.0;
	}
	points[point] = {y,x,d2r(degree)};
	//points.push_back(p);
}

void Robot::runPathFinder(Waypoint* points, int POINT_LENGTH, int height)
{

	gyro.ZeroYaw();

	for (int i = 0; i < POINT_LENGTH; ++i)
	{
		points[i].y *= 1.135;
	}

	for (int i = 0; i < POINT_LENGTH; ++i)
		std::cout << points[i].x << " " << points[i].y << " " << "\n";

	TrajectoryCandidate cand;

	pathfinder_prepare(points, POINT_LENGTH, FIT_HERMITE_CUBIC, PATHFINDER_SAMPLES_FAST, 0.1, Constants::maxVelocity, Constants::maxAcceleration, Constants::maxJerk, &cand);

	std::cout << "YEETY" << "\n";

	int length = cand.length;

	Segment *traj = new Segment[length];

	int result = pathfinder_generate(&cand, traj);

	SmartDashboard::PutNumber("auto actually works", result);

	if (result >= 0)
	{
		Segment leftTraj[length];
		Segment rightTraj[length];

		pathfinder_modify_tank(traj, length, leftTraj, rightTraj, Constants::distanceBetweenWheelsMeters);

//		SmartDashboard::PutNumber("list length auto", length);
//
//		for (int i = 0; i < length; ++i)
//		{
//			Segment left = leftTraj[i];
//			Segment right = rightTraj[i];
//
//			SmartDashboard::PutNumber("left power auto", Constants::powerPerVelocity * left.velocity);
//			SmartDashboard::PutNumber("right power auto", Constants::powerPerVelocity * right.velocity);
//
//			float error = robotDrive.AngleConvert(traj[i].heading*57.29577958) - robotDrive.AngleConvert(gyro.GetYaw());
////			error = fabs(error) > 180 ? -1*copysign(360-fabs(error),error) : error;
//			float l = -0.0002*error;//-0.2*pid.PIDAngle(robotDrive.AngleConvert(traj[1].heading*57.29577958), robotDrive.AngleConvert(gyro.GetYaw())); //call pid loop
//			float r = -1*l;
//			SmartDashboard::PutNumber("POV", driveStick.GetPOV());
//
//			std::cout << error << "             " << l << "\n\n";
//
//			robotDrive.TankDrive(-1 * Constants::powerPerVelocity * left.velocity + l, -1 * Constants::powerPerVelocity * right.velocity + r);
//			pid.resetPIDAngle();
//			frc::Wait(0.1);
//		}

		EncoderFollower lenc;
		lenc.last_error = 0;
		lenc.segment = 0;
		lenc.finished = 0;

		EncoderConfig lconf = {robotDrive.GetLeftEncoder(), Constants::ticksPerRev, Constants::wheelCircumference, 0.3, 0.0, 0.0, 1.0 / Constants::actualMaxVelocity, 0.00};

		EncoderFollower renc;
		renc.last_error = 0;
		renc.segment = 0;
		renc.finished = 0;

		EncoderConfig rconf = {robotDrive.GetRightEncoder(), Constants::ticksPerRev, Constants::wheelCircumference, 0.3, 0.0, 0.0, 1.0 / Constants::actualMaxVelocity, 0.00};

		lift.SetPosition(height/Constants::lifterHeightPerRev);

		for (int i = 0; i < length; ++i)
		{
			float l = pathfinder_follow_encoder(lconf, &lenc, leftTraj, length, robotDrive.GetLeftEncoder());
			float r = pathfinder_follow_encoder(rconf, &renc, rightTraj, length, robotDrive.GetRightEncoder());

			float angle = gyro.GetYaw();
			angle = angle > 180 ? angle - 360 : angle;
			angle = angle < -180 ? angle + 360 : angle;

			float desang = -1*r2d(lenc.heading);
			desang = desang > 180 ? desang - 360 : desang;
			desang = desang < -180 ? desang + 360 : desang;

			float diff = desang - angle;
			std::cout << angle << "            " << desang << "           " << diff << "\n";
			float turn = 0.001 * diff;

			robotDrive.TankDrive(-1*(l+turn), -1*(r-turn));

			SmartDashboard::PutNumber("joe left", l);
			SmartDashboard::PutNumber("joe right", r);
			SmartDashboard::PutNumber("joe gyro", angle);
			SmartDashboard::PutNumber("joe looky angle", desang);
			SmartDashboard::PutNumber("joe angle error", diff);
			lift.Position();

			frc::Wait(0.1);
		}

		robotDrive.TankDrive(0.0,0.0);

		for (int i = 0; i < length; ++i)
			std::cout << leftTraj[i].velocity << ","; //Use these values to store and create a constant array containing the appropriate auto trajectory. They will print out in the rio log.
		std::cout << "\n";
		for (int i = 0; i < length; ++i)
			std::cout << rightTraj[i].velocity << ",";//These too
		std::cout << "\n";
	}

	delete traj;

}

void Robot::Autonomous()
{
	colorSend(8);
	SmartDashboard::PutBoolean("In Auto", true);
//	int POINT_LENGTH = 4;
//
	Waypoint *points  = new Waypoint[4];//POINT_LENGTH];
//	points[0] = {0,0,0};
//	points[1] = {1.0,-1.0,d2r(-45)};
//	points[2] = {1.5,-1.3,d2r(-20)};
//	points[3] = {3.3,-1.5,d2r(0)};

//	runPathFinder(points, POINT_LENGTH);

	std::string gameData("");
	int startingPos = 0; // 0 == left; 1 == middle; 2 == right;
	int target = 0; // 0 == our switch; 1 == scale; 2 == enemy switch
	bool justGoStraight = true;
	int ctr = 0;

	//Call from FMS and SmartDashboard
	while(gameData.length() < 2 && ctr < 100)
	{
		gameData = frc::DriverStation::GetInstance().GetGameSpecificMessage();
		ctr++;
		frc::Wait(0.05);
	}

	startingPos = SmartDashboard::GetNumber("Position (0L - 2R)", startingPos);
	target = SmartDashboard::GetNumber("Target (0Sw 1Sc 2eSw)", target);
	justGoStraight = SmartDashboard::GetBoolean("Just Go Straight", justGoStraight);
	robotDrive.DriveStraightReset();
	shifter.set(false);

//	if(gameData[0] == 'L' && startingPos == 0)
//	{
//		delete points;
//		points = new Waypoint[2];
////		setPoints(points,0,0,0,0);
////		setPoints(points,166.6,40,90,1);
//		points[0] = {0,0,0};
//		points[1] = {166.6*.0254,-40*.0254,d2r(-90)};
////		runPathFinder(points,2);
//		delete points;
//	}


	//New Auto Code
	//aim.GetHorizontalAngles()
	//robotDrive.DriveStraightDistance(.8,57);
	//robotDrive.turnToAngle(270);
	//Waypoint *points = new Waypoint[POINT_LENGTH];
	//points[0] = {0,0,0}
	//d2r(90)
	//Middle To Left Switch Then Right Scale
	if (justGoStraight || ctr >= 100)
	{
		//robotDrive.TankDrive(-0.35, -0.35);
		//frc::Wait(2.5);
		//robotDrive.TankDrive(0,0);
		robotDrive.DriveStraightDistance(0.5F, 12);
	}
	else
	{

		if (startingPos == 0 && gameData[0] == 'L')
		{
			robotDrive.TankDrive(-0.35, -0.35);
			frc::Wait(2.5);
			robotDrive.TankDrive(-0.3,0.3);
			frc::Wait(0.3);
			robotDrive.TankDrive(-0.25,-0.25);
			frc::Wait(1);
			lift.Lift(0.5);
			frc::Wait(1);
			grip.setDropper(!grip.getDropper());
			frc::Wait(0.5);
			grip.setMotors(-1.0);
			frc::Wait(0.5);
			grip.setMotors(0.0);
		}
		else if (startingPos == 0 && gameData[0] == 'R')
		{
			robotDrive.TankDrive(-0.35, -0.35);
			frc::Wait(2.5);
			robotDrive.TankDrive(0,0);
		}

		if (startingPos == 2 && gameData[0] == 'R')
		{
			robotDrive.TankDrive(-0.35, -0.35);
			frc::Wait(2.5);
			robotDrive.TankDrive(0.3,-0.3);
			frc::Wait(0.3);
			robotDrive.TankDrive(-0.25,-0.25);
			frc::Wait(1);
			lift.Lift(0.5);
			frc::Wait(1);
			grip.setDropper(!grip.getDropper());
			frc::Wait(0.5);
			grip.setMotors(-1.0);
			frc::Wait(0.5);
			grip.setMotors(0.0);
		}
		else if (startingPos == 2 && gameData[0] == 'L')
		{
			robotDrive.TankDrive(-0.35, -0.35);
			frc::Wait(2.5);
			robotDrive.TankDrive(0,0);
		}

//		if (gameData[0] == 'R' && startingPos == 2)
//		{
//			delete points;
//			grip.setDropper(false);
//			grip.setHolder(true);
//			points = new Waypoint[3];
//			setPoints(points,0,0,0,0);
//			setPoints(points,0,88,-30,1);
//			setPoints(points,37,168,-90,2);
//			runPathFinder(points, 3,31); //73
//			grip.setDropper(true);
//			robotDrive.ArcadeDrive(-0.25,0);
//			frc::Wait(0.3);
//			robotDrive.ArcadeDrive(0,0);
//			grip.setHolder(false);
//		}
//		if (gameData[0] == 'L' && startingPos == 0)
//		{
//			delete points;
//			grip.setDropper(false);
//			grip.setHolder(true);
//			points = new Waypoint[3];
//			setPoints(points,0,0,0,0);
//			setPoints(points,0,88,30,1);
//			setPoints(points,-37,168,90,2);
//			runPathFinder(points, 3,31); //73
//			grip.setDropper(true);
//			robotDrive.ArcadeDrive(-0.25,0);
//			frc::Wait(0.3);
//			robotDrive.ArcadeDrive(0,0);
//			grip.setHolder(false);
//		}
//		if (gameData[0] == 'R' && startingPos == 1)
//		{
//			delete points;
//			grip.setDropper(false);
//			grip.setHolder(false);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-54,140,0,1);
//			runPathFinder(points, 2,31);
//			grip.setDropper(true);
//			robotDrive.ArcadeDrive(-0.25,0);
//			frc::Wait(1);
//			robotDrive.ArcadeDrive(0,0);
//			grip.setHolder(false);
//		}
//		if (gameData[0] == 'L' && startingPos == 1)
//		{
//			delete points;
//			grip.setDropper(false);
//			grip.setHolder(false);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,54,144,0,1);
//			runPathFinder(points, 2,31);
//			grip.setDropper(true);
//			robotDrive.ArcadeDrive(-0.25,0);
//			frc::Wait(1);
//			robotDrive.ArcadeDrive(0,0);
//			grip.setHolder(false);
//		}
////		if (gameData[0] == 'L' && startingPos == 2)
////		{
////			delete points;
////			grip.setDropper(false);
////			grip.setHolder(true);
////			points = new Waypoint[3];
////			setPoints(points,0,0,0,0);
////			setPoints(points,0,88,-30,1);
////			setPoints(points,37,168,-90,2);
////			runPathFinder(points, 3,31); //73
////			grip.setDropper(true);
////			robotDrive.ArcadeDrive(-0.25,0);
////			frc::Wait(0.3);
////			robotDrive.ArcadeDrive(0,0);
////			grip.setHolder(false);
////		}
////		if (gameData[0] == 'R' && startingPos == 0)
////		{
////			delete points;
////			grip.setDropper(false);
////			grip.setHolder(true);
////			points = new Waypoint[3];
////			setPoints(points,0,0,0,0);
////			setPoints(points,0,88,30,1);
////			setPoints(points,-37,168,90,2);
////			runPathFinder(points, 3,31); //73
////			grip.setDropper(true);
////			robotDrive.ArcadeDrive(-0.25,0);
////			frc::Wait(0.3);
////			robotDrive.ArcadeDrive(0,0);
////			grip.setHolder(false);
////		}
//
//		if(gameData[0] == 'R' && gameData[1] =='L' && startingPos == 0)
//		{
////			delete points;
////			grip.setDropper(false);
////			grip.setHolder(true);
////			points = new Waypoint[3];
////			setPoints(points,0,0,0,0);
////			setPoints(points,10,180,0,1);
////			setPoints(points,-21,240,90,2);
////			runPathFinder(points, 3,73); //73
////			grip.setDropper(true);
////			grip.setHolder(false);
//			robotDrive.DriveStraightDistance(-0.75, 120);
//		}
//		if(gameData[0] == 'L' && gameData[1] =='R' && startingPos == 2)
//		{
////			delete points;
////			grip.setDropper(false);
////			grip.setHolder(true);
////			points = new Waypoint[3];
////			setPoints(points,0,0,0,0);
////			setPoints(points,-10,180,0,1);
////			setPoints(points,21,240,-90,2);
////			runPathFinder(points, 3,73); //73
////			grip.setDropper(true);
////			grip.setHolder(false);
//			robotDrive.DriveStraightDistance(-0.75, 120);
//		}
//		if (startingPos == 3)
//		{
//			delete points;
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-20,40,0,1);
//			runPathFinder(points, 2,73); //73
//			delete points;
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-40,40,90,1);
//			runPathFinder(points, 2,73); //73
//		}



//		if(gameData[0] == 'L' && gameData[1] =='R' && startingPos == 1)
//		{
//			delete points;
//			grip.setDropper(false);
//			grip.setHolder(true);
//			points = new Waypoint[4];
//			setPoints(points,0,0,0,0);
//			setPoints(points,0,70,270,1);
//			setPoints(points,-120,77,0,2);
//			setPoints(points,-86,166,90,3);
//			runPathFinder(points, 4,31);
//			grip.setDropper(true);
//			grip.setHolder(false);
//			frc::Wait(.2);
//			delete points;
//			robotDrive.DriveStraightDistance(-0.75,50);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,7,35,90,1);
//			runPathFinder(points,2,0);
//			grip.setMotors(.75);
//			grip.setHolder(true);
//			grip.setDropper(false);
//			frc::Wait(.2);
//			delete points;
//			points = new Waypoint[4];
//			setPoints(points,0,0,0,0);
//			setPoints(points,60,32,90,1);
//			setPoints(points,175,40,90,2);
//			setPoints(points,179,118,270,3);
//			runPathFinder(points, 4,73);
//			grip.setDropper(true);
//			grip.setHolder(false);
//		}
//
//		//Middle to Left Switch then Left Scale
//		if(gameData[0] == 'L' && gameData[1] =='L' && startingPos == 1)
//		{
//			delete points;
//			grip.setDropper(false);
//			grip.setHolder(true);
//			points = new Waypoint[4];
//			setPoints(points,0,0,0,0);
//			setPoints(points,0,70,270,1);
//			setPoints(points,-120,77,0,2);
//			setPoints(points,-86,166,90,3);
//			runPathFinder(points, 4,31);
//			grip.setDropper(true);
//			grip.setHolder(false);
//			frc::Wait(.2);
//			delete points;
//			robotDrive.DriveStraightDistance(-0.75,50);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,7,35,90,1);
//			runPathFinder(points, 2,0);
//			grip.setMotors(.75);
//			grip.setHolder(true);
//			grip.setDropper(false);
//			frc::Wait(.2);
//			delete points;
//			robotDrive.DriveStraightDistance(-0.75,50);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-15,120,90,1);
//			runPathFinder(points, 2,73);
//			grip.setDropper(true);
//			grip.setHolder(false);
//
//		}
//
//		//Middle to Right Switch then Left Scale
//		if(gameData[0] == 'R' && gameData[1] =='L' && startingPos == 1)
//		{
//			delete points;
//			grip.setDropper(false);
//			grip.setHolder(true);
//			points = new Waypoint[3];
//			setPoints(points,0,0,0,0);
//			setPoints(points,109,120,0,1);
//			setPoints(points,86,164,270,2);
//			runPathFinder(points, 3,31);
//			grip.setDropper(true);
//			grip.setHolder(false);
//			frc::Wait(.2);
//			delete points;
//			robotDrive.DriveStraightDistance(-0.75,30);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-2,34,270,1);
//			runPathFinder(points, 2,0);
//			grip.setMotors(.75);
//			grip.setHolder(true);
//			grip.setDropper(false);
//			frc::Wait(.2);
//			delete points;
//			robotDrive.DriveStraightDistance(-0.75,30);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-179,40,270,1);
//			runPathFinder(points, 2);
//			delete points;
//			robotDrive.DriveStraightDistance(0.75,30);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-1,83,90,1);
//			runPathFinder(points, 2,73);
//			grip.setDropper(true);
//			grip.setHolder(false);
//		}
//
//		//Middle to Right Switch then right scale
//		if(gameData[0] == 'R' && gameData[1] =='R' && startingPos == 1)
//		{
//			delete points;
//			grip.setDropper(false);
//			grip.setHolder(true);
//			points = new Waypoint[3];
//			setPoints(points,0,0,0,0);
//			setPoints(points,109,120,0,1);
//			setPoints(points,86,164,270,2);
//			runPathFinder(points, 3,31);
//			grip.setDropper(true);
//			grip.setHolder(false);
//			frc::Wait(.2);
//			delete points;
//			robotDrive.DriveStraightDistance(-0.75,30);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-2,34,270,1);
//			runPathFinder(points, 2,0);
//			grip.setMotors(.75);
//			grip.setHolder(true);
//			grip.setDropper(false);
//			frc::Wait(.2);
//			delete points;
//			robotDrive.DriveStraightDistance(-0.75,30);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,7,123,270,1);
//			runPathFinder(points, 2,73);
//			grip.setDropper(true);
//			grip.setHolder(false);
//		}
//
//		//Right to right scale then left switch
//		if(gameData	[0] == 'L' && gameData[1] =='R' && startingPos == 2)
//		{
//			delete points;
//			grip.setDropper(false);
//			grip.setHolder(true);
//			points = new Waypoint[3];
//			setPoints(points,0,0,0,0);
//			setPoints(points,39,166,0,1);
//			setPoints(points,-10,319,270,2);
//			runPathFinder(points, 3,73);
//			grip.setDropper(true);
//			grip.setHolder(false);
//			frc::Wait(.2);
//			delete points;
//			robotDrive.DriveStraightDistance(-0.75,30);
//			points = new Waypoint[4];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-14,-87,270,1);
//			setPoints(points,-120,-92,270,2);
//			setPoints(points,-172,-110,180,3);
//			runPathFinder(points, 4,0);
//			grip.setMotors(.75);
//			grip.setHolder(true);
//			grip.setDropper(false);
//			frc::Wait(.2);
//			delete points;
//			robotDrive.DriveStraightDistance(-0.75,20);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-7,-44,90,1);
//			runPathFinder(points, 2,31);
//			grip.setDropper(true);
//			grip.setHolder(false);
//		}
//
//		//Right to right switch then left scale
//		if(gameData[0] == 'R' && gameData[1] =='L' && startingPos == 2)
//		{
//			delete points;
//			grip.setDropper(false);
//			grip.setHolder(true);
//			points = new Waypoint[3];
//			setPoints(points,0,0,0,0);
//			setPoints(points,14,117,0,1);
//			setPoints(points,-24,162,270,2);
//			runPathFinder(points, 3,31);
//			grip.setDropper(true);
//			grip.setHolder(false);
//			frc::Wait(.2);
//			delete points;
//			robotDrive.DriveStraightDistance(-0.75,30);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-3,38,270,1);
//			runPathFinder(points, 2,0);
//			grip.setMotors(.75);
//			grip.setHolder(true);
//			grip.setDropper(false);
//			frc::Wait(.2);
//			delete points;
//			points = new Waypoint[3];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-38,67,270,1);
//			setPoints(points,-152,78,270,2);
//			runPathFinder(points, 3);
//			delete points;
//			robotDrive.DriveStraightDistance(0.75,50);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-30,78,90,1);
//			runPathFinder(points, 2,73);
//			grip.setDropper(true);
//			grip.setHolder(false);
//		}
//
//
//		//Right to left scale then left switch
//		if(gameData[0] == 'L' && gameData[1] =='L' && startingPos == 2)
//		{
//			delete points;
//			grip.setDropper(false);
//			grip.setHolder(true);
//			points = new Waypoint[4];
//			setPoints(points,0,0,0,0);
//			setPoints(points,0,238,270,1);
//			setPoints(points,-195,251,270,2);
//			setPoints(points,-208,314,90,3);
//			runPathFinder(points, 4,73); //73
//			grip.setDropper(true);
//			grip.setHolder(false);
//			delete points;
//			frc::Wait(0.2);
//			robotDrive.DriveStraightDistance(-0.75,50);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,16,-115,90,1);
//			runPathFinder(points, 2,0); //73
//			grip.setDropper(true);
//			grip.setMotors(.75);
//			grip.setHolder(true);
//			grip.setDropper(false);
//			delete points;
//			frc::Wait(0.2);
//			robotDrive.DriveStraightDistance(-0.75,50);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,0,-32,90,1);
//			runPathFinder(points, 2,31); //73
//			grip.setDropper(true);
//			grip.setHolder(false);
//		}
//
//		//Right to right switch then right scale
//		if(gameData[0] == 'R' && gameData[1] =='R' && startingPos == 2)
//		{
//			delete points;
//			grip.setDropper(false);
//			grip.setHolder(true);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-16,143,270,1);
//			runPathFinder(points, 2,31); //73
//			grip.setDropper(true);
//			grip.setHolder(false);
//			delete points;
//			frc::Wait(0.2);
//			robotDrive.DriveStraightDistance(-0.75,50);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-16,20,270,1);
//			runPathFinder(points, 2,0); //73
//			grip.setDropper(true);
//			grip.setMotors(.75);
//			grip.setHolder(true);
//			grip.setDropper(false);
//			delete points;
//			frc::Wait(0.2);
//			robotDrive.DriveStraightDistance(-0.75,50);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,10,153,270,1);
//			runPathFinder(points, 2,73); //73
//			grip.setDropper(true);
//			grip.setHolder(false);
//		}
//
//		//Left to left scale then right switch
//		if(gameData[0] == 'R' && gameData[1] =='L' && startingPos == 0)
//		{
//			delete points;
//			grip.setDropper(false);
//			grip.setHolder(true);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,21,318,90,1);
//			runPathFinder(points, 2,73); //73
//			grip.setDropper(true);
//			grip.setHolder(false);
//			delete points;
//			frc::Wait(0.2);
//			robotDrive.DriveStraightDistance(-0.75,50);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,24,-108,180,1);
//			runPathFinder(points, 2,0); //73
//			grip.setDropper(true);
//			grip.setMotors(.75);
//			grip.setHolder(true);
//			grip.setDropper(false);
//			delete points;
//			frc::Wait(0.2);
//			robotDrive.DriveStraightDistance(-0.75,30);
//			points = new Waypoint[3];
//			setPoints(points,0,0,0,0);
//			setPoints(points,171,22,90,1);
//			setPoints(points,160,-47,270,2);
//			runPathFinder(points, 3,31); //73
//			grip.setDropper(true);
//			grip.setHolder(false);
//		}
//
//		//Left to left switch then right scale I DIDNT DO
//		if(gameData[0] == 'L' && gameData[1] =='R' && startingPos == 0)
//		{
//			delete points;
//			grip.setDropper(false);
//			grip.setHolder(true);
//			points = new Waypoint[3];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-27,88,0,1);
//			setPoints(points,37,165,90,2);
//			runPathFinder(points, 3,31); //73
//			grip.setDropper(true);
//			grip.setHolder(false);
//			delete points;
//			frc::Wait(0.2);
//			robotDrive.DriveStraightDistance(-0.75,50);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-17,33,90,1);
//			runPathFinder(points, 2,0);
//			grip.setMotors(.75);
//			grip.setHolder(true);
//			grip.setDropper(false);
//			points = new Waypoint[5];
//			setPoints(points,0,0,0,0);
//			setPoints(points,41,65,90,1);
//			setPoints(points,67,73,90,2);
//			setPoints(points,191,733,90,3);
//			setPoints(points,179,153,270,4);
//			runPathFinder(points, 5);
//		}
//
//		//Left to right scale then right switch
//		if(gameData[0] == 'R' && gameData[1] =='R' && startingPos == 0)
//		{
//			delete points;
//			grip.setDropper(false);
//			grip.setHolder(true);
//			points = new Waypoint[5];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-22,118,0,1);
//			setPoints(points,-22,233,90,2);
//			setPoints(points,220,247,90,3);
//			setPoints(points,216,319,270,4);
//			runPathFinder(points, 5,73); //73
//			grip.setDropper(true);
//			grip.setHolder(false);
//			delete points;
//			frc::Wait(0.2);
//			robotDrive.DriveStraightDistance(-0.75,50);
//			points = new Waypoint[3];
//			setPoints(points,0,0,0,0);
//			setPoints(points,4,-69,180,1);
//			setPoints(points,-16,-118,270,2);
//			runPathFinder(points, 3,0); //73
//			grip.setDropper(true);
//			grip.setMotors(.75);
//			grip.setHolder(true);
//			grip.setDropper(false);
//			delete points;
//			frc::Wait(0.2);
//			robotDrive.DriveStraightDistance(-0.75,50);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,1,-35,270,1);
//			runPathFinder(points, 2,31); //73
//			grip.setDropper(true);
//			grip.setHolder(false);
//		}
//
//		//Left to left switch then left scale
//		if(gameData[0] == 'L' && gameData[1] =='L' && startingPos == 0)
//		{
//			delete points;
//			grip.setDropper(false);
//			grip.setHolder(true);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,35,161,90,1);
//			runPathFinder(points, 2,31); //73
//			grip.setDropper(true);
//			grip.setHolder(false);
//			delete points;
//			frc::Wait(0.2);
//			robotDrive.DriveStraightDistance(-0.75,50);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,1,37,90,1);
//			runPathFinder(points, 2,0); //73
//			grip.setDropper(true);
//			grip.setMotors(.75);
//			grip.setHolder(true);
//			grip.setDropper(false);
//			delete points;
//			frc::Wait(0.2);
//			robotDrive.DriveStraightDistance(-0.75,50);
//			points = new Waypoint[2];
//			setPoints(points,0,0,0,0);
//			setPoints(points,-14,122,90,1);
//			runPathFinder(points, 2,73); //73
//			grip.setDropper(true);
//			grip.setHolder(false);
//
//		}
		delete points;
	}
}

START_ROBOT_CLASS(Robot)
