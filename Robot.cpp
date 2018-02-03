#include "Robot.h"
#include "I2C.h"
#include "WPILib.h"

Robot::Robot() :
		gyro(SPI::Port::kMXP),
		pid(),
		robotDrive(Constants::frontLeftDriveChannel,Constants::frontRightDriveChannel,Constants::rearLeftDriveChannel,Constants::rearRightDriveChannel,gyro,pid),
		driveStick(Constants::driveStickChannel),
		operatorStick(Constants::operatorStickChannel),
		compressor(),
		pdp(),
		lift(Constants::liftChannel),
		i2c(I2C::kOnboard,Constants::ledAddress)
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
	//TODO: remove these pid puts
	SmartDashboard::PutNumber("angle p val", .005);
	SmartDashboard::PutNumber("angle i val", .001);
	SmartDashboard::PutNumber("angle d val", .001);
	SmartDashboard::PutNumber("angle min turn val", .25);
	SmartDashboard::PutNumber("angle max turn val", .8);
	if (!IsEnabled())
	{
		colorSend(1);
	}
}

void Robot::OperatorControl()
{

	gyro.Reset();
	SmartDashboard::PutBoolean("in op ctrl", true);

	pid.setAngle(SmartDashboard::GetNumber("angle p val", .005), SmartDashboard::GetNumber("angle i val", .001), SmartDashboard::GetNumber("angle d val", .001));
	pid.setMinTurnSpeed(SmartDashboard::GetNumber("angle min turn val", .25));
	pid.setMaxTurnSpeed(SmartDashboard::GetNumber("angle max turn val", .8));

	float angle = 0;
	int driveMode = 0;

	bool togglingDriveMode = false;
	if (IsEnabled())
	{
		colorSend(2);
	}
	while(IsOperatorControl() && IsEnabled())
	{

		//////////////////////////////////////////////////////////////////////////////
		//								DRIVING CODE								//
		//////////////////////////////////////////////////////////////////////////////


		angle = robotDrive.AngleConvert(gyro.GetYaw());

		if(driveStick.GetPOV() != -1 && gyro.IsConnected())
		{ //turn to angle 0, 90, 180, 270
			float left = pid.PIDAngle(angle, driveStick.GetPOV()); //call pid loop
			float right = -1*left;
			robotDrive.TankDrive(left,right);
			SmartDashboard::PutNumber("POV", driveStick.GetPOV());
		}
		else if (driveStick.GetRawButton(Constants::driveStraightButtonFor12Inches))
		{
			robotDrive.DriveStraightDistance(0.15,12);
		}
		else
		{
			if (driveMode == 0)
			{
				SmartDashboard::PutNumber("drivemode", 0.0);

				float left = Constants::percentDrivePower*driveStick.GetRawAxis(Constants::tankDriveLeftAxis);
				float right = Constants::percentDrivePower*driveStick.GetRawAxis(Constants::tankDriveRightAxis);
				if (driveStick.GetRawButton(Constants::driveStraightButton))
				{
					robotDrive.DriveStraight(left,right);
					SmartDashboard::PutBoolean("Driving Straight", true);
				}
				else
				{
					robotDrive.TankDrive(left,right);
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
					robotDrive.DriveStraightArcade(fwd);
					SmartDashboard::PutBoolean("Driving Straight", true);
				}
				else
				{
					robotDrive.ArcadeDrive(fwd,turn);
					robotDrive.DriveStraightReset();
					SmartDashboard::PutBoolean("Driving Straight", false);
				}
			}
			else if (driveMode == 2)
			{
				float up = -1*Constants::percentDrivePower*driveStick.GetRawAxis(Constants::fieldOrientedDriveVerticalAxis);
				float right = Constants::percentDrivePower*driveStick.GetRawAxis(Constants::fieldOrientedDriveHorizontalAxis);
				robotDrive.FieldOrientedDrive(up,right);
			}
		}


		//////////////////////////////////////////////////////////////////////////////
		//								LIFT CODE									//
		//////////////////////////////////////////////////////////////////////////////


		if (driveStick.GetRawButton(Constants::liftUpButton))
		{
			sendColor(5);
			lift.Lift(-1 * (driveStick.GetRawAxis(Constants::liftUpAxis)/2.0 + 0.5));
		}
		else if (driveStick.GetRawButton(Constants::liftDownButton))
		{
			sendColor(7);
			lift.Lift(-1*-1*(driveStick.GetRawAxis(Constants::liftDownAxis)/2.0 + 0.5));
		}
		else
		{

			lift.Lift(0.0);
		}

		//////////////////////////////////////////////////////////////////////////////
		//							SMART DASHBOARD STUFF							//
		//////////////////////////////////////////////////////////////////////////////


		SmartDashboard::PutNumber("drive axis 1", driveStick.GetRawAxis(Constants::tankDriveLeftAxis));
		SmartDashboard::PutNumber("drive axis 5", driveStick.GetRawAxis(Constants::tankDriveRightAxis));
		SmartDashboard::PutNumber("Left Encoder", robotDrive.GetLeftEncoder());
		SmartDashboard::PutNumber("Left Encoder Inches", robotDrive.GetLeftEncoder() / Constants::ticksPerInch);
		SmartDashboard::PutNumber("Right Encoder", robotDrive.GetRightEncoder());
		SmartDashboard::PutNumber("Right Encoder Inches", robotDrive.GetRightEncoder() / Constants::ticksPerInch);
		SmartDashboard::PutNumber("gyro", angle);

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

		frc::Wait(0.005);

	}
	colorSend(1,1,0);

}
//struct Point
//{
//	float x;
//	float y;
//	float angle;
//};
//Point p;
//std::vector <Point> points;
float xR = 0;
float yR = 0;
float angleR = 0;
Waypoint *points = new Waypoint[1];
void setPoints(float x, float y, float degree, int point)
{
	xR = x*0.0254;
	yR = y*0.0254 - 0.7112;
	points[point] = {yR,xR,d2r(degree)};
	//points.push_back(p);
}
void Robot::Autonomous()
{

	std::string gameData;
	int startingPos = 0; // 0 == left; 1 == middle; 2 == right;
	int target = 0; // 0 == our switch; 1 == scale; 2 == enemy switch

	//Call from FMS and SmartDashboard
	gameData = frc::DriverStation::GetInstance().GetGameSpecificMessage();
	SmartDashboard::GetNumber("Position (0L - 2R)", startingPos);
	SmartDashboard::GetNumber("Target (0Sw 1Sc 2eSw)", target);

//	New Auto Code
//	aim.GetHorizontalAngles()
//	robotDrive.DriveStraightDistance(.8,57);
//	robotDrive.turnToAngle(270);
//	Waypoint *points = new Waypoint[POINT_LENGTH];
//	points[0] = {0,0,0}
//	d2r(90)
//	Middle To Left Switch Then Right Scale
	colorSend(8);
	if(gameData[0] == 'L' && gameData[1] =='R' && startingPos == 1)
		{
			Waypoint *points = new Waypoint[7];
			setPoints(0,70,270,0);
			setPoints(-120,77,0,1);
			setPoints(-86,166,90,2);
			robotDrive.DriveStraightDistance(-1,50);
			setPoints(-79,201,90,3);
			setPoints(-19,233,90,4);
			setPoints(96,241,90,5);
			setPoints(100,319,270,6);
		}

		//Middle to Left Switch then Left Scale
		if(gameData[0] == 'L' && gameData[1] =='L' && startingPos == 1)
		{
			Waypoint *points = new Waypoint[5];
			setPoints(0,70,270,0);
			setPoints(-120,77,0,1);
			setPoints(-86,166,90,2);
			robotDrive.DriveStraightDistance(-1,50);
			setPoints(-79,201,90,3);
			robotDrive.DriveStraightDistance(-1,50);
			setPoints(-94,321,90,4);
		}

		//Middle to Right Switch then Left Scale
		if(gameData[0] == 'R' && gameData[1] =='L' && startingPos == 1)
		{
			Waypoint *points = new Waypoint[5];
			setPoints(109,120,0,0);
			setPoints(86,164,270,1);
			robotDrive.DriveStraightDistance(-1,30);
			setPoints(84,198,270,2);
			robotDrive.DriveStraightDistance(-1,30);
			setPoints(-95,238,270,3);
			robotDrive.DriveStraightDistance(1,30);
			setPoints(-94,321,90,4);
		}

		//Middle to Right Switch then right scale
		if(gameData[0] == 'R' && gameData[1] =='R' && startingPos == 1)
		{
			Waypoint *points = new Waypoint[4];
			setPoints(109,120,0,0);
			setPoints(86,164,270,1);
			robotDrive.DriveStraightDistance(-1,30);
			setPoints(84,198,270,2);
			robotDrive.DriveStraightDistance(-1,30);
			setPoints(91,321,270,3);
		}

		//Right to right scale then left switch
		if(gameData[0] == 'L' && gameData[1] =='R' && startingPos == 2)
		{
			Waypoint *points = new Waypoint[6];
			setPoints(39,166,0,0);
			setPoints(-10,319,270,1);
			robotDrive.DriveStraightDistance(-1,30);
			setPoints(-24,232,270,2);
			setPoints(-130,227,270,3);
			setPoints(-182,209,180,4);
			robotDrive.DriveStraightDistance(-1,20);
			setPoints(-189,165,90,5);
		}

		//Right to right switch then left scale
		if(gameData[0] == 'R' && gameData[1] =='L' && startingPos == 2)
		{
			Waypoint *points = new Waypoint[6];
			setPoints(14,117,0,0);
			setPoints(-24,162,270,1);
			robotDrive.DriveStraightDistance(-1,30);
			setPoints(-27,200,270,2);
			setPoints(-62,229,270,3);
			setPoints(-176,240,270,4);
			robotDrive.DriveStraightDistance(1,50);
			setPoints(-206,318,90,5);
		}


		//Right to left scale then left switch
		if(gameData[0] == 'L' && gameData[1] =='L' && startingPos == 2)
		{
         Waypoint *points = new Waypoint[5];
			setPoints(0,238,270,0);
			setPoints(-195,251,270,1);
			setPoints(-208,314,90,2);
			robotDrive.DriveStraightDistance(-1,50);
			setPoints(-192,199,90,3);
			robotDrive.DriveStraightDistance(-1,50);
			setPoints(-192,167,90,4);
		}

		//Right to right switch then right scale
		if(gameData[0] == 'R' && gameData[1] =='R' && startingPos == 2)
		{
         Waypoint *points = new Waypoint[3];
			setPoints(-16,143,270,0);
			robotDrive.DriveStraightDistance(-1,50);
			setPoints(-32,163,270,1);
			robotDrive.DriveStraightDistance(-1,50);
			setPoints(-22,316,270,2);
		}

		//Left to left scale then right switch
		if(gameData[0] == 'R' && gameData[1] =='L' && startingPos == 0)
		{
         Waypoint *points = new Waypoint[4];
			setPoints(21,318,90,0);
			robotDrive.DriveStraightDistance(-1,50);
			setPoints(45,210,180,1);
			robotDrive.DriveStraightDistance(-1,30);
			setPoints(216,232,90,2);
			setPoints(205,163,270,3);
		}

		//Left to left switch then right scale
		if(gameData[0] == 'L' && gameData[1] =='R' && startingPos == 0)
		{
         Waypoint *points = new Waypoint[8];
			setPoints(-27,88,0,0);
			setPoints(37,165,90,1);
			robotDrive.DriveStraightDistance(-1,50);
			setPoints(20,198,90,3);
			setPoints(78,230,90,4);
			setPoints(104,238,90,5);
			setPoints(228,238,90,6);
			setPoints(216,318,270,7);
		}

		//Left to right scale then right switch
		if(gameData[0] == 'R' && gameData[1] =='R' && startingPos == 0)
		{
         Waypoint *points = new Waypoint[7];
			setPoints(-22,118,0,0);
			setPoints(-22,233,90,1);
			setPoints(220,247,90,2);
			setPoints(216,319,270,3);
         robotDrive.DriveStraightDistance(-1,50);
         setPoints(220,250,180,4);
			setPoints(200,201,270,5);
         robotDrive.DriveStraightDistance(-1,50);
			setPoints(201,166,270,6);
		}

		//Left to left switch then left scale
		if(gameData[0] == 'L' && gameData[1] =='L' && startingPos == 0)
		{
         Waypoint *points = new Waypoint[3];
			setPoints(35,161,90,0);
         robotDrive.DriveStraightDistance(-1,50);
			setPoints(36,198,90,1);
			setPoints(22,320,90,2);
		}
	/*
	if(gameData[0] == 'L' && gameData[1] =='R' && startingPos == 1)
	{
		Waypoint *points = new Waypoint[];
		p.angle = 0;
		setPoints(0,77);
		robotDrive.DriveStraightDistance(-1,50);
		setPoints(-105,141);
		p.angle = 90;
		setPoints(-86,166);
		setPoints(-122,195);
		p.angle = 90;
		setPoints(-79,201);
		setPoints(-19,233);
		setPoints(96,241);
		setPoints(119,319);
		p.angle = 270;
		setPoints(100,319);
	}

	//Middle to Left Switch then Left Scale
	if(gameData[0] == 'L' && gameData[1] =='L' && startingPos == 1)
	{
		p.angle = 0;
		setPoints(0,77);
		setPoints(-105,141);
		p.angle = 90;
		setPoints(-86,166);
		setPoints(-122,195);
		p.angle = 90;
		setPoints(-79,201);
		setPoints(-118,310);
		p.angle = 90;
		setPoints(-94,321);
	}

	//Middle to Right Switch then Left Scale
	if(gameData[0] == 'R' && gameData[1] =='L' && startingPos == 1)
	{
		p.angle = 0;
		setPoints(109,120);
		p.angle = 270;
		setPoints(86,164);
		setPoints(105,179);
		p.angle = 270;
		setPoints(84,198);
		setPoints(63,236);
		setPoints(-95,238);
		setPoints(-132,303);
		p.angle = 90;
		setPoints(-94,321);
	}

	//Middle to Right Switch then right scale
	if(gameData[0] == 'R' && gameData[1] =='R' && startingPos == 1)
	{
		p.angle = 0;
		setPoints(109,120);
		p.angle = 270;
		setPoints(86,164);
		setPoints(105,179);
		p.angle = 270;
		setPoints(84,198);
		setPoints(139,290);
		p.angle = 270;
		setPoints(91,321);
	}

	//Right to right scale then left switch
	if(gameData[0] == 'L' && gameData[1] =='R' && startingPos == 2)
	{
		p.angle = 0;
		setPoints(39,166);
		p.angle = 270;
		setPoints(-10,319);
		p.angle = 225;
		setPoints(-24,232);
		setPoints(-141,227);
		setPoints(-182,209);
		setPoints(-212,170);
		p.angle = 90;
		setPoints(-189,165);
	}

	//Right to right switch then left scale
	if(gameData[0] == 'R' && gameData[1] =='L' && startingPos == 2)
	{
		p.angle = 0;
		setPoints(14,117);
		setPoints(0,144);
		p.angle = 270;
		setPoints(-24,162);
		setPoints(-14,181);
		p.angle = 270;
		setPoints(-27,200);
		setPoints(-62,229);
		setPoints(-176,240);
		setPoints(-233,263);
		setPoints(-219,318);
		p.angle = 90;
		setPoints(-206,318);
	}

	//Right to left scale then left switch
	if(gameData[0] == 'L' && gameData[1] =='L' && startingPos == 2)
	{
		p.angle = 0;
		setPoints(-30,171);
		setPoints(-24,216);
		setPoints(-195,251);
		p.angle = 90;
		setPoints(-208,314);
		p.angle = 210;
		setPoints(-244,254);
		p.angle = 90;
		setPoints(-192,199);
		setPoints(-210,184);
		p.angle = 90;
		setPoints(-192,167);
	}

	//Right to right switch then right scale
	if(gameData[0] == 'R' && gameData[1] =='R' && startingPos == 2)
	{
		p.angle = 0;
		setPoints(-16,143);
		p.angle = 270;
		setPoints(-32,163);
		p.angle = 45;
		setPoints(-18,185);
		setPoints(-33,200);
		setPoints(22,298);
		p.angle = 270;
		setPoints(-22,316);
	}

	//Left to left scale then right switch
	if(gameData[0] == 'R' && gameData[1] =='L' && startingPos == 0)
	{
		p.angle = 0;
		setPoints(-14,235);
		p.angle = 90;
		setPoints(21,318);
		p.angle = 160;
		setPoints(0,284);
		setPoints(45,210);
		setPoints(111,241);
		setPoints(216,232);
		p.angle = 270;
		setPoints(205,163);
	}

	//Left to left switch then right scale
	if(gameData[0] == 'L' && gameData[1] =='R' && startingPos == 0)
	{
		p.angle = 0;
		setPoints(-27,88);
		p.angle = 90;
		setPoints(37,165);
		p.angle = 315;
		setPoints(20,191);
		setPoints(36,196);
		setPoints(104,238);
		setPoints(228,254);
		setPoints(236,309);
		p.angle = 270;
		setPoints(216,318);
	}

	//Left to right scale then right switch
	if(gameData[0] == 'R' && gameData[1] =='R' && startingPos == 0)
	{
		p.angle = 0;
		setPoints(-22,118);
		setPoints(33,233);
		setPoints(220,247);
		p.angle = 270;
		setPoints(216,319);
		p.angle = 80;
		setPoints(220,250);
		setPoints(200,201);
		setPoints(233,182);
		p.angle = 90;
		setPoints(201,166);
	}

	//Left to left switch then left scale
	if(gameData[0] == 'L' && gameData[1] =='L' && startingPos == 0)
	{
		p.angle = 0;
		setPoints(-17,109);
		p.angle = 90;
		setPoints(35,161);
		p.angle = 315;
		setPoints(10,195);
		setPoints(36,198);
		setPoints(0,275);
		p.angle = 90;
		setPoints(22,320);
	}
	*/



	/* Change all 0.889 to 0.7112
	p.x = 0;
	p.y = 77*0.0254 - .7112;
	p.x = -86*0.0254;
	p.y = 166*0.0254 - .7112;;
	points.push_back(p);
	p.x = -122*0.0254;
	p.y = 195*0.0254 - 0.889;
	points.push_back(p);
	p.x = -79*0.0254;
	p.y = 201*0.0254 - 0.889;
	points.push_back(p);
	p.x = -19*0.0254;
	p.y = 233*0.0254 - 0.889;
	points.push_back(p);
	p.x = 96*0.0254;
	p.y = 241*0.0254 - 0.889;
	points.push_back(p);
	p.x = 119*0.0254;
	p.y = 319*0.0254 - 0.889;
	points.push_back(p);
	p.x = 100*0.0254;
	p.y = 319*0.0254 - 0.889;
	points.push_back(p);

	//Middle to Left Switch then Left Scale
	p.x = 0;
	p.y = 77*0.0254 - 0.889;
	points.push_back(p);
	p.x = -105*0.0254;
	p.y = 141*0.0254 - 0.889;
	points.push_back(p);
	p.x = -86*0.0254;
	p.y = 166*0.0254 - 0.889;
	points.push_back(p);
	p.x = -122*0.0254;
	p.y = 195*0.0254 - 0.889;
	points.push_back(p);
	p.x = -79*0.0254;
	p.y = 201*0.0254 - 0.889;
	points.push_back(p);
	p.x = -118*0.0254;
	p.y = 310*0.0254 - 0.889;
	points.push_back(p);
	p.x = -94*0.0254;
	p.y = 321*0.0254 - 0.889;
	points.push_back(p);

	//Middle to Right Switch then Left to Scale
	p.x = 109*0.0254;
	p.y = 120*0.0254 - 0.889;
	points.push_back(p);
	p.x = 86*0.0254;
	p.y = 164*0.0254 - 0.889;
	points.push_back(p);
	p.x = 105*0.0254;
	p.y = 179*0.0254 - 0.889;
	points.push_back(p);
	p.x = 84*0.0254;
	p.y = 198*0.0254 - 0.889;
	points.push_back(p);
	p.x = 63*0.0254;
	p.y = 236*0.0254 - 0.889;
	points.push_back(p);
	p.x = -95*0.0254;
	p.y = 238*0.0254 - 0.889;
	points.push_back(p);
	p.x = -132*0.0254;
	p.y = 303*0.0254 - 0.889;
	points.push_back(p);
	p.x = -94*0.0254;
	p.y = 321*0.0254 - 0.889;
	points.push_back(p);

	//Middle to Right Switch then Right Scale
	p.x = 109*0.0254;
	p.y = 120*0.0254 - 0.889;
	points.push_back(p);
	p.x = 86*0.0254;
	p.y = 164*0.0254 - 0.889;
	points.push_back(p);
	p.x = 105*0.0254;
	p.y = 179*0.0254 - 0.889;
	points.push_back(p);
	p.x = 84*0.0254;
	p.y = 198*0.0254 - 0.889;
	points.push_back(p);
	p.x = 131*0.0254;
	p.y = 290*0.0254 - 0.889;
	points.push_back(p);
	p.x = 91*0.0254;
	p.y = 321*0.0254 - 0.889;
	points.push_back(p);

	//Right to Right Scale then Left Switch
	p.x = 39*0.0254;
	p.y = 166*0.0254 - 0.889;
	points.push_back(p);
	p.x = -10*0.0254;
	p.y = 319*0.0254 - 0.889;
	points.push_back(p);
	p.x = -24*0.0254;
	p.y = 232*0.0254 - 0.889;
	points.push_back(p);
	p.x = -141*0.0254;
	p.y = 227*0.0254 - 0.889;
	points.push_back(p);
	p.x = -182*0.0254;
	p.y = 209*0.0254 - 0.889;
	points.push_back(p);
	p.x = -212*0.0254;
	p.y = 170*0.0254 - 0.889;
	points.push_back(p);
	p.x = -181*0.0254;
	p.y = 165*0.0254 - 0.889;
	points.push_back(p);

	//Right to Right Switch then Left Scale
	p.x = 14*0.0254;
	p.y = 117*0.0254 - 0.889;
	points.push_back(p);
	p.x = 0*0.0254;
	p.y = 144*0.0254 - 0.889;
	points.push_back(p);
	p.x = -24*0.0254;
	p.y = 162*0.0254 - 0.889;
	points.push_back(p);
	p.x = -14*0.0254;
	p.y = 181*0.0254 - 0.889;
	points.push_back(p);
	p.x = -27*0.0254;
	p.y = 200*0.0254 - 0.889;
	points.push_back(p);
	p.x = -62*0.0254;
	p.y = 229*0.0254 - 0.889;
	points.push_back(p);
	p.x = -176*0.0254;
	p.y = 240*0.0254 - 0.889;
	points.push_back(p);
	p.x = -233*0.0254;
	p.y = 263*0.0254 - 0.889;
	points.push_back(p);
	p.x = -219*0.0254;
	p.y = 318*0.0254 - 0.889;
	points.push_back(p);
	p.x = -206*0.0254;
	p.y = 318*0.0254 - 0.889;
	points.push_back(p);

	//Right to Left Scale then Left Switch
	p.x = -30*0.0254;
	p.y = 171*0.0254 - 0.889;
	points.push_back(p);
	p.x = -24*0.0254;
	p.y = 216*0.0254 - 0.889;
	points.push_back(p);
	p.x = -195*0.0254;
	p.y = 251*0.0254 - 0.889;
	points.push_back(p);
	p.x = -208*0.0254;
	p.y = 314*0.0254 - 0.889;
	points.push_back(p);
	p.x = -244*0.0254;
	p.y = 254*0.0254 - 0.889;
	points.push_back(p);
	p.x = -192*0.0254;
	p.y = 199*0.0254 - 0.889;
	points.push_back(p);
	p.x = -210*0.0254;
	p.y = 184*0.0254 - 0.889;
	points.push_back(p);
	p.x = -192*0.0254;
	p.y = 167*0.0254 - 0.889;
	points.push_back(p);

	//Right to Right Switch then Right Scale
	p.x = -16*0.0254;
	p.y = 143*0.0254 - 0.889;
	points.push_back(p);
	p.x = -32*0.0254;
	p.y = 163*0.0254 - 0.889;
	points.push_back(p);
	p.x = -18*0.0254;
	p.y = 185*0.0254 - 0.889;
	points.push_back(p);
	p.x = -33*0.0254;
	p.y = 200*0.0254 - 0.889;
	points.push_back(p);
	p.x = 22*0.0254;
	p.y = 298*0.0254 - 0.889;
	points.push_back(p);
	p.x = -22*0.0254;
	p.y = 316*0.0254 - 0.889;
	points.push_back(p);

	//Left to Left Scale then Right Switch
	p.x = -14*0.0254;
	p.y = 235*0.0254 - 0.889;
	points.push_back(p);
	p.x = 21*0.0254;
	p.y = 318*0.0254 - 0.889;
	points.push_back(p);
	p.x = 0*0.0254;
	p.y = 284*0.0254 - 0.889;
	points.push_back(p);
	p.x = 45*0.0254;
	p.y = 210*0.0254 - 0.889;
	points.push_back(p);
	p.x = 111*0.0254;
	p.y = 241*0.0254 - 0.889;
	points.push_back(p);
	p.x = 216*0.0254;
	p.y = 232*0.0254 - 0.889;
	points.push_back(p);
	p.x = 205*0.0254;
	p.y = 163*0.0254 - 0.889;
	points.push_back(p);

	//Left to Left Switch then Right Scale
	p.x = -27*0.0254;
	p.y = 88*0.0254 - 0.889;
	points.push_back(p);
	p.x = 37*0.0254;
	p.y = 165*0.0254 - 0.889;
	points.push_back(p);
	p.x = 20*0.0254;
	p.y = 191*0.0254 - 0.889;
	points.push_back(p);
	p.x = 36*0.0254;
	p.y = 196*0.0254 - 0.889;
	points.push_back(p);
	p.x = 104*0.0254;
	p.y = 238*0.0254 - 0.889;
	points.push_back(p);
	p.x = 228*0.0254;
	p.y = 254*0.0254 - 0.889;
	points.push_back(p);
	p.x = 236*0.0254;
	p.y = 309*0.0254 - 0.889;
	points.push_back(p);
	p.x = 216*0.0254;
	p.y = 318*0.0254 - 0.889;
	points.push_back(p);

	//Left to Right Scale then Right Switch
	p.x = -22*0.0254;
	p.y = 118*0.0254 - 0.889;
	points.push_back(p);
	p.x = 33*0.0254;
	p.y = 233*0.0254 - 0.889;
	points.push_back(p);
	p.x = 220*0.0254;
	p.y = 247*0.0254 - 0.889;
	points.push_back(p);
	p.x = 216*0.0254;
	p.y = 319*0.0254 - 0.889;
	points.push_back(p);
	p.x = 220*0.0254;
	p.y = 250*0.0254 - 0.889;
	points.push_back(p);
	p.x = 200*0.0254;
	p.y = 201*0.0254 - 0.889;
	points.push_back(p);
	p.x = 233*0.0254;
	p.y = 182*0.0254 - 0.889;
	points.push_back(p);
	p.x = 201*0.0254;
	p.y = 166*0.0254 - 0.889;
	points.push_back(p);

	//Left to Left Switch then Left Scale
	p.x = -17*0.0254;
	p.y = 109*0.0254 - 0.889;
	points.push_back(p);
	p.x = 35*0.0254;
	p.y = 161*0.0254 - 0.889;
	points.push_back(p);
	p.x = 10*0.0254;
	p.y = 195*0.0254 - 0.889;
	points.push_back(p);
	p.x = 36*0.0254;
	p.y = 198*0.0254 - 0.889;
	points.push_back(p);
	p.x = 0*0.0254;
	p.y = 275*0.0254 - 0.889;
	points.push_back(p);
	p.x = 22*0.0254;
	p.y = 320*0.0254 - 0.889;
	points.push_back(p);
	*/




	/*
	//These if statements require smart dashboard integration.			F I X E D
	if(gameData[0] == 'L' && startingPos == 0 && target == 0){
	//Left to Our Left
		robotDrive.DriveStraightDistance(0.8,166);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 40);
		SuperLifter::SuperLifter(36);
	}
	if(gameData[0] == 'R' && startingPos == 0 && target == 0 ){
	//Left to Our Right
		robotDrive.DriveStraightDistance(0.8,232);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 232);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 58);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 27);
	}
	if(gameData[2] == 'L' && startingPos == 0 && target == 2 ){
	//Left to Enemy Left
		robotDrive.DriveStraightDistance(0.8,478);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 40);
	}
	if(gameData[2] == 'R' && startingPos == 0 && target == 2 ){
	//Left to Enemy Right
		robotDrive.DriveStraightDistance(0.8,232);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 250);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 255);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 43);
	}
	if(gameData[0] == 'R' && startingPos == 2 && target == 0 ){
	//Right to Our Right
		robotDrive.DriveStraightDistance(0.8,166);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 40);
	}
	if(gameData[2] == 'R' && startingPos == 2 && target == 2 ){
	//Right to Enemy Right
		robotDrive.DriveStraightDistance(0.8,478);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 40);
	}
	//Right to Our Left
	if(gameData[0] == 'L' && startingPos == 2 && target == 0 ){
		robotDrive.DriveStraightDistance(0.8,232);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 232);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 58);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 27);
	}
	if(gameData[2] == 'L' && startingPos == 2 && target == 2 ){
	//Right to Enemy Left
		robotDrive.DriveStraightDistance(0.8,232);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 250);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 255);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 43);
	}
	if(gameData[0] == 'R' && startingPos == 1 && target == 0 ){
		//Middle to Our Right
		robotDrive.DriveStraightDistance(.8,57);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 95);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 110);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 25);
	}
	if(gameData[2] == 'R' && startingPos == 1 && target == 2 ){
	//Middle to Enemy Right
		robotDrive.DriveStraightDistance(.8,58);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 110);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 422);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 35);
	}
	//Middle to our Left
	if(gameData[0] == 'L' && startingPos == 1 && target == 0 ){
		robotDrive.DriveStraighDistance(.8,57);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 130);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 108);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 40);
	}
	if(gameData[2] == 'L' && startingPos == 1 && target == 2 ){
	//Middle to Enemy Left
		robotDrive.DriveStraightDistance(.8,57);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 130);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 423);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 40);
	}

	if(gameData[1] == 'L' && startingPos == 0 && target == 1 ){
	//Left to Left Scale
		robotDrive.DriveStraightDistance(.8,324);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 19);
	}
	//Left to Right Scale
	if(gameData[1] == 'R' && startingPos == 0 && target == 1 ){
		robotDrive.DriveStraightDistance(.8,225);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 227);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 98);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 17);
	}


	if(gameData[1] == 'R' && startingPos == 2 && target == 1 ){
	//Right to Right Scale
		robotDrive.DriveStraightDistance(.8,324);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 19);
	}


	if(gameData[1] == 'L' && startingPos == 2 && target == 1 ){
		//Right to Left Scale
		robotDrive.DriveStraightDistance(.8,225);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(0.8, 227);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 98);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(0.8, 17);
	}



	if(gameData[1] == 'L' && startingPos == 1 && target == 1 ){
	//Our Middle to Left Scale
		robotDrive.DriveStraightDistance(.8, 57);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(.8, 119);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(.8, 267);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(.8, 14);
	}


	if(gameData[1] == 'R' && startingPos == 1 && target == 1 ){
		//Our Middle to Right Scale
		robotDrive.DriveStraightDistance(.8, 58);
		robotDrive.turnToAngle(90);
		robotDrive.DriveStraightDistance(.8, 107);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(.8, 267);
		robotDrive.turnToAngle(270);
		robotDrive.DriveStraightDistance(.8, 19);
	}
	*/

}



