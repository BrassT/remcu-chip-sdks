/**
 * \file
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "sam4cm.h"

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/* External oscillator definition, to be overriden by application */
#define CHIP_FREQ_XTAL_8M (8000000UL)

#if (!defined CHIP_FREQ_XTAL)
#  define CHIP_FREQ_XTAL CHIP_FREQ_XTAL_8M
#endif

/* Clock Settings (4MHz) using Internal Fast RC */
uint32_t SystemCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;

/**
 * \brief Setup the microcontroller system.
 *
 * Initialize the System and update the SystemFrequency variable.
 */
void SystemInit( void )
{
  /*
   * TODO:
   * Add code to initialize the system according to your application.
   *
   * For SAM4CM, the internal 4MHz fast RC oscillator is the default clock
   * selected at system reset state.
   *
   * Note:
   * After reset, the core 1 is hold in reset and with no clock.
   */

  /* Set FWS according to default clock configuration */
  EFC->EEFC_FMR = EEFC_FMR_FWS(1);
}

/**
 * \brief Get Core Clock Frequency.
 */
void SystemCoreClockUpdate( void )
{
  /* Determine clock frequency according to clock register values */
  switch ( PMC->PMC_MCKR & (uint32_t) PMC_MCKR_CSS_Msk )
  {
    case PMC_MCKR_CSS_SLOW_CLK: /* Slow clock */
      if ( SUPC->SUPC_SR & SUPC_SR_OSCSEL )
      {
        SystemCoreClock = CHIP_FREQ_XTAL_32K;
      }
      else
      {
        SystemCoreClock = CHIP_FREQ_SLCK_RC;
      }
    break;

    case PMC_MCKR_CSS_MAIN_CLK: /* Main clock */
      if ( PMC->CKGR_MOR & CKGR_MOR_MOSCSEL )
      {
        SystemCoreClock = CHIP_FREQ_XTAL;
      }
      else
      {
        SystemCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;

        switch ( PMC->CKGR_MOR & CKGR_MOR_MOSCRCF_Msk )
        {
          case CKGR_MOR_MOSCRCF_4_MHz:
            SystemCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;
          break;

          case CKGR_MOR_MOSCRCF_8_MHz:
            SystemCoreClock = CHIP_FREQ_MAINCK_RC_8MHZ;
          break;

          case CKGR_MOR_MOSCRCF_12_MHz:
            SystemCoreClock = CHIP_FREQ_MAINCK_RC_12MHZ;
          break;

          default:
            // TODO: We are out of specs.
          break;
        }
      }
    break;

    case PMC_MCKR_CSS_PLLA_CLK: /* PLLA clock */
      SystemCoreClock = CHIP_FREQ_PLLA_TYPICAL;
    break;

    case PMC_MCKR_CSS_PLLB_CLK: /* PLLB clock */
      if ( PMC->CKGR_MOR & CKGR_MOR_MOSCSEL )
      {
        SystemCoreClock = CHIP_FREQ_XTAL;
      }
      else
      {
        SystemCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;

        switch ( PMC->CKGR_MOR & CKGR_MOR_MOSCRCF_Msk )
        {
          case CKGR_MOR_MOSCRCF_4_MHz:
            SystemCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;
          break;

          case CKGR_MOR_MOSCRCF_8_MHz:
            SystemCoreClock = CHIP_FREQ_MAINCK_RC_8MHZ;
          break;

          case CKGR_MOR_MOSCRCF_12_MHz:
            SystemCoreClock = CHIP_FREQ_MAINCK_RC_12MHZ;
          break;

          default:
          break;
        }
      }

      SystemCoreClock *= ((((PMC->CKGR_PLLBR) & CKGR_PLLBR_MULB_Msk) >>	CKGR_PLLBR_MULB_Pos) + 1U);
      SystemCoreClock /= ((((PMC->CKGR_PLLBR) & CKGR_PLLBR_DIVB_Msk) >>	CKGR_PLLBR_DIVB_Pos));
    break;

    default:
      // TODO: We are out of specs.
    break;
  }

  if ( (PMC->PMC_MCKR & PMC_MCKR_PRES_Msk) == PMC_MCKR_PRES_CLK_3 )
  {
    SystemCoreClock /= 3U;
  }
  else
  {
    SystemCoreClock >>= ((PMC->PMC_MCKR & PMC_MCKR_PRES_Msk) >> PMC_MCKR_PRES_Pos);
  }
}

/**
 * \brief Initialize flash wait state according to operating frequency.
 *
 * \param ul_clk System clock frequency.
 */
void system_init_flash( uint32_t ul_clk )
{
  /* Set FWS for embedded Flash access according to operating frequency */
  if ( ul_clk < CHIP_FREQ_FWS_0 )
  {
    EFC->EEFC_FMR = EEFC_FMR_FWS(0);
  }
  else
  {
    if ( ul_clk < CHIP_FREQ_FWS_1 )
    {
      EFC->EEFC_FMR = EEFC_FMR_FWS(1);
    }
    else
    {
      if ( ul_clk < CHIP_FREQ_FWS_2 )
      {
        EFC->EEFC_FMR = EEFC_FMR_FWS(2);
      }
      else
      {
        if ( ul_clk < CHIP_FREQ_FWS_3 )
        {
          EFC->EEFC_FMR = EEFC_FMR_FWS(3);
        }
        else
        {
          if ( ul_clk < CHIP_FREQ_FWS_4 )
          {
            EFC->EEFC_FMR = EEFC_FMR_FWS(4);
          }
          else
          {
            EFC->EEFC_FMR = EEFC_FMR_FWS(5);
          }
        }
      }
    }
  }
}

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */