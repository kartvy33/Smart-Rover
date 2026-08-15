/*
=========================================================
 SMART ROVER WEBSITE
 ESP32-WROOM + ESP32-CAM
=========================================================
*/

const ROVER_IP = "192.168.4.1";

const CAMERA_IP = "192.168.4.2";

const ROVER =
    "http://" + ROVER_IP;

const CAMERA =
    "http://" + CAMERA_IP;


/* =====================================================
   ELEMENTS
   ===================================================== */

const connectionDot =
    document.getElementById(
        "connectionDot"
    );

const connectionText =
    document.getElementById(
        "connectionText"
    );

const cameraStatus =
    document.getElementById(
        "cameraStatus"
    );


/* =====================================================
   CONNECTION
   ===================================================== */

function setOnline()
{
    connectionDot.classList.remove(
        "offline"
    );

    connectionDot.classList.add(
        "online"
    );

    connectionText.textContent =
        "Rover Connected";
}


function setOffline()
{
    connectionDot.classList.remove(
        "online"
    );

    connectionDot.classList.add(
        "offline"
    );

    connectionText.textContent =
        "Rover Offline";
}


/* =====================================================
   ROVER COMMAND
   ===================================================== */

async function sendCommand(command)
{
    try
    {
        const response =
            await fetch(
                ROVER +
                "/" +
                command,
                {
                    method:"GET",
                    cache:"no-store"
                }
            );

        if(response.ok)
        {
            setOnline();
        }
        else
        {
            setOffline();
        }
    }
    catch(error)
    {
        console.log(
            "Rover command error:",
            error
        );

        setOffline();
    }
}


/* =====================================================
   ROVER BUTTONS
   ===================================================== */

document
    .getElementById("forwardBtn")
    .addEventListener(
        "click",
        () => sendCommand("forward")
    );


document
    .getElementById("backwardBtn")
    .addEventListener(
        "click",
        () => sendCommand("backward")
    );


document
    .getElementById("leftBtn")
    .addEventListener(
        "click",
        () => sendCommand("left")
    );


document
    .getElementById("rightBtn")
    .addEventListener(
        "click",
        () => sendCommand("right")
    );


document
    .getElementById("stopBtn")
    .addEventListener(
        "click",
        () => sendCommand("stop")
    );


/* =====================================================
   CAMERA COMMAND
   ===================================================== */

async function cameraCommand(command)
{
    try
    {
        const response =
            await fetch(
                CAMERA +
                "/api/camera/" +
                command,
                {
                    method:"POST",
                    cache:"no-store"
                }
            );

        if(!response.ok)
        {
            console.log(
                "Camera command failed"
            );
        }
    }
    catch(error)
    {
        console.log(
            "Camera command error:",
            error
        );
    }
}


/* =====================================================
   CAMERA BUTTONS
   ===================================================== */

document
    .getElementById("cameraUp")
    .addEventListener(
        "click",
        () => cameraCommand("up")
    );


document
    .getElementById("cameraDown")
    .addEventListener(
        "click",
        () => cameraCommand("down")
    );


document
    .getElementById("cameraLeft")
    .addEventListener(
        "click",
        () => cameraCommand("left")
    );


document
    .getElementById("cameraRight")
    .addEventListener(
        "click",
        () => cameraCommand("right")
    );


document
    .getElementById("cameraCenter")
    .addEventListener(
        "click",
        () => cameraCommand("center")
    );


/* =====================================================
   ROVER STATUS
   ===================================================== */

async function updateRoverStatus()
{
    try
    {
        const response =
            await fetch(
                ROVER +
                "/status",
                {
                    cache:"no-store"
                }
            );

        if(!response.ok)
        {
            setOffline();
            return;
        }

        const data =
            await response.json();

        setOnline();

        if(data.battery !== undefined)
        {
            document.getElementById(
                "battery"
            ).textContent =
                data.battery;
        }

        if(data.voltage !== undefined)
        {
            document.getElementById(
                "voltage"
            ).textContent =
                data.voltage;
        }

        if(data.satellites !== undefined)
        {
            document.getElementById(
                "satellites"
            ).textContent =
                data.satellites;
        }

        if(data.radio !== undefined)
        {
            document.getElementById(
                "radio"
            ).textContent =
                data.radio
                ? "Connected"
                : "Disconnected";
        }

        if(data.distance !== undefined)
        {
            document.getElementById(
                "distance"
            ).textContent =
                data.distance;
        }

        if(data.ip !== undefined)
        {
            document.getElementById(
                "roverIP"
            ).textContent =
                data.ip;
        }

    }
    catch(error)
    {
        console.log(
            "Rover status error:",
            error
        );

        setOffline();
    }
}


/* =====================================================
   CAMERA STATUS
   ===================================================== */

async function updateCameraStatus()
{
    try
    {
        const response =
            await fetch(
                CAMERA +
                "/api/status",
                {
                    cache:"no-store"
                }
            );

        if(!response.ok)
            throw new Error(
                "Camera status failed"
            );

        const data =
            await response.json();

        document.getElementById(
            "cameraConnection"
        ).textContent =
            "Connected";

        cameraStatus.textContent =
            "Connected";

        if(data.pan !== undefined)
        {
            document.getElementById(
                "pan"
            ).textContent =
                data.pan;
        }

        if(data.tilt !== undefined)
        {
            document.getElementById(
                "tilt"
            ).textContent =
                data.tilt;
        }

        if(data.temperature !== undefined)
        {
            document.getElementById(
                "temperature"
            ).textContent =
                data.temperature;
        }

        if(data.humidity !== undefined)
        {
            document.getElementById(
                "humidity"
            ).textContent =
                data.humidity;
        }

        if(data.rain !== undefined)
        {
            document.getElementById(
                "rain"
            ).textContent =
                data.rain
                ? "Detected"
                : "No Rain";
        }

    }
    catch(error)
    {
        console.log(
            "Camera status error:",
            error
        );

        document.getElementById(
            "cameraConnection"
        ).textContent =
            "Disconnected";

        cameraStatus.textContent =
            "Disconnected";
    }
}


/* =====================================================
   KEYBOARD CONTROL
   ===================================================== */

document.addEventListener(
    "keydown",
    function(event)
    {
        if(event.repeat)
            return;

        switch(event.key)
        {
            case "ArrowUp":
            case "w":
            case "W":
                sendCommand("forward");
                break;

            case "ArrowDown":
            case "s":
            case "S":
                sendCommand("backward");
                break;

            case "ArrowLeft":
            case "a":
            case "A":
                sendCommand("left");
                break;

            case "ArrowRight":
            case "d":
            case "D":
                sendCommand("right");
                break;

            case " ":
                sendCommand("stop");
                break;
        }
    }
);


document.addEventListener(
    "keyup",
    function(event)
    {
        if(
            event.key === "ArrowUp" ||
            event.key === "ArrowDown" ||
            event.key === "ArrowLeft" ||
            event.key === "ArrowRight" ||
            ["w","a","s","d"].includes(
                event.key.toLowerCase()
            )
        )
        {
            sendCommand("stop");
        }
    }
);


/* =====================================================
   INITIALIZATION
   ===================================================== */

function initialize()
{
    document.getElementById(
        "roverIP"
    ).textContent =
        ROVER_IP;

    updateRoverStatus();

    updateCameraStatus();

    setInterval(
        updateRoverStatus,
        1000
    );

    setInterval(
        updateCameraStatus,
        1000
    );
}

initialize();
