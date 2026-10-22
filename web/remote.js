// Remote-control page for the Sharp TV remote. Served by ESPHome's web_server
// as /0.js; it replaces the default UI and talks to the REST API:
//   POST /button/<name>/press, POST /select/<name>/set?option=...
// It deliberately doesn't hold the /events stream open: the ESP32 has only a
// few sockets, and a long-lived connection per tab can starve page loads.

const CODE_SET = "Code Set";
const LAST_SENT = "Last Sent";

// [entity name, label, extra class, hold-to-repeat]
const layout = [
  { cls: "row3", keys: [["Input", "INPUT"], ["TV", "TV"], ["Power", "⏻", "power"]] },
  { cls: "row2 small", keys: [["Power On", "Power On"], ["Power Off", "Power Off"]] },
  {
    cls: "numpad",
    keys: [
      ["Digit 1", "1"], ["Digit 2", "2"], ["Digit 3", "3"],
      ["Digit 4", "4"], ["Digit 5", "5"], ["Digit 6", "6"],
      ["Digit 7", "7"], ["Digit 8", "8"], ["Digit 9", "9"],
      ["Dot", "•"], ["Digit 0", "0"], ["Ent", "ENT"],
    ],
  },
  { cls: "rockers" },
  {
    cls: "row4 small",
    keys: [
      ["AV Mode", "AV MODE", "accent"], ["View Mode", "VIEW MODE", "accent"],
      ["Freeze", "FREEZE"], ["Audio", "AUDIO"],
      ["Display", "DISPLAY"], ["Sleep", "SLEEP"], ["CC", "CC"], ["3D", "3D"],
    ],
  },
  {
    cls: "row3 small",
    keys: [["Smart Central", "SMART CENTRAL"], ["Netflix", "NETFLIX", "netflix"], ["Fav Ch", "FAV CH"]],
  },
  { cls: "row3 small", keys: [["Menu", "MENU"], ["Option", "OPTION"], ["Exit", "EXIT"]] },
  { cls: "dpad" },
  { cls: "row2 small", keys: [["Return", "↩ RETURN"], ["Manual", "MANUAL"]] },
  {
    cls: "row4 colours",
    keys: [["A", "A", "red"], ["B", "B", "green"], ["C", "C", "yellow"], ["D", "D", "blue"]],
  },
  {
    cls: "row4",
    keys: [["Prev", "⏮"], ["Rewind", "⏪", "", true], ["Fast Forward", "⏩", "", true], ["Next", "⏭"]],
  },
  { cls: "row3", keys: [["Play", "▶"], ["Pause", "⏸"], ["Stop", "⏹"]] },
  { cls: "row3 small", keys: [["App 1", "APP 1"], ["App 2", "APP 2"], ["App 3", "APP 3"]] },
];

const inputs = [
  ["HDMI 1", "HDMI 1"], ["HDMI 2", "HDMI 2"], ["HDMI 3", "HDMI 3"], ["HDMI 4", "HDMI 4"],
  ["Component", "COMP"], ["Video 1", "VIDEO 1"], ["Video 2", "VIDEO 2"], ["PC", "PC"],
];

// ESPHome rejects POSTs without a Content-Length (HTTP 411), so always send an empty body.
const POST = { method: "POST", body: "" };

const url = (domain, name, action = "") =>
  `/${domain}/${encodeURIComponent(name)}${action ? "/" + action : ""}`;

const el = (tag, attrs = {}, ...children) => {
  const node = document.createElement(tag);
  for (const [k, v] of Object.entries(attrs)) {
    if (k === "class") node.className = v;
    else node.setAttribute(k, v);
  }
  node.append(...children);
  return node;
};

let status;

function flash(button, ok) {
  button.classList.remove("ok", "err");
  void button.offsetWidth; // restart the animation
  button.classList.add(ok ? "ok" : "err");
}

async function showLastSent() {
  // The device updates "Last Sent" just after answering the press.
  await new Promise((resolve) => setTimeout(resolve, 150));
  try {
    const res = await fetch(url("text_sensor", LAST_SENT));
    const data = await res.json();
    if (data.state) status.textContent = `Sent: ${data.state}`;
  } catch {
    // Status line is cosmetic; the press itself already succeeded.
  }
}

async function press(name, button) {
  if (navigator.vibrate) navigator.vibrate(15);
  try {
    const res = await fetch(url("button", name, "press"), POST);
    flash(button, res.ok);
    if (res.ok) showLastSent();
    else status.textContent = `Error ${res.status} sending ${name}`;
  } catch {
    flash(button, false);
    status.textContent = "Not connected to the remote's Wi-Fi?";
  }
}

// Volume, channel and arrows repeat while held.
function key([name, label, cls = "", repeat = false]) {
  const button = el("button", { class: `key ${cls}`, type: "button", "aria-label": name }, label);
  let delay, timer;
  const stop = () => {
    clearTimeout(delay);
    clearInterval(timer);
  };
  button.addEventListener("pointerdown", (e) => {
    e.preventDefault();
    press(name, button);
    if (repeat) {
      delay = setTimeout(() => (timer = setInterval(() => press(name, button), 250)), 450);
    }
  });
  for (const ev of ["pointerup", "pointerleave", "pointercancel"]) button.addEventListener(ev, stop);
  button.addEventListener("keydown", (e) => {
    if (e.key === "Enter" || e.key === " ") {
      e.preventDefault();
      press(name, button);
    }
  });
  return button;
}

function rockers() {
  const rocker = (up, down, label) =>
    el("div", { class: "rocker" }, key([up, "+", "", true]), el("span", {}, label), key([down, "−", "", true]));
  return el(
    "div",
    { class: "rockers" },
    rocker("Vol Up", "Vol Down", "VOL"),
    el("div", { class: "rocker-mid" }, key(["Mute", "🔇 MUTE", "small"]), key(["Flashback", "FLASH BACK", "small"])),
    rocker("Ch Up", "Ch Down", "CH"),
  );
}

function dpad() {
  return el(
    "div",
    { class: "dpad" },
    key(["Up", "▲", "up", true]),
    key(["Left", "◀", "left", true]),
    key(["Enter", "ENTER", "enter"]),
    key(["Right", "▶", "right", true]),
    key(["Down", "▼", "down", true]),
  );
}

async function loadCodeSet(select) {
  try {
    const res = await fetch(url("select", CODE_SET) + "?detail=all");
    const data = await res.json();
    select.replaceChildren(...data.option.map((o) => el("option", { value: o }, o)));
    select.value = data.state;
  } catch {
    select.replaceChildren(el("option", {}, "(unavailable)"));
    status.textContent = "Not connected to the remote's Wi-Fi?";
  }
}

function setupCodeSet(select) {
  loadCodeSet(select);
  // Another phone may have changed it while this tab was in the background.
  document.addEventListener("visibilitychange", () => {
    if (document.visibilityState === "visible") loadCodeSet(select);
  });
  select.addEventListener("change", async () => {
    await fetch(url("select", CODE_SET, "set") + "?option=" + encodeURIComponent(select.value), POST);
    status.textContent = `Code set: ${select.value}`;
  });
}

function tester() {
  const address = el("input", { type: "number", min: "0", max: "31", value: "1", inputmode: "numeric" });
  const command = el("input", { type: "number", min: "0", max: "255", value: "0", inputmode: "numeric" });
  const send = el("button", { class: "key small", type: "button" }, "SEND");
  const next = el("button", { class: "key small", type: "button" }, "SEND NEXT");
  const setNumber = (name, value) =>
    fetch(url("number", name, "set") + "?value=" + encodeURIComponent(value), POST);
  const sendCurrent = async () => {
    await setNumber("Test Address", address.value);
    await setNumber("Test Command", command.value);
    await press("Send Test Code", send);
  };
  send.addEventListener("click", sendCurrent);
  next.addEventListener("click", () => {
    command.value = (Number(command.value) + 1) % 256;
    sendCurrent();
  });
  return el(
    "details",
    {},
    el("summary", {}, "Code tester (Sharp protocol)"),
    el("p", { class: "hint" }, "Send any address/command to hunt for codes not on the remote. Address 1 and 17 are used by this TV."),
    el("div", { class: "tester" },
      el("label", {}, "Address", address), el("label", {}, "Command", command), send, next),
  );
}

function build() {
  const select = el("select", { id: "codeset", "aria-label": "Code set" });
  status = el("div", { class: "status", role: "status" }, "Ready");

  const body = el("main", { class: "remote" },
    el("header", {},
      el("div", { class: "brand" }, "SHARP ", el("span", {}, "AQUOS")),
      el("label", { class: "codeset" }, "Code set", select),
      status,
    ),
  );

  for (const section of layout) {
    if (section.cls === "rockers") body.append(rockers());
    else if (section.cls === "dpad") body.append(dpad());
    else body.append(el("div", { class: `grid ${section.cls}` }, ...section.keys.map(key)));
  }

  body.append(
    el("details", {},
      el("summary", {}, "Direct inputs"),
      el("div", { class: "grid row4 small" }, ...inputs.map(key)),
    ),
    tester(),
  );

  document.title = "Sharp TV Remote";
  const viewport = el("meta", { name: "viewport", content: "width=device-width, initial-scale=1, user-scalable=no" });
  document.head.append(viewport, el("meta", { name: "theme-color", content: "#15171a" }));
  document.body.replaceChildren(body);

  setupCodeSet(select);
}

build();
