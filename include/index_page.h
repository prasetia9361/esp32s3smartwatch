#pragma once
#include <Arduino.h>

static const char PAGE_index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width,initial-scale=1" />
  <title>Alarm Motion</title>
  <style>
    body{font-family:system-ui,Arial,sans-serif;margin:1.2rem;line-height:1.35;background:#111;color:#eee}
    h1{font-size:1.4rem;margin:0 0 .5rem}
    card,form,section{background:#1e1e1e;padding:1rem;border-radius:8px;margin-bottom:1rem;box-shadow:0 2px 4px #0008}
    input,button{font:inherit;padding:.4rem .6rem;border-radius:4px;border:1px solid #444;background:#222;color:#eee}
    button{cursor:pointer;background:#2d6cdf;border-color:#2d6cdf}
    button:hover{filter:brightness(1.1)}
    .time{font-size:1.2rem;font-weight:600;letter-spacing:.5px}
    footer{margin-top:2rem;font-size:.7rem;opacity:.6;text-align:center}
    .row{display:flex;gap:.5rem;flex-wrap:wrap}
    label{display:flex;flex-direction:column;font-size:.7rem;text-transform:uppercase;gap:.2rem;min-width:70px}
  </style>
</head>
<body>
  <h1>RTC Time</h1>
  <section>
    <div>Waktu Device:</div>
    <div class="time" id="time">--</div>
    <button id="refresh">Refresh</button>
  </section>
  <section>
    <h2 style="font-size:1rem;margin:.8rem 0 .4rem">Kalibrasi Waktu Device</h2>
    <p style="font-size:.7rem;opacity:.75">Lakukan Kalibrasi Jika Waktu Device Tidak Tepat</p>
    <div style="display:flex;gap:.6rem;align-items:center;flex-wrap:wrap">
      <button id="syncNow">Klaibrasi</button>
      <div id="calStatus" style="font-size:.7rem"></div>
      <div style="font-size:.65rem;opacity:.75">Offset: <span id="offset">--</span> ms</div>
    </div>
  </section>
  <section>
    <h2 style="font-size:1rem;margin:.8rem 0 .4rem">Jadwal Aktif</h2>
    <form id="scheduleForm" class="row" autocomplete="off">
      <label>Mulai<input type="time" name="start" id="startTime" required step="60" /></label>
      <label>Selesai<input type="time" name="end" id="endTime" required step="60" /></label>
      <div style="align-self:flex-end;display:flex;gap:.5rem">
        <button type="submit">Simpan</button>
      </div>
    </form>
    <div id="scheduleStatus" style="margin-top:.4rem;font-size:.7rem"></div>
  </section>
  <div style="margin:1rem 0"><a style="color:#7fb3ff;text-decoration:none" href="/settings">WiFi Settings &raquo;</a></div>
  <footer>KiBo Labs</footer>
  <script>
    const $ = id => document.getElementById(id);
    async function loadTime(){
      try{
        const r = await fetch('/api/time');
        if(!r.ok) throw new Error();
        const j = await r.json();
        $('time').textContent = j.iso;
      }catch(e){$('time').textContent='Error';}
    }
    $('refresh').addEventListener('click', e=>{e.preventDefault();loadTime();});
    setInterval(loadTime,5000);
    async function syncBrowserTime(){
      const d0=performance.now();
      const local=new Date();
      const params=new URLSearchParams({year:local.getFullYear(),month:local.getMonth()+1,day:local.getDate(),hour:local.getHours(),minute:local.getMinutes(),second:local.getSeconds()});
      calStatus.textContent='Syncing...';
      try{
        const r=await fetch('/api/time',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:params.toString()});
        if(!r.ok){calStatus.textContent='Failed ('+r.status+')';return;}
        const d1=performance.now();
        const latency=d1-d0;
        setTimeout(async()=>{
          const g=await fetch('/api/time');
          if(!g.ok){calStatus.textContent='Check failed';return;}
            const j=await g.json();
            const m=j.iso.match(/(\d+)-(\d+)-(\d+)T(\d+):(\d+):(\d+)/);
            let off=NaN; if(m){const srv=new Date(Date.UTC(m[1],m[2]-1,m[3],m[4],m[5],m[6]));off=srv.getTime()-Date.now();}
            offset.textContent=isNaN(off)?'n/a':off;
            calStatus.textContent='Synced (RTT '+latency.toFixed(1)+' ms)';
            loadTime();
        },150);
      }catch(err){calStatus.textContent='Error';}
    }
    syncNow.addEventListener('click',e=>{e.preventDefault();syncBrowserTime();});
    // Schedule handling
    async function loadSchedule(){
      try{const r=await fetch('/api/schedule'); if(!r.ok) return; const j=await r.json(); startTime.value=(j.start||'').slice(0,5); endTime.value=(j.end||'').slice(0,5); }catch(e){}
    }
    function toMinutes(hhmm){if(!hhmm||hhmm.length<5) return NaN;return parseInt(hhmm.slice(0,2))*60+parseInt(hhmm.slice(3,5));}
    function validateTimes(){
      const s = toMinutes(startTime.value);
      const e = toMinutes(endTime.value);
      if(isNaN(s)||isNaN(e)) return false;
      if(e < s){
        scheduleStatus.textContent='End time must be after start time';
        return false;
      }
      scheduleStatus.textContent='';
      return true;
    }
    startTime.addEventListener('change', validateTimes);
    endTime.addEventListener('change', validateTimes);
    scheduleForm.addEventListener('submit', async e=>{
      e.preventDefault();
      if(!validateTimes()) return;
      scheduleStatus.textContent='Saving...';
      // Normalize
      if(startTime.value.length>5) startTime.value=startTime.value.slice(0,5);
      if(endTime.value.length>5) endTime.value=endTime.value.slice(0,5);
      const fd=new FormData(scheduleForm);
      const params=new URLSearchParams(fd).toString();
      try{const r=await fetch('/api/schedule',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:params});const t=await r.text();scheduleStatus.textContent=r.ok?'Saved':'Error '+t;}catch(err){scheduleStatus.textContent='Error '+err;}
    });
    loadTime();
    loadSchedule();
  </script>
</body>
</html>
)rawliteral";

static const size_t PAGE_index_html_L = sizeof(PAGE_index_html) - 1;
