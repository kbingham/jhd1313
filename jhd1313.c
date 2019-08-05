// SPDX-License-Identifier: GPL-2.0+

/*
 * JHD1313 I2C Character LCD driver for Linux.
 *
 */

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>

#include <linux/i2c.h>
#include <misc/charlcd.h>

struct jhd1313 {
	struct i2c_client *client;
};

static void jhd1313_backlight(struct charlcd *lcd, int on)
{
	/* MFD to the PCA9633 ? - or link with DT? */
	pr_err("Set backlight %s \n", on ? "on" : "off");
}

static void jhd1313_write_cmd(struct charlcd *lcd, int cmd)
{
	struct jhd1313 *jhd = lcd->drvdata;
	struct i2c_client *client = jhd->client;

	i2c_smbus_write_byte_data(client, 0x00, cmd);

	/* The shortest command takes at least 120 us */
	udelay(120);
}

static void jhd1313_write_data(struct charlcd *lcd, int data)
{
	struct jhd1313 *jhd = lcd->drvdata;
	struct i2c_client *client = jhd->client;

	i2c_smbus_write_byte_data(client, 0x40, data);

	/* The shortest data takes at least 45 us */
	udelay(45);
}

static const struct charlcd_ops jhd1313_ops = {
	.write_cmd	= jhd1313_write_cmd,
	.write_data	= jhd1313_write_data,
	.backlight	= jhd1313_backlight,
};

static int jhd1313_probe(struct i2c_client *client)
{
	struct charlcd *lcd;
	struct jhd1313 *jhd;
	int ret;

	if (!i2c_check_functionality(client->adapter,
				     I2C_FUNC_SMBUS_WRITE_BYTE_DATA)) {
		dev_err(&client->dev, "i2c_check_functionality error\n");
		return -EIO;
	}

	lcd = charlcd_alloc(sizeof(struct jhd1313));
	if (!lcd)
		return -ENOMEM;

	jhd = lcd->drvdata;
	i2c_set_clientdata(client, lcd);
	jhd->client = client;

	lcd->width = 16;
	lcd->height = 2;
	lcd->ifwidth = 8;
	lcd->ops = &jhd1313_ops;

	ret = charlcd_register(lcd);
	if (ret) {
		charlcd_free(lcd);
		dev_err(&client->dev, "Failed to register JHD1313");
	}

	return ret;
}

static int jhd1313_remove(struct i2c_client *client)
{
	struct charlcd *lcd = i2c_get_clientdata(client);

	charlcd_unregister(lcd);
	charlcd_free(lcd);

	return 0;
}

static const struct i2c_device_id jhd1313_id[] = {
	{ "jhd1313", 0 },
	{ },
};
MODULE_DEVICE_TABLE(i2c, jhd1313_id);

static const struct of_device_id jhd1313_of_table[] = {
	{ .compatible = "jhd,jhd1313" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, jhd1313_of_table);

static struct i2c_driver jhd1313_driver = {
	.driver = {
		.name = "jhd1313",
		.of_match_table = jhd1313_of_table,
	},
	.probe_new = jhd1313_probe,
	.remove = jhd1313_remove,
	.id_table = jhd1313_id,
};

module_i2c_driver(jhd1313_driver);

MODULE_DESCRIPTION("JHD1313 I2C Character LCD driver");
MODULE_AUTHOR("Kieran Bingham <kieran.bingham@ideasonboard.com>");
MODULE_LICENSE("GPL");
