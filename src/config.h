#pragma once
// Capsule Radar — Marine (AIS) — build & user configuration.
// Sibling of the aircraft "Capsule Radar". The pin map, panel gaps, touch driver and
// which peripherals exist all live in a per-board header (see the Board section below).

#define FW_VERSION "0.1.1"   // shown on the web config page + Stats screen; bump on release

// ---------- Home location (default: Dénia, Spain — coastal, good AIS coverage) ----------
// Overridable at runtime via the captive portal (stored in NVS).
#define HOME_LAT_DEFAULT   38.8409
#define HOME_LON_DEFAULT    0.1059

// ---------- Radar (range in NAUTICAL MILES — marine convention) ----------
#define RANGE_NM_DEFAULT    10.0f          // display range (outer ring)
static const float RANGE_STEPS_NM[] = {2.0f, 5.0f, 10.0f, 20.0f, 40.0f};
// The AIS subscription bounding box is built from home ± a delta wider than the display
// range, so vessels just outside the scope show as edge markers. 1 deg lat ~= 60 nm;
// 1 deg lon ~= 60*cos(lat) nm. AIS_BOX_NM is the half-extent of the box (per side).
#define AIS_BOX_MARGIN      1.5f           // box half-extent = display range * this factor
#define MOTION_INTERP       1              // 1 = glyphs glide between updates; 0 = snap
// Ships report far less often than planes (moving: 2-10 s, anchored: ~3 min) and move
// slowly, so expire gently — don't drop a vessel that simply went quiet at anchor.
#define SHIP_STALE_MS       (12 * 60 * 1000)   // drop vessels not seen in ~12 min
#define AIS_MAX_SHIPS       100                // hard cap parsed/kept (protect RAM in busy harbours)

// Colour mode: 0 = by navigation status (under way / anchored / moored / fishing / sailing),
// 1 = by ship type category (cargo / tanker / passenger / fishing / sailing / service / other).
// Default is navigation status (the richer marine signal); switchable in settings.
#define COLOR_MODE_DEFAULT  0

// ---------- Screen (CO5300 AMOLED) ----------
#define SCREEN_W            466
#define SCREEN_H            466
#define SCREEN_CX           233
#define SCREEN_CY           233
#define RADAR_R_OUTER_PX    218            // outer ring radius in pixels
#define LV_COLOR_DEPTH_BITS 16
// LCD_COL_OFFSET / LCD_ROW_OFFSET / LCD_QSPI_HZ are panel-specific -> board header.
#define BRIGHTNESS_DEFAULT  200            // 0..255, panel brightness via cmd 0x51
#define TZ_STR              "CET-1CEST,M3.5.0,M10.5.0/3"  // POSIX TZ (Spain) for local time/date
#define BRIGHTNESS_IDLE     25             // dimmed after no touch for IDLE_DIM_MS
#define IDLE_DIM_MS         20000          // dim the screen after this long without a touch

// ---------- AIS feed (aisstream.io — free, non-commercial WebSocket) ----------
// Open ONE persistent secure WebSocket, send the subscription JSON once, then receive
// PositionReport + ShipStaticData messages pushed in real time. See docs/DATA_SOURCE.md.
#define AIS_HOST            "stream.aisstream.io"
#define AIS_PORT            443
#define AIS_PATH            "/v0/stream"
#define AIS_USER_AGENT      "CapsuleRadarMarine/0.1 (ESP32-S3 hobby; +https://github.com/socquique/capsule-radar-ais)"
#define AIS_TLS_INSECURE    1               // 1 = setInsecure() (hobby). 0 = pin a root CA.
// The API key is entered via the captive portal and stored in NVS — NEVER committed.
// A compile-time fallback for bench testing only (leave empty for production builds):
#define AIS_API_KEY_FALLBACK ""
#define AIS_RECONNECT_MIN_MS  2000          // backoff floor after a disconnect/error
#define AIS_RECONNECT_MAX_MS  30000         // backoff ceiling
// Self-heal: aisstream can stay "connected" yet stream nothing. If no AIS message
// arrives for this long while connected, drop + reopen the socket (re-subscribes).
#define AIS_STALE_RECONNECT_MS (8 * 60 * 1000)

// ---------- Debug ----------
#define DEBUG_MEM           0               // 1 = print a [mem] heap/fps line every 5s on serial

// ---------- Board ----------
// The pin map, panel gaps, touch driver and which peripherals exist all live in a
// per-board header. Select one with a build flag in platformio.ini; the 1.75 is the
// default so an unflagged build behaves exactly as before.
//   -DBOARD_AMOLED_143  -> Waveshare ESP32-S3-Touch-AMOLED-1.43
//   (none)              -> Waveshare ESP32-S3-Touch-AMOLED-1.75  (reference board)
// Never guess pins for a new board: take them from the vendor demo or the Arduino
// core board variant, then confirm them on hardware before committing.
#if defined(BOARD_AMOLED_143)
#  include "boards/board_amoled_143.h"
#else
#  include "boards/board_amoled_175.h"
#endif

// Safety net: catches a board header that still has placeholder pins in it.
#if (PIN_LCD_SCLK < 0) || (PIN_I2C_SDA < 0)
#  error "board header: QSPI/I2C pins are placeholders (-1). Fill in the real values."
#endif
