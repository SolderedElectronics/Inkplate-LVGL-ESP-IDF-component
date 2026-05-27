# Inkplate LVGL ESP-IDF component

[![Build Examples](https://github.com/SolderedElectronics/Inkplate-LVGL-ESP-IDF-component/actions/workflows/build-examples.yml/badge.svg)](https://github.com/SolderedElectronics/Inkplate-LVGL-ESP-IDF-component/actions/workflows/build-examples.yml)
[![Format Check](https://github.com/SolderedElectronics/Inkplate-LVGL-ESP-IDF-component/actions/workflows/format-check.yml/badge.svg)](https://github.com/SolderedElectronics/Inkplate-LVGL-ESP-IDF-component/actions/workflows/format-check.yml)

<p align="center">
  <img src="https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Arduino-library/master/extras/InkplateImage.jpg">
</p>

The Soldered Inkplate LVGL ESP-IDF component brings full [LVGL](https://lvgl.io/) support to the Soldered Inkplate family of e-paper displays.
It provides ready-to-use integrations for display control, touch input (where available), and file system (FS) access, so you can focus entirely on designing and implementing your user interface.
All necessary ESP-IDF and LVGL initialization is handled internally through the class constructors — meaning you can start building your UI right away without worrying about setup details.
LVGL is bundled within the component — no separate LVGL dependency needed.

Supported boards: Inkplate 2, 4, 5, 6, 6Color, 6Flick, 10, 13.

### Getting started

1. If you aren't using macOS, install CH340 drivers (in case you don't have them yet) — instructions [here](https://soldered.com/blogs/learn/ch340-driver-installation)

2. Install **ESP-IDF v6.0 or newer**. Follow the official [Getting Started guide](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html) for your operating system.

3. **Add the component** to your project:
   ```
   idf.py add-dependency "solderedelectronics/inkplate_lvgl>=1.0.0"
   ```

4. **Add required config** to your project's `sdkconfig.defaults`:
   ```
   CONFIG_PARTITION_TABLE_SINGLE_APP_LARGE=y
   CONFIG_SPIRAM=y
   CONFIG_SPIRAM_SPEED_40M=y
   CONFIG_SPIRAM_BOOT_INIT=y
   CONFIG_ESP_MAIN_TASK_STACK_SIZE=16384
   ```
   > **Inkplate 13 only:** also add `CONFIG_SPIRAM_MODE_OCT=y`

5. **Register the component** in your `main/CMakeLists.txt`:
   ```cmake
   idf_component_register(SRCS "main.cpp"
                          REQUIRES "inkplate_lvgl"
                          INCLUDE_DIRS ".")
   ```

6. **Select your board** via menuconfig:
   ```
   idf.py menuconfig
   ```
   Navigate to **Inkplate Boards** and select your Inkplate model.

7. **Build, flash, and monitor:**
   ```
   idf.py build flash monitor
   ```

### Basic usage

```cpp
#include "Inkplate.h"

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);

    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, "Hello World!");
    lv_obj_center(label);

    lv_refr_now(lv_display_get_default()); // Render LVGL scene into framebuffer
    display.display();                     // Push framebuffer to e-paper panel
}
```

### Code examples

Examples are available on the [component registry page](https://components.espressif.com/components/solderedelectronics/inkplate_lvgl). Basic hello world examples are available for all supported boards: Inkplate 2, 4, 5, 6, 6Color, 6Flick, 10, and 13.

### Documentation

Find Inkplate documentation [here](https://soldered.com/documentation/inkplate/).

### Battery power

Inkplate boards has two options for powering it. First one is obvious - USB port at side of the board. Just plug any micro USB cable and you are good to go. Second option is battery. Supported batteries are standard Li-Ion/Li-Poly batteries with 3.7V nominal voltage. Connector for the battery is standard 2.00mm pitch JST connector (except on Inkplate 2, it uses SMD solder pads for battery terminals). The onboard charger will charge the battery with 500mA when USB is plugged at the same time. You can use battery of any size or capacity if you don't have a enclosure. If you are using our enclosure, battery size shouldn't exceed 90mm x 40mm (3.5 x 1.57 inch) and 5mm (0.19 inch) in height (excluding Inkplate 2, it uses [this battery](https://soldered.com/product/li-ion-baterija-600mah-3-7v/)). [This battery](https://soldered.com/product/li-ion-battery-1200mah-3-7v/) is a good fit for the Inkplate. Also, Inkplate's hardware is specially optimized for low power consumption in deep sleep mode, making it extremely suitable for battery applications.

#### WARNING
Please check the polarity on the battery JST connector! Some batteries that can be purchased from the web have reversed polarity that can damage Inkplate board! You are safe if you are using the pouch battery from [soldered.com](https://soldered.com/categories/power-sources-batteries/batteries/lithium-batteries/) or Inkplate with the built-in battery.

#### NOTE
CR2032 battery is only for RTC backup. Inkplate cannot be powered with it.

### About Soldered

<img src="https://raw.githubusercontent.com/SolderedElectronics/Soldered-IIS2DULPXTR-Accelerometer-Arduino-Library/dev/extras/Soldered-logo-color.png" alt="soldered-logo" width="500"/>

At Soldered, we design and manufacture a wide selection of electronic products to help you turn your ideas into acts and bring you one step closer to your final project. Our products are intended for makers and crafted in-house by our experienced team in Osijek, Croatia. We believe that sharing is a crucial element for improvement and innovation, and we work hard to stay connected with all our makers regardless of their skill or experience level. Therefore, all our products are open-source. Finally, we always have your back. If you face any problem concerning either your shopping experience or your electronics project, our team will help you deal with it, offering efficient customer service and cost-free technical support anytime.

### Where to buy

Inkplate boards are available for purchase via:

- [soldered.com](https://soldered.com/categories/inkplate/)
- [Crowd Supply](https://www.crowdsupply.com/soldered)
- [Mouser](https://hr.mouser.com/Search/Refine?Keyword=inkplate)

For all questions and issues please reach us via [e-mail](mailto:hello@soldered.com) or our [contact form](https://soldered.com/contact/).
