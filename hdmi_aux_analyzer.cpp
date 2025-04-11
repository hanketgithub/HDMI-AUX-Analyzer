
#include <stdint.h>
#include <stdio.h>

#include "hdmi_aux_analyzer.h"

static uint16_t read_le_u16(uint8_t *ptr)
{
  return ptr[0] | (ptr[1] << 8);
}

int ParseAVIInfoFrame(uint8_t *data)
{
  AviPB1_T *p = (AviPB1_T *)&data[4];
  AviPB2_T *q = (AviPB2_T *)&data[5];

  uint8_t Y = p->color_space;
  uint8_t A = p->active_format_info_present;
  uint8_t B = p->bar_info;
  uint8_t S = p->scan_info;

  uint8_t C = q->colorimetry;
  uint8_t M = q->picture_aspect_ratio;
  uint8_t R = q->active_aspect_ratio;
  AviPB3_T *r = (AviPB3_T *)&data[6];
  //uint8_t ITC = r->ITC;
  uint8_t EC = r->extended_colorimetry;
  uint8_t Q = r->quantization;
  uint8_t SC = r->scaled;
  uint8_t VIC = data[7] & 0x7f;

  printf("[AVI InfoFrame]\n");

  printf("Version = %d\n", data[1]);
  printf("Length  = %d\n", data[2]);
  printf("Chksum  = 0x%02X\n", data[3]);

  printf("Y (Color Format) = %u -> ", Y);
  switch (Y)
  {
  case 0:
      printf("RGB");
      break;
  case 1:
      printf("YCbCr 4:2:2");
      break;
  case 2:
      printf("YCbCr 4:4:4");
      break;
  default:
      printf("Reserved");
      break;
  }
  printf("\n");

  printf("A (Active Format Info Present) = %u\n", A);

  printf("B (Bar Info) = %u -> ", B);
  switch (B)
  {
  case 0:
      printf("No bar info");
      break;
  case 1:
      printf("Vertical bar (top/bottom)");
      break;
  case 2:
      printf("Horizontal bar (left/right)");
      break;
  case 3:
      printf("Both vertical & horizontal bar");
      break;
  }
  printf("\n");

  printf("S (Scan Info) = %u -> ", S);
  switch (S)
  {
  case 0:
      printf("Unknown");
      break;
  case 1:
      printf("Overscan");
      break;
  case 2:
      printf("Underscan");
      break;
  default:
      printf("Reserved");
      break;
  }
  printf("\n");
  //printf("C = %d M = %d R = %d\n", C, M, R);

  printf("M (Picture Aspect Ratio) = %u -> ", M);
  switch (M)
  {
  case 0:
      printf("Unknown");
      break;
  case 1:
      printf("4:3");
      break;
  case 2:
      printf("16:9");
      break;
  default:
      printf("Reserved");
      break;
  }
  printf("\n");

  if (A)
  {
      printf("R (Active Format Aspect Ratio) = %u -> ", R);
      switch (R)
      {
      case 0:
          printf("same as picture aspect ratio");
          break;
      case 1:
          printf("4:3");
          break;
      case 2:
          printf("16:9");
          break;
      case 3:
          printf("14:9");
          break;
      default:
          printf("Reserved");
          break;
      }
  }
  printf("\n");

  printf("Colorimetry = %u -> ", C);
  if (C == 2)
  {
      if (EC == 5 || EC == 6)
      {
          printf("BT.2020");
      }
      else
      {
          printf("Extended (EC = %d)", EC);
      }
  }
  else if (C == 1)
  {
      printf("BT.709");
  }
  else
  {
      printf("BT.601");
  }
  printf("\n");

  printf("Quantization = %u -> ", Q);
  switch (Q)
  {
  case 0:
      printf("Default");
      break;
  case 1:
      printf("Limited Range");
      break;
  case 2:
      printf("Full Range");
      break;
  default:
      printf("Reserved");
      break;
  }
  printf("\n");

  printf("SC (Scaling) = %u -> ", SC);
  switch (SC)
  {
  case 0:
      printf("Unknown");
      break;
  case 1:
      printf("Scaled");
      break;
  case 2:
      printf("Not Scaled");
      break;
  default:
      printf("Reserved");
      break;
  }
  printf("\n");

  printf("VIC = %u -> ", VIC);
  switch (VIC)
  {
  case 100:
      printf("3840x2160@50Hz (YUV420)");
      break;
  case 95:
      printf("3840x2160@30Hz");
      break;
  case 16:
      printf("1920x1080@60Hz");
      break;
  default:
      printf("Unknown or custom resolution");
      break;
  }
  printf("\n");

  return 0;
}

int ParseHDRInfoFrame(uint8_t *data)
{
  uint8_t eotf = data[4];
  uint8_t sm_id = data[5];
  uint16_t gx = read_le_u16(&data[6]);
  uint16_t gy = read_le_u16(&data[8]);
  uint16_t bx = read_le_u16(&data[10]);
  uint16_t by = read_le_u16(&data[12]);
  uint16_t rx = read_le_u16(&data[14]);
  uint16_t ry = read_le_u16(&data[16]);
  uint16_t wpx = read_le_u16(&data[18]);
  uint16_t wpy = read_le_u16(&data[20]);

  uint16_t max_lum = read_le_u16(&data[22]);
  uint16_t min_lum = read_le_u16(&data[24]);
  uint16_t max_cll = read_le_u16(&data[26]);
  uint16_t max_fall = read_le_u16(&data[28]);

  printf("[HDR InfoFrame]\n");

  printf("Version = %d\n", data[1]);
  printf("Length  = %d\n", data[2]);
  printf("Chksum  = 0x%02X\n", data[3]);

  printf("EOTF = %d -> ", eotf);
  switch (eotf)
  {
  case 0:
      printf("Traditional SDR (Gamma)");
      break;
  case 1:
      printf("Traditional HDR (Gamma)");
      break;
  case 2:
      printf("PQ (ST 2084)");
      break;
  case 3:
      printf("HLG (Hybrid Log-Gamma)");
      break;
  default:
      printf("Reserved / Unknown");
      break;
  }
  printf("\n");


  printf("Static Metadata Descriptor ID = %d\n", sm_id);

  printf("G - x=%u, y=%u\n", gx, gy);
  printf("B - x=%u, y=%u\n", bx, by);
  printf("R - x=%u, y=%u\n", rx, ry);
  printf("WP - x=%u, y=%u\n", wpx, wpy);

  printf("Max Display Mastering Luminance = %u cd/m^2\n", max_lum);
  printf("Min Display Mastering Luminance = %u cd/m^2\n", min_lum);
  printf("MaxCLL = %u cd/m^2\n", max_cll);
  printf("MaxFALL = %u cd/m^2\n", max_fall);

  return 0;
}