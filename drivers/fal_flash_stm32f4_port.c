#include <fal.h>

#include <stm32f4xx.h>
#include <stm32f4xx_hal_flash.h>
#include <stm32f4xx_hal_flash_ex.h>

#define STM32_FLASH_START_ADRESS     ((uint32_t)0x08000000)
#define STM32_FLASH_SIZE             (256 * 1024)
#define STM32_FLASH_END_ADDRESS      ((uint32_t)(STM32_FLASH_START_ADRESS + STM32_FLASH_SIZE))

/* base address of the flash sectors */
#define ADDR_FLASH_SECTOR_0      ((uint32_t)0x08000000) /* Base address of Sector 0, 16 K bytes   */
#define ADDR_FLASH_SECTOR_1      ((uint32_t)0x08004000) /* Base address of Sector 1, 16 K bytes   */
#define ADDR_FLASH_SECTOR_2      ((uint32_t)0x08008000) /* Base address of Sector 2, 16 K bytes   */
#define ADDR_FLASH_SECTOR_3      ((uint32_t)0x0800C000) /* Base address of Sector 3, 16 K bytes   */
#define ADDR_FLASH_SECTOR_4      ((uint32_t)0x08010000) /* Base address of Sector 4, 64 K bytes   */
#define ADDR_FLASH_SECTOR_5      ((uint32_t)0x08020000) /* Base address of Sector 5, 128 K bytes  */
#define ADDR_FLASH_SECTOR_6      ((uint32_t)0x08040000) /* Base address of Sector 6, 128 K bytes  */
#define ADDR_FLASH_SECTOR_7      ((uint32_t)0x08060000) /* Base address of Sector 7, 128 K bytes  */
#define ADDR_FLASH_SECTOR_8      ((uint32_t)0x08080000) /* Base address of Sector 8, 128 K bytes  */
#define ADDR_FLASH_SECTOR_9      ((uint32_t)0x080A0000) /* Base address of Sector 9, 128 K bytes  */
#define ADDR_FLASH_SECTOR_10     ((uint32_t)0x080C0000) /* Base address of Sector 10, 128 K bytes */
#define ADDR_FLASH_SECTOR_11     ((uint32_t)0x080E0000) /* Base address of Sector 11, 128 K bytes */

typedef unsigned long long              rt_uint64_t;    /**< 64bit unsigned integer type */

/**
 * Get the sector of a given address
 *
 * @param address flash address
 *
 * @return The sector of a given address
 */
static uint32_t GetSector(uint32_t address)
{
    uint32_t sector = 0;

    if ((address < ADDR_FLASH_SECTOR_1) && (address >= ADDR_FLASH_SECTOR_0))
    {
        sector = FLASH_SECTOR_0;
    }
    else if ((address < ADDR_FLASH_SECTOR_2) && (address >= ADDR_FLASH_SECTOR_1))
    {
        sector = FLASH_SECTOR_1;
    }
    else if ((address < ADDR_FLASH_SECTOR_3) && (address >= ADDR_FLASH_SECTOR_2))
    {
        sector = FLASH_SECTOR_2;
    }
    else if ((address < ADDR_FLASH_SECTOR_4) && (address >= ADDR_FLASH_SECTOR_3))
    {
        sector = FLASH_SECTOR_3;
    }
    else if ((address < ADDR_FLASH_SECTOR_5) && (address >= ADDR_FLASH_SECTOR_4))
    {
        sector = FLASH_SECTOR_4;
    }
    else if ((address < ADDR_FLASH_SECTOR_6) && (address >= ADDR_FLASH_SECTOR_5))
    {
        sector = FLASH_SECTOR_5;
    }

    return sector;
}

/**
 * Read data from flash.
 * @note This operation's units is word.
 *
 * @param addr flash address
 * @param buf buffer to store read data
 * @param size read bytes size
 *
 * @return result
 */
int stm32_flash_read(rt_uint32_t addr, rt_uint8_t *buf, size_t size)
{
    size_t i;

    if ((addr + size) > STM32_FLASH_END_ADDRESS)
    {
        rt_kprintf("read outrange flash size! addr is (0x%p)", (void*)(addr + size));
        return -1;
    }

    for (i = 0; i < size; i++, buf++, addr++)
    {
        *buf = *(rt_uint8_t *) addr;
    }

    return size;
}

/**
 * Write data to flash.
 * @note This operation's units is word.
 * @note This operation must after erase. @see flash_erase.
 *
 * @param addr flash address
 * @param buf the write data buffer
 * @param size write bytes size
 *
 * @return result
 */
int stm32_flash_write(rt_uint32_t addr, const rt_uint8_t *buf, size_t size)
{
    rt_err_t result      = RT_EOK;
    rt_uint32_t end_addr = addr + size;

    if ((end_addr) > STM32_FLASH_END_ADDRESS)
    {
        rt_kprintf("write outrange flash size! addr is (0x%p)", (void*)(addr + size));
        return -RT_EINVAL;
    }

    if (size < 1)
    {
        return -RT_EINVAL;
    }

    HAL_FLASH_Unlock();

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    for (size_t i = 0; i < size; i++, addr++, buf++)
    {
        /* write data to flash */
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, (rt_uint64_t)(*buf)) == HAL_OK)
        {
            if (*(rt_uint8_t *)addr != *buf)
            {
                result = -RT_ERROR;
                break;
            }
        }
        else
        {
            result = -RT_ERROR;
            break;
        }
    }

    HAL_FLASH_Lock();

    if (result != RT_EOK)
    {
        return result;
    }

    return size;
}

/**
 * Erase data on flash.
 * @note This operation is irreversible.
 * @note This operation's units is different which on many chips.
 *
 * @param addr flash address
 * @param size erase bytes size
 *
 * @return result
 */
int stm32_flash_erase(rt_uint32_t addr, size_t size)
{
    rt_err_t result = RT_EOK;
    rt_uint32_t FirstSector = 0, NbOfSectors = 0;
    rt_uint32_t SECTORError = 0;

    if ((addr + size) > STM32_FLASH_END_ADDRESS)
    {
        rt_kprintf("ERROR: erase outrange flash size! addr is (0x%p)\n", (void*)(addr + size));
        return -RT_EINVAL;
    }

    /*Variable used for Erase procedure*/
    FLASH_EraseInitTypeDef EraseInitStruct;

    /* Unlock the Flash to enable the flash control register access */
    HAL_FLASH_Unlock();

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    /* Get the 1st sector to erase */
    FirstSector = GetSector(addr);
    /* Get the number of sector to erase from 1st sector*/
    NbOfSectors = GetSector(addr + size - 1) - FirstSector + 1;
    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector        = FirstSector;
    EraseInitStruct.NbSectors     = NbOfSectors;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, (uint32_t *)&SECTORError) != HAL_OK)
    {
        result = -RT_ERROR;
        goto __exit;
    }

__exit:
    HAL_FLASH_Lock();

    if (result != RT_EOK)
    {
        return result;
    }

    rt_kprintf("erase done: addr (0x%p), size %d", (void*)addr, size);
    return result;
}

#if defined(PKG_USING_FAL)

static int fal_flash_read_16k(long offset, rt_uint8_t *buf, size_t size);
static int fal_flash_read_64k(long offset, rt_uint8_t *buf, size_t size);
static int fal_flash_read_128k(long offset, rt_uint8_t *buf, size_t size);

static int fal_flash_write_16k(long offset, const rt_uint8_t *buf, size_t size);
static int fal_flash_write_64k(long offset, const rt_uint8_t *buf, size_t size);
static int fal_flash_write_128k(long offset, const rt_uint8_t *buf, size_t size);

static int fal_flash_erase_16k(long offset, size_t size);
static int fal_flash_erase_64k(long offset, size_t size);
static int fal_flash_erase_128k(long offset, size_t size);

//const struct fal_flash_dev stm32f2_onchip_flash = { "stm32_onchip", 0x08000000, 1024*1024, 128*1024, {init, read, write, erase} };
const struct fal_flash_dev stm32_onchip_flash_16k = { "onchip_flash_16k", STM32_FLASH_START_ADRESS_16K, FLASH_SIZE_GRANULARITY_16K, (16 * 1024), {NULL, fal_flash_read_16k, fal_flash_write_16k, fal_flash_erase_16k} };
const struct fal_flash_dev stm32_onchip_flash_64k = { "onchip_flash_64k", STM32_FLASH_START_ADRESS_64K, FLASH_SIZE_GRANULARITY_64K, (64 * 1024), {NULL, fal_flash_read_64k, fal_flash_write_64k, fal_flash_erase_64k} };
const struct fal_flash_dev stm32_onchip_flash_128k = { "onchip_flash_128k", STM32_FLASH_START_ADRESS_128K, FLASH_SIZE_GRANULARITY_128K, (128 * 1024), {NULL, fal_flash_read_128k, fal_flash_write_128k, fal_flash_erase_128k} };

static int fal_flash_read_16k(long offset, rt_uint8_t *buf, size_t size)
{
    return stm32_flash_read(stm32_onchip_flash_16k.addr + offset, buf, size);
}
static int fal_flash_read_64k(long offset, rt_uint8_t *buf, size_t size)
{
    return stm32_flash_read(stm32_onchip_flash_64k.addr + offset, buf, size);
}
static int fal_flash_read_128k(long offset, rt_uint8_t *buf, size_t size)
{
    return stm32_flash_read(stm32_onchip_flash_128k.addr + offset, buf, size);
}

static int fal_flash_write_16k(long offset, const rt_uint8_t *buf, size_t size)
{
    return stm32_flash_write(stm32_onchip_flash_16k.addr + offset, buf, size);
}
static int fal_flash_write_64k(long offset, const rt_uint8_t *buf, size_t size)
{
    return stm32_flash_write(stm32_onchip_flash_64k.addr + offset, buf, size);
}
static int fal_flash_write_128k(long offset, const rt_uint8_t *buf, size_t size)
{
    return stm32_flash_write(stm32_onchip_flash_128k.addr + offset, buf, size);
}

static int fal_flash_erase_16k(long offset, size_t size)
{
    return stm32_flash_erase(stm32_onchip_flash_16k.addr + offset, size);
}
static int fal_flash_erase_64k(long offset, size_t size)
{
    return stm32_flash_erase(stm32_onchip_flash_64k.addr + offset, size);
}
static int fal_flash_erase_128k(long offset, size_t size)
{
    return stm32_flash_erase(stm32_onchip_flash_128k.addr + offset, size);
}

#endif
