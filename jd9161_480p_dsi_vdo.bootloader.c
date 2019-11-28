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
	{0xFF,5,{0xFF,0x98,0x06,0x04,0x01}},// Change to Page 1
		{REGFLAG_DELAY, 20, {}}, 
		{0x08,1,{0x10}},// output SDA
		{REGFLAG_DELAY, 20, {}}, 
		{0x21,1,{0x01}},// DE = 1 Active
		{REGFLAG_DELAY, 20, {}}, 
		{0x30,1,{0x02}},// 480 X 800
		{REGFLAG_DELAY, 20, {}}, 
		{0x31,1,{0x02}},// Column Inversion
		{REGFLAG_DELAY, 20, {}}, 
		{0x40,1,{0x10}}, // 2.5VCI/-2VCL  15
		{REGFLAG_DELAY, 20, {}}, 
		{0x41,1,{0x33}}, // 6/-6 44  55
		{REGFLAG_DELAY, 20, {}}, 
		{0x42,1,{0x03}},// DDVDH+VCI-VCL/DDVDL-DDVDH 
		{REGFLAG_DELAY, 20, {}}, 
		{0x43,1,{0x09}},// VGH_CLAMP 0FF ;
		{REGFLAG_DELAY, 20, {}}, 
		{0x44,1,{0x07}},// VGL_CLAMP ON ; 
		{REGFLAG_DELAY, 20, {}}, 
		{0x50,1,{0x78}},//50
		{REGFLAG_DELAY, 20, {}}, 
		{0x51,1,{0x78}},//50
		{REGFLAG_DELAY, 20, {}}, 
		{0x52,1,{0x00}},//Flicker
		{REGFLAG_DELAY, 20, {}}, 
		{0x53,1,{0x73}},//Flicker6D  
		{REGFLAG_DELAY, 20, {}}, 
		{0x57,1,{0x50}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x60,1,{0x07}}, // SDTI
		{REGFLAG_DELAY, 20, {}}, 
		{0x61,1,{0x00}},// CRTI
		{REGFLAG_DELAY, 20, {}}, 
		{0x62,1,{0x08}},// EQTI
		{REGFLAG_DELAY, 20, {}}, 
		{0x63,1,{0x00}}, // PCTI
		{REGFLAG_DELAY, 20, {}}, 
		//++++++++++++++++++ Gamma Setting ++++++++++++++++++//
		{0xA0,1,{0x00}},// Gamma 255 
		{REGFLAG_DELAY, 20, {}}, 
		{0xA1,1,{0x07}},// Gamma 251 
		{REGFLAG_DELAY, 20, {}}, 
		{0xA2,1,{0x0D}}, // Gamma 247
		{REGFLAG_DELAY, 20, {}}, 
		{0xA3,1,{0x0C}},// Gamma 239
		{REGFLAG_DELAY, 20, {}}, 
		{0xA4,1,{0x04}},// Gamma 231
		{REGFLAG_DELAY, 20, {}}, 
		{0xA5,1,{0x07}},// Gamma 203
		{REGFLAG_DELAY, 20, {}}, 
		{0xA6,1,{0x06}},// Gamma 175
		{REGFLAG_DELAY, 20, {}}, 
		{0xA7,1,{0x04}},// Gamma 147
		{REGFLAG_DELAY, 20, {}}, 
		{0xA8,1,{0x09}},// Gamma 108
		{REGFLAG_DELAY, 20, {}}, 
		{0xA9,1,{0x0C}},// Gamma 80
		{REGFLAG_DELAY, 20, {}}, 
		{0xAA,1,{0x14}},// Gamma 52
		{REGFLAG_DELAY, 20, {}}, 
		{0xAB,1,{0x08}},// Gamma 24
		{REGFLAG_DELAY, 20, {}}, 
		{0xAC,1,{0x0E}},// Gamma 16
		{REGFLAG_DELAY, 20, {}}, 
		{0xAD,1,{0x17}},// Gamma 8
		{REGFLAG_DELAY, 20, {}}, 
		{0xAE,1,{0x0E}},// Gamma 4  
		{REGFLAG_DELAY, 20, {}}, 
		{0xAF,1,{0x00}},// Gamma 0
		{REGFLAG_DELAY, 20, {}}, 
		
		///==============Nagitive

		{0xC0,1,{0x00}},// Gamma 255 
		{REGFLAG_DELAY, 20, {}}, 
		{0xC1,1,{0x06}},// Gamma 251
		{REGFLAG_DELAY, 20, {}}, 
		{0xC2,1,{0x0C}},// Gamma 247
		{REGFLAG_DELAY, 20, {}}, 
		{0xC3,1,{0x0C}},// Gamma 239
		{REGFLAG_DELAY, 20, {}}, 
		{0xC4,1,{0x04}},// Gamma 231
		{REGFLAG_DELAY, 20, {}}, 
		{0xC5,1,{0x07}}, // Gamma 203
		{REGFLAG_DELAY, 20, {}}, 
		{0xC6,1,{0x06}},// Gamma 175
		{REGFLAG_DELAY, 20, {}}, 
		{0xC7,1,{0x04}}, // Gamma 147
		{REGFLAG_DELAY, 20, {}}, 
		{0xC8,1,{0x08}},// Gamma 108
		{REGFLAG_DELAY, 20, {}}, 
		{0xC9,1,{0x0C}},// Gamma 80
		{REGFLAG_DELAY, 20, {}}, 
		{0xCA,1,{0x13}},// Gamma 52
		{REGFLAG_DELAY, 20, {}}, 
		{0xCB,1,{0x08}},// Gamma 24
		{REGFLAG_DELAY, 20, {}}, 
		{0xCC,1,{0x0E}},// Gamma 16
		{REGFLAG_DELAY, 20, {}}, 
		{0xCD,1,{0x18}},// Gamma 8
		{REGFLAG_DELAY, 20, {}}, 
		{0xCE,1,{0x0E}},// Gamma 4
		{REGFLAG_DELAY, 20, {}}, 
		{0xCF,1,{0x00}}, // Gamma 0
		{REGFLAG_DELAY, 20, {}}, 
//****************************************************************************//
//****************************** Page 6 Command ******************************//
//****************************************************************************//

		{0xFF,5,{0xFF,0x98,0x06,0x04,0x06}},// Change to Page 6
		{REGFLAG_DELAY, 20, {}}, 
		{0x00,1,{0x21}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x01,1,{0x0A}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x02,1,{0x00}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x03,1,{0x00}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x04,1,{0x01}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x05,1,{0x01}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x06,1,{0x80}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x07,1,{0x06}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x08,1,{0x01}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x09,1,{0x80}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x0A,1,{0x00}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x0B,1,{0x00}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x0C,1,{0x0A}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x0D,1,{0x0A}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x0E,1,{0x00}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x0F,1,{0x00}},
		{REGFLAG_DELAY, 20, {}}, 
		
		{0x10,1,{0xF0}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x11,1,{0xF4}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x12,1,{0x04}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x13,1,{0x00}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x14,1,{0x00}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x15,1,{0xC0}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x16,1,{0x08}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x17,1,{0x00}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x18,1,{0x00}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x19,1,{0x00}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x1A,1,{0x00}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x1B,1,{0x00}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x1C,1,{0x00}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x1D,1,{0x00}},
		{REGFLAG_DELAY, 20, {}}, 
		
		{0x20,1,{0x01}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x21,1,{0x23}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x22,1,{0x45}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x23,1,{0x67}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x24,1,{0x01}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x25,1,{0x23}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x26,1,{0x45}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x27,1,{0x67}},
		{REGFLAG_DELAY, 20, {}}, 
		
		{0x30,1,{0x01}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x31,1,{0x11}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x32,1,{0x00}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x33,1,{0xEE}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x34,1,{0xFF}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x35,1,{0xCB}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x36,1,{0xDA}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x37,1,{0xAD}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x38,1,{0xBC}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x39,1,{0x76}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x3A,1,{0x67}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x3B,1,{0x22}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x3C,1,{0x22}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x3D,1,{0x22}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x3E,1,{0x22}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x3F,1,{0x22}},
		{REGFLAG_DELAY, 20, {}}, 
		
		{0x40,1,{0x22}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x52,1,{0x10}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x53,1,{0x10}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x54,1,{0x13}},
		{REGFLAG_DELAY, 20, {}}, 
//****************************************************************************//
//****************************** Page 7 Command ******************************//
//****************************************************************************//		
		
		{0xFF,5,{0xFF,0x98,0x06,0x04,0x07}},// Change to Page 7
		{REGFLAG_DELAY, 20, {}}, 
		{0xB3,1,{0x10}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x17,1,{0x22}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x02,1,{0x77}},
		{REGFLAG_DELAY, 20, {}}, 
		{0x26,1,{0xB2}},
		{REGFLAG_DELAY, 20, {}}, 
		{0xE1,1,{0x79}},
		{REGFLAG_DELAY, 20, {}}, 
//****************************************************************************//

		{0xFF,5,{0xFF,0x98,0x06,0x04,0x00}},// Change to Page 0
		{REGFLAG_DELAY, 20, {}}, 
		{0x35,1,{0x00}},
		{0x11,1,{0x00}},// Sleep-Out
		{REGFLAG_DELAY, 120, {}},  
		{0x29,1,{0x00}},	// Display on
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

params->dsi.PLL_CLOCK = 130;// 160 208; 210	


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
    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
	//init_lcm_registers();
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

