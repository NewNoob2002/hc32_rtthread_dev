/*********************
 *      INCLUDES
 *********************/
#include <stdlib.h>
#include "lv_port_disp.h"
/*********************
 *      DEFINES
 *********************/
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
// static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//         const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/
uint8_t *lvgl_txbuffer = NULL;
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
	/*-------------------------
	 * Initialize your display
	 * -----------------------*/
	disp_init();

	/* Example for 1) */
	static lv_disp_draw_buf_t draw_buf_dsc_1;
	static lv_color_t buf_1[MY_DISP_HOR_RES * 140];								/*A buffer for 10 rows*/
	lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 140); /*Initialize the display buffer*/
//	static lv_disp_draw_buf_t draw_buf_dsc_1;
//	static lv_color_t buf_1[126 * 294/2];	
//	static lv_color_t buf_2[126 * 294/2];
//	lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, buf_2, 126 * 294/2); /*Initialize the display buffer*/


	static lv_disp_drv_t disp_drv; /*Descriptor of a display driver*/
	lv_disp_drv_init(&disp_drv);   /*Basic initialization*/

	disp_drv.hor_res = 126;
	disp_drv.ver_res = 294;

	/*Used to copy the buffer's content to the display*/
	disp_drv.flush_cb = disp_flush;

	/*Set a display buffer*/
	disp_drv.draw_buf = &draw_buf_dsc_1;

	disp_drv.sw_rotate = 1;
	disp_drv.rotated = LV_DISP_ROT_90;

	/*Finally register the driver*/
	lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
	  Spi_Config();
    Spi_DmaConfig();
    LCD_Simple_init();
}

// ???????????? num ? data

void LCD_Draw_Color_fill(int x1, int y1, int x2, int y2, lv_color_t *color_p)
{

}

static inline uint16_t LCD_PointConv(const uint32_t point) {
    return ( ((point >> 16 & 0xF8) << 8)   // 高8位分量
           + ((point >> 8 & 0xE0) << 3)    // 中间分量高3位
           + ((point & 0xF8) >> 3)         // 低8位分量高3位
           + ((point >> 8 & 0x1C) << 3) ); // 中间分量低2位，移位后与其他部分无重叠
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready(void)' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
	int w, h;

	w = area->x2 - area->x1 + 1;
	h = area->y2 - area->y1 + 1;

	int total_pixels = w * h;
	uint32_t total_bytes = total_pixels * 2;
	lvgl_txbuffer = (uint8_t *)lv_mem_alloc(total_bytes);

	LCD_SPI_SetDisplayWindow(area->x1, area->x2, area->y1, area->y2);
//	const uint32_t MAX_DMA_BYTES = 65535;
//	uint32_t max_pixels_per_chunk = MAX_DMA_BYTES / 2;

//	uint32_t pixels_sent = 0;

//	while (pixels_sent < total_pixels)
//	{
//		uint32_t chunk_pixels = (total_pixels - pixels_sent > max_pixels_per_chunk) ? max_pixels_per_chunk : (total_pixels - pixels_sent);

		for (uint32_t i = 0; i < total_pixels; i++)
		{
			uint16_t data = LCD_PointConv(color_p->full);
			lvgl_txbuffer[i * 2] = data >> 8;
			lvgl_txbuffer[i * 2 + 1] = data;
			color_p++;
		}

		spi_dma_push(lvgl_txbuffer, total_bytes);
//		pixels_sent += chunk_pixels;
//	}
	/*IMPORTANT!!!
	 *Inform the graphics library that you are ready with the flushing*/
	lv_disp_flush_ready(disp_drv);
	if(lvgl_txbuffer !=NULL)
	{
		lv_mem_free(lvgl_txbuffer);
	}
}
