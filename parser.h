

#ifndef ___PARSER___
#define ___PARSER___

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


int ParseAVIInfoFrame(uint8_t *data);


int ParseHDRInfoFrame(uint8_t *data);


#endif