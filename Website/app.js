// ============================================================
// SMART ROVER WEBSITE - app.js
// ============================================================

const state = {
    rover: localStorage.getItem("roverUrl") || "",
    cam: localStorage.getItem("camUrl") || ""
};

const $ = (id) => document.getElementById(id);


// ============================================================
// INITIAL SETTINGS
// ============================================================

$("roverUrl").value = state.rover;
$("camUrl").value = state.cam;


// ============================================================
// API HELPER
// ============================================================

function api(path, options = {}) {

    if (!state.rover) {
        return Promise.reject(
            new Error("Rover URL is not configured")
        );
    }

    const base = state.rover.replace(/\/$/, "");

    return fetch(base + path, {
        ...options,

        headers: {
            "Content-Type": "application/json",
            ...(options.headers || {})
        }
    });
}


// ============================================================
// ROVER COMMAND
// ============================================================

function command(cmd) {

    const speed = Number($("speed").value);

    return api("/api/command", {
        method: "POST",

        body: JSON.stringify({
            command: cmd,
            speed: speed
        })
    }).catch(() => {
        console.log("Command failed:", cmd);
    });
}


// ============================================================
// DRIVE BUTTONS
// ============================================================

document
    .querySelectorAll("[data-drive]")
    .forEach(button => {

        const cmd = button.dataset.drive;

        button.addEventListener("click", () => {
            command(cmd);
        });

        // Mobile touch support
        button.addEventListener(
            "touchstart",
            event => {

                event.preventDefault();

                command(cmd);

            },
            {
                passive: false
            }
        );
    });


// ============================================================
// SPEED CONTROL
// ============================================================

$("speed").addEventListener(
    "input",
    () => {

        $("speedValue").textContent =
            $("speed").value + "%";

    }
);


// ============================================================
// EMERGENCY STOP
// ============================================================

$("emergency").addEventListener(
    "click",
    () => {

        command("stop");

    }
);


// ============================================================
// CAMERA SERVO CONTROL
// ============================================================

document
    .querySelectorAll("[data-cam]")
    .forEach(button => {

        button.addEventListener(
            "click",
            () => {

                const direction =
                    button.dataset.cam;

                const paths = {

                    up: "/api/camera/up",

                    down: "/api/camera/down",

                    left: "/api/camera/left",

                    right: "/api/camera/right",

                    center: "/api/camera/center"

                };

                const path = paths[direction];

                if (!path) {
                    return;
                }

                api(path, {
                    method: "POST"
                }).catch(() => {

                    console.log(
                        "Camera command failed:",
                        direction
                    );

                });

            }
        );

    });


// ============================================================
// SPEAKER
// ============================================================

$("speakBtn").addEventListener(
    "click",
    () => {

        const text =
            $("speakerText").value.trim();

        if (!text) {
            return;
        }

        api("/api/speak", {

            method: "POST",

            body: JSON.stringify({
                text: text
            })

        }).catch(() => {

            console.log("Speaker command failed");

        });

    }
);


// ============================================================
// SAVE CONNECTION SETTINGS
// ============================================================

$("saveSettings").addEventListener(
    "click",
    () => {

        state.rover =
            $("roverUrl").value.trim();

        state.cam =
            $("camUrl").value.trim();

        localStorage.setItem(
            "roverUrl",
            state.rover
        );

        localStorage.setItem(
            "camUrl",
            state.cam
        );

        setCamera();

        updateConnection();

    }
);


// ============================================================
// CAMERA STREAM
// ============================================================

function setCamera() {

    const image =
        $("cameraFeed");

    const placeholder =
        $("cameraPlaceholder");


    if (!state.cam) {

        image.style.display = "none";

        placeholder.style.display =
            "block";

        return;
    }


    image.src =
        state.cam.replace(/\/$/, "") +
        "/stream";


    image.onload = () => {

        image.style.display =
            "block";

        placeholder.style.display =
            "none";

    };


    image.onerror = () => {

        image.style.display =
            "none";

        placeholder.style.display =
            "block";

    };

}


// ============================================================
// FULLSCREEN CAMERA
// ============================================================

$("fullscreenBtn").addEventListener(
    "click",
    () => {

        const camera =
            $("cameraFeed");

        if (camera.requestFullscreen) {

            camera.requestFullscreen();

        }

    }
);


// ============================================================
// CONNECTION + TELEMETRY
// ============================================================

async function updateConnection() {

    if (!state.rover) {

        $("connectionDot").style.background =
            "var(--danger)";

        $("connectionText").textContent =
            "Offline";

        return;

    }


    try {

        const response =
            await api("/api/status");


        if (!response.ok) {
            throw new Error(
                "Status request failed"
            );
        }


        const data =
            await response.json();


        // Connected
        $("connectionDot").style.background =
            "var(--ok)";

        $("connectionText").textContent =
            "Connected";


        // Battery
        $("battery").textContent =
            data.battery != null
                ? data.battery + "%"
                : "--%";


        // Temperature
        $("temperature").textContent =
            data.temperature != null
                ? data.temperature + " °C"
                : "-- °C";


        // Humidity
        $("humidity").textContent =
            data.humidity != null
                ? data.humidity + "%"
                : "--%";


        // Rain
        $("rain").textContent =
            data.rain != null
                ? (
                    data.rain
                        ? "Detected"
                        : "Clear"
                )
                : "--";


        // Ultrasonic distance
        $("distance").textContent =
            data.distance != null
                ? data.distance + " cm"
                : "-- cm";


        // GPS
        $("gps").textContent =
            data.gps || "--";


        // Radar
        if (
            data.radarAngle != null &&
            data.radarDistance != null
        ) {

            drawRadar(
                data.radarAngle,
                data.radarDistance
            );


            $("radarText").textContent =
                data.radarAngle +
                "° • " +
                data.radarDistance +
                " cm";

        }

    }
    catch (error) {

        $("connectionDot").style.background =
            "var(--danger)";

        $("connectionText").textContent =
            "Offline";

    }

}


// ============================================================
// RADAR DRAWING
// ============================================================

function drawRadar(angle, distance) {

    const canvas =
        $("radarCanvas");

    const ctx =
        canvas.getContext("2d");


    const width =
        canvas.width;

    const height =
        canvas.height;


    const centerX =
        width / 2;

    const centerY =
        height - 10;


    ctx.clearRect(
        0,
        0,
        width,
        height
    );


    // Radar rings
    ctx.strokeStyle =
        "#245d45";


    const rings = [
        55,
        110,
        165,
        210
    ];


    rings.forEach(radius => {

        ctx.beginPath();

        ctx.arc(
            centerX,
            centerY,
            radius,
            Math.PI,
            Math.PI * 2
        );

        ctx.stroke();

    });


    // Convert angle
    const radians =
        Math.PI -
        (
            angle *
            Math.PI /
            180
        );


    // Limit distance
    const radarDistance =
        Math.min(
            distance,
            210
        );


    const pointX =
        centerX +
        radarDistance *
        Math.cos(radians);


    const pointY =
        centerY +
        radarDistance *
        Math.sin(radians);


    // Radar scanning line
    ctx.strokeStyle =
        "#55ff9a";

    ctx.lineWidth = 2;

    ctx.beginPath();

    ctx.moveTo(
        centerX,
        centerY
    );

    ctx.lineTo(
        pointX,
        pointY
    );

    ctx.stroke();


    // Object
    ctx.fillStyle =
        "#ff5b6b";

    ctx.beginPath();

    ctx.arc(
        pointX,
        pointY,
        6,
        0,
        Math.PI * 2
    );

    ctx.fill();

}


// ============================================================
// INITIALIZE
// ============================================================

setCamera();

updateConnection();


// ============================================================
// TELEMETRY UPDATE
// ============================================================

setInterval(
    updateConnection,
    1000
);


// ============================================================
// STOP ROVER WHEN PAGE CLOSES
// ============================================================

window.addEventListener(
    "beforeunload",
    () => {

        if (!state.rover) {
            return;
        }


        const url =
            state.rover.replace(/\/$/, "") +
            "/api/command";


        const data =
            JSON.stringify({
                command: "stop"
            });


        navigator.sendBeacon(

            url,

            new Blob(
                [data],
                {
                    type:
                        "application/json"
                }
            )

        );

    }
);
