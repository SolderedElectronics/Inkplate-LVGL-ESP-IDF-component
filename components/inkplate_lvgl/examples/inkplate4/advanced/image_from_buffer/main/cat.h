/**
 * @file cat.h
 * @brief Placeholder 600x600 L8 grayscale image buffer for Inkplate 4.
 *
 * Replace the empty array with actual pixel data (1 byte per pixel, 0=black,
 * 255=white). Total size: CAT_WIDTH * CAT_HEIGHT = 360000 bytes.
 *
 * To convert an image:
 *   python3 -c "
 *   from PIL import Image
 *   img = Image.open('cat.jpg').convert('L').resize((600,600))
 *   data = list(img.tobytes())
 *   print('static const uint8_t cat[] = {' + ','.join(map(str,data)) + '};')
 *   " > cat_data.h
 */

#pragma once
#define CAT_WIDTH  600
#define CAT_HEIGHT 600
static const uint8_t cat[CAT_WIDTH * CAT_HEIGHT] = {};
