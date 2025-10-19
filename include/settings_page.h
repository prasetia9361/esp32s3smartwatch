#pragma once
#include <Arduino.h>

// Settings page: calibration, set time, wifi config
static const char PAGE_settings_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8"/>
<meta name="viewport" content="width=device-width,initial-scale=1" />
<title>Settings</title>
<style>
 body{font-family:system-ui,Arial,sans-serif;margin:1.1rem;background:#111;color:#eee;line-height:1.35}
 h1{font-size:1.3rem;margin:0 0 .75rem}
 section{background:#1e1e1e;padding:1rem;border-radius:8px;margin-bottom:1rem;box-shadow:0 2px 4px #0008}
 input,button{font:inherit;padding:.45rem .7rem;border-radius:4px;border:1px solid #444;background:#222;color:#eee}
 button{cursor:pointer;background:#2d6cdf;border-color:#2d6cdf}
 button:hover{filter:brightness(1.1)}
 .row{display:flex;gap:.5rem;flex-wrap:wrap}
 label{display:flex;flex-direction:column;font-size:.65rem;text-transform:uppercase;gap:.25rem;min-width:70px}
 footer{margin-top:2rem;font-size:.65rem;opacity:.6;text-align:center}
 a{color:#7fb3ff;text-decoration:none}
 a:hover{text-decoration:underline}
 .time{font-weight:600}
</style>
</head>
<body>
  <nav style="margin-bottom:1rem;font-size:.8rem"><a href="/">&larr; Back</a></nav>
  <h1>Device Settings</h1>
  <!-- Calibration & Manual time removed (moved to index / dropped) -->
  <section>
    <h2 style="font-size:1rem;margin:.2rem 0 .6rem">WiFi AP Settings</h2>
    <form id="wifiForm" class="row" autocomplete="off">
      <label>SSID<input name="ssid" id="wifiSsid" required maxlength="63" /></label>
      <label>Password<input name="password" id="wifiPass" type="password" maxlength="63" /></label>
      <div style="align-self:flex-end;display:flex;gap:.5rem">
        <button type="submit">Save WiFi</button>
      </div>
    </form>
    <div id="wifiStatus" style="margin-top:.5rem;font-size:.7rem"></div>
  <p style="font-size:.6rem;opacity:.6;margin-top:.5rem">Password must be 8-63 chars (leave blank for open). AP restarts automatically.</p>
  </section>
  <footer>&copy; Device Settings</footer>
<script>
const $=id=>document.getElementById(id);
async function loadWifi(){
  try{
    const r=await fetch('/api/wifi');
    if(!r.ok) return;
    const j=await r.json();
    // Prefer live SSID (current AP) else stored config
    wifiSsid.value=j.live||j.ssid||'';
    wifiPass.value=j.password||'';
    if(j.live && j.live!==j.ssid){wifiStatus.textContent='(Live SSID differs from stored; form shows live)';}
  }catch(e){}
}
wifiForm.addEventListener('submit',async e=>{
  e.preventDefault();
  const fd=new FormData(wifiForm);
  const params=new URLSearchParams(fd).toString();
  wifiStatus.textContent='Saving...';
  try{
    const r=await fetch('/api/wifi',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:params});
    const txt=await r.text();
    if(r.status===400 && txt.includes('pwd_len')){ wifiStatus.textContent='Error: password must be 8-63 chars'; return; }
    if(r.ok){ wifiStatus.textContent='Saved (AP restarting...)'; setTimeout(loadWifi, 1500); }
    else { wifiStatus.textContent='Error '+txt; }
  }catch(err){wifiStatus.textContent='Error '+err;}
});
loadWifi();
</script>
</body></html>
)rawliteral";

static const size_t PAGE_settings_html_L = sizeof(PAGE_settings_html) - 1;
