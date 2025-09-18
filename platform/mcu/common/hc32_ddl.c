#include "hc32_ddl.h"

/**
 * @defgroup LL_Global_Functions LL Global Functions
 * @{
 */
void LL_PERIPH_WE(uint32_t u32Peripheral)
{
#if (DDL_EFM_ENABLE == DDL_ON)
    if ((u32Peripheral & LL_PERIPH_EFM) != 0UL) {
        /* Unlock all EFM registers */
        EFM_Unlock();
    }
#endif
#if (DDL_FCG_ENABLE == DDL_ON)
    if ((u32Peripheral & LL_PERIPH_FCG) != 0UL) {
        /* Unlock FCG register */
        PWC_FCG0_REG_Unlock();
    }
#endif
#if (DDL_GPIO_ENABLE == DDL_ON)
    if ((u32Peripheral & LL_PERIPH_GPIO) != 0UL) {
        /* Unlock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
        PORT_Unlock();
    }
#endif
#if (DDL_MPU_ENABLE == DDL_ON)
    if ((u32Peripheral & LL_PERIPH_MPU) != 0UL) {
        /* Unlock all MPU registers */
        MPU_REG_Unlock();
    }
#endif
//#if (DDL_PWC_ENABLE == DDL_ON)
//    if ((u32Peripheral & LL_PERIPH_LVD) != 0UL) {
//        /* Unlock LVD registers, @ref PWC_REG_Write_Unlock_Code for details */
//        ENABLE_PVD_REG_WRITE();
//    }
//#endif
//#if (DDL_PWC_ENABLE == DDL_ON)
//    if ((u32Peripheral & LL_PERIPH_PWC_CLK_RMU) != 0UL) {
//        /* Unlock PWC, CLK, RMU registers, @ref PWC_REG_Write_Unlock_Code for details */
//        PWC_REG_Unlock(PWC_UNLOCK_CODE0 | PWC_UNLOCK_CODE1);
//    }
//#endif
#if (DDL_SRAM_ENABLE == DDL_ON)
    if ((u32Peripheral & LL_PERIPH_SRAM) != 0UL) {
        /* Unlock SRAM register: WTCR, CKCR */
				SRAM_WT_Enable();
				SRAM_CK_Enable();
    }
#endif
}

void LL_PERIPH_WP(uint32_t u32Peripheral)
{
#if (DDL_EFM_ENABLE == DDL_ON)
    if ((u32Peripheral & LL_PERIPH_EFM) != 0UL) {
        /* Lock all EFM registers */
        EFM_Lock();
    }
#endif
#if (DDL_FCG_ENABLE == DDL_ON)
    if ((u32Peripheral & LL_PERIPH_FCG) != 0UL) {
        /* Lock FCG register */
        PWC_FCG0_REG_Lock();
    }
#endif
#if (DDL_GPIO_ENABLE == DDL_ON)
    if ((u32Peripheral & LL_PERIPH_GPIO) != 0UL) {
        /* Unlock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
        PORT_Lock();
    }
#endif
#if (DDL_MPU_ENABLE == DDL_ON)
    if ((u32Peripheral & LL_PERIPH_MPU) != 0UL) {
        /* Lock all MPU registers */
        MPU_REG_Lock();
    }
#endif
#if (LL_PWC_ENABLE == DDL_ON)
    if ((u32Peripheral & LL_PERIPH_LVD) != 0UL) {
        /* Lock LVD registers, @ref PWC_REG_Write_Unlock_Code for details */
        PWC_REG_Lock(PWC_UNLOCK_CODE2);
    }
#endif
#if (LL_PWC_ENABLE == DDL_ON)
    if ((u32Peripheral & LL_PERIPH_PWC_CLK_RMU) != 0UL) {
        /* Lock PWC, CLK, RMU registers, @ref PWC_REG_Write_Unlock_Code for details */
        PWC_REG_Lock(PWC_UNLOCK_CODE0 | PWC_UNLOCK_CODE1);
    }
#endif
#if (DDL_SRAM_ENABLE == DDL_ON)
    if ((u32Peripheral & LL_PERIPH_SRAM) != 0UL) {
        /* Lock SRAM register: WTCR, CKCR */
				SRAM_WT_Disable();
				SRAM_CK_Disable();
    }
#endif
}