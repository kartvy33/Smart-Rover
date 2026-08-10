void cameraServoBegin()
{
    panServo.setPeriodHertz(50);
    tiltServo.setPeriodHertz(50);

    panServo.attach(CAMERA_PAN_PIN, 500, 2400);
    tiltServo.attach(CAMERA_TILT_PIN, 500, 2400);

    currentPan = CAMERA_PAN_CENTER;
    currentTilt = CAMERA_TILT_CENTER;

    panServo.write(currentPan);
    tiltServo.write(currentTilt);
}

void cameraServoUpdate()
{
}

void cameraServoCenter()
{
    currentPan = CAMERA_PAN_CENTER;
    currentTilt = CAMERA_TILT_CENTER;

    panServo.write(currentPan);
    tiltServo.write(currentTilt);
}

int getCameraPan()
{
    return currentPan;
}

int getCameraTilt()
{
    return currentTilt;
}
