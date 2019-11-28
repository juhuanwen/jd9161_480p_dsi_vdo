/* BEGIN PN: , Added by h84013687, 2013.08.13*/

#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
 //   #include <platform/mt_gpio.h>
	
#elif defined(BUILD_UBOOT)
//    #include <asm/arch/mt_gpio.h>
#else
    //#include <linux/delay.h>
//    #include <mach/mt_gpio.h>
#endif
#ifdef BUILD_LK
#define LCD_DEBUG(fmt)  printf(fmt)
#else
#define LCD_DEBUG(fmt)  printk(fmt)
#endif


const static unsigned char LCD_MODULE_ID = 0x09;//ID0->1;ID1->X
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define LCM_DSI_CMD_MODE									0
#define FRAME_WIDTH  										(480)
#define FRAME_HEIGHT 										(800)

#define JD9161_LCM_FOUR_LANE 2

#define REGFLAG_DELAY             								0xFC
#define REGFLAG_END_OF_TABLE      							0xFD   // END OF REGISTERS MARKER

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

const static unsigned int BL_MIN_LEVEL =20;
static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))
// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)										lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   			lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    

struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[128];
};

//update initial param for IC boe_nt35521 0.01
static struct LCM_setting_table lcm_initialization_setting[] = {
{0xBF,3,{0x91,0x61,0xF2}},
{0xB3,2,{0x00,0x7E}},
{0xB4,2,{0x00,0x7E}},
{0xB8,6,{0x00,0xBF,0x01,0x00,0xBF,0x01}},
{0xBA,3,{0x3E,0x23,0x00}},
{0xC3,1,{0x02}},
{0xC4,2,{0x00,0x64}},
{0xC7,9,{0x00,0x01,0x31,0x05,0x65,0x2C,0x13,0xA5,0xA5}},
{0xC8,38,{0x7E,0x6F,0x64,0x59,0x57,0x49,0x4D,0x36,0x4D,0x49,0x47,0x62,0x4E,0x53,0x44,0x40,0x3A,0x1E,0x14,0x7E,0x6F,0x64,0x59,0x57,0x49,0x4D,0x36,0x4D,0x49,0x47,0x62,0x4E,0x53,0x44,0x40,0x3A,0x1E,0x14}},
{0xD4,16,{0x1E,0x1F,0x17,0x37,0x06,0x04,0x0A,0x08,0x00,0x02,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F}},
{0xD5,16,{0x1E,0x1F,0x17,0x37,0x07,0x05,0x0B,0x09,0x01,0x03,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F}},
{0xD6,16,{0x1F,0x1E,0x17,0x17,0x09,0x0B,0x05,0x07,0x03,0x01,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F}},
{0xD7,16,{0x1F,0x1E,0x17,0x17,0x08,0x0A,0x04,0x06,0x02,0x00,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F}},
{0xD8,20,{0x20,0x00,0x00,0x30,0x03,0x30,0x01,0x02,0x00,0x01,0x02,0x06,0x70,0x00,0x00,0x73,0x07,0x06,0x70,0x08}},
{0xD9,19,{0x00,0x0A,0x0A,0x80,0x00,0x00,0x06,0x7b,0x00,0x80,0x00,0x33,0x6A,0x1F,0x00,0x00,0x00,0x03,0x7b}},
{0xBE,1,{0x01}},
{0xC1,1,{0x10}}, 
{0xCC,10,{0x34,0x20,0x38,0x60,0x11,0x91,0x00,0x40,0x00,0x00}},
{0xBE,1,{0x00}},
{0x11,1,{0x00}},
{REGFLAG_DELAY, 120, {}},  
{0x29,1,{0x00}},	
{REGFLAG_DELAY, 20, {}},  
{REGFLAG_END_OF_TABLE, 0x00, {}} 
};
#if 1
static void init_lcm_registers(void){
unsigned int data_array[16];
data_array[0]=0x00043902;
data_array[1]=0xf26191bf;
dsi_set_cmdq(data_array, 2, 1);
data_array[0]=0x00033902;
data_array[1]=0x007e00b3;
dsi_set_cmdq(data_array, 2, 1);
data_array[0]=0x00033902;
data_array[1]=0x007e00b4;
dsi_set_cmdq(data_array, 2, 1);
data_array[0]=0x00073902;
data_array[1]=0x01bf00b8;
data_array[2]=0x0001bf00;
dsi_set_cmdq(data_array, 3, 1);
data_array[0]=0x00043902;
data_array[1]=0x00233eba;
dsi_set_cmdq(data_array, 2, 1);
data_array[0]=0x00023902;
data_array[1]=0x000002c3;
dsi_set_cmdq(data_array, 2, 1);
data_array[0]=0x00033902;
data_array[1]=0x006400c4;
dsi_set_cmdq(data_array, 2, 1);
data_array[0]=0x000a3902;
data_array[1]=0x310100c7;
data_array[2]=0x132c6505;
data_array[3]=0x0000a5a5;
dsi_set_cmdq(data_array, 4, 1);
data_array[0]=0x00273902;
data_array[1]=0x646f7ec8;
data_array[2]=0x4d495759;
data_array[3]=0x47494d36;
data_array[4]=0x44534e62;
data_array[5]=0x141e3a40;
data_array[6]=0x59646f7e;
data_array[7]=0x364d4957;
data_array[8]=0x6247494d;
data_array[9]=0x4044534e;
data_array[10]=0x00141e3a;
dsi_set_cmdq(data_array, 11, 1);
data_array[0]=0x00113902;
data_array[1]=0x171f1ed4;
data_array[2]=0x0a040637;
data_array[3]=0x1f020008;
data_array[4]=0x1f1f1f1f;
data_array[5]=0x0000001f;
dsi_set_cmdq(data_array, 6, 1);
data_array[0]=0x00113902;
data_array[1]=0x171f1ed5;
data_array[2]=0x0b050737;
data_array[3]=0x1f030109;
data_array[4]=0x1f1f1f1f;
data_array[5]=0x0000001f;
dsi_set_cmdq(data_array, 6, 1);
data_array[0]=0x00113902;
data_array[1]=0x171e1fd6;
data_array[2]=0x050b0917;
data_array[3]=0x1f010307;
data_array[4]=0x1f1f1f1f;
data_array[5]=0x0000001f;
dsi_set_cmdq(data_array, 6, 1);
data_array[0]=0x00113902;
data_array[1]=0x171e1fd7;
data_array[2]=0x040a0817;
data_array[3]=0x1f000206;
data_array[4]=0x1f1f1f1f;
data_array[5]=0x0000001f;
dsi_set_cmdq(data_array, 6, 1);
data_array[0]=0x00153902;
data_array[1]=0x000020d8;
data_array[2]=0x01300330;
data_array[3]=0x02010002;
data_array[4]=0x00007006;
data_array[5]=0x70060773;
data_array[6]=0x00000008;
dsi_set_cmdq(data_array, 7, 1);
data_array[0]=0x00143902;
data_array[1]=0x0a0a00d9;
data_array[2]=0x06000080;
data_array[3]=0x0080007b;
data_array[4]=0x001f6a33;
data_array[5]=0x7b030000;
dsi_set_cmdq(data_array, 6, 1);
data_array[0]=0x00023902;
data_array[1]=0x000001be;
dsi_set_cmdq(data_array, 2, 1);
data_array[0]=0x00023902;
data_array[1]=0x000010c1;
dsi_set_cmdq(data_array, 2, 1);
data_array[0]=0x000b3902;
data_array[1]=0x382034cc;
data_array[2]=0x00911160;
data_array[3]=0x00000040;
dsi_set_cmdq(data_array, 4, 1);
data_array[0]=0x00023902;
data_array[1]=0x000000be;
dsi_set_cmdq(data_array, 2, 1);
data_array[0]=0x00023902;
data_array[1]=0x00000011;
dsi_set_cmdq(data_array, 2, 1);
MDELAY(120);
data_array[0]=0x00023902;
data_array[1]=0x00000029;
dsi_set_cmdq(data_array, 2, 1);
MDELAY(20);

#if 0
data_array[0]=0x00023902;
data_array[1]=0x000000be;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00110500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(500);
	
data_array[0]= 0x00290500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(200);  
#endif
}
#endif

static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {

    {0xFF, 3, {0x98,0x81,0x00}},	
    // Display off sequence
    {0x28, 1, {0x00}},
    {REGFLAG_DELAY, 20, {}},

    // Sleep Mode On
    {0x10, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for(i = 0; i < count; i++)
    {
        unsigned cmd;
        cmd = table[i].cmd;

        switch (cmd) {
		case REGFLAG_DELAY:
			MDELAY(table[i].count);
			break;

            case REGFLAG_END_OF_TABLE :
                break;

            default:
                dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
        }
    }
}

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));
	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;
	// enable tearing-free
	params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
	params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode   = CMD_MODE;
#else
	params->dsi.mode   =SYNC_PULSE_VDO_MODE; //SYNC_PULSE_VDO_MODE; //BURST_VDO_MODE;
	//params->dsi.mode   = BURST_VDO_MODE;
#endif

	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM				= LCM_TWO_LANE;
	params->dsi.data_format.format      		= LCM_DSI_FORMAT_RGB888;

	//video mode timing
    params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

params->dsi.vertical_sync_active				= 2; //8;	//2;
params->dsi.vertical_backporch					= 12; //18;	//14;
params->dsi.vertical_frontporch					= 6; //20;	//16;
params->dsi.vertical_active_line				= FRAME_HEIGHT;

params->dsi.horizontal_sync_active				= 8;	//2;
params->dsi.horizontal_backporch				= 8;//120;	//60;	//42;
params->dsi.horizontal_frontporch				= 8;//100;	//60;	//44;
params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

params->dsi.PLL_CLOCK = 160;//208; 210	

}

void lcm_display_on_jd9161(void)
{
	unsigned int array[16];
#if 1
	array[0]=0x00023902;      
	array[1]=0x000000be;          
	dsi_set_cmdq(array, 2, 1);       
	MDELAY(10); 
	  
	array[0]= 0x00110500;
	dsi_set_cmdq(array, 1, 1);
	MDELAY(200);
	
	array[0]= 0x00290500;
	dsi_set_cmdq(array, 1, 1);
	MDELAY(10);   
#else
	array[0]=0x00023902;
	array[1]=0x000000be;
	dsi_set_cmdq(array, 2, 1);
	array[0]=0x00023902;
	array[1]=0x00000011;
	array(array, 2, 1);
	MDELAY(120);
	array[0]=0x00023902;
	array[1]=0x00000029;
	dsi_set_cmdq(array, 2, 1);
	MDELAY(20);
#endif
}

/*to prevent electric leakage*/
static void lcm_init(void)
{
	
 
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);
    //push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
	init_lcm_registers();
	lcm_display_on_jd9161();
    LCD_DEBUG("uboot:boe_nt35521_lcm_init\n");
}


static void lcm_suspend(void)
{

    push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);

	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(120);
    LCD_DEBUG("kernel:boe_nt35521_lcm_suspend\n");
}

static void lcm_resume(void)
{

	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);
    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);


    LCD_DEBUG("kernel:boe_nt35521_lcm_resume\n");
}



static unsigned int lcm_compare_id(void)
{
	return 1;
} 

LCM_DRIVER jd9161_480p_lcm_drv = 
{
    .name			="jd9161_480p_lcm_drv",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id    = lcm_compare_id,
};

