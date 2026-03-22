#ifndef IMAGE_COMPARE_H
#define IMAGE_COMPARE_H

#include "vg_lite.h"
#include <string>

namespace vglite {
namespace test {

// Compare two RGBA buffers pixel by pixel with tolerance
// Returns true if images match within tolerance
// tolerance: 0.0-1.0, where 0.01 = 1% difference allowed
bool CompareBuffers(const uint8_t* actual, const uint8_t* expected, 
                   int width, int height, float tolerance = 0.01f);

// Save buffer to PNG file
bool SaveBufferToPNG(const vg_lite_buffer_t* buffer, const std::string& path);

// Load PNG file to buffer (allocates memory)
bool LoadPNGToBuffer(const std::string& path, uint8_t** out_data, int* out_width, int* out_height);

// Compare buffer with reference image file
// reference_name: relative path from tests/reference/ directory
bool CompareWithReference(const vg_lite_buffer_t* buffer, const std::string& reference_name, 
                         float tolerance = 0.01f);

// Get absolute path to reference image
std::string GetReferencePath(const std::string& reference_name);

// Get absolute path to output directory (for saving diffs)
std::string GetOutputPath();

} // namespace test
} // namespace vglite

#endif /* IMAGE_COMPARE_H */
