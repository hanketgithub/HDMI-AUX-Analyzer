# HDMI-AUX-Analyzer

## Build Instructions

To compile the project:

```bash
make
```

This will build the executable `hdmi_aux_analyzer`.

---

## Usage

Run the tool with:

```bash
./hdmi_aux_analyzer [input_file]
```

- `[input_file]` should be a binary file containing HDMI AUX frames.
- The tool parses and displays recognized frames, including:
  - **Text Frames** (Type `0x83`)
  - **AVI InfoFrames** (Type `0x82`)
  - **HDR InfoFrames** (Type `0x87`)

If no input file is provided, the tool will print a usage message and exit.

### Example

```bash
./hdmi_aux_analyzer AuxData.bin
```

---

## Sample Console Output

```plaintext
===== HDMI-AUX-Analyzer Started =====
[INFO] Opening file: AuxData.bin
[INFO] File size: 23872 bytes
[INFO] File read successfully
[INFO] Starting analysis...
[FOUND] Text Frame detected
[WARNING] Unhandled Frame Type: 3
[WARNING] Unhandled Frame Type: 132
[FOUND] AVI InfoFrame detected
[AVI InfoFrame]
Version = 2
Length  = 13
Checksum = 0xE4
Y (Color Format) = 1 -> YCbCr 4:2:2
A (Active Format Info Present) = 0
B (Bar Info) = 0 -> No bar info
S (Scan Info) = 1 -> Overscan
M (Picture Aspect Ratio) = 0 -> Unknown
Colorimetry = 0 -> BT.601
Quantization = 2 -> Full Range
SC (Scaling) = 0 -> Unknown
VIC = 100 -> 3840x2160@50Hz (YUV420)
[WARNING] Unhandled Frame Type: 129
[FOUND] HDR InfoFrame detected
[HDR InfoFrame]
Version = 1
Length  = 26
Checksum = 0x27
EOTF = 236 -> Reserved / Unknown
Static Metadata Descriptor ID = 2
G - x = 60928, y = 30588
B - x = 58432, y = 14990
R - x = 29980, y = 7519
WP - x = 20898, y = 2315
Max Display Mastering Luminance = 18237 cd/m²
Min Display Mastering Luminance = 64 cd/m²
MaxCLL = 20992 cd/m²
MaxFALL = 0 cd/m²
[FOUND] Text Frame detected
[WARNING] Unhandled Frame Type: 3
[WARNING] Unhandled Frame Type: 132
===== HDMI-AUX-Analyzer Finished =====
```

---

## AVI InfoFrame Structure

| Field Name                      | Offset             | Bits        | Description                                          |
| -------------------------------- | ------------------ | ----------- | ---------------------------------------------------- |
| Packet Type                     | byte[0]            | -           | Fixed 0x82 (AVI InfoFrame)                           |
| Version                         | byte[1]            | -           | AVI InfoFrame version (typically 2)                  |
| Length                          | byte[2]            | -           | Payload length (typically 13 bytes)                  |
| Checksum                        | byte[3]            | -           | Header checksum                                      |
| Y (Color Space)                 | byte[4]            | [6:5]       | 00: RGB, 01: YCbCr 4:2:2, 10: YCbCr 4:4:4            |
| A (Active Format Info Present)  | byte[4]            | [4]         | 1: Active Format Info is present                     |
| B (Bar Info)                    | byte[4]            | [3:2]       | 00: No bar, 01: Top/Bottom, 10: Left/Right, 11: Both |
| S (Scan Info)                   | byte[4]            | [1:0]       | 00: Unknown, 01: Overscan, 10: Underscan             |
| C (Colorimetry)                 | byte[5]            | [7:6]       | 00: None (BT.601), 01: BT.709, 10: Extended          |
| M (Picture Aspect Ratio)        | byte[5]            | [5:4]       | 00: Unknown, 01: 4:3, 10: 16:9                       |
| R (Active Aspect Ratio)         | byte[5]            | [3:0]       | Active aspect ratio code                             |
| ITC (ITU Colorimetry)           | byte[6]            | [7]         | 1: Extended colorimetry is valid                     |
| EC (Extended Colorimetry)       | byte[6]            | [6:4]       | 000: xvYCC601, 001: xvYCC709, 010: sYCC601, etc.     |
| Q (Quantization Range)          | byte[6]            | [3:2]       | 00: Default, 01: Limited, 10: Full Range             |
| SC (Scaling Info)               | byte[6]            | [1:0]       | 00: Unknown, 01: Horizontally, 10: Vertically        |
| VIC (Video Identification Code) | byte[7]            | [6:0]       | HDMI VIC (Video Timing ID)                           |
| YCC Quantization Range          | byte[8]            | [7:6]       | 00: Limited range, 01: Full range                    |
| Content Type                    | byte[8]            | [5:4]       | 00: Graphics, 01: Photo, 10: Cinema, 11: Game        |
| Pixel Repetition Factor         | byte[8]            | [3:0]       | 0 = No repetition, 1 = 2x repetition, etc.           |
| Top Bar                         | byte[9]+[10]       | 16 bits LE  | Top bar position (Little Endian)                     |
| Bottom Bar                      | byte[11]+[12]      | 16 bits LE  | Bottom bar position (Little Endian)                  |
| Left Bar                        | byte[13]+[14]      | 16 bits LE  | Left bar position (Little Endian)                    |
| Right Bar                       | byte[15]+[16]      | 16 bits LE  | Right bar position (Little Endian)                   |

---

## HDR InfoFrame Structure

| Field Name     | Offset            | Description                               |
| -------------- | ----------------- | ----------------------------------------- |
| EOTF           | byte[0]            |                                           |
| Metadata Type  | byte[1]            |                                           |
| G.x            | byte[2] + byte[3]  | Little Endian, 16-bit                     |
| G.y            | byte[4] + byte[5]  | Little Endian, 16-bit                     |
| B.x            | byte[6] + byte[7]  | Little Endian, 16-bit                     |
| B.y            | byte[8] + byte[9]  | Little Endian, 16-bit                     |
| R.x            | byte[10] + byte[11]| Little Endian, 16-bit                     |
| R.y            | byte[12] + byte[13]| Little Endian, 16-bit                     |
| WP.x           | byte[14] + byte[15]| Little Endian, 16-bit                     |
| WP.y           | byte[16] + byte[17]| Little Endian, 16-bit                     |
| MaxMDL         | byte[18] + byte[19]| Little Endian, 16-bit, unit: cd/m²        |
| MinMDL         | byte[20] + byte[21]| Little Endian, 16-bit, unit: cd/m²/10000  |
| MaxCLL         | byte[22] + byte[23]| Little Endian, 16-bit, unit: cd/m²        |
| MaxFALL        | byte[24] + byte[25]| Little Endian, 16-bit, unit: cd/m²        |
