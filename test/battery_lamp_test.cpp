// SquachWatch-CYD — the CLEAR-screen battery lamp preference.
#include "test_util.h"
#include "settings.h"
#include "theme.h"
#include "clock.h"
#include <cstdlib>

namespace Theme { void applyPalette(uint8_t) {} }
namespace Clock {
uint8_t     zoneCount()           { return 1; }
uint8_t     zoneStep(uint8_t, int){ return 0; }
const char* zoneName(uint8_t)     { return "UTC"; }
void        applyZone(uint8_t)    {}
}

int main() {
    setenv("SQUACHSIM_NVS", "out", 1);
    remove("out/settings.nvs");
    Settings::load();

    suite("Battery lamp setting");
    ck("shown by default", Settings::batteryLamp());
    Settings::toggleBatteryLamp();
    ck("can be turned off", !Settings::batteryLamp());

    Settings::load();
    ck("off choice survives reload", !Settings::batteryLamp());
    Settings::toggleBatteryLamp();
    ck("can be turned back on", Settings::batteryLamp());

    return report();
}
