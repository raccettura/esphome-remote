# Sharp TV Wi-Fi Remote (ESPHome)

An ESP32-C3 + IR LED that acts as a remote for a **Sharp Aquos LC-70EQ10U**. It runs its
own Wi-Fi network, so you join it from a phone or computer, open a web page, and get
every button on the original remote (GB118WJSA / GB005WJSA), plus discrete power and
input codes that the physical remote doesn't have.

No home network, internet connection, Home Assistant or app is needed.

## Files

| File | What it is |
|---|---|
| `sharp-tv-remote.yaml` | The ESPHome configuration (flash this) |
| `sharp_tv_codes.h` | IR code tables and the Sharp protocol encoder |
| `common/tv_key.yaml` | Template for one remote button (used by the main YAML) |
| `web/remote.js`, `web/remote.css` | The remote-control web page, built into the firmware |
| `secrets.yaml.example` | Wi-Fi and OTA passwords, copy it to `secrets.yaml` |

## 1. Parts

- **Wemos/Lolin C3 Mini** (ESP32-C3). The config uses `board: lolin_c3_mini`. Other
  ESP32 boards work too. Change `esp32: board:` and `ir_pin`.
- **940 nm IR LED**, ideally a 5 mm high-power type (e.g. TSAL6200, or the LED out of a
  "KY-005" IR transmitter module).
- **NPN transistor** (2N2222, PN2222 or BC337), **1 kΩ resistor**, **33–47 Ω resistor**.
- USB cable and a 5 V USB power supply.

> A bare IR LED wired straight to a GPIO pin (or a KY-005 module) also works, but only
> within a metre or two of the TV. The transistor driver below reaches across a room.

## 2. Wiring

```
C3 Mini GPIO10 ──[1 kΩ]── B  (NPN, e.g. 2N2222)
                           C ──── IR LED cathode (−, short leg, flat side)
C3 Mini GND ────────────── E
C3 Mini VBUS/5V ─[39 Ω]─── IR LED anode (+, long leg)
```

Point the LED at the TV's IR window (bottom centre of the LC-70EQ10U's front bezel). To
use a different pin, change `ir_pin` at the top of `sharp-tv-remote.yaml`.

**Check the LED works:** most phone cameras can see IR. Point the camera at the LED and
press a button on the web page, and it should flicker purple/white. (Some iPhone rear
cameras filter IR; try the front camera.)

## 3. Install ESPHome

You need Python 3.11 or newer.

```sh
python3 -m venv venv
source venv/bin/activate
pip install esphome
esphome version     # tested with 2026.10.0
```

(Or use the ESPHome Device Builder add-on in Home Assistant: copy all the files above
into its config folder, keeping the `common/` and `web/` subfolders.)

## 4. Set the passwords

```sh
cp secrets.yaml.example secrets.yaml
```

Edit `secrets.yaml`:

- `ap_password`: the password for the remote's Wi-Fi network, **8 or more characters**.
- `ota_password`: protects wireless firmware updates.

## 5. Flash it

Plug the ESP32 into your computer over USB, then:

```sh
esphome run sharp-tv-remote.yaml
```

Pick the USB serial port when asked. The first build downloads the ESP32 toolchain and
takes a few minutes.

The C3 Mini uses the chip's built-in USB, so only one program can have the port open
at a time. Close any other `esphome run`/`esphome logs` session first. If upload still
fails, hold the **9** (BOOT) button, tap **RST**, release **9**, and try again.

Later updates can be done wirelessly: join the remote's Wi-Fi from your computer and run
`esphome run sharp-tv-remote.yaml --device 192.168.4.1`.

## 6. Use it

1. Power the ESP32 from any USB supply near the TV.
2. On your phone or computer, join the Wi-Fi network **"Sharp TV Remote"** with your
   `ap_password`.
3. Your device will warn that the network has **no internet**. Choose *Stay connected*
   / *Use without internet* (Android), or simply stay joined (iOS / macOS).
   Turn off mobile data if your phone keeps switching away.
4. Open **http://192.168.4.1** in a browser. Add it to your home screen for an app-like
   remote.

### The page

- **Code set** dropdown at the top (see below). It is saved on the device.
- The remote layout follows the GB118WJSA: Input / TV / Power, number pad with `•` and
  `ENT`, VOL and CH rockers with Mute and Flashback, and so on.
- **AV MODE** cycles the *picture* presets (Standard, Movie, Game, …).
  **VIEW MODE** cycles the *aspect ratio / screen size* (Side Bar, S.Stretch, Zoom,
  Stretch, Dot by Dot, …). On Sharp TVs the aspect ratio button is VIEW MODE.
- Freeze, Audio, Display, Sleep, CC, 3D, Fav Ch, Menu, Smart Central, Netflix, Option,
  Exit, Return, the D-pad with Enter, A/B/C/D colour keys, playback keys, App 1–3
  and Manual.
- Volume, channel, arrows, rewind and fast-forward **repeat while held**.
- **Power On / Power Off** are discrete codes, handy when you can't see whether the TV
  is on. **Power** is the normal toggle.
- **Direct inputs** (collapsed section): jump straight to HDMI 1–4, Component,
  Video 1/2 or PC.
- **Code tester** (collapsed section): send any Sharp address/command to find codes
  that aren't in the tables. "Send next" steps through the commands one at a time.
- The line under the dropdown shows the last code sent, e.g. `View Mode (Sharp 1/160)`.

## Code sets

| Option | Protocol | Use it for |
|---|---|---|
| **Sharp Aquos 2011-2016 (LC-70EQ10U)**, the default | Sharp 15-bit, addresses 1 and 17 | **This TV.** Every button. |
| Sharp 2017+ Hisense-built (NEC) | NEC, address 0x04 (LG style) | Later Sharp-branded LC-xxN6000U/N7000U sets. Fallback only. |
| Sharp alternate NEC (basic keys only) | NEC, address 0x7F00 | An "Aquos" set from the Flipper IR database of unknown model. Power, volume, channel, input, menu, arrows, OK, exit and display only. |

The default set is the Logitech Harmony database entry for the LC-70EQ10U itself.
Harmony uses the same codes for every Sharp-built 2011-2016 Aquos (LC-xxLE6xx,
LC-xxEQ10U, …), so it should be right for your TV. The other two sets are fallbacks for
other Sharp TVs and are not expected to work on the EQ10U. A button with no code in the
selected set does nothing, and the status line says "not available in this code set".

## Troubleshooting

- **Nothing happens on the TV:** check the LED with a phone camera (above). If it lights,
  move it closer and aim it straight at the TV's IR window. If it doesn't, check the
  transistor's orientation and the LED polarity.
- **The page won't load:** make sure you're still on the "Sharp TV Remote" network (phones
  sometimes drop no-internet networks) and that you typed `http://`, not `https://`.
  If the USB log shows `httpd_accept_conn: error in accept (23)`, the device has run
  out of network sockets. The config raises the limit to 16 (`CONFIG_LWIP_MAX_SOCKETS`)
  and the page avoids long-lived connections. Close extra tabs open on the remote.
- **Works but only up close:** use the transistor driver, a lower LED resistor (no lower
  than about 33 Ω at 5 V), or two LEDs in series with about 15 Ω.
- **Logs:** with the ESP32 on USB, run `esphome logs sharp-tv-remote.yaml` to watch every
  code as it's sent.

## Customising

- **Pin, board, network name:** see `substitutions:` at the top of the YAML. For an
  ESP32-S3/C3 board, change `esp32: board:`.
- **Also join your home Wi-Fi:** add `ssid:` and `password:` under `wifi:`. The AP then
  only appears when the home network is unreachable, and you can add `api:` to use the
  buttons from Home Assistant.
- **Add or change a code:** edit the tables in `sharp_tv_codes.h`. Sharp codes are
  `{Key, address, command}`. New buttons need a `Key` entry, a line in the `button:`
  list of the YAML, and an entry in `web/remote.js`.

## Sources

- IR codes: [Logitech Harmony IR archive](https://github.com/pickysysadmin/logitech-harmony-ir-archive):
  `devices/Sharp/LC-70EQ10U.json` (codeset `e8dbd6b84bfcbafb`), and `LC-65N7000U` for the
  NEC fallback. The encoder's waveform was checked against all 70 Harmony Pronto codes.
- Alternate NEC set: [Flipper-IRDB](https://github.com/Lucaslhm/Flipper-IRDB)
  `TVs/Sharp/Aquos.ir`.
