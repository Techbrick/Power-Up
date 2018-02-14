#ifndef SRC_POSITIONATOR_H_
#define SRC_POSITIONATOR_H_
struct Position
{
	float xPosition;
	float yPosition;
};
class Positionator {
public:
	Positionator();

	float findPosition(float angle, int leftEncoder, int rightEncoder);
};

#endif /* SRC_POSITIONATOR_H_ */
