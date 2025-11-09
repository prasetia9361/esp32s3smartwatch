# Quick Reference - Build & Upload

## If UI is Stuck After Pressing Alarm Button

### Root Cause
LVGL memory (48KB) too small → Switch to PSRAM (8MB)

### Quick Fix Commands

```bash
# STEP 1: Clean everything
pio run -t clean

# STEP 2: Erase flash (REQUIRED - partition changed!)
pio run -t erase

# STEP 3: Build and upload
pio run -t upload

# STEP 4: Monitor serial for memory logs
pio device monitor
```

### What to Check in Serial Monitor

✅ **Good (Working):**
```
I (1234) VARS: LVGL Memory before timeAlarm - Free: 7890000, Used: 2450
I (5678) Smartwatch: Watchdog timer initialized
```

❌ **Bad (Still broken):**
```
E (1234) ALLOC: Failed to allocate 128 bytes
E (5678) VARS: LVGL Memory - Free: 1024, Used: 48000
```

### Files Changed
- ✅ `include/lv_conf.h` - LVGL memory config
- ✅ `partitions.csv` - Partition table
- ✅ `lib/ui/vars.cpp` - Memory monitoring
- ✅ `src/main.cpp` - Watchdog timer

### What Changed
| Config | Before | After |
|--------|--------|-------|
| LVGL Memory | 48KB fixed | **Unlimited (PSRAM)** |
| App Partition | 12MB | **13MB** |
| Watchdog | None | **10s timeout** |
| Monitoring | None | **Real-time logs** |

### Testing Steps
1. Open alarm screen → should not freeze
2. Set time multiple times → should be smooth
3. Check serial log → memory usage should be low
4. Leave idle 1 min → no watchdog reset

### If Still Stuck
1. Check serial monitor for errors
2. Verify PSRAM is detected: `I (123) PSRAM: Found 8MB PSRAM`
3. Check free memory: should be > 7MB
4. Report error logs

---
See `MEMORY_FIX_ALARM_STUCK.md` for technical details.
