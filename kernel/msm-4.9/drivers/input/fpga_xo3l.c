/*
 * Copyright (c) 2019 zhangdong
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

static int  XO3lupdate_status = 0;
static int  xo3loffline_status = 0;
static int  xo3lerase_status = 0;

struct fpga_xo3l_dev {
	struct i2c_client *cl;
	//struct mutex lock;
	
};
static struct fpga_xo3l_dev fpga;

struct kobject *xo3l_kobj;
#define xo3l_attr(_name) \
static struct kobj_attribute _name##_attr = {	\
	.attr	= {				\
		.name = __stringify(_name),	\
		.mode = 0660,			\
	},					\
	.show	= _name##_show,			\
	.store	= _name##_store,		\
}

int xo3l_offline_flash(struct i2c_client *client)
{
	u8 data[2];
	int ret;

	data[0] = 0x08;
	data[1] = 0x00;

	ret = i2c_smbus_write_i2c_block_data(client,
					0x74,
					ARRAY_SIZE(data),
					data);//0xc6 enable offline
	return ret;
}

int xo3l_erase_flash(struct i2c_client *client)
{
	u8 data[3];
	int ret;

	data[0] = 0x04;
	data[1] = 0x00;
	data[2] = 0x00;

	ret = i2c_smbus_write_i2c_block_data(client,
					0x0e,
					ARRAY_SIZE(data),
					data);//0x0e erase flash
	return ret;
}

int xo3l_reset_cfg(struct i2c_client *client)
{
	u8 data[3];
	int ret;

	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x00;

	ret = i2c_smbus_write_i2c_block_data(client,
					0x46,
					ARRAY_SIZE(data),
					data);//0x46 reset configuration address location to zero
	return ret;
}

int xo3l_programdone_flash(struct i2c_client *client)
{
	u8 data[3];
	int ret;

	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x00;

	ret = i2c_smbus_write_i2c_block_data(client,
					0x5e,
					ARRAY_SIZE(data),
					data);//0x5e programdone flash
	data[0] = 0xff;
	data[1] = 0xff;
	data[2] = 0xff;

	ret = i2c_smbus_write_i2c_block_data(client,
					0xff,
					ARRAY_SIZE(data),
					data);//0xff programdone flash

	return ret;
}

int xo3l_programexit_flash(struct i2c_client *client)
{
	u8 data1[2];
	u8 data2[3];
	int ret;

	data1[0] = 0x00;
	data1[1] = 0x00;
	ret = i2c_smbus_write_i2c_block_data(client,
					0x26,
					ARRAY_SIZE(data1),
					data1);//0x26 exit program
	data2[0] = 0xff;
	data2[1] = 0xff;
	data2[2] = 0xff;
	ret = i2c_smbus_write_i2c_block_data(client,
						0xff,
						ARRAY_SIZE(data2),
						data2);//0xff exit program

	return ret;
}

int xo3l_IssueRefresh_flash(struct i2c_client *client)
{
	u8 data1[2];
	u8 data2[3];
	int ret;

	data1[0] = 0x00;
	data1[1] = 0x00;
	ret = i2c_smbus_write_i2c_block_data(client,
					0x79,
					ARRAY_SIZE(data1),
					data1);//0x79 Issue Refresh command
	data2[0] = 0xff;
	data2[1] = 0xff;
	data2[2] = 0xff;
	ret = i2c_smbus_write_i2c_block_data(client,
						0xff,
						ARRAY_SIZE(data2),
						data2);//0xff  Issue Refresh command

	return ret;

}

int xo3l_readID(struct i2c_client *client)
{
	u8 data[3];
	int ret;
	//u8 values[4];
	u8 value;
	
	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x00;
		
	ret = i2c_smbus_write_i2c_block_data(client,
					0xe0,
					ARRAY_SIZE(data),
					data);//0xe0 read device ID Code
	value = i2c_smbus_read_byte(client);
	printk("--value:%02x\n",value);
	value = i2c_smbus_read_byte(client);
	printk("--value:%02x\n",value);
	value = i2c_smbus_read_byte(client);
	printk("--value:%02x\n",value);
	//ret = i2c_smbus_read_i2c_block_data(client, 0xe0, 4,  values);
	//printk("-ID CODE-values[0]: %02x, values[1]:%02x, values[2]:%02x, values[3]:%02x\n",values[0],values[1],values[2], values[3]);
	return ret;
}

int xo3l_program_flash(struct i2c_client *client)

{
	int res;
	u8 data[19];
	u8 data1[19];
	u8 data3[19];
	struct fpga_xo3l_dev *xo3l_dev = &fpga;

	

	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x01;
	data[3] = 0x98;
	data[4] = 0x00;
	data[5] = 0x00;
	data[6] = 0x11;
	data[7] = 0x40;
	data[8] = 0x98;
	data[9] = 0x00;
	data[10] = 0x07;
	data[11] = 0x05;
	data[12] = 0x38;
	data[13] = 0x00;
	data[14] = 0x9c;
	data[15] = 0x20;
	data[16] = 0x48;
	data[17] = 0x00;
	data[18] = 0x00;

	res = i2c_smbus_write_i2c_block_data(xo3l_dev->cl,
					0x70,
					ARRAY_SIZE(data),
					data);//0x70 program
	if(res < 0){
			printk("--ZD xo3l program flash1  fail--\n");
		}else{
			printk("--ZD xo3l program flash 1 ok--\n");
		}
	mdelay(1000);

	data1[0] = 0x00;
	data1[1] = 0x00;
	data1[2] = 0x02;
	data1[3] = 0x00;
	data1[4] = 0x04;
	data1[5] = 0x00;
	data1[6] = 0x00;
	data1[7] = 0x00;
	data1[8] = 0xff;
	data1[9] = 0xff;
	data1[10] = 0xff;
	data1[11] = 0xff;
	data1[12] = 0xff;
	data1[13] = 0xff;
	data1[14] = 0xff;
	data1[15] = 0x5e;
	data1[16] = 0x00;
	data1[17] = 0x00;
	data1[18] = 0x00;
	res = i2c_smbus_write_i2c_block_data(xo3l_dev->cl,
					0x70,
					ARRAY_SIZE(data1),
					data1);//0x70 program
	if(res < 0){
			printk("--ZD xo3l program flash 2 fail--\n");
		}else{
			printk("--ZD xo3l program flash 2 ok--\n");
		}
	mdelay(1000);

	data3[0] = 0x00;
	data3[1] = 0x00;
	data3[2] = 0x03;
	data3[3] = 0x02;
	data3[4] = 0x68;
	data3[5] = 0x25;
	data3[6] = 0x9c;
	data3[7] = 0x41;
	data3[8] = 0x2c;
	data3[9] = 0xc4;
	data3[10] = 0x20;
	data3[11] = 0x00;
	data3[12] = 0x00;
	data3[13] = 0x00;
	data3[14] = 0x00;
	data3[15] = 0x00;
	data3[16] = 0x00;
	data3[17] = 0x00;
	data3[18] = 0x00;
	res = i2c_smbus_write_i2c_block_data(xo3l_dev->cl,
					0x70,
					ARRAY_SIZE(data3),
					data3);//0x70 program
	if(res < 0){
			printk("--ZD xo3l program flash  3 fail--\n");
		}else{
			printk("--ZD xo3l program flash  3 ok--\n");
		}
		mdelay(1000);
	
	return res;
}


#if 0
int xo3l_isbusy(struct i2c_client *client)
{
	u8 data[3];
	int ret;
	u8 value;

	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x00;
	
	ret = i2c_smbus_write_i2c_block_data(client,
					0xf0,
					ARRAY_SIZE(data),
					data);//0xf0 check busy flag
	value = i2c_smbus_read_byte(client);
	printk("--value:%02x\n",value);
	return ret;
	
}

int xo3l_issuccess(struct i2c_client *client)
{
	u8 data[3];
	int ret;
	u8 values[4];

	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x00;

	ret = i2c_smbus_write_i2c_block_data(client,
					0x3c,
					ARRAY_SIZE(data),
					data);//0x3c read status register
	ret = i2c_smbus_read_i2c_block_data(client, 0, 4,  values);
	printk("--values[0]: %02x, values[1]:%02x, values[2]:%02x, values[3]:%02x\n",values[0],values[1],values[2], values[3]);
	return ret;


}
#endif
static ssize_t xo3l_offline_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	char* p = buf;

	p += sprintf(p, "no supported\n");
	return p - buf;
}

static ssize_t xo3l_offline_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t n)
{
	int res;
	int count = 0;
	unsigned long mode;
	struct fpga_xo3l_dev *xo3l_dev = &fpga;

	res = kstrtoul(buf, 0, &mode);
	if(mode == 1){
		res = xo3l_offline_flash(xo3l_dev->cl);
		if(res < 0){
			printk("--ZD xo3 offline fail--\n");
			while(res < 0 && count < 3 ){
				mdelay(1);
				res = xo3l_offline_flash(xo3l_dev->cl);
				count++;
				printk("--ZD xo3 offline try :  %d--\n",count);
			}
			count = 0;
			if(res < 0){
				printk("--ZD xo3 offline fail again--\n");
			}else{
				printk("--ZD xo3 offline ok--\n");
				xo3loffline_status = 1;
			}
			
		}else{
			printk("--ZD xo3 offline ok--\n");
			xo3loffline_status = 1;
		}
	}else{
		printk("--input value is error--\n");
	}
	return n;
}

static ssize_t xo3l_erase_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	char* p = buf;

	p += sprintf(p, "no supported\n");
	return p - buf;
}

static ssize_t xo3l_erase_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t n)
{
	int res;
	int count = 0;
	unsigned long mode;
	struct fpga_xo3l_dev *xo3l_dev = &fpga;

	res = kstrtoul(buf, 0, &mode);
	if(mode == 1){
		res = xo3l_erase_flash(xo3l_dev->cl);
		if(res < 0){
			printk("--ZD xo3l erase flash fail--\n");

			while(res < 0 && count < 3 ){
				mdelay(1);
				res = xo3l_erase_flash(xo3l_dev->cl);
				count++;
				printk("--ZD xo3 erase flash try :  %d--\n",count);
			}
			count = 0;
			if(res < 0){
				printk("--ZD xo3 erase flash fail again--\n");
			}else{
				printk("--ZD xo3 erase flash ok--\n");
				xo3lerase_status = 1;
			}
			
		}else{
			printk("--ZD xo3l erase flash ok--\n");
			xo3lerase_status = 1;
		}
	}else{
		printk("--input value is error--\n");
	}
	return n;
}

static ssize_t xo3l_resetcfg_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	char* p = buf;

	p += sprintf(p, "no supported\n");
	return p - buf;
}

static ssize_t xo3l_resetcfg_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t n)
{
	int res;
	int count = 0;
	unsigned long mode;
	struct fpga_xo3l_dev *xo3l_dev = &fpga;

	res = kstrtoul(buf, 0, &mode);
	if(mode == 1){
		res = xo3l_reset_cfg(xo3l_dev->cl);
		if(res < 0){
			printk("--ZD xo3l reset cfg fail--\n");

			while(res < 0 && count < 3 ){
				mdelay(1);
				res = xo3l_reset_cfg(xo3l_dev->cl);
				count++;
				printk("--ZD xo3  reset cfg try :  %d--\n",count);
			}
			count = 0;
			if(res < 0){
				printk("--ZD xo3  reset cfg fail again--\n");
			}else{
				printk("--ZD xo3  reset cfg ok--\n");
			}
			
		}else{
			printk("--ZD xo3l reset cfg ok--\n");
		}
	}else{
		printk("--input value is error--\n");
	}
	return n;
}

static ssize_t xo3l_program_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	char* p = buf;

	p += sprintf(p, "no supported\n");
	return p - buf;
}

static ssize_t xo3l_program_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t n)
{
	int res;
	int count = 0;
	int i;
	unsigned long page_num;
	int values[16];
	u8 data[19];
	static int num =0;
	struct fpga_xo3l_dev *xo3l_dev = &fpga;
	
	//mutex_lock(&xo3l_dev->lock);

	res = sscanf(buf, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %lu", 
		&values[0],  &values[1], &values[2], &values[3], &values[4], &values[5], &values[6], &values[7], 
		&values[8], &values[9], &values[10], &values[11], &values[12], &values[13], &values[14],&values[15] ,&page_num);

	printk("--page_num: %lu\n", page_num);
	
	printk("--num: %d\n", num);
	num++;

	data[0] = 0x00;
	data[1] = ((page_num&0xff00)>>8);
	data[2] = (page_num&0x00ff);
	for(i = 3;i < 19; i++){
		data[i] = values[i-3];
	}

	res = i2c_smbus_write_i2c_block_data(xo3l_dev->cl,
					0x70,
					ARRAY_SIZE(data),
					data);//0x70 program

	if(res < 0){
		printk("--ZD xo3l program flash fail--\n");

		while(res < 0 && count < 3 ){
			mdelay(1);
			res = i2c_smbus_write_i2c_block_data(xo3l_dev->cl,
				0x70,
				ARRAY_SIZE(data),
				data);//0x70 program
			count++;
			printk("--ZD xo3 program try :  %d--\n",count);
		}
		count = 0;
		if(res < 0){
			printk("--ZD xo3 program flash fail again--\n");
			
			for(i=0;i<19;i++){
				printk("--data[%d]: %02x\n", i, data[i]);
			}
		}else{
			printk("--ZD xo3 program flash ok--\n");
		}
			
	}else{
		printk("--ZD xo3l program flash ok--\n");
	}
	//mutex_unlock(&xo3l_dev->lock);
	return n;
}

static ssize_t xo3l_programdone_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	char* p = buf;

	p += sprintf(p, "no supported\n");
	return p - buf;
}

static ssize_t xo3l_programdone_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t n)
{
	int res;
	int count = 0;
	unsigned long mode;
	struct fpga_xo3l_dev *xo3l_dev = &fpga;

	res = kstrtoul(buf, 0, &mode);
	if(mode == 1){
		res = xo3l_programdone_flash(xo3l_dev->cl);
		if(res < 0){
			printk("--ZD xo3l programdone flash fail--\n");

			while(res < 0 && count < 3 ){
				mdelay(1);
				res = xo3l_programdone_flash(xo3l_dev->cl);
				count++;
				printk("--ZD xo3 programdone flash try :  %d--\n",count);
			}
			count = 0;
			if(res < 0){
				printk("--ZD xo3 programdone flash fail again--\n");
			}else{
				printk("--ZD xo3 programdone flash ok--\n");
			}
			
		}else{
			printk("--ZD xo3l programdone flash ok--\n");
		}
	}else{
		printk("--input value is error--\n");
	}
	return n;
}

static ssize_t xo3l_programexit_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	char* p = buf;

	p += sprintf(p, "no supported\n");
	return p - buf;
}

static ssize_t xo3l_programexit_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t n)
{
	int res;
	int count = 0;
	unsigned long mode;
	struct fpga_xo3l_dev *xo3l_dev = &fpga;

	res = kstrtoul(buf, 0, &mode);
	if(mode == 1){
		res = xo3l_programexit_flash(xo3l_dev->cl);
		if(res < 0){
			printk("--ZD xo3l programexit flash fail--\n");

			while(res < 0 && count < 3 ){
				mdelay(1);
				res = xo3l_programexit_flash(xo3l_dev->cl);
				count++;
				printk("--ZD xo3 programexit flash try :  %d--\n",count);
			}
			count = 0;
			if(res < 0){
				printk("--ZD xo3 programexit flash fail again--\n");
			}else{
				printk("--ZD xo3 programexit flash ok--\n");
			}
			
		}else{
			printk("--ZD xo3l programexit flash ok--\n");
		}
	}else{
		printk("--input value is error--\n");
	}
	return n;
}

static ssize_t xo3l_IssueRefresh_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	char* p = buf;

	p += sprintf(p, "no supported\n");
	return p - buf;
}

static ssize_t xo3l_IssueRefresh_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t n)
{
	int res;
	int count = 0;
	unsigned long mode;
	struct fpga_xo3l_dev *xo3l_dev = &fpga;

	res = kstrtoul(buf, 0, &mode);
	if(mode == 1){
		res = xo3l_IssueRefresh_flash(xo3l_dev->cl);
		if(res < 0){
			printk("--ZD xo3l IssueRefresh flash fail--\n");

			while(res < 0 && count < 3 ){
				mdelay(1);
				res = xo3l_IssueRefresh_flash(xo3l_dev->cl);
				count++;
				printk("--ZD xo3 IssueRefresh flash try :  %d--\n",count);
			}
			count = 0;
			if(res < 0){
				printk("--ZD xo3 IssueRefresh flash fail again--\n");
			}else{
				printk("--ZD xo3 IssueRefresh flash ok--\n");
			}
			
		}else{
			printk("--ZD xo3l IssueRefresh flash ok--\n");
		}
	}else{
		printk("--input value is error--\n");
	}
	return n;
}



static ssize_t xo3l_test_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	char* p = buf;

	p += sprintf(p, "no supported\n");
	return p - buf;
}

static ssize_t xo3l_test_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t n)
{
	int res;
	unsigned long mode;
	struct fpga_xo3l_dev *xo3l_dev = &fpga;

	res = kstrtoul(buf, 0, &mode);
	if(mode == 1){
		res = xo3l_offline_flash(xo3l_dev->cl);
		if(res < 0){
			printk("--ZD xo3 offline fail--\n");
		}else{
			printk("--ZD xo3 offline ok--\n");
		}
		mdelay(10000);

	res = xo3l_erase_flash(xo3l_dev->cl);
			if(res < 0){
				printk("--ZD xo3l erase flash fail--\n");
			}else{
				printk("--ZD xo3l erase flash ok--\n");
			}
			mdelay(15000);

	res = xo3l_reset_cfg(xo3l_dev->cl);
			if(res < 0){
				printk("--ZD xo3l reset cfg fail--\n");
			}else{
				printk("--ZD xo3l reset cfg ok--\n");
			}
			mdelay(1000);

	res = xo3l_program_flash(xo3l_dev->cl);
			if(res < 0){
				printk("--ZD xo3l program flash fail--\n");
			}else{
				printk("--ZD xo3l program flash ok--\n");
			}
			mdelay(1000);



	res = xo3l_programdone_flash(xo3l_dev->cl);
			if(res < 0){
				printk("--ZD xo3l programdone flash fail--\n");
			}else{
				printk("--ZD xo3l programdone flash ok--\n");
			}
			mdelay(1000);

	res = xo3l_programexit_flash(xo3l_dev->cl);
			if(res < 0){
				printk("--ZD xo3l programexit flash fail--\n");
			}else{
				printk("--ZD xo3l programexit flash ok--\n");
			}

	mdelay(1000);

	res = xo3l_IssueRefresh_flash(xo3l_dev->cl);
			if(res < 0){
				printk("--ZD xo3l IssueRefresh flash fail--\n");
			}else{
				printk("--ZD xo3l IssueRefresh flash ok--\n");
			}

			mdelay(1000);
	}else{
		printk("--input value is error--\n");
	}
	return n;
}

static ssize_t xo3lupdate_status_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	char* p = buf;
	int status;

	if(xo3loffline_status == 1 && xo3lerase_status == 1){
		XO3lupdate_status = 1;
		status = XO3lupdate_status;
	}else{
		XO3lupdate_status = 0;
		status = XO3lupdate_status;
	}
	printk("------ZD xo3lupdate_status =%d \n",status);
	p +=sprintf(p,  "%d\n",status);
	return p - buf;
}

static ssize_t xo3lupdate_status_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t n)
{
	int res;
	unsigned long mode;

	res = kstrtoul(buf, 0, &mode);
	XO3lupdate_status = mode;

	return n;
}

xo3l_attr(xo3l_offline);
xo3l_attr(xo3l_resetcfg);
xo3l_attr(xo3l_erase);
xo3l_attr(xo3l_program);
xo3l_attr(xo3l_programdone);
xo3l_attr(xo3l_programexit);
xo3l_attr(xo3l_IssueRefresh);
xo3l_attr(xo3l_test);
xo3l_attr(xo3lupdate_status);


static struct attribute *g_attr[] = {
	&xo3l_offline_attr.attr,
	&xo3l_resetcfg_attr.attr,
	&xo3l_erase_attr.attr,
	&xo3l_program_attr.attr,
	&xo3l_programdone_attr.attr,
	&xo3l_programexit_attr.attr,
	&xo3l_IssueRefresh_attr.attr,
	&xo3l_test_attr.attr,
	&xo3lupdate_status_attr.attr,

	NULL,
};

static struct attribute_group xo3l_attr_group = {
	.attrs = g_attr,
};

static int fpga_xo3l_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	int ret;
	struct fpga_xo3l_dev *xo3l_dev = &fpga;

	i2c_set_clientdata(client, xo3l_dev);
	xo3l_dev->cl = client;

	xo3l_kobj = kobject_create_and_add("fpga_xo3l", NULL);
	ret = sysfs_create_group(xo3l_kobj, &xo3l_attr_group);
	xo3l_readID(xo3l_dev->cl);

	printk("=====ZD fpga_xo3l_probe finished =====\n");
	return 0;

}

static int fpga_xo3l_remove(struct i2c_client *client)
{
	return 0;
}

static const struct i2c_device_id fpga_xo3l_id[] = {
	{ "xo3l", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, fpga_xo3l_id);

#ifdef CONFIG_OF
static const struct of_device_id of_fpga_xo3l_match[] = {
	{ .compatible = "fpga_xo3l", },
	{},
};

MODULE_DEVICE_TABLE(of, of_fpga_xo3l_match);
#endif

static struct i2c_driver fpga_xo3l_driver = {
	.driver = {
		.name	= "fpga_xo3l",
		.of_match_table = of_match_ptr(of_fpga_xo3l_match),
	},
	.probe		= fpga_xo3l_probe,
	.remove		= fpga_xo3l_remove,
	.id_table	= fpga_xo3l_id,
};

module_i2c_driver(fpga_xo3l_driver);

MODULE_DESCRIPTION("FPGA XO3R Driver");
MODULE_AUTHOR("LENOVO");
MODULE_LICENSE("GPL");
