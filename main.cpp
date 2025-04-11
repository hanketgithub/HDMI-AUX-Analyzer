
#include <stdio.h>
#include <iostream>

#include <fcntl.h>
#include <unistd.h>

using namespace std;

typedef struct
{
    uint8_t scan_info : 2;                  // S
    uint8_t bar_info : 2;                   // B
    uint8_t active_format_info_present : 1; // A
    uint8_t color_space : 2;                // Y
    uint8_t rsvd : 1;
} AviPB1_T;

// C[7:6] M[5:4] R[3:0]
typedef struct
{
    uint8_t active_aspect_ratio : 4;  // R
    uint8_t picture_aspect_ratio : 2; // M
    uint8_t colorimetry : 2;          // C
} AviPB2_T;

// ITC[7] EC[6:4] Q[3:2] SC[1:0]
typedef struct
{
    uint8_t scaled : 2;               // SC
    uint8_t quantization : 2;         // Q
    uint8_t extended_colorimetry : 3; // EC
    uint8_t itc : 1;                  // ITC
} AviPB3_T;

void ParseAVIInfoFrame(uint8_t *data)
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
}

uint16_t read_le_u16(uint8_t *ptr)
{
    return ptr[0] | (ptr[1] << 8);
}

void ParseHDRInfoFrame(uint8_t *data)
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
}

int main(int argc, const char *argv[]) {
  int ifd;
  cout << "HDMI-AUX-Analyzer" << endl;

  if (argc < 2)
  {
    fprintf(stderr, "useage: %s [input_file]\n", argv[0]);        
    return -1;
  }

  // get input file name from comand line
  ifd = open(argv[1], O_RDONLY);
  if (ifd < 0)
  {
    perror(argv[1]);
    exit(EXIT_FAILURE);
  }  

  size_t file_sz = 0x1000;
  uint8_t *data = (uint8_t *) calloc(1, file_sz);

  read(ifd, data, file_sz);

  uint8_t *p = data;
  for (int i = 0; i < file_sz; i++) {
    if (p[i] == 0xcd && p[i+1] == 0xab) { // key
      uint8_t type = p[i+2];

      switch (type) {
        case 0x83: {
          printf("Text\n");
          break;
        }
        case 0x82: {
          ParseAVIInfoFrame(&p[i+2]);
          break;
        }
        case 0x87: {
          ParseHDRInfoFrame(&p[i+2]);
          break;
        }
        default: {
          printf("Type %d not processed\n\n", type);
          break;
        }
      }
    }
  }

  return 0;
}
