/**
 * @file I2S.cpp
 * @author Fran Fodor for Soldered
 * @brief Helper for I2S communication.
 *
 * https://github.com/SolderedElectronics/Inkplate-Esp-library
 * For more info about the product, please check:
 * https://docs.soldered.com/inkplate/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "esp_private/i2s_platform.h"
#include "esp_private/periph_ctrl.h"
#include "soc/gpio_periph.h"
#include "soc/gpio_struct.h"
#include "soc/i2s_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/soc.h"

#include "I2S.h"

static const char *TAG = "ESP_I2S";

/* -------------------------------------------------------------------------- */
/*                              Public functions                              */
/* -------------------------------------------------------------------------- */

I2S::I2S(uint8_t clockDivider) {
  m_i2s = &I2S1;

  ESP_ERROR_CHECK(i2s_platform_acquire_occupation(I2S_CTLR_HP, 1, "inkplate"));

  // Reset the FIFO Buffer in I2S module.
  m_i2s->conf.rx_fifo_reset = 1;
  m_i2s->conf.rx_fifo_reset = 0;
  m_i2s->conf.tx_fifo_reset = 1;
  m_i2s->conf.tx_fifo_reset = 0;

  // Reset I2S DMA controller.
  m_i2s->lc_conf.in_rst = 1;
  m_i2s->lc_conf.in_rst = 0;
  m_i2s->lc_conf.out_rst = 1;
  m_i2s->lc_conf.out_rst = 0;

  // Reset I2S TX and RX module.
  m_i2s->conf.rx_reset = 1;
  m_i2s->conf.tx_reset = 1;
  m_i2s->conf.rx_reset = 0;
  m_i2s->conf.tx_reset = 0;

  // Set LCD mode on I2S, setup delays on SD and WR lines.
  m_i2s->conf2.val = 0;
  m_i2s->conf2.lcd_en = 1;
  m_i2s->conf2.lcd_tx_wrx2_en = 1;
  m_i2s->conf2.lcd_tx_sdx2_en = 0;

  m_i2s->sample_rate_conf.val = 0;
  m_i2s->sample_rate_conf.rx_bits_mod = 8;
  m_i2s->sample_rate_conf.tx_bits_mod = 8;
  m_i2s->sample_rate_conf.rx_bck_div_num = 2;
  m_i2s->sample_rate_conf.tx_bck_div_num = 2;

  // Do not use APLL, divide by 5 by default, BCK should be ~16MHz.
  m_i2s->clkm_conf.val = 0;
  m_i2s->clkm_conf.clka_en = 0;
  m_i2s->clkm_conf.clkm_div_b = 0;
  m_i2s->clkm_conf.clkm_div_a = 1;
  m_i2s->clkm_conf.clkm_div_num = clockDivider;

  // FIFO buffer setup. Byte packing for FIFO: 0A0B_0B0C = 0, 0A0B_0C0D = 1,
  // 0A00_0B00 = 3. Use dual mono single data
  m_i2s->fifo_conf.val = 0;
  m_i2s->fifo_conf.rx_fifo_mod_force_en = 1;
  m_i2s->fifo_conf.tx_fifo_mod_force_en = 1;
  m_i2s->fifo_conf.tx_fifo_mod = 1;
  m_i2s->fifo_conf.rx_data_num = 1;
  m_i2s->fifo_conf.tx_data_num = 1;
  m_i2s->fifo_conf.dscr_en = 1;

  // Send BCK only when needed (needs to be powered on in einkOn() function and
  // disabled in einkOff()).
  m_i2s->conf1.val = 0;
  m_i2s->conf1.tx_stop_en = 0;
  m_i2s->conf1.tx_pcm_bypass = 1;

  m_i2s->conf_chan.val = 0;
  m_i2s->conf_chan.tx_chan_mod = 1;
  m_i2s->conf_chan.rx_chan_mod = 1;

  m_i2s->conf.tx_right_first = 0; //!!invert_clk; // should be false / 0
  m_i2s->conf.rx_right_first = 0; //!!invert_clk;

  m_i2s->timing.val = 0;
}

void I2S::sendDataI2S() {
  // Stop any on-going transmission (just in case).
  m_i2s->out_link.stop = 1;
  m_i2s->out_link.start = 0;
  m_i2s->conf.tx_start = 0;

  // Reset the FIFO.
  m_i2s->conf.tx_fifo_reset = 1;
  m_i2s->conf.tx_fifo_reset = 0;

  // Reset the I2S DMA Controller.
  m_i2s->lc_conf.out_rst = 1;
  m_i2s->lc_conf.out_rst = 0;

  // Reset I2S TX module.
  m_i2s->conf.tx_reset = 1;
  m_i2s->conf.tx_reset = 0;

  // Setup a DMA descriptor.
  m_i2s->lc_conf.val = I2S_OUT_DATA_BURST_EN | I2S_OUTDSCR_BURST_EN;
  m_i2s->out_link.addr = (uint32_t)(m_dmaI2SDesc) & 0x000FFFFF;

  // Start sending the data
  m_i2s->out_link.start = 1;

  // Pull SPH low -> Start pushing data into the row of EPD.
  SPH_CLEAR;

  // Set CKV to HIGH.
  CKV_SET;

  // Start sending I2S data out.
  m_i2s->conf.tx_start = 1;

  while (!m_i2s->int_raw.out_total_eof)
    ;

  SPH_SET;

  // Clear the interrupt flags and stop the transmission.
  m_i2s->int_clr.val = m_i2s->int_raw.val;
  m_i2s->out_link.stop = 1;
  m_i2s->out_link.start = 0;
}

void I2S::setI2S1pin(uint32_t pin, uint32_t function, uint32_t inv) {
  // Check if valid pin is selected
  if (pin > 39)
    return;

  // Wrong pin selected? Return!
  if (GPIO_PIN_MUX_REG[pin] == 0)
    return;

  // Setup GPIO Matrix for selected pin signal
  GPIO.func_out_sel_cfg[pin].func_sel = function; // Set the pin function
  GPIO.func_out_sel_cfg[pin].inv_sel =
      inv; // Does pin logic needs to be inverted?
  GPIO.func_out_sel_cfg[pin].oen_sel = 0; // Force output enable if bit is set

  // Registers are different for GPIOs from 0 to 32 and from 32 to 40.
  if (pin < 32) {
    // Enable GPIO pin (set it as output).
    GPIO.enable_w1ts = ((uint32_t)1 << pin);
  } else {
    // Enable GPIO pin (set it as output).
    GPIO.enable1_w1ts.data = ((uint32_t)1 << (pin - 32));
  }

  // Set the highest drive strength.
  REG_WRITE(GPIO_PIN_MUX_REG[pin], 0);
  REG_WRITE(GPIO_PIN_MUX_REG[pin], ((3 << FUN_DRV_S) | (2 << MCU_SEL_S)));
}
