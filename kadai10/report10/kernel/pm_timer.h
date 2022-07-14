#include "hardware.h"
typedef char UINT8;
typedef unsigned short CHAR16;
typedef unsigned short UINT16;
typedef short INT16;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef unsigned long long UINTN;
typedef long long INTN;
typedef char BOOLEAN;
typedef unsigned long long EFI_STATUS;
typedef void *EFI_HANDLE;
typedef UINT64 EFI_PHYSICAL_ADDRESS;
typedef UINT64 EFI_VIRTUAL_ADDRESS;

static const unsigned int freq_hz=3579545;
static unsigned short pm_timer_bulk;
static unsigned short pm_timer_is_32;
void init_acpi_pm_timer(struct RSDP * rsdp);
void pm_timer_wait_millisec(unsigned int msec);