# Smart Rover Website API

## Command
POST `/api/command`
```json
{"command":"forward","speed":70}
```
Commands: `forward`, `backward`, `left`, `right`, `stop`.

## Status
GET `/api/status`
```json
{"battery":82,"temperature":29.4,"humidity":61,"rain":false,"distance":48,"gps":"19.1234,72.5678","radarAngle":90,"radarDistance":120}
```

## Camera
POST:
`/api/camera/up`
`/api/camera/down`
`/api/camera/left`
`/api/camera/right`
`/api/camera/center`

## Speaker
POST `/api/speak`
```json
{"text":"Hello from Smart Rover"}
```

## Stream
GET `/stream`
