// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2013, The Linux Foundation. All rights reserved.

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>

struct dijing_ili9881c {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct backlight_device *backlight;
	struct gpio_desc *reset_gpio;

	bool prepared;
	bool enabled;
};

static inline struct dijing_ili9881c *to_dijing_ili9881c(struct drm_panel *panel)
{
	return container_of(panel, struct dijing_ili9881c, panel);
}

#define dsi_generic_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_generic_write(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void dijing_ili9881c_reset(struct dijing_ili9881c *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	msleep(20);
}

static int dijing_ili9881c_on(struct dijing_ili9881c *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi_generic_write_seq(dsi, 0xff, 0x98, 0x81, 0x03);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x01, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x02, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x03, 0x72);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x04, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x05, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x06, 0x09);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x07, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x08, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x09, 0x01);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x0a, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x0b, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x0c, 0x01);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x0d, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x0e, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x0f, 0x14);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x10, 0x14);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x11, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x12, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x13, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x14, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x15, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x16, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x17, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x18, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x19, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x1a, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x1b, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x1c, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x1d, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x1e, 0x40);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x1f, 0x80);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x20, 0x05);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x21, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x22, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x23, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x24, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x25, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x26, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x27, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x28, 0x33);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x29, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x2a, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x2b, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x2c, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x2d, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x2e, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x2f, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x30, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x31, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x32, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x33, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x34, 0x04);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x35, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x36, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x37, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x38, 0x78);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x39, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x3a, 0x40);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x3b, 0x40);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x3c, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x3d, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x3e, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x3f, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x40, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x41, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x42, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x43, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x44, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x50, 0x01);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x51, 0x23);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x52, 0x45);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x53, 0x67);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x54, 0x89);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x55, 0xab);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x56, 0x01);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x57, 0x23);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x58, 0x45);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x59, 0x67);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x5a, 0x89);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x5b, 0xab);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x5c, 0xcd);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x5d, 0xef);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x5e, 0x11);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x5f, 0x01);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x60, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x61, 0x15);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x62, 0x14);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x63, 0x0e);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x64, 0x0f);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x65, 0x0c);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x66, 0x0d);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x67, 0x06);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x68, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x69, 0x07);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x6a, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x6b, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x6c, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x6d, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x6e, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x6f, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x70, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x71, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x72, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x73, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x74, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x75, 0x01);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x76, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x77, 0x14);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x78, 0x15);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x79, 0x0e);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x7a, 0x0f);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x7b, 0x0c);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x7c, 0x0d);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x7d, 0x06);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x7e, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x7f, 0x07);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x80, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x81, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x82, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x83, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x84, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x85, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x86, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x87, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x88, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x89, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x8a, 0x02);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xff, 0x98, 0x81, 0x04);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x6c, 0x15);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x6e, 0x2a);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x6f, 0x33);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x3a, 0x94);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x8d, 0x1a);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x87, 0xba);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x26, 0x76);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xb2, 0xd1);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xb5, 0x06);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xff, 0x98, 0x81, 0x01);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x22, 0x0a);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x31, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x53, 0x81);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x55, 0x8f);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x50, 0xc0);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x51, 0xc0);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x60, 0x08);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xa0, 0x08);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xa1, 0x10);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xa2, 0x25);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xa3, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xa4, 0x24);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xa5, 0x19);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xa6, 0x12);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xa7, 0x1b);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xa8, 0x77);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xa9, 0x19);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xaa, 0x25);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xab, 0x6e);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xac, 0x20);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xad, 0x17);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xae, 0x54);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xaf, 0x24);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xb0, 0x27);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xb1, 0x52);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xb2, 0x63);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xb3, 0x39);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xc0, 0x08);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xc1, 0x20);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xc2, 0x23);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xc3, 0x22);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xc4, 0x06);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xc5, 0x34);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xc6, 0x25);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xc7, 0x20);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xc8, 0x86);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xc9, 0x1f);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xca, 0x2b);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xcb, 0x74);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xcc, 0x16);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xcd, 0x1b);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xce, 0x46);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xcf, 0x21);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xd0, 0x29);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xd1, 0x54);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xd2, 0x65);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xd3, 0x39);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0xff, 0x98, 0x81, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x35, 0x00);
	usleep_range(1000, 2000);

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}

	return 0;
}

static int dijing_ili9881c_off(struct dijing_ili9881c *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display off: %d\n", ret);
		return ret;
	}
	msleep(50);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	return 0;
}

static int dijing_ili9881c_prepare(struct drm_panel *panel)
{
	struct dijing_ili9881c *ctx = to_dijing_ili9881c(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	dijing_ili9881c_reset(ctx);

	ret = dijing_ili9881c_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 0);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int dijing_ili9881c_unprepare(struct drm_panel *panel)
{
	struct dijing_ili9881c *ctx = to_dijing_ili9881c(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = dijing_ili9881c_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 0);

	ctx->prepared = false;
	return 0;
}

static int dijing_ili9881c_enable(struct drm_panel *panel)
{
	struct dijing_ili9881c *ctx = to_dijing_ili9881c(panel);
	int ret;

	if (ctx->enabled)
		return 0;

	ret = backlight_enable(ctx->backlight);
	if (ret < 0) {
		dev_err(&ctx->dsi->dev, "Failed to enable backlight: %d\n", ret);
		return ret;
	}

	ctx->enabled = true;
	return 0;
}

static int dijing_ili9881c_disable(struct drm_panel *panel)
{
	struct dijing_ili9881c *ctx = to_dijing_ili9881c(panel);
	int ret;

	if (!ctx->enabled)
		return 0;

	ret = backlight_disable(ctx->backlight);
	if (ret < 0) {
		dev_err(&ctx->dsi->dev, "Failed to disable backlight: %d\n", ret);
		return ret;
	}

	ctx->enabled = false;
	return 0;
}

static const struct drm_display_mode dijing_ili9881c_mode = {
	.clock = (720 + 150 + 10 + 150) * (1280 + 20 + 2 + 19) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 150,
	.hsync_end = 720 + 150 + 10,
	.htotal = 720 + 150 + 10 + 150,
	.vdisplay = 1280,
	.vsync_start = 1280 + 20,
	.vsync_end = 1280 + 20 + 2,
	.vtotal = 1280 + 20 + 2 + 19,
	.vrefresh = 60,
	.width_mm = 62,
	.height_mm = 110,
};

static int dijing_ili9881c_get_modes(struct drm_panel *panel)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(panel->drm, &dijing_ili9881c_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	panel->connector->display_info.width_mm = mode->width_mm;
	panel->connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(panel->connector, mode);

	return 1;
}

static const struct drm_panel_funcs dijing_ili9881c_panel_funcs = {
	.disable = dijing_ili9881c_disable,
	.unprepare = dijing_ili9881c_unprepare,
	.prepare = dijing_ili9881c_prepare,
	.enable = dijing_ili9881c_enable,
	.get_modes = dijing_ili9881c_get_modes,
};

static int dijing_ili9881c_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct dijing_ili9881c *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(ctx->reset_gpio)) {
		ret = PTR_ERR(ctx->reset_gpio);
		dev_err(dev, "Failed to get reset-gpios: %d\n", ret);
		return ret;
	}

	ctx->backlight = devm_of_find_backlight(dev);
	if (IS_ERR(ctx->backlight)) {
		ret = PTR_ERR(ctx->backlight);
		dev_err(dev, "Failed to get backlight: %d\n", ret);
		return ret;
	}

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_SYNC_PULSE |
			  MIPI_DSI_MODE_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS | MIPI_DSI_MODE_LPM;

	drm_panel_init(&ctx->panel);
	ctx->panel.dev = dev;
	ctx->panel.funcs = &dijing_ili9881c_panel_funcs;

	ret = drm_panel_add(&ctx->panel);
	if (ret < 0) {
		dev_err(dev, "Failed to add panel: %d\n", ret);
		return ret;
	}

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		return ret;
	}

	return 0;
}

static int dijing_ili9881c_remove(struct mipi_dsi_device *dsi)
{
	struct dijing_ili9881c *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);

	return 0;
}

static const struct of_device_id dijing_ili9881c_of_match[] = {
	{ .compatible = "mdss,dijing_ili9881c" }, // FIXME
	{ }
};
MODULE_DEVICE_TABLE(of, dijing_ili9881c_of_match);

static struct mipi_dsi_driver dijing_ili9881c_driver = {
	.probe = dijing_ili9881c_probe,
	.remove = dijing_ili9881c_remove,
	.driver = {
		.name = "panel-dijing-ili9881c",
		.of_match_table = dijing_ili9881c_of_match,
	},
};
module_mipi_dsi_driver(dijing_ili9881c_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>");
MODULE_DESCRIPTION("DRM driver for dijing ILI9881C 720p video mode dsi panel");
MODULE_LICENSE("GPL v2");