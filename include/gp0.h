#pragma once
#include "bitwise.h"
#include "gpu.h"
#include "inttypes.h"
#include "draw.h"
#include "vram.h"
namespace GPU{
        uint32_t texture_window_mask_x;
        uint32_t texture_window_mask_y;
        uint32_t texture_window_offset_x;
        uint32_t texture_window_offset_y;
        uint32_t drawing_area_x1;
        uint32_t drawing_area_y1;
        uint32_t drawing_area_x2;
        uint32_t drawing_area_y2;
        uint32_t x_offset;
        uint32_t y_offset;
        uint32_t display_area_x;
        uint32_t display_area_y;
        uint32_t display_area_x1;
        uint32_t display_area_y1;
        uint32_t display_area_x2;
        uint32_t display_area_y2;
        //They’re used to mirror a textured rectangle horizontally or vertically:
        bool textured_rectangle_x_flip;// mirror textured rectangles along the x axis
        bool textured_rectangle_y_flip;// mirror textured rectangles along the y axis
        typedef union {// GPU Status Register
            struct  // 1F801814h - GPUSTAT - GPU Status Register (R)
            {
                uint32_t tpagex:4;              //  0-3   Texture page X Base   (N*64)                              ;GP0(E1h).0-3
                uint32_t tpagey:1;              //   4     Texture page Y Base   (N*256) (ie. 0 or 256)              ;GP0(E1h).4
                uint32_t transparency:2;        //   5-6   Semi Transparency     (0=B/2+F/2, 1=B+F, 2=B-F, 3=B+F/4)  ;GP0(E1h).5-6
                uint32_t tpagecolor:2;          //   7-8   Texture page colors   (0=4bit, 1=8bit, 2=15bit, 3=Reserved)GP0(E1h).7-8
                uint32_t dither:1;              //   9     Dither 24bit to 15bit (0=Off/strip LSBs, 1=Dither Enabled);GP0(E1h).9
                uint32_t draw_to_display:1;     //   10    Drawing to display area (0=Prohibited, 1=Allowed)         ;GP0(E1h).10
                uint32_t apply_mask:1;          //   11    Set Mask-bit when drawing pixels (0=No, 1=Yes/Mask)       ;GP0(E6h).0
                uint32_t no_draw_over_mask:1;   //   12    Draw Pixels           (0=Always, 1=Not to Masked areas)   ;GP0(E6h).1
                uint32_t _InterlaceField:1;
                uint32_t _Reverseflag:1;
                uint32_t _TextureDisable:1;
        //   13    Interlace Field       (or, always 1 when GP1(08h).5=0)
        //   14    "Reverseflag"         (0=Normal, 1=Distorted)             ;GP1(08h).7
        //   15    Texture Disable       (0=Normal, 1=Disable Textures)      ;GP0(E1h).11
                uint32_t width1:1;              //   16    Horizontal Resolution 2     (0=256/320/512/640, 1=368)    ;GP1(08h).6
                uint32_t width0:2;              //   17-18 Horizontal Resolution 1     (0=256, 1=320, 2=512, 3=640)  ;GP1(08h).0-1
                uint32_t height:1;              //   19    Vertical Resolution         (0=240, 1=480, when Bit22=1)  ;GP1(08h).2
                uint32_t video_mode:1;          //   20    Video Mode                  (0=NTSC/60Hz, 1=PAL/50Hz)     ;GP1(08h).3
                uint32_t use_24bpp:1;           //   21    Display Area Color Depth    (0=15bit, 1=24bit)            ;GP1(08h).4
                uint32_t interlacing:1;         //   22    Vertical Interlace          (0=Off, 1=On)                 ;GP1(08h).5
                uint32_t display_enable:1;      //   23    Display Enable              (0=Enabled, 1=Disabled)       ;GP1(03h).0
                uint32_t irq:1;                 //   24    Interrupt Request (IRQ1)    (0=Off, 1=IRQ)       ;GP0(1Fh)/GP1(02h)
                uint32_t dma:1;
        //   25    DMA / Data Request, meaning depends on GP1(04h) DMA Direction:
        //           When GP1(04h)=0 ---> Always zero (0)
        //           When GP1(04h)=1 ---> FIFO State  (0=Full, 1=Not Full)
        //           When GP1(04h)=2 ---> Same as GPUSTAT.28
        //           When GP1(04h)=3 ---> Same as GPUSTAT.27
                uint32_t ready_for_cmd:1;       //   26    Ready to receive Cmd Word   (0=No, 1=Ready)  ;GP0(...) ;via GP0
                uint32_t ready_for_vram:1;      //   27    Ready to send VRAM to CPU   (0=No, 1=Ready)  ;GP0(C0h) ;via GPUREAD
                uint32_t ready_for_dma:1;       //   28    Ready to receive DMA Block  (0=No, 1=Ready)  ;GP0(...) ;via GP0
                uint32_t dma_direction:2;       //   29-30 DMA Direction (0=Off, 1=?, 2=CPUtoGP0, 3=GPUREADtoCPU)    ;GP1(04h).0-1
                uint32_t draw_even_lines:1;     //   31    Drawing even/odd lines in interlace mode (0=Even or Vblank, 1=Odd)
            };
            uint32_t raw = 0x14802000; // GP1(00h) - Reset GPU GPUSTAT becomes 14802000h. ;
        }GPUstat_t;
        typedef uint32_t GPUread_t;
        
        struct CPU2GPU_transfer_t{
            struct {
                uint32_t x;
                uint32_t y;
                uint32_t w;
                uint32_t h;                
            }reg;
            struct {
                uint32_t active;
                uint32_t x;
                uint32_t y;
            }run;
        };
        
        struct GPU2CPU_transfer_t{
            struct {
                uint32_t x;
                uint32_t y;
                uint32_t w;
                uint32_t h;                
            }reg;
            struct {
                uint32_t active;
                uint32_t x;
                uint32_t y;
            }run;
        };

        struct point_t{
            uint32_t x;
            uint32_t y;
        };

        // namespace gouraud{
        //     pixel_t point;
        //     void draw_poly3(){

        //     }
        //     void draw_poly4(){

        //     }
        // }

        CPU2GPU_transfer_t  cpu2gpu_transfer;
        GPU2CPU_transfer_t  gpu2cpu_transfer;
        // using namespace VRAM;
        void vram_transfer(uint16_t data){// be used frequently
            if(!cpu2gpu_transfer.run.active){
                return;
            }
            VRAM::vwrite(
                cpu2gpu_transfer.reg.x + cpu2gpu_transfer.run.x,
                cpu2gpu_transfer.reg.y + cpu2gpu_transfer.run.y,
                uint16_t(data)
            );
            cpu2gpu_transfer.run.x++;
            if(cpu2gpu_transfer.run.x == cpu2gpu_transfer.reg.w){
                cpu2gpu_transfer.run.x = 0;
                cpu2gpu_transfer.run.y ++;
                if(cpu2gpu_transfer.run.y == cpu2gpu_transfer.reg.h){
                    cpu2gpu_transfer.run.y = 0;
                    cpu2gpu_transfer.run.active = false;// finish
                    x__log("cpu2vram transfer finish");
                }
            }
        }

        uint16_t vram_transfer(){// overload , but be used not so often
            if(!gpu2cpu_transfer.run.active){
                return 0;
            }
            uint16_t data = VRAM::vread(
                gpu2cpu_transfer.reg.x + gpu2cpu_transfer.run.x,
                gpu2cpu_transfer.reg.y + gpu2cpu_transfer.run.y);

            gpu2cpu_transfer.run.x++;
            if(gpu2cpu_transfer.run.x == gpu2cpu_transfer.reg.w){
                gpu2cpu_transfer.run.x = 0;
                gpu2cpu_transfer.run.y ++;
                if(gpu2cpu_transfer.run.y == gpu2cpu_transfer.reg.h){
                    gpu2cpu_transfer.run.y = 0;
                    gpu2cpu_transfer.run.active = false;// finish
                    x__log("vram2cpu transfer finish");
                }
            }
            return data;
        }

        std::vector<unsigned int> cmd;// It (=GP0 only?) has a 64-byte (16-word) command FIFO buffer.
        
        GPUstat_t GP1;
        GPUread_t GP0;


        
        
}

namespace GP0_CMDS{
    using namespace Bitwise;
    using namespace GPU;
    #define get_bit(nr,value) ( ( (value>>nr) & 0x1) ? 0b1:0b0 )

    void E1_draw_mode_setting(uint32_t data){
// GP0(E1h) - Draw Mode setting (aka "Texpage")
        GP1.tpagex = extract(0,3,data) & 0b1111;            //0-3   Texture page X Base   (N*64) (ie. in 64-halfword steps)    ;GPUSTAT.0-3
        GP1.tpagey = get_bit(4,data) & 0b1;                 //4     Texture page Y Base   (N*256) (ie. 0 or 256)               ;GPUSTAT.4
        GP1.transparency = extract(5,6,data) & 0b11;        //5-6   Semi Transparency     (0=B/2+F/2, 1=B+F, 2=B-F, 3=B+F/4)   ;GPUSTAT.5-6
        GP1.tpagecolor = extract(7,8,data) & 0b11;          //7-8   Texture page colors   (0=4bit, 1=8bit, 2=15bit, 3=Reserved);GPUSTAT.7-8
        GP1.dither = get_bit(9,data);                       //9     Dither 24bit to 15bit (0=Off/strip LSBs, 1=Dither Enabled) ;GPUSTAT.9
        GP1.draw_to_display = get_bit(10,data) & 0b1;       //10    Drawing to display area (0=Prohibited, 1=Allowed)          ;GPUSTAT.10
        GP1._TextureDisable = get_bit(11,data) & 0b1;       //11    Texture Disable (0=Normal, 1=Disable if GP1(09h).Bit0=1)   ;GPUSTAT.15
        textured_rectangle_x_flip = get_bit(12,data) & 0b1;  //  12    Textured Rectangle X-Flip   (BIOS does set this bit on power-up...?)
        textured_rectangle_y_flip = get_bit(13,data) & 0b1;  //  13    Textured Rectangle Y-Flip   (BIOS does set it equal to GPUSTAT.13...?)
        // 14-23 Not used (should be 0)
        // 24-31 Command  (E1h)
    }

    void E2_texture_window_setting(uint32_t data){
// GP0(E2h) - Texture Window setting
        texture_window_mask_x = extract(0,4,data);//   0-4    Texture window Mask X   (in 8 pixel steps)
        texture_window_mask_y = extract(5,9,data);//   5-9    Texture window Mask Y   (in 8 pixel steps)
        texture_window_offset_x = extract(10,14,data);//   10-14  Texture window Offset X (in 8 pixel steps)
        texture_window_offset_y = extract(15,19,data);//   15-19  Texture window Offset Y (in 8 pixel steps)
        //   20-23  Not used (zero)
        //   24-31  Command  (E2h)
    }
    void E3_set_drawing_area_top_left(uint32_t data){
// GP0(E3h) - Set Drawing Area top left (X1,Y1)
        drawing_area_x1 = extract(0,9,data);
        drawing_area_y1 = extract(11,20,data);
    }

    void E4_set_drawing_area_bottom_right(uint32_t data){
// GP0(E4h) - Set Drawing Area bottom right (X2,Y2)
        drawing_area_x2 = extract(0,9,data);    //   0-9    X-coordinate (0..1023)
        drawing_area_y2 = extract(10,19,data);  //   10-18  Y-coordinate (0..511)   ;\on Old 160pin GPU (max 1MB VRAM)
        //   19-23  Not used (zero)         ;/
        //   10-19  Y-coordinate (0..1023)  ;\on New 208pin GPU (max 2MB VRAM)
        //   20-23  Not used (zero)         ;/(retail consoles have only 1MB though)
        //   24-31  Command  (Exh)
        // Sets the drawing area corners. The Render commands GP0(20h..7Fh) are automatically clipping any pixels that are outside of this region.
    }
    uint32_t sign_extend11to32(uint32_t value_11b)
    {
        uint16_t mask = 1 << 10;
        bool sign = (value_11b & mask) ? true : false;
        uint32_t tmp = value_11b & 0x7ff;
        uint32_t extendedValue = (sign) ? (0xfffff800 | tmp) : tmp;
        return extendedValue;
    }
    void E5_drawing_offset(uint32_t data){
// GP0(E5h) - Set Drawing Offset (X,Y)
        x_offset = extract(0,10,data);  //   0-10   X-offset (-1024..+1023) (usually within X1,X2 of Drawing Area)
        y_offset = extract(11,21,data); //   11-21  Y-offset (-1024..+1023) (usually within Y1,Y2 of Drawing Area)
        //   22-23  Not used (zero)
        //   24-31  Command  (E5h)
        x_offset = sign_extend11to32(x_offset);
        y_offset = sign_extend11to32(y_offset);
        //     We have to be careful with that one because the x and y
        // parameters are 11 bit signed two’s complement values. It means that the GPU
        // can handle negative offsets.
    }

    void E6_mask_setting(uint32_t data){
// GP0(E6h) - Mask Bit Setting
        GP1.apply_mask =  get_bit(0,data);         //   0     Set mask while drawing (0=TextureBit15, 1=ForceBit15=1)   ;GPUSTAT.11
        GP1.no_draw_over_mask = get_bit(1,data);   //   1     Check mask before draw (0=Draw Always, 1=Draw if Bit15=0) ;GPUSTAT.12
        //   2-23  Not used (zero)
        //   24-31 Command  (E6h)
    }
    
    

    void GP0_28h_quad_mono_opaque(uint32_t data){
// GP0(28h) - Monochrome four-point polygon, opaque
        using namespace Graphics;
        x__err("画图 Draw quad 未完待续");
        

        // cmd[0];
        // uint32_t color = cmd[0];
        // uint32_t r = (color & 0xff);
        // uint32_t g = (color>>8) & 0xff;
        // uint32_t b = (color>>16) & 0xff;
        // vertex verts[4];
        // for(int i=0;i<4;i++){
        //     int x = x_offset + (cmd[i]&0xffff);
        //     int y = y_offset + (cmd[i]>>16);
        //     verts[i].r = r;
        //     verts[i].g = g;
        //     verts[i].b = b;
        //     verts[i].a = 255;
        // }
        
        // uint32_t point1 = cmd[1];
        // uint32_t point2 = cmd[2];
        // uint32_t point3 = cmd[3];
        // uint32_t point4 = cmd[4];
    }
// GP0(20h) - Monochrome three-point polygon, opaque
// GP0(22h) - Monochrome three-point polygon, semi-transparent

// GP0(2Ah) - Monochrome four-point polygon, semi-transparent
//   1st  Color+Command     (CcBbGgRrh)
//   2nd  Vertex1           (YyyyXxxxh)
//   3rd  Vertex2           (YyyyXxxxh)
//   4th  Vertex3           (YyyyXxxxh)
//  (5th) Vertex4           (YyyyXxxxh) (if any)
    void A0_load_image(uint32_t data){//   1st  Command           (Cc000000h)
// GP0(A0h) - Copy Rectangle (CPU to VRAM)
        cpu2gpu_transfer.reg.x = cmd[1] & 0xffff;//   2nd  Destination Coord (YyyyXxxxh)  ;Xpos counted in halfwords
        cpu2gpu_transfer.reg.y = cmd[1] >> 16;
        cpu2gpu_transfer.reg.w = cmd[2] & 0xffff;//   3rd  Width+Height      (YsizXsizh)  ;Xsiz counted in halfwords
        cpu2gpu_transfer.reg.h = cmd[2] >> 16;
        //   ...  Data              (...)      <--- usually transferred via DMA
        // uint32_t imagesize = cpu2gpu_transfer.reg.w*cpu2gpu_transfer.reg.h;
        // imagesize = (imagesize+1);
        // uint32_t gp0_words_remaining = (imagesize)/2;
        // x__log("gp0_words_remaining %d",gp0_words_remaining);
        cpu2gpu_transfer.run.x = 0;
        cpu2gpu_transfer.run.y = 0;
        cpu2gpu_transfer.run.active = true;
    }
    void C0_store_image(uint32_t data){//   1st  Command           (Cc000000h) ;\
// GP0(C0h) - Copy Rectangle (VRAM to CPU)
        gpu2cpu_transfer.reg.x = cmd[1] & 0xffff;//   2nd  Source Coord      (YyyyXxxxh) ; write to GP0 port (as usually)
        gpu2cpu_transfer.reg.y = cmd[1] >> 16;
        gpu2cpu_transfer.reg.w = cmd[2] & 0xffff;//   3rd  Width+Height      (YsizXsizh) ;/
        gpu2cpu_transfer.reg.h = cmd[2] >> 16;

        gpu2cpu_transfer.run.x = 0;
        gpu2cpu_transfer.run.y = 0;
        gpu2cpu_transfer.run.active = true;


    }
    void GP0_38h_quad_shaded_opaque(){
// GP0(38h) - Shaded four-point polygon, opaque
// GP0(3Ah) - Shaded four-point polygon, semi-transparent


//   1st  Color1+Command    (CcBbGgRrh)
//   2nd  Vertex1           (YyyyXxxxh)
//   3rd  Color2            (00BbGgRrh)
//   4th  Vertex2           (YyyyXxxxh)
//   5th  Color3            (00BbGgRrh)
//   6th  Vertex3           (YyyyXxxxh)
//  (7th) Color4            (00BbGgRrh) (if any)
//  (8th) Vertex4           (YyyyXxxxh) (if any)


    }
    void GP0_30h_triangle_shaded_opaque(){
    //GP0(30h) - Shaded three-point polygon, opaque

    }
    void GP0_2ch_quad_texture_blend_opaque(){
    // GP0(2Ch) - Textured four-point polygon, opaque, texture-blending

//   1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
//   2nd  Vertex1           (YyyyXxxxh)
//   3rd  Texcoord1+Palette (ClutYyXxh)
//   4th  Vertex2           (YyyyXxxxh)
//   5th  Texcoord2+Texpage (PageYyXxh)
//   6th  Vertex3           (YyyyXxxxh)
//   7th  Texcoord3         (0000YyXxh)
//  (8th) Vertex4           (YyyyXxxxh) (if any)
//  (9th) Texcoord4         (0000YyXxh) (if any)
    }




}