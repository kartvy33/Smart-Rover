#include "app_httpd.h"

#include "esp_camera.h"
#include "esp_http_server.h"

#include "camera_index.h"
#include "camera_servo.h"

#include <Arduino.h>
#include <WiFi.h>

// ============================================================
// SERVER HANDLES
// ============================================================

static httpd_handle_t camera_httpd = NULL;
static httpd_handle_t stream_httpd = NULL;


// ============================================================
// CAMERA STREAM SETTINGS
// ============================================================

#define PART_BOUNDARY "123456789000000000000987654321"

static const char* STREAM_CONTENT_TYPE =
    "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;

static const char* STREAM_BOUNDARY =
    "\r\n--" PART_BOUNDARY "\r\n";

static const char* STREAM_PART =
    "Content-Type: image/jpeg\r\n"
    "Content-Length: %u\r\n\r\n";


// ============================================================
// WEB PAGE
// ============================================================

static esp_err_t index_handler(httpd_req_t *req)
{
      httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");

    return httpd_resp_send(
        req,
        (const char *)index_ov2640_html_gz,
        index_ov2640_html_gz_len
    );
}


// ============================================================
// SIMPLE JSON RESPONSE
// ============================================================

static esp_err_t sendOK(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");

    httpd_resp_send(
        req,
        "{\"ok\":true}",
        HTTPD_RESP_USE_STRLEN
    );

    return ESP_OK;
}


static esp_err_t sendError(
    httpd_req_t *req,
    const char *message
)
{
    httpd_resp_set_status(
        req,
        "400 Bad Request"
    );

    httpd_resp_set_type(
        req,
        "application/json"
    );

    String json = "{\"ok\":false,\"error\":\"";
    json += message;
    json += "\"}";

    httpd_resp_send(
        req,
        json.c_str(),
        json.length()
    );

    return ESP_OK;
}


// ============================================================
// CAMERA STREAM
// ============================================================

static esp_err_t stream_handler(httpd_req_t *req)
{
    camera_fb_t *fb = NULL;

    esp_err_t res = ESP_OK;

    size_t jpg_buf_len = 0;

    uint8_t *jpg_buf = NULL;


    res = httpd_resp_set_type(
        req,
        STREAM_CONTENT_TYPE
    );

    if (res != ESP_OK)
    {
        return res;
    }


    httpd_resp_set_hdr(
        req,
        "Access-Control-Allow-Origin",
        "*"
    );


    while (true)
    {
        fb = esp_camera_fb_get();

        if (!fb)
        {
            Serial.println(
                "Camera capture failed"
            );

            res = ESP_FAIL;

            break;
        }


        if (fb->format != PIXFORMAT_JPEG)
        {
            bool converted =
                frame2jpg(
                    fb,
                    80,
                    &jpg_buf,
                    &jpg_buf_len
                );

            esp_camera_fb_return(fb);

            fb = NULL;

            if (!converted)
            {
                Serial.println(
                    "JPEG conversion failed"
                );

                res = ESP_FAIL;

                break;
            }
        }
        else
        {
            jpg_buf =
                fb->buf;

            jpg_buf_len =
                fb->len;
        }


        char part_buf[64];

        size_t hlen =
            snprintf(
                part_buf,
                sizeof(part_buf),
                STREAM_PART,
                jpg_buf_len
            );


        res = httpd_resp_send_chunk(
            req,
            part_buf,
            hlen
        );


        if (res == ESP_OK)
        {
            res =
                httpd_resp_send_chunk(
                    req,
                    (const char *)jpg_buf,
                    jpg_buf_len
                );
        }


        if (res == ESP_OK)
        {
            res =
                httpd_resp_send_chunk(
                    req,
                    STREAM_BOUNDARY,
                    strlen(STREAM_BOUNDARY)
                );
        }


        if (fb)
        {
            esp_camera_fb_return(fb);

            fb = NULL;
        }
        else if (jpg_buf)
        {
            free(jpg_buf);

            jpg_buf = NULL;
        }


        if (res != ESP_OK)
        {
            break;
        }
    }


    return res;
}


// ============================================================
// CAMERA SERVO API
// ============================================================

static esp_err_t camera_up_handler(
    httpd_req_t *req
)
{
    cameraServoMove(
        0,
        -5
    );

    return sendOK(req);
}


static esp_err_t camera_down_handler(
    httpd_req_t *req
)
{
    cameraServoMove(
        0,
        5
    );

    return sendOK(req);
}


static esp_err_t camera_left_handler(
    httpd_req_t *req
)
{
    cameraServoMove(
        -5,
        0
    );

    return sendOK(req);
}


static esp_err_t camera_right_handler(
    httpd_req_t *req
)
{
    cameraServoMove(
        5,
        0
    );

    return sendOK(req);
}


static esp_err_t camera_center_handler(
    httpd_req_t *req
)
{
    cameraServoCenter();

    return sendOK(req);
}


// ============================================================
// CAMERA POSITION STATUS
// ============================================================

static esp_err_t camera_status_handler(
    httpd_req_t *req
)
{
    int pan =
        getCameraPan();

    int tilt =
        getCameraTilt();


    String json =
        "{\"ok\":true,\"pan\":";

    json += pan;

    json += ",\"tilt\":";

    json += tilt;

    json += "}";


    httpd_resp_set_type(
        req,
        "application/json"
    );

    httpd_resp_send(
        req,
        json.c_str(),
        json.length()
    );

    return ESP_OK;
}


// ============================================================
// GENERAL STATUS
// ============================================================

static esp_err_t status_handler(
    httpd_req_t *req
)
{
    int pan =
        getCameraPan();

    int tilt =
        getCameraTilt();


    String json =
        "{";

    json +=
        "\"connected\":true,";

    json +=
        "\"camera\":true,";

    json +=
        "\"pan\":";

    json += pan;

    json +=
        ",\"tilt\":";

    json += tilt;

    json +=
        "}";


    httpd_resp_set_type(
        req,
        "application/json"
    );

    httpd_resp_send(
        req,
        json.c_str(),
        json.length()
    );

    return ESP_OK;
}


// ============================================================
// CORS
// ============================================================

static void addCORS(
    httpd_req_t *req
)
{
    httpd_resp_set_hdr(
        req,
        "Access-Control-Allow-Origin",
        "*"
    );

    httpd_resp_set_hdr(
        req,
        "Access-Control-Allow-Methods",
        "GET,POST,OPTIONS"
    );

    httpd_resp_set_hdr(
        req,
        "Access-Control-Allow-Headers",
        "Content-Type"
    );
}


// ============================================================
// OPTIONS HANDLER
// ============================================================

static esp_err_t options_handler(
    httpd_req_t *req
)
{
    addCORS(req);

    httpd_resp_set_status(
        req,
        "204 No Content"
    );

    httpd_resp_send(
        req,
        NULL,
        0
    );

    return ESP_OK;
}


// ============================================================
// REGISTER CAMERA SERVER
// ============================================================

void startCameraServer()
{
    httpd_config_t config =
        HTTPD_DEFAULT_CONFIG();

    config.server_port = 80;

    config.ctrl_port = 32768;

    config.max_uri_handlers = 24;


    // --------------------------------------------------------
    // CAMERA HTTP SERVER
    // --------------------------------------------------------

    if (
        httpd_start(
            &camera_httpd,
            &config
        ) == ESP_OK
    )
    {

        // ----------------------------------------------------
        // MAIN CAMERA PAGE
        // ----------------------------------------------------

        httpd_uri_t index_uri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = index_handler,
            .user_ctx  = NULL
        };


        httpd_register_uri_handler(
            camera_httpd,
            &index_uri
        );


        // ----------------------------------------------------
        // CAMERA UP
        // ----------------------------------------------------

        httpd_uri_t camera_up_uri = {
            .uri       = "/api/camera/up",
            .method    = HTTP_POST,
            .handler   = camera_up_handler,
            .user_ctx  = NULL
        };


        httpd_register_uri_handler(
            camera_httpd,
            &camera_up_uri
        );


        // ----------------------------------------------------
        // CAMERA DOWN
        // ----------------------------------------------------

        httpd_uri_t camera_down_uri = {
            .uri       = "/api/camera/down",
            .method    = HTTP_POST,
            .handler   = camera_down_handler,
            .user_ctx  = NULL
        };


        httpd_register_uri_handler(
            camera_httpd,
            &camera_down_uri
        );


        // ----------------------------------------------------
        // CAMERA LEFT
        // ----------------------------------------------------

        httpd_uri_t camera_left_uri = {
            .uri       = "/api/camera/left",
            .method    = HTTP_POST,
            .handler   = camera_left_handler,
            .user_ctx  = NULL
        };


        httpd_register_uri_handler(
            camera_httpd,
            &camera_left_uri
        );


        // ----------------------------------------------------
        // CAMERA RIGHT
        // ----------------------------------------------------

        httpd_uri_t camera_right_uri = {
            .uri       = "/api/camera/right",
            .method    = HTTP_POST,
            .handler   = camera_right_handler,
            .user_ctx  = NULL
        };


        httpd_register_uri_handler(
            camera_httpd,
            &camera_right_uri
        );


        // ----------------------------------------------------
        // CAMERA CENTER
        // ----------------------------------------------------

        httpd_uri_t camera_center_uri = {
            .uri       = "/api/camera/center",
            .method    = HTTP_POST,
            .handler   = camera_center_handler,
            .user_ctx  = NULL
        };


        httpd_register_uri_handler(
            camera_httpd,
            &camera_center_uri
        );


        // ----------------------------------------------------
        // CAMERA POSITION
        // ----------------------------------------------------

        httpd_uri_t camera_status_uri = {
            .uri       = "/api/camera/status",
            .method    = HTTP_GET,
            .handler   = camera_status_handler,
            .user_ctx  = NULL
        };


        httpd_register_uri_handler(
            camera_httpd,
            &camera_status_uri
        );


        // ----------------------------------------------------
        // GENERAL STATUS
        // ----------------------------------------------------

        httpd_uri_t status_uri = {
            .uri       = "/api/status",
            .method    = HTTP_GET,
            .handler   = status_handler,
            .user_ctx  = NULL
        };


        httpd_register_uri_handler(
            camera_httpd,
            &status_uri
        );


        // ----------------------------------------------------
        // OPTIONS
        // ----------------------------------------------------

        httpd_uri_t options_uri = {
            .uri       = "/api/*",
            .method    = HTTP_OPTIONS,
            .handler   = options_handler,
            .user_ctx  = NULL
        };


        // Some ESP32 HTTP-server versions don't support
        // wildcard URI matching. Registration is therefore
        // intentionally omitted here.


        Serial.println(
            "Camera HTTP server started"
        );
    }


    // ========================================================
    // STREAM SERVER
    // ========================================================

    config.server_port += 1;

    config.ctrl_port += 1;


    if (
        httpd_start(
            &stream_httpd,
            &config
        ) == ESP_OK
    )
    {

        httpd_uri_t stream_uri = {
            .uri       = "/stream",
            .method    = HTTP_GET,
            .handler   = stream_handler,
            .user_ctx  = NULL
        };


        httpd_register_uri_handler(
            stream_httpd,
            &stream_uri
        );


        Serial.println(
            "Camera stream started"
        );

        Serial.println(
            "Stream URL: /stream"
        );
    }
}
