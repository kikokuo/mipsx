// #include "mipsx.h"
#include <inttypes.h>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include "cmipsx.h"
#include "monitor.h"
#include "debug.h"
#include "ui.h"
// https://packages.ubuntu.com/source/bionic/allegro5

int recode_cycle = 20;
// syscall 
// [2695642] 0xbfc0d964 0x0000000c
// [2766784] 0x8005aa94 0x0000000c
// lwl 24502246
// 0xbfc00000	0x88c10003	lwl  R01, [R06 + 0x3]
// 到15000000为止，pc和IR都不错
// 19246121出问题了
// 正确[19246538] 0x80052a30 0x00004012
// [19246541] 0x80052a30 0x00004012
// [19246537] 0x80052a2c 0x00c1001a

// [19246537] 0x80052a2c 0x00c1001a 0x00c1001a	div R06, R01
// PSX HI=0x0000005e LO=0x00005ebc
// GPR00: r0 00000000 at 0000000c v0 00000024 v1 0000003c
// GPR04: a0 00000003 a1 00000000 a2 00000024 a3 00000063
// GPR08: t0 1f801c00 t1 00000000 t2 0000dff1 t3 00000000
// GPR12: t4 00000200 t5 800699e0 t6 1f801c00 t7 800697e0
// GPR16: s0 00000000 s1 00000000 s2 00000002 s3 00000001
// GPR20: s4 00000000 s5 800dea68 s6 00000001 s7 00000000
// GPR24: t8 0000dff1 t9 1f801c00 k0 8005aa18 k1 00000f1c
// GPR28: gp a0010ff0 sp 801ffc28 s8 00000024 ra 80052fe0
// [19246538] 0x80052a30 0x00004012 0x00004012	mflo R08
// PSX HI=0000000000 LO=0x00000003
// GPR00: r0 00000000 at 0000000c v0 00000024 v1 0000003c
// GPR04: a0 00000003 a1 00000000 a2 00000024 a3 00000063
// GPR08: t0 1f801c00 t1 00000000 t2 0000dff1 t3 00000000
// GPR12: t4 00000200 t5 800699e0 t6 1f801c00 t7 800697e0
// GPR16: s0 00000000 s1 00000000 s2 00000002 s3 00000001
// GPR20: s4 00000000 s5 800dea68 s6 00000001 s7 00000000
// GPR24: t8 0000dff1 t9 1f801c00 k0 8005aa18 k1 00000f1c
// GPR28: gp a0010ff0 sp 801ffc28 s8 00000024 ra 80052fe0
// [19246539] 0x80052a34 0x30aeffff
// PSX HI=0000000000 LO=0x00000003
// GPR00: r0 00000000 at 0000000c v0 00000024 v1 0000003c
// GPR04: a0 00000003 a1 00000000 a2 00000024 a3 00000063
// GPR08: t0 00000003 t1 00000000 t2 0000dff1 t3 00000000
// GPR12: t4 00000200 t5 800699e0 t6 1f801c00 t7 800697e0
// GPR16: s0 00000000 s1 00000000 s2 00000002 s3 00000001
// GPR20: s4 00000000 s5 800dea68 s6 00000001 s7 00000000
// GPR24: t8 0000dff1 t9 1f801c00 k0 8005aa18 k1 00000f1c
// GPR28: gp a0010ff0 sp 801ffc28 s8 00000024 ra 80052fe0
// 前200000个里面，寄存器确保都正确
// 到19246571，寄存器都正确
// 24502251 0x88c10003
// OK [19202245] 0x80054168 0x030fc023
// [19222245] 0x80054178 0x27280001
// OK [19248870] 0x00000c84 0x00000000
// [19282255] 0x80059e08 0x0105082a
// 19258608可能是分界
// [19258626] 0xbfc04168 0x1720000e还是正确的

//出错在19259713
// 19259715 已经错了
// 又是从IO地址1f8010e8加载 归根结底的DMA 的OTC没实现
// Commonly used DMA Control Register values for starting DMA transfers   DMA6 OTC      11000002h (always)
//   1F8010Exh      DMA6 channel 6 - OTC (reverse clear OT) (GPU related)
// 0x8d8d0000	lw  R13, [R12 + 0x0]
// [19259688]时存进去 [19259688] 0x8005023c 0xac600000 0xac600000	sw  R00, [R03 + 0x0]
// [19259699]写入 是DMA 做完DMA的事后要         (OTC)channels[6].control.raw &= ~0x01000000;
// [19259710] 0x8005027c 0x8d8d0000
// PSX HI=0x0000066d LO=0x00000594
// GPR00: r0 00000000 at 800e0000 v0 1f8010f0 v1 1f8010e8
// GPR04: a0 800ea8d8 a1 00000400 a2 00000000 a3 00000280
// GPR08: t0 800eb8d4 t1 1f8010e0 t2 1f8010e4 t3 11000002
// GPR12: t4 1f8010e8 t5 0000000a t6 00000000 t7 000000f0
// GPR16: s0 01000000 s1 00000004 s2 00000000 s3 00000000
// GPR20: s4 00000000 s5 00000000 s6 00000000 s7 00000000
// GPR24: t8 00001000 t9 800eb8d8 k0 8005aa18 k1 00000f1c
// GPR28: gp a0010ff0 sp 801ffd10 s8 801fff00 ra 80050270
// [19259711] 0x80050280 0x00000000
// PSX HI=0x0000066d LO=0x00000594
// GPR00: r0 00000000 at 800e0000 v0 1f8010f0 v1 1f8010e8
// GPR04: a0 800ea8d8 a1 00000400 a2 00000000 a3 00000280
// GPR08: t0 800eb8d4 t1 1f8010e0 t2 1f8010e4 t3 11000002
// GPR12: t4 1f8010e8 t5 10000002 t6 00000000 t7 000000f0
// GPR16: s0 01000000 s1 00000004 s2 00000000 s3 00000000
// GPR20: s4 00000000 s5 00000000 s6 00000000 s7 00000000
// GPR24: t8 00001000 t9 800eb8d8 k0 8005aa18 k1 00000f1c
// GPR28: gp a0010ff0 sp 801ffd10 s8 801fff00 ra 80050270


// int main()
int main( int argc, char* args[] )
{
    MIPSX_SYSTEM psx;
    Monitor monitor(psx); 
    // const int total_cycle =19662070;
    // const int total_cycle =19663630;
    // const int total_cycle =19663630;
    // const int total_cycle =24500672;
    // const int total_cycle =24501258;
    // const int total_cycle =34502258;
    const int total_cycle =84502258;
    PSX_UI::psx_ui_init();
    
    


  
    
    // const int total_cycle =25900972;
    //  const int total_cycle =21663930;
    for( mipsx_cycle=-2;mipsx_cycle<=total_cycle+1;mipsx_cycle++)
    {   
        // if(mipsx_cycle>=19660026){
          if(mipsx_cycle>=19663620){
            // Log::log = true;
        }
        psx.tick();
        psx.hack_intercept_BIOS_Putchar();
        if(Log::log)
            monitor.showStatus();
    }
    // printf("%x",R3000_CP0::cp0_regs.EPC);
    //x__err("[19258618] 0x8cce0000	lw  R14, [R06 + 0x0]\n这里出错了,此时R14是0x1f801814,这里是从GPU加载状态,然而,现在GPU没有实现");
    // al_flip_display();

    // al_rest(10.0);

    // al_destroy_display(PSX_UI::display);
    PSX_UI::psx_ui_quit();
   
    return 0;
}

// [24500671] [24500672]是vram transfer的分界点

//0x5c86260a
// Our next stop will be an unhandled LW at address 0x1f801814. This register
// is GPUSTAT when read and GP1 when written.
// [19663622] 0x8005116c 0x8c4b0000
// 0x8c4b0000	lw  R11, [R02 + 0x0]
// 0x8005116c 0x8c4b0000
// PSX HI=0000000000 LO=0000000000
// GPR00: r0 00000000 at 00000000 v0 1f801814 v1 05000000
// GPR04: a0 00000000 a1 800eccf8 a2 00000000 a3 00000260
// GPR08: t0 0eccf000 t1 000eccf0 t2 050eccf0 t3 80000008
// GPR12: t4 00000800 t5 00000008 t6 000000f0 t7 00000000
// GPR16: s0 01000000 s1 60000000 s2 10000000 s3 00000000
// GPR20: s4 00000000 s5 00000000 s6 00000000 s7 00000000
// GPR24: t8 000eccf0 t9 050eccf0 k0 80050f20 k1 00000f1c
// GPR28: gp a0010ff0 sp 801ffd10 s8 801fff00 ra 80051144


//真正出错的地方在19258628,以下为正解
// 	0x8cce0000	lw  R14, [R06 + 0x0]
// 1f801814关键是GPU GP0的相关指令没有implement
// [19258626] 0xbfc04168 0x1720000e
// PSX HI=0x0000066d LO=0x00000594
// GPR00: r0 00000000 at 60000000 v0 00000000 v1 10000000
// GPR04: a0 10000000 a1 80050dac a2 1f801814 a3 00000008
// GPR08: t0 bfc03fac t1 00000124 t2 000000a0 t3 00000009
// GPR12: t4 00000023 t5 0000002b t6 1c802000 t7 00000000
// GPR16: s0 00000000 s1 00000000 s2 00000000 s3 00000000
// GPR20: s4 00000000 s5 00000000 s6 00000000 s7 00000000
// GPR24: t8 1c802000 t9 10000000 k0 8005aa18 k1 00000f1c
// GPR28: gp a0010ff0 sp 801ffce8 s8 801fff00 ra bfc03fbc
// [19258627] 0xbfc0416c 0x00000000
// PSX HI=0x0000066d LO=0x00000594
// GPR00: r0 00000000 at 60000000 v0 00000000 v1 10000000
// GPR04: a0 10000000 a1 80050dac a2 1f801814 a3 00000008
// GPR08: t0 bfc03fac t1 00000124 t2 000000a0 t3 00000009
// GPR12: t4 00000023 t5 0000002b t6 1c802000 t7 00000000
// GPR16: s0 00000000 s1 00000000 s2 00000000 s3 00000000
// GPR20: s4 00000000 s5 00000000 s6 00000000 s7 00000000
// GPR24: t8 1c802000 t9 10000000 k0 8005aa18 k1 00000f1c
// GPR28: gp a0010ff0 sp 801ffce8 s8 801fff00 ra bfc03fbc
// [19258628] 0xbfc041a4 0x1000002a
// PSX HI=0x0000066d LO=0x00000594
// GPR00: r0 00000000 at 60000000 v0 00000000 v1 10000000
// GPR04: a0 10000000 a1 80050dac a2 1f801814 a3 00000008
// GPR08: t0 bfc03fac t1 00000124 t2 000000a0 t3 00000009
// GPR12: t4 00000023 t5 0000002b t6 1c802000 t7 00000000
// GPR16: s0 00000000 s1 00000000 s2 00000000 s3 00000000
// GPR20: s4 00000000 s5 00000000 s6 00000000 s7 00000000
// GPR24: t8 1c802000 t9 10000000 k0 8005aa18 k1 00000f1c
// GPR28: gp a0010ff0 sp 801ffce8 s8 801fff00 ra bfc03fbc

// PS-X Realtime Kernel Ver.2.5
// Copyright 1993,1994 (C) Sony Computer Entertainment Inc. 
// KERNEL SETUP!

// Configuration : EvCB	0x10		TCB	0x04
// System ROM Version 2.2 12/04/95 A
// Copyright 1993,1994,1995 (C) Sony Computer Entertainment Inc.
// ResetCallback: _96_remove ..
// VSync: timeout (1:0)
// VSync: timeout (1:0)
// VSync: timeout (1:0)
// VSync: timeout (1:0)
// VSync: timeout (1:0)
// VSync: timeout (1:0)
// VSync: timeout (1:0)
// VSync: timeout (1:0)

// interrupt
// [19660027] 0x80050b7c 0xad6a0000
// PSX HI=0000000000 LO=0000000000
// GPR00: r0 00000000 at 800e0000 v0 1f8010f0 v1 800ea884
// GPR04: a0 800ea884 a1 00000000 a2 00000000 a3 00000000
// GPR08: t0 1f8010a0 t1 1f8010a4 t2 01000401 t3 1f8010a8
// GPR12: t4 00000000 t5 800dde98 t6 04000002 t7 1f801814
// GPR16: s0 10000000 s1 800dde98 s2 01000000 s3 04000000
// GPR20: s4 00000000 s5 00000000 s6 00000000 s7 00000000
// GPR24: t8 0f6f4321 t9 0f6f4b21 k0 8005aa18 k1 00000f1c
// GPR28: gp a0010ff0 sp 801ffcd8 s8 801fff00 ra 80050f20
// [19660028] 0x80050f20 0x3c088008
// PSX HI=0000000000 LO=0000000000
// GPR00: r0 00000000 at 800e0000 v0 1f8010f0 v1 800ea884
// GPR04: a0 800ea884 a1 00000000 a2 00000000 a3 00000000
// GPR08: t0 1f8010a0 t1 1f8010a4 t2 01000401 t3 1f8010a8
// GPR12: t4 00000000 t5 800dde98 t6 04000002 t7 1f801814
// GPR16: s0 10000000 s1 800dde98 s2 01000000 s3 04000000
// GPR20: s4 00000000 s5 00000000 s6 00000000 s7 00000000
// GPR24: t8 0f6f4321 t9 0f6f4b21 k0 8005aa18 k1 00000f1c
// GPR28: gp a0010ff0 sp 801ffcd8 s8 801fff00 ra 80050f20
// [19660029] 0x80000080 0x3c1a0000
// PSX HI=0000000000 LO=0000000000
// GPR00: r0 00000000 at 800e0000 v0 1f8010f0 v1 800ea884
// GPR04: a0 800ea884 a1 00000000 a2 00000000 a3 00000000
// GPR08: t0 1f8010a0 t1 1f8010a4 t2 01000401 t3 1f8010a8
// GPR12: t4 00000000 t5 800dde98 t6 04000002 t7 1f801814
// GPR16: s0 10000000 s1 800dde98 s2 01000000 s3 04000000
// GPR20: s4 00000000 s5 00000000 s6 00000000 s7 00000000
// GPR24: t8 0f6f4321 t9 0f6f4b21 k0 8005aa18 k1 00000f1c
// GPR28: gp a0010ff0 sp 801ffcd8 s8 801fff00 ra 80050f20
// [19660030] 0x80000084 0x275a0c80

// [19661066] 0x00001014 0x42000010
// PSX HI=0000000000 LO=0000000000
// GPR00: r0 00000000 at 800e0000 v0 1f8010f0 v1 800ea884
// GPR04: a0 800ea884 a1 00000000 a2 00000000 a3 00000000
// GPR08: t0 1f8010a0 t1 1f8010a4 t2 01000401 t3 1f8010a8
// GPR12: t4 00000000 t5 800dde98 t6 04000002 t7 1f801814
// GPR16: s0 10000000 s1 800dde98 s2 01000000 s3 04000000
// GPR20: s4 00000000 s5 00000000 s6 00000000 s7 00000000
// GPR24: t8 0f6f4321 t9 0f6f4b21 k0 80050f20 k1 00000f1c
// GPR28: gp a0010ff0 sp 801ffcd8 s8 801fff00 ra 80050f20
// [19661067] 0x80050f20 0x3c088008
// PSX HI=0000000000 LO=0000000000
// GPR00: r0 00000000 at 800e0000 v0 1f8010f0 v1 800ea884
// GPR04: a0 800ea884 a1 00000000 a2 00000000 a3 00000000
// GPR08: t0 1f8010a0 t1 1f8010a4 t2 01000401 t3 1f8010a8
// GPR12: t4 00000000 t5 800dde98 t6 04000002 t7 1f801814
// GPR16: s0 10000000 s1 800dde98 s2 01000000 s3 04000000
// GPR20: s4 00000000 s5 00000000 s6 00000000 s7 00000000
// GPR24: t8 0f6f4321 t9 0f6f4b21 k0 80050f20 k1 00000f1c
// GPR28: gp a0010ff0 sp 801ffcd8 s8 801fff00 ra 80050f20
// [19661068] 0x80050f24 0x8d08929c
// PSX HI=0000000000 LO=0000000000
// GPR00: r0 00000000 at 800e0000 v0 1f8010f0 v1 800ea884
// GPR04: a0 800ea884 a1 00000000 a2 00000000 a3 00000000
// GPR08: t0 80080000 t1 1f8010a4 t2 01000401 t3 1f8010a8
// GPR12: t4 00000000 t5 800dde98 t6 04000002 t7 1f801814
// GPR16: s0 10000000 s1 800dde98 s2 01000000 s3 04000000
// GPR20: s4 00000000 s5 00000000 s6 00000000 s7 00000000
// GPR24: t8 0f6f4321 t9 0f6f4b21 k0 80050f20 k1 00000f1c
// GPR28: gp a0010ff0 sp 801ffcd8 s8 801fff00 ra 80050f20