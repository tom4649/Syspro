#include"stdio.h"
#include"stdlib.h"

int main(){
    // unsigned long long virtual=0x040000;//physical_address(Byte)/4096
    // unsigned long long virtual=0x041000;//physical_address(Byte)/4096
    // unsigned long long virtual=0x101000;//physical_address(Byte)/4096
    // unsigned long long virtual=0x100000;//physical_address(Byte)/4096
    unsigned long long virtual=0x0c0000;//physical_address(Byte)/4096
    unsigned long long pml4,pdp,pd,pt;
    unsigned long long mod = 512;
    pt=virtual%mod;
    pd=(virtual/mod)%mod;
    pdp=(virtual/mod/mod)%mod;
    pml4=(virtual/mod/mod/mod)%mod;
    
    printf("pml4:%llu/pdp:%llu/pd:%llu/pt:%llu\n",pml4,pdp,pd,pt);
}