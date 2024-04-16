#pragma once
#include<Arduino.h>

typedef uint32_t msecu32_t;

  typedef int8_t gpin_t;
  enum class cmd_t : uint8_t {
    swreset = 0x01, // Software Reset
    slpin = 0x10,   // Sleep in
    slpout = 0x11,  // Sleep out
    noron = 0x13,   // Normal Display Mode On
    invoff = 0x20,  // Display Inversion Off
    dispon = 0x29,  // Display On
    caset = 0x2A,   // Column Address Set
    raset = 0x2B,   // Row Address Set
    ramwr = 0x2C,   // Memory Write
    madctl = 0x36,  // Memory Data Access Control
    colmod = 0x3A,  // Interface Pixel Format
    pgc = 0xE0,     // Positive Gamma Control
    ngc = 0xE1,     // Negative Gamma Control
    cscon = 0xF0,   // Command Set wqaControl
  };
    enum class madctl_t : uint8_t {
    my = 0x80,  // Row Addr Order: 0=Inc(↓), 1=Dec(↑)
    mx = 0x40,  // Column Addr Order: 0=Inc(→), 1=Dec(←)
    mv = 0x20,  // Row/Col exchange: 0=Norm, 1=Exchange
    ml = 0x10,  // Vertical Refresh Order
    bgr = 0x08, // Subpixel rendering: BGR order
    mh = 0x10,  // Horizontal Refresh Order
    rgb = 0x00, // Subpixel rendering: RGB order
    // MADCTL rotation bitmasks
    // We can conveniently translate these bitmasks to unique values 0..3,
    // andddd preserve the ordering for computing degrees of rotation, as
    // follows:
    //    enum: ((mask >> 5) & 3)          degrees: enum * 90
    tall = my,                    // ((128 >> 5) & 3) * 90 == 0 * 90 =   0°
    wide = mv,                    //  ((32 >> 5) & 3) * 90 == 1 * 90 =  90°
    tall_inverted = mx,           //  ((64 >> 5) & 3) * 90 == 2 * 90 = 180°
    wide_inverted = my | mv | mx, // ((224 >> 5) & 3) * 90 == 3 * 90 = 270°
  };
    enum class colmod_t : uint8_t {
    rgb16 = 0x50,
    ctrl16 = 0x05,
    rgb656 = rgb16 | ctrl16,
  };
    // GPIO pins
  static gpin_t constexpr _pin_sdi = 12U; // SPI MISO
  static gpin_t constexpr _pin_sdo = 13U; // SPI MOSI
  static gpin_t constexpr _pin_sck = 14U; // SPI SCLK
  static gpin_t constexpr _pin_sel = 15U; // SPI SS/CS
  static gpin_t constexpr _pin_sdc = 2U;  // LCD DC/RS
  static gpin_t constexpr _pin_blt = 27U; // delicious BLT
  static gpin_t constexpr _pin_scl = 32U; // I²C SCL
  static gpin_t constexpr _pin_sda = 33U; // I²C SDA
  static gpin_t constexpr _pin_int = 21U; // TPC INT
  static gpin_t constexpr _pin_rst = 25U; // TPC RST
  // SPI interfaces
  static uint8_t constexpr _spi_bus = 2U;          // HSPI
  static uint32_t constexpr _bus_freq = 80000000U; // 80 MHz
  static uint8_t constexpr _word_ord = SPI_MSBFIRST;
  static uint8_t constexpr _sig_mode = SPI_MODE0;
  // I²C interfaces
  static uint8_t constexpr _i2c_bus = 1U;    // Wire1
  static uint8_t constexpr _dev_addr = 0x5D; // I²C touch device address
  // Backlight PWM
  static uint8_t constexpr _pwm_blt_chan = 12U;
  static uint32_t constexpr _pwm_blt_freq = 5000U; // 5 kHz
  static uint8_t constexpr _pwm_blt_bits = 8U;
  static uint8_t constexpr _pwm_blt_hres = (1U << _pwm_blt_bits) - 1U;
  // TFT configuration
  static msecu32_t constexpr _tft_refresh = msecu32_t{10}; // milliseconds
  static uint16_t constexpr _tft_width = 320U;  // Physical dimensions, does
  static uint16_t constexpr _tft_height = 480U; // not consider rotations.
  static uint8_t constexpr _tft_depth = static_cast<uint8_t>(colmod_t::rgb656);
  static uint8_t constexpr _tft_subpixel = static_cast<uint8_t>(madctl_t::rgb);
  static uint8_t constexpr _tft_aspect =
      static_cast<uint8_t>(madctl_t::wide_inverted);
  // Touch MMIO
  static uint16_t constexpr _reg_prod_id = 0x8140; // product ID (byte 1/4)
  static size_t constexpr _size_prod_id = 4U;      // bytes
  static uint16_t constexpr _reg_stat = 0x814E;    // buffer/track status
  static uint16_t constexpr _reg_base_point = 0x814F;
  static size_t constexpr _touch_max = 5U; // simultaneous touch points
/*
  template <class... E> void tx(cmd_t const code, E... e) {
    constexpr size_t size = sizeof...(e);
    uint8_t data[size] = {static_cast<uint8_t>(e)...};
    digitalWrite(_pin_sdc, LOW); // D/C ⇒ command
    beginTransaction(SPISettings(_bus_freq, _word_ord, _sig_mode));
    digitalWrite(_pin_sel, LOW); // C/S ⇒ enable
    SPIClass::write(static_cast<uint8_t>(code));
    if (size > 0) {
      digitalWrite(_pin_sdc, HIGH); // D/C ⇒ data
      writeBytes(data, size);
    }
    digitalWrite(_pin_sel, HIGH); // C/S ⇒ disable
    endTransaction();
  }
*/
