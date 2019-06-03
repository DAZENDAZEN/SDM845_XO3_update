/*
 * Copyright (c) 2018 Simon
 */

#include <linux/delay.h>
#include <linux/firmware.h>
#include <linux/i2c.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/slab.h>
#include<linux/gpio.h>

enum reg_address {
    box_reg_index = 0x1,
    box_reg_date  = 0x2,
    box_reg_month = 0x3,
    box_reg_year  = 0x4,
    box_reg_id    = 0x5,
};

struct fpga_box_dev {
	int value;
	int reg;
	int id;
	int index;
	int date;
	int month;
	int year;
	char *version;
	struct i2c_client *cl;
};
static struct fpga_box_dev fpga;

struct kobject *box_kobj;
#define box_attr(_name) \
static struct kobj_attribute _name##_attr = {	\
	.attr	= {				\
		.name = __stringify(_name),	\
		.mode = 0660,			\
	},					\
	.show	= _name##_show,			\
	.store	= _name##_store,		\
}

static int box_read(struct i2c_client *client, u8 reg, u8 *val)
{
	s32 ret;

	ret = i2c_smbus_read_byte_data(client, reg);
	if (ret < 0)
		return ret;

	*val = ret;
	return 0;
}

int box_write(struct i2c_client *client, u8 reg, u8 val)
{
	return i2c_smbus_write_byte_data(client, reg, val);
}

static ssize_t value_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	char* p = buf;
	u8 exec;
	int ret;
	struct fpga_box_dev *box_dev = &fpga;

	ret = box_read(box_dev->cl, box_dev->reg, &exec);
	if (ret)
		return -1;

	box_dev->value = exec;
	p += sprintf(p, "BOX version is 0x%x\n",box_dev->value);

	return p - buf;
}

static ssize_t value_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t n)
{
	int value;
	int res;
	struct fpga_box_dev *box_dev = &fpga;

	res = sscanf(buf, "0x%x", &value);
	box_write(box_dev->cl, box_dev->reg, value);
	printk("--->Simon: res=%d. [box reg value=0x%x]\n",res, value);

	return n;
}

static ssize_t reg_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	char* p = buf;
	struct fpga_box_dev *box_dev = &fpga;

	p += sprintf(p, "BOX reg is 0x%x\n",box_dev->reg);

	return p - buf;
}

static ssize_t reg_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t n)
{
	int reg;
	int res;
	struct fpga_box_dev *box_dev = &fpga;

	res = sscanf(buf, "0x%x", &reg);
	box_dev->reg = reg;
	printk("--->Simon: res=%d. [BOX reg=0x%x]\n",res, box_dev->reg);

	return n;
}

static ssize_t version_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	char* p = buf;
	u8 exec;
	int ret;
	char a[10];
	char ver[50] = "";
	struct fpga_box_dev *box_dev = &fpga;

	ret = box_read(box_dev->cl, box_reg_year, &exec);
	if (ret)
		return -1;
	box_dev->year = exec;
	sprintf(a,"year:0x%x\n",box_dev->year);
	strcat(ver, a);

	ret = box_read(box_dev->cl, box_reg_month, &exec);
	if (ret)
		return -1;
	box_dev->month = exec;
	sprintf(a,"month:0x%x\n",box_dev->month);
	strcat(ver, a);

	ret = box_read(box_dev->cl, box_reg_date, &exec);
	if (ret)
		return -1;
	box_dev->date = exec;
	sprintf(a,"date:0x%x\n",box_dev->date);
	strcat(ver, a);

	ret = box_read(box_dev->cl, box_reg_index, &exec);
	if (ret)
		return -1;
	box_dev->index = exec;
	sprintf(a,"index:0x%x",box_dev->index);
	strcat(ver, a);

	box_dev->version = ver;

	p += sprintf(p, "%s\n",box_dev->version);

	return p - buf;
}

static ssize_t version_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t n)
{
	printk("--->Version store is not supported!\n");

	return n;
}

static ssize_t id_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	char* p = buf;
	u8 exec;
	int ret;
	struct fpga_box_dev *box_dev = &fpga;

	ret = box_read(box_dev->cl, box_reg_id, &exec);
	if (ret)
		return -1;

	box_dev->id = exec;
	p += sprintf(p, "BOX ID is 0x%x\n",box_dev->id);

	return p - buf;
}

static ssize_t id_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t n)
{
	int value;
	int res;
	struct fpga_box_dev *box_dev = &fpga;

	res = sscanf(buf, "0x%x", &value);
	box_write(box_dev->cl, box_reg_id, value);
	printk("--->Simon: res=%d. [box id=0x%x]\n",res, value);

	return n;
}


box_attr(value);
box_attr(reg);
box_attr(version);
box_attr(id);


static struct attribute *g_attr[] = {
	&value_attr.attr,
	&reg_attr.attr,
	&version_attr.attr,
	&id_attr.attr,
	NULL,
};

static struct attribute_group box_attr_group = {
	.attrs = g_attr,
};


static int fpga_box_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	int ret;
	struct fpga_box_dev *box_dev = &fpga;

	i2c_set_clientdata(client, box_dev);
	box_dev->cl = client;

	box_kobj = kobject_create_and_add("fpga_box", NULL);
	ret = sysfs_create_group(box_kobj, &box_attr_group);

	printk("[BOX]===== G2. fpga_box_probe finished =====\n");
	return 0;

}

static int fpga_box_remove(struct i2c_client *client)
{
	return 0;
}

static const struct i2c_device_id fpga_box_id[] = {
	{ "box", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, fpga_box_id);

#ifdef CONFIG_OF
static const struct of_device_id of_fpga_box_match[] = {
	{ .compatible = "fpga_box", },
	{},
};

MODULE_DEVICE_TABLE(of, of_fpga_box_match);
#endif

static struct i2c_driver fpga_box_driver = {
	.driver = {
		.name	= "fpga_box",
		.of_match_table = of_match_ptr(of_fpga_box_match),
	},
	.probe		= fpga_box_probe,
	.remove		= fpga_box_remove,
	.id_table	= fpga_box_id,
};

module_i2c_driver(fpga_box_driver);

MODULE_DESCRIPTION("FPGA BOX Driver");
MODULE_AUTHOR("LENOVO");
MODULE_LICENSE("GPL");
