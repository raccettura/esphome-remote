#pragma once

// IR code sets for the Sharp Aquos LC-70EQ10U (and related Sharp TVs).
//
// Code set 0 comes from the Logitech Harmony database entry for the LC-70EQ10U
// (https://github.com/pickysysadmin/logitech-harmony-ir-archive,
//  devices/Sharp/LC-70EQ10U.json -> codesets/e8/e8dbd6b84bfcbafb.json).
// The same codes are used by every 2011-2016 Sharp-built Aquos in that database
// (LC-xxLE6xx, LC-xxEQ10U, ...), i.e. the GB118WJSA / GB005WJSA remotes.
//
// The Sharp protocol: 5 address bits + 8 command bits + expansion bit + check bit,
// LSB first, sent twice - the second frame has the command, expansion and check
// bits inverted. Timings below are taken from the Harmony Pronto waveforms.

#include <string>
#include "esphome/core/log.h"
#include "esphome/components/remote_base/remote_base.h"
#include "esphome/components/select/select.h"

namespace sharp_tv {

using esphome::remote_base::RemoteTransmitData;
using esphome::remote_base::RemoteTransmitterBase;

static const char *const TAG = "sharp_tv";

// Every button on the remote (plus discrete codes the remote itself doesn't have).
enum class Key : uint8_t {
  // Top
  POWER, POWER_ON, POWER_OFF, INPUT, TV,
  // Number pad
  NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9, DOT, ENT,
  // Volume / channel
  VOL_UP, VOL_DOWN, MUTE, CH_UP, CH_DOWN, FLASHBACK,
  // Picture / sound / misc
  AV_MODE, VIEW_MODE, FREEZE, AUDIO, DISPLAY, SLEEP, CC, MODE_3D, FAV_CH, MANUAL,
  // Menus and navigation
  MENU, SMART_CENTRAL, NETFLIX, OPTION, EXIT, RETURN, UP, DOWN, LEFT, RIGHT, ENTER,
  // Coloured A/B/C/D
  KEY_A, KEY_B, KEY_C, KEY_D,
  // Playback
  REWIND, PLAY, FAST_FORWARD, SKIP_BACK, PAUSE, SKIP_FORWARD, STOP,
  // Favourite app shortcuts
  FAV_APP_1, FAV_APP_2, FAV_APP_3,
  // Discrete inputs (not on the remote, but the TV accepts them)
  HDMI_1, HDMI_2, HDMI_3, HDMI_4, COMPONENT, VIDEO_1, VIDEO_2, PC,
};

enum class Proto : uint8_t { SHARP, NEC };

struct Code {
  Key key;
  uint16_t address;
  uint16_t command;
};

// --- Code set 0: Sharp Aquos 2011-2016, Sharp protocol (address, command) ---
static const Code AQUOS_SHARP[] = {
    {Key::POWER, 1, 22},          {Key::POWER_ON, 17, 74},      {Key::POWER_OFF, 17, 75},
    {Key::INPUT, 1, 19},          {Key::TV, 17, 138},

    {Key::NUM_0, 1, 10},          {Key::NUM_1, 1, 1},           {Key::NUM_2, 1, 2},
    {Key::NUM_3, 1, 3},           {Key::NUM_4, 1, 4},           {Key::NUM_5, 1, 5},
    {Key::NUM_6, 1, 6},           {Key::NUM_7, 1, 7},           {Key::NUM_8, 1, 8},
    {Key::NUM_9, 1, 9},           {Key::DOT, 17, 58},           {Key::ENT, 1, 11},

    {Key::VOL_UP, 1, 20},         {Key::VOL_DOWN, 1, 21},       {Key::MUTE, 1, 23},
    {Key::CH_UP, 1, 17},          {Key::CH_DOWN, 1, 18},        {Key::FLASHBACK, 1, 47},

    {Key::AV_MODE, 1, 248},       {Key::VIEW_MODE, 1, 160},     {Key::FREEZE, 1, 83},
    {Key::AUDIO, 1, 24},          {Key::DISPLAY, 1, 27},        {Key::SLEEP, 1, 26},
    {Key::CC, 1, 74},             {Key::MODE_3D, 17, 179},      {Key::FAV_CH, 17, 143},
    {Key::MANUAL, 17, 180},

    {Key::MENU, 1, 32},           {Key::SMART_CENTRAL, 17, 107}, {Key::NETFLIX, 17, 29},
    {Key::OPTION, 17, 200},       {Key::EXIT, 1, 243},          {Key::RETURN, 1, 244},
    {Key::UP, 1, 87},             {Key::DOWN, 1, 88},           {Key::LEFT, 1, 245},
    {Key::RIGHT, 1, 246},         {Key::ENTER, 1, 247},

    {Key::KEY_A, 1, 177},         {Key::KEY_B, 1, 178},         {Key::KEY_C, 1, 179},
    {Key::KEY_D, 1, 180},

    {Key::REWIND, 17, 196},       {Key::PLAY, 17, 194},         {Key::FAST_FORWARD, 17, 197},
    {Key::SKIP_BACK, 17, 204},    {Key::PAUSE, 17, 205},        {Key::SKIP_FORWARD, 17, 203},
    {Key::STOP, 17, 195},

    {Key::FAV_APP_1, 17, 97},     {Key::FAV_APP_2, 17, 98},     {Key::FAV_APP_3, 17, 99},

    {Key::HDMI_1, 1, 50},         {Key::HDMI_2, 1, 51},         {Key::HDMI_3, 1, 55},
    {Key::HDMI_4, 1, 211},        {Key::COMPONENT, 17, 76},     {Key::VIDEO_1, 17, 80},
    {Key::VIDEO_2, 17, 149},      {Key::PC, 17, 151},
};

// --- Code set 1: Sharp 2017+ Hisense-built (LC-xxN6000U / N7000U ...), NEC ---
// From Harmony codeset 43e6e75b3670fcc1 (LC-65N7000U). Values are ESPHome NEC
// address/command (LSB-first 16-bit). Keys without an equivalent are absent.
static const Code HISENSE_NEC[] = {
    {Key::POWER, 0xFB04, 0xF708},     {Key::INPUT, 0xFB04, 0xF40B},     {Key::TV, 0xFB04, 0x8B74},

    {Key::NUM_0, 0xFB04, 0xEF10},     {Key::NUM_1, 0xFB04, 0xEE11},     {Key::NUM_2, 0xFB04, 0xED12},
    {Key::NUM_3, 0xFB04, 0xEC13},     {Key::NUM_4, 0xFB04, 0xEB14},     {Key::NUM_5, 0xFB04, 0xEA15},
    {Key::NUM_6, 0xFB04, 0xE916},     {Key::NUM_7, 0xFB04, 0xE817},     {Key::NUM_8, 0xFB04, 0xE718},
    {Key::NUM_9, 0xFB04, 0xE619},     {Key::DOT, 0xFB04, 0xF50A},

    {Key::VOL_UP, 0xFB04, 0xFD02},    {Key::VOL_DOWN, 0xFB04, 0xFC03},  {Key::MUTE, 0xFB04, 0xF609},
    {Key::CH_UP, 0xFB04, 0xFF00},     {Key::CH_DOWN, 0xFB04, 0xFE01},

    {Key::SLEEP, 0xFB04, 0xF10E},     {Key::CC, 0xFB04, 0xC639},

    {Key::MENU, 0xFB04, 0xBC43},      {Key::SMART_CENTRAL, 0xFB04, 0xB54A}, {Key::NETFLIX, 0xFB04, 0xB34C},
    {Key::EXIT, 0xFB04, 0xB24D},      {Key::RETURN, 0xFB04, 0xFB04},
    {Key::UP, 0xFB04, 0xA956},        {Key::DOWN, 0xFB04, 0xA857},      {Key::LEFT, 0xFB04, 0xA758},
    {Key::RIGHT, 0xFB04, 0xA659},     {Key::ENTER, 0xFB04, 0xA55A},

    {Key::REWIND, 0xFB04, 0xB04F},    {Key::PLAY, 0xFB04, 0xBD42},      {Key::FAST_FORWARD, 0xFB04, 0xA45B},
    {Key::PAUSE, 0xFB04, 0xBE41},     {Key::STOP, 0xFB04, 0xB14E},

    {Key::HDMI_1, 0xFB04, 0x837C},    {Key::HDMI_2, 0xFB04, 0x827D},    {Key::HDMI_3, 0xFB04, 0x817E},
    {Key::HDMI_4, 0xFB04, 0x807F},    {Key::COMPONENT, 0xFB04, 0x8679}, {Key::VIDEO_1, 0xFB04, 0x8976},
};

// --- Code set 2: alternate "Aquos" NEC set (Flipper-IRDB TVs/Sharp/Aquos.ir) ---
// Model unknown and only the basic keys were captured.
static const Code ALT_NEC[] = {
    {Key::POWER, 0x7F00, 0xF50A},  {Key::INPUT, 0x7F00, 0xAC53},    {Key::MUTE, 0x7F00, 0xAF50},
    {Key::VOL_UP, 0x7F00, 0xE11E}, {Key::VOL_DOWN, 0x7F00, 0xA05F}, {Key::CH_UP, 0x7F00, 0xE01F},
    {Key::CH_DOWN, 0x7F00, 0xA35C}, {Key::MENU, 0x7F00, 0xA857},    {Key::UP, 0x7F00, 0xA15E},
    {Key::DOWN, 0x7F00, 0xA956},   {Key::LEFT, 0x7F00, 0xA45B},     {Key::RIGHT, 0x7F00, 0xA758},
    {Key::ENTER, 0x7F00, 0xA55A},  {Key::EXIT, 0x7F00, 0xB14E},     {Key::DISPLAY, 0x7F00, 0xF40B},
};

struct CodeSet {
  Proto proto;
  const Code *codes;
  size_t count;
};

// Order must match the options of the "Code Set" select in sharp-tv-remote.yaml.
static const CodeSet CODE_SETS[] = {
    {Proto::SHARP, AQUOS_SHARP, sizeof(AQUOS_SHARP) / sizeof(Code)},
    {Proto::NEC, HISENSE_NEC, sizeof(HISENSE_NEC) / sizeof(Code)},
    {Proto::NEC, ALT_NEC, sizeof(ALT_NEC) / sizeof(Code)},
};
static const size_t CODE_SET_COUNT = sizeof(CODE_SETS) / sizeof(CodeSet);

// Sharp protocol timing (microseconds), from the Harmony Pronto data.
static const uint32_t SHARP_CARRIER_HZ = 37000;
static const uint32_t SHARP_MARK_US = 270;
static const uint32_t SHARP_ZERO_US = 784;
static const uint32_t SHARP_ONE_US = 1837;
// Each frame plus the gap after it lasts a fixed period, so the gap depends on the bits sent.
static const uint32_t SHARP_PERIOD_US = 67800;

// Appends one frame and the gap that follows it.
inline void sharp_frame(RemoteTransmitData *data, uint8_t address, uint8_t command, bool inverted) {
  uint8_t cmd = inverted ? static_cast<uint8_t>(~command) : command;
  uint16_t bits = (address & 0x1F) | (static_cast<uint16_t>(cmd) << 5) | ((inverted ? 0 : 1) << 13) |
                  ((inverted ? 1 : 0) << 14);
  uint32_t elapsed = 0;
  for (uint8_t i = 0; i < 15; i++) {
    uint32_t space = (bits >> i) & 1 ? SHARP_ONE_US : SHARP_ZERO_US;
    data->item(SHARP_MARK_US, space);
    elapsed += SHARP_MARK_US + space;
  }
  data->mark(SHARP_MARK_US);
  data->space(SHARP_PERIOD_US - elapsed - SHARP_MARK_US);
}

inline void transmit_sharp(RemoteTransmitterBase *tx, uint8_t address, uint8_t command) {
  ESP_LOGD(TAG, "Sending Sharp: address=%u, command=%u", address, command);
  auto call = tx->transmit();
  RemoteTransmitData *data = call.get_data();
  data->set_carrier_frequency(SHARP_CARRIER_HZ);
  data->reserve(66);
  sharp_frame(data, address, command, false);
  sharp_frame(data, address, command, true);
  call.perform();
}

// NEC timing (microseconds). Encoded here rather than with ESPHome's NECProtocol,
// whose source is only compiled when the YAML itself uses an NEC action.
static const uint32_t NEC_CARRIER_HZ = 38000;
static const uint32_t NEC_HEADER_MARK_US = 9000;
static const uint32_t NEC_HEADER_SPACE_US = 4500;
static const uint32_t NEC_BIT_MARK_US = 560;
static const uint32_t NEC_ONE_US = 1690;
static const uint32_t NEC_ZERO_US = 560;

// address and command are 16-bit, LSB first (same convention as ESPHome's NEC actions).
inline void transmit_nec(RemoteTransmitterBase *tx, uint16_t address, uint16_t command) {
  ESP_LOGD(TAG, "Sending NEC: address=0x%04X, command=0x%04X", address, command);
  auto call = tx->transmit();
  RemoteTransmitData *data = call.get_data();
  data->set_carrier_frequency(NEC_CARRIER_HZ);
  data->reserve(68);
  data->item(NEC_HEADER_MARK_US, NEC_HEADER_SPACE_US);
  uint32_t bits = address | (static_cast<uint32_t>(command) << 16);
  for (uint8_t i = 0; i < 32; i++)
    data->item(NEC_BIT_MARK_US, (bits >> i) & 1 ? NEC_ONE_US : NEC_ZERO_US);
  data->mark(NEC_BIT_MARK_US);
  call.perform();
}

// Sends `key` using the code set chosen in `code_set`; returns a status line for the UI.
inline std::string press(RemoteTransmitterBase *tx, esphome::select::Select *code_set, Key key, const char *name) {
  size_t index = code_set->active_index().value_or(0);
  if (index >= CODE_SET_COUNT)
    index = 0;
  const CodeSet &set = CODE_SETS[index];

  for (size_t i = 0; i < set.count; i++) {
    const Code &code = set.codes[i];
    if (code.key != key)
      continue;
    char buf[64];
    if (set.proto == Proto::SHARP) {
      transmit_sharp(tx, code.address, code.command);
      snprintf(buf, sizeof(buf), "%s (Sharp %u/%u)", name, code.address, code.command);
    } else {
      transmit_nec(tx, code.address, code.command);
      snprintf(buf, sizeof(buf), "%s (NEC 0x%04X/0x%04X)", name, code.address, code.command);
    }
    return buf;
  }

  ESP_LOGW(TAG, "'%s' has no code in code set %u", name, (unsigned) index);
  return std::string(name) + ": not available in this code set";
}

}  // namespace sharp_tv
