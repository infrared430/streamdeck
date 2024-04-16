#include <ESP323248S035.h>
#include "scrn_main.h"

// TODO: Need to see how to deal with multiple screeens...
//      lv_obj_t *scr = lv_obj_create(NULL, NULL); lv_scr_load(scr);
// Need a Screen for:
//  - Splash
//  - Wifi Chooser
//  - Recipie Navigator
//  - Recipie Viewer
//  - QR Viewer

static MainScreen view=MainScreen::getInstance();
static ESP323248S035C<View> target(view);

#if (LV_USE_LOG)
const TPC_LCD::Log TPC_LCD::log = [](const char *message) {
  Serial.println(message);
};
#endif

void brightRegulate(){
  uint16_t amblight=4095-target.hw<CDS_ADC>().get();
  int16_t brillo=map(amblight,0,4095,0,225)+25;
  target.hw<TPC_LCD>().set_backlight(brillo);
}

void ledFeedback(){
/*  if(!true){
    target.hw<RGB_PWM>().set(LV_COLOR_MAKE32(128,128,0));
    return;
  }
  target.hw<RGB_PWM>().set(LV_COLOR_MAKE32(0,0,0));
  */
}

void cron(){
  // if next day update recipies from web into SD in a second thread
}

void setup() {
  Serial.begin(115200);
  target.init();
  target.hw<RGB_PWM>().set(LV_COLOR_MAKE32(0,0,0));
  Serial.println(lv_version_info());
  Serial.println("Thread loop1 started");
}

void loop() {
  // check wifi
  // if not, add ttl and when exhausted dialog to connect wifi password, but still try to connect
  // THIS IS NOT OK, THIS NEEDS TO BE A WIFI STATE LISTENER
  //wifiCheck();
  brightRegulate();
  ledFeedback();
  cron();
  target.update();
  Serial.printf("ESP FREE SIZE HEAP: %d, int %d, min %d\n", esp_get_free_heap_size(), esp_get_free_internal_heap_size(), esp_get_minimum_free_heap_size());
  delay(50);
}
