#ifndef CAR_H
#define CAR_H

typedef enum CarMovementDirection {
    UP,
    DOWN
} CarMovementDirection;

typedef enum CarOperatingMode {
   AUTOMATIC,
   MANUAL
} CarOperatingMode;

typedef struct Car {
    CarMovementDirection movementDirection;
    CarOperatingMode operatingMode;
    int currentFloor;
} Car;

extern Car car1;
extern Car car2;
extern Car car3;

#endif