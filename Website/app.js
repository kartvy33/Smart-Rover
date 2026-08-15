/*
=========================================================
 SMART ROVER WEBSITE
 ESP32-WROOM + ESP32-CAM
=========================================================
*/


/* =====================================================
   NETWORK CONFIGURATION
   ===================================================== */

// The website is served by the ESP32-WROOM.
//
// Therefore we use the same IP that the browser used
// to open the website.
//
// Example:
// http://192.168.4.1
//
const ROVER_IP = window.location.hostname;


// Your ESP32-CAM IP
const CAMERA_IP = "192.168.4.2";


/* =====================================================
   URL HELPERS
   ===================================================== */

function roverURL(path) {
    return "http://" + ROVER_IP + path;
}

function cameraURL(path) {
    return "http://" + CAMERA_IP + path;
}


/* =====================================================
   ELEMENTS
   ===================================================== */

const connectionDot =
    document.getElementById("connectionDot");

const connectionText =
    document.getElementById("connectionText");

const cameraStatus =
    document.getElementById("cameraStatus");

const roverIP =
    document.getElementById("roverIP");


/* =====================================================
   CONNECTION STATUS
   ===================================================== */

function setOnline() {

    connectionDot.classList.remove("offline");

    connectionDot.classList.add("online");

    connectionText.textContent =
        "Rover Connected";
}


function setOffline() {

    connectionDot.classList.remove("online");

    connectionDot.classList.add("offline");

    connectionText.textContent =
        "Rover Offline";
}


/* =====================================================
   CAMERA
   ===================================================== */

function checkCamera() {

    const frame =
        document.getElementById("cameraFrame");

    frame.src =
        cameraURL("/");

    cameraStatus.textContent =
        "192.168.4.2";
}


/* =====================================================
   ROVER COMMAND
   ===================================================== */

async function sendCommand(command) {

    try {

        const response =
            await fetch(
                roverURL("/" + command),
                {
                    method: "GET",
                    cache: "no-store"
                }
            );

        if (response.ok) {

            setOnline();

        } else {

            setOffline();

        }

    } catch (error) {

        console.log(
            "Command error:",
            error
        );

        setOffline();
    }
}


/* =====================================================
   ROVER MOVEMENT
   ===================================================== */

function forward() {
    sendCommand("forward");
}

function backward() {
    sendCommand("backward");
}

function left() {
    sendCommand("left");
}

function right() {
    sendCommand("right");
}

function stop() {
    sendCommand("stop");
}


/* =====================================================
   BUTTONS
   ===================================================== */

document
    .getElementById("forwardBtn")
    .addEventListener("click", forward);

document
    .getElementById("backwardBtn")
    .addEventListener("click", backward);

document
    .getElementById("leftBtn")
    .addEventListener("click", left);

document
    .getElementById("rightBtn")
    .addEventListener("click", right);

document
    .getElementById("stopBtn")
    .addEventListener("click", stop);


/* =====================================================
   KEYBOARD CONTROL
   ===================================================== */

document.addEventListener(
    "keydown",
    function(event) {

        if (event.repeat) return;

        switch (event.key) {

            case "ArrowUp":
            case "w":
            case "W":
                forward();
                break;

            case "ArrowDown":
            case "s":
            case "S":
                backward();
                break;

            case "ArrowLeft":
            case "a":
            case "A":
                left();
                break;

            case "ArrowRight":
            case "d":
            case "D":
                right();
                break;

            case " ":
                stop();
                break;
        }
    }
);


document.addEventListener(
    "keyup",
    function(event) {

        if (
            event.key === "ArrowUp" ||
            event.key === "ArrowDown" ||
            event.key === "ArrowLeft" ||
            event.key === "ArrowRight" ||
            event.key.toLowerCase() === "w" ||
            event.key.toLowerCase() === "a" ||
            event.key.toLowerCase() === "s" ||
            event.key.toLowerCase() === "d"
        ) {

            stop();

        }
    }
);


/* =====================================================
   CAMERA PAN / TILT
   ===================================================== */

function cameraUp() {

    fetch(
        cameraURL("/camera/up")
    ).catch(console.log);
}


function cameraDown() {

    fetch(
        cameraURL("/camera/down")
    ).catch(console.log);
}


function cameraLeft() {

    fetch(
        cameraURL("/camera/left")
    ).catch(console.log);
}


function cameraRight() {

    fetch(
        cameraURL("/camera/right")
    ).catch(console.log);
}


function cameraCenter() {

    fetch(
        cameraURL("/camera/center")
    ).catch(console.log);
}


/* =====================================================
   CAMERA BUTTONS
   ===================================================== */

document
    .getElementById("cameraUp")
    .addEventListener(
        "click",
        cameraUp
    );

document
    .getElementById("cameraDown")
    .addEventListener(
        "click",
        cameraDown
    );

document
    .getElementById("cameraLeft")
    .addEventListener(
        "click",
        cameraLeft
    );

document
    .getElementById("cameraRight")
    .addEventListener(
        "click",
        cameraRight
    );

document
    .getElementById("cameraCenter")
    .addEventListener(
        "click",
        cameraCenter
    );


/* =====================================================
   ROVER STATUS
   ===================================================== */

async function updateStatus() {

    try {

        const response =
            await fetch(
                roverURL("/status"),
                {
                    method: "GET",
                    cache: "no-store"
                }
            );

        if (!response.ok) {

            setOffline();

            return;
        }

        const data =
            await response.json();

        setOnline();

        updateStatusDisplay(data);

    } catch (error) {

        console.log(
            "Status error:",
            error
        );

        setOffline();
    }
}


/* =====================================================
   STATUS DISPLAY
   ===================================================== */

function updateStatusDisplay(data) {

    if (data.battery !== undefined) {

        document.getElementById(
            "battery"
        ).textContent =
            data.battery;
    }


    if (data.satellites !== undefined) {

        document.getElementById(
            "satellites"
        ).textContent =
            data.satellites;
    }


    if (data.radio !== undefined) {

        document.getElementById(
            "radio"
        ).textContent =
            data.radio ?
            "Connected" :
            "Disconnected";
    }


    if (data.distance !== undefined) {

        document.getElementById(
            "distance"
        ).textContent =
            data.distance;
    }


    if (data.temperature !== undefined) {

        document.getElementById(
            "temperature"
        ).textContent =
            data.temperature;
    }


    if (data.humidity !== undefined) {

        document.getElementById(
            "humidity"
        ).textContent =
            data.humidity;
    }


    if (data.rain !== undefined) {

        document.getElementById(
            "rain"
        ).textContent =
            data.rain ?
            "Detected" :
            "No Rain";
    }


    if (data.pan !== undefined) {

        document.getElementById(
            "pan"
        ).textContent =
            data.pan;
    }


    if (data.tilt !== undefined) {

        document.getElementById(
            "tilt"
        ).textContent =
            data.tilt;
    }
}


/* =====================================================
   INITIALIZATION
   ===================================================== */

function initialize() {

    roverIP.textContent =
        ROVER_IP;

    checkCamera();

    updateStatus();

    setInterval(
        updateStatus,
        1000
    );
}


initialize();
