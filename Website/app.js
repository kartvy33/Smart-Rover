/* =========================================
   SMART ROVER
   Frontend Controller
========================================= */


/* =========================================
   Configuration
========================================= */

const ROVER_IP = window.location.hostname || "192.168.4.1";

const CAMERA_IP = "192.168.4.3";

const API_BASE = `http://${ROVER_IP}`;


/* =========================================
   Helper
========================================= */

function api(path, options = {})
{
    return fetch(API_BASE + path, options);
}


/* =========================================
   Connection
========================================= */

function setConnection(online)
{
    const dot =
        document.getElementById("connectionDot");

    const text =
        document.getElementById("connectionText");


    if (online)
    {
        dot.className = "dot online";

        text.textContent = "Rover Online";
    }
    else
    {
        dot.className = "dot offline";

        text.textContent = "Rover Offline";
    }
}


/* =========================================
   Rover Movement
========================================= */

function move(direction)
{
    api("/" + direction)
        .then(() =>
        {
            setConnection(true);
        })
        .catch(() =>
        {
            setConnection(false);
        });
}


function stopRover()
{
    api("/stop")
        .then(() =>
        {
            setConnection(true);
        })
        .catch(() =>
        {
            setConnection(false);
        });
}


/* =========================================
   Camera
========================================= */

function openCamera()
{
    window.open(
        `http://${CAMERA_IP}/`,
        "_blank"
    );
}


function loadCamera()
{
    const stream =
        document.getElementById("cameraStream");

    const placeholder =
        document.getElementById("cameraPlaceholder");

    /*
       Current ESP32-CAM web server
       */

    stream.src =
        `http://${CAMERA_IP}/`;

    /*
       The ESP32-CAM webpage itself is not
       necessarily an MJPEG image stream.

       Therefore we don't display it as an
       <img> until a proper stream endpoint
       is available.
       */

    stream.style.display = "none";

    placeholder.style.display = "block";
}


/* =========================================
   Camera Movement
========================================= */

function cameraMove(direction)
{
    /*
       These endpoints will be provided by
       the ESP32-CAM HTTP server.
    */

    fetch(
        `http://${CAMERA_IP}/camera_${direction}`,
        {
            method: "GET"
        }
    )
    .then(() =>
    {
        console.log(
            "Camera:",
            direction
        );
    })
    .catch(error =>
    {
        console.log(
            "Camera command failed:",
            error
        );
    });
}


function cameraCenter()
{
    fetch(
        `http://${CAMERA_IP}/camera_center`,
        {
            method: "GET"
        }
    )
    .catch(error =>
    {
        console.log(error);
    });
}


/* =========================================
   Speaker
========================================= */

function speakText()
{
    const input =
        document.getElementById("speakerText");

    const text =
        input.value.trim();


    if (!text)
    {
        return;
    }


    fetch(
        API_BASE +
        "/speak?text=" +
        encodeURIComponent(text)
    )
    .then(() =>
    {
        input.value = "";
    })
    .catch(error =>
    {
        console.log(
            "Speaker error:",
            error
        );
    });
}


/* =========================================
   Status
========================================= */

function updateElement(
    id,
    value,
    fallback = "--"
)
{
    const element =
        document.getElementById(id);


    if (!element)
    {
        return;
    }


    if (
        value === undefined ||
        value === null ||
        value === ""
    )
    {
        element.textContent = fallback;
    }
    else
    {
        element.textContent = value;
    }
}


/* =========================================
   Status API
========================================= */

function updateStatus()
{
    api("/status")

        .then(response =>
        {
            if (!response.ok)
            {
                throw new Error(
                    "Status request failed"
                );
            }

            return response.json();
        })

        .then(data =>
        {
            setConnection(true);


            /*
               Battery
            */

            updateElement(
                "battery",
                data.battery
            );


            updateElement(
                "voltage",
                data.voltage
            );


            /*
               GPS
            */

            updateElement(
                "satellites",
                data.satellites
            );


            updateElement(
                "latitude",
                data.latitude
            );


            updateElement(
                "longitude",
                data.longitude
            );


            updateElement(
                "altitude",
                data.altitude
            );


            /*
               Environment
            */

            updateElement(
                "temperature",
                data.temperature
            );


            updateElement(
                "humidity",
                data.humidity
            );


            updateElement(
                "rain",
                data.rain
            );


            /*
               Radio
            */

            const radio =
                data.radio
                ? "Connected"
                : "Disconnected";


            updateElement(
                "radio",
                radio
            );


            /*
               System
            */

            updateElement(
                "systemState",
                data.state
            );


            /*
               Safety
            */

            updateElement(
                "obstacle",
                data.obstacle
            );


            updateElement(
                "cliff",
                data.cliff
            );


            updateElement(
                "batterySafety",
                data.batterySafety
            );


            /*
               Camera
            */

            updateElement(
                "cameraConnection",
                data.camera
                ? "Connected"
                : "Disconnected"
            );


            /*
               System information
            */

            updateElement(
                "roverIP",
                ROVER_IP
            );


            updateElement(
                "cameraIP",
                CAMERA_IP
            );


            updateElement(
                "firmware",
                data.firmware
            );


            updateElement(
                "wifiStatus",
                data.wifi
                ? "Online"
                : "Offline"
            );
        })

        .catch(error =>
        {
            console.log(
                "Status error:",
                error
            );

            setConnection(false);
        });
}


/* =========================================
   Keyboard Control
========================================= */

document.addEventListener(
    "keydown",
    function(event)
    {
        /*
           Ignore keyboard when typing
           in the speaker box.
        */

        if (
            event.target.tagName ===
            "INPUT"
        )
        {
            return;
        }


        switch(event.key)
        {
            case "ArrowUp":
            case "w":
            case "W":

                move("forward");

                break;


            case "ArrowDown":
            case "s":
            case "S":

                move("backward");

                break;


            case "ArrowLeft":
            case "a":
            case "A":

                move("left");

                break;


            case "ArrowRight":
            case "d":
            case "D":

                move("right");

                break;


            case " ":

                stopRover();

                break;
        }
    }
);


/* =========================================
   Page Startup
========================================= */

document.addEventListener(
    "DOMContentLoaded",
    function()
    {
        /*
           Show rover IP
        */

        updateElement(
            "roverIP",
            ROVER_IP
        );


        /*
           Camera IP
        */

        updateElement(
            "cameraIP",
            CAMERA_IP
        );


        /*
           Load camera information
        */

        loadCamera();


        /*
           Get status immediately
        */

        updateStatus();


        /*
           Continue updating status
           every second.
        */

        setInterval(
            updateStatus,
            1000
        );
    }
);
