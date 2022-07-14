#include "hardware.h"
#include "pm_timer.h"
#include "util.h"
extern const unsigned int freq_hz;
extern unsigned short pm_timer_bulk;
extern unsigned short pm_timer_is_32;
UINT64  mod_large=((UINT64)1<<32);
UINT64  mod_small=(1<<24);



void init_acpi_pm_timer(struct RSDP * rsdp){
    UINT32 length=*((char*)(rsdp->xsdt_address)+4);
    UINT64 *other_tables=((char*)(rsdp->xsdt_address)+36);

    while(other_tables<((char*)(rsdp->xsdt_address) + length)){
        char* start_address = *other_tables;
        if(compare((char *)start_address,"FACP",4)){
            struct FADT* fadt=start_address;
            pm_timer_is_32=fadt->flags==1 ? 1 : 0;
            pm_timer_bulk=fadt->PM_TMR_BLK;
            puts("pm_timer found\n");
            return;
        }
        other_tables++;
    }
    puts("pm_timer not found\n");
}


static inline UINT32 get_time(UINT16 port){
    UINT32 ret;
    asm volatile("inl %1, %0"
        :"=a"(ret)
        :"Nd"(port));
        return ret;
}
void pm_timer_wait_millisec(unsigned int msec){
    UINT64 mod = pm_timer_is_32 ==1 ? mod_large :mod_small;
    UINT64 target=(msec*freq_hz/1000)%mod;
    UINT64 start,ret;
    start=(get_time(pm_timer_bulk))%mod;
    if(target+start<mod){while(start+target>(ret=(get_time(pm_timer_bulk))%mod));}
    else {while((ret=(get_time(pm_timer_bulk)%mod)>start)||((start+target)%mod>ret));}
    puts("finished\n");
}