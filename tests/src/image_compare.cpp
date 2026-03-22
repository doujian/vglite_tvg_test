#define STB_IMAGE_IMPLEMENTATION
#include "../utils/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../utils/stb_image_write.h"

#include "../include/image_compare.h"
#include <cstring>
#include <cmath>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>

namespace vglite {
namespace test {

namespace fs = std::filesystem;

// Convert BGRA to RGBA for PNG output
static void BGRAtoRGBA(uint8_t* data, int width, int height) {
    for (int i = 0; i < width * height; ++i) {
        uint8_t temp = data[i * 4 + 0]; // B
        data[i * 4 + 0] = data[i * 4 + 2]; // R
        data[i * 4 + 2] = temp; // B
    }
}

// Convert RGBA to BGRA for internal use
static void RGBAtoBGRA(uint8_t* data, int width, int height) {
    for (int i = 0; i < width * height; ++i) {
        uint8_t temp = data[i * 4 + 0]; // R
        data[i * 4 + 0] = data[i * 4 + 2]; // B
        data[i * 4 + 2] = temp; // R
    }
}

bool CompareBuffers(const uint8_t* actual, const uint8_t* expected, 
                   int width, int height, float tolerance) {
    if (!actual || !expected || width <= 0 || height <= 0) {
        return false;
    }
    
    const int totalPixels = width * height;
    int differentPixels = 0;
    const int toleranceThreshold = static_cast<int>(255.0f * tolerance);
    
    for (int i = 0; i < totalPixels; ++i) {
        const uint8_t* act = actual + i * 4;
        const uint8_t* exp = expected + i * 4;
        
        // Compare each channel (R, G, B, A)
        bool pixelDifferent = false;
        for (int c = 0; c < 4; ++c) {
            int diff = std::abs(static_cast<int>(act[c]) - static_cast<int>(exp[c]));
            if (diff > toleranceThreshold) {
                pixelDifferent = true;
                break;
            }
        }
        
        if (pixelDifferent) {
            differentPixels++;
        }
    }
    
    // Calculate difference percentage
    float diffPercentage = static_cast<float>(differentPixels) / static_cast<float>(totalPixels);
    
    // Return true if difference is within tolerance
    return diffPercentage <= tolerance;
}

bool SaveBufferToPNG(const vg_lite_buffer_t* buffer, const std::string& path) {
    if (!buffer || !buffer->memory || buffer->width <= 0 || buffer->height <= 0) {
        return false;
    }
    
    // Create output directory if needed
    fs::path filePath(path);
    if (filePath.has_parent_path()) {
        fs::create_directories(filePath.parent_path());
    }
    
    // Copy buffer data
    const int width = buffer->width;
    const int height = buffer->height;
    const int stride = buffer->stride;
    
    std::vector<uint8_t> imageData(width * height * 4);
    uint8_t* src = static_cast<uint8_t*>(buffer->memory);
    
    // Copy with stride handling and convert to RGBA for PNG
    for (int y = 0; y < height; ++y) {
        const uint8_t* row = src + y * stride;
        for (int x = 0; x < width; ++x) {
            // Handle different buffer formats
            if (buffer->format == VG_LITE_BGRA8888 || buffer->format == VG_LITE_BGRX8888) {
                // BGRA to RGBA
                imageData[(y * width + x) * 4 + 0] = row[x * 4 + 2]; // R
                imageData[(y * width + x) * 4 + 1] = row[x * 4 + 1]; // G
                imageData[(y * width + x) * 4 + 2] = row[x * 4 + 0]; // B
                imageData[(y * width + x) * 4 + 3] = row[x * 4 + 3]; // A
            } else {
                // RGBA to RGBA (no conversion needed)
                imageData[(y * width + x) * 4 + 0] = row[x * 4 + 0]; // R
                imageData[(y * width + x) * 4 + 1] = row[x * 4 + 1]; // G
                imageData[(y * width + x) * 4 + 2] = row[x * 4 + 2]; // B
                imageData[(y * width + x) * 4 + 3] = row[x * 4 + 3]; // A
            }
        }
    }
    
    // Save as PNG
    int result = stbi_write_png(path.c_str(), width, height, 4, 
                                imageData.data(), width * 4);
    
    return result != 0;
}

bool LoadPNGToBuffer(const std::string& path, uint8_t** out_data, int* out_width, int* out_height) {
    if (!out_data || !out_width || !out_height) {
        return false;
    }
    
    int width, height, channels;
    uint8_t* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    
    if (!data) {
        return false;
    }
    
    // Convert RGBA to BGRA for internal VGLite format
    RGBAtoBGRA(data, width, height);
    
    *out_data = data;
    *out_width = width;
    *out_height = height;
    
    return true;
}

std::string GetReferencePath(const std::string& reference_name) {
    // Get the directory where the tests are located
    // Assumes running from project root or tests directory
    fs::path basePath;
    
    // Try to find the reference directory
    std::vector<fs::path> searchPaths = {
        fs::current_path() / "tests" / "reference",
        fs::current_path() / "reference",
        fs::current_path() / ".." / "tests" / "reference",
    };
    
    for (const auto& p : searchPaths) {
        if (fs::exists(p)) {
            basePath = p;
            break;
        }
    }
    
    if (basePath.empty()) {
        // Default to current_path/tests/reference
        basePath = fs::current_path() / "tests" / "reference";
    }
    
    return (basePath / reference_name).string();
}

std::string GetOutputPath() {
    fs::path outputDir = fs::current_path() / "tests" / "output";
    fs::create_directories(outputDir);
    return outputDir.string();
}

static bool SaveDiffImage(const uint8_t* actual, const uint8_t* expected, 
                         int width, int height, const std::string& path) {
    std::vector<uint8_t> diffImage(width * height * 4);
    
    for (int i = 0; i < width * height; ++i) {
        // Create a diff visualization
        // Different pixels: red, same pixels: dimmed original
        bool pixelDifferent = false;
        for (int c = 0; c < 3; ++c) {
            if (std::abs(static_cast<int>(actual[i * 4 + c]) - 
                        static_cast<int>(expected[i * 4 + c])) > 5) {
                pixelDifferent = true;
                break;
            }
        }
        
        if (pixelDifferent) {
            // Mark different pixels in red (in RGBA)
            diffImage[i * 4 + 0] = 255; // R
            diffImage[i * 4 + 1] = 0;   // G
            diffImage[i * 4 + 2] = 0;   // B
            diffImage[i * 4 + 3] = 255; // A
        } else {
            // Dimmed original
            diffImage[i * 4 + 0] = actual[i * 4 + 2] / 3; // R <- B
            diffImage[i * 4 + 1] = actual[i * 4 + 1] / 3; // G
            diffImage[i * 4 + 2] = actual[i * 4 + 0] / 3; // B <- R
            diffImage[i * 4 + 3] = 255; // A
        }
    }
    
    int result = stbi_write_png(path.c_str(), width, height, 4, 
                                diffImage.data(), width * 4);
    return result != 0;
}

bool CompareWithReference(const vg_lite_buffer_t* buffer, const std::string& reference_name, 
                         float tolerance) {
    if (!buffer || !buffer->memory) {
        std::cerr << "CompareWithReference: Invalid buffer" << std::endl;
        return false;
    }
    
    std::string refPath = GetReferencePath(reference_name);
    
    // Check if reference file exists
    if (!fs::exists(refPath)) {
        std::cerr << "CompareWithReference: Reference file not found: " << refPath << std::endl;
        // Save actual image for reference
        std::string outputPath = GetOutputPath();
        std::string actualPath = outputPath + "/" + reference_name;
        fs::create_directories(fs::path(actualPath).parent_path());
        SaveBufferToPNG(buffer, actualPath);
        std::cerr << "Saved actual image to: " << actualPath << std::endl;
        return false;
    }
    
    // Load reference image
    uint8_t* refData = nullptr;
    int refWidth, refHeight;
    
    if (!LoadPNGToBuffer(refPath, &refData, &refWidth, &refHeight)) {
        std::cerr << "CompareWithReference: Failed to load reference: " << refPath << std::endl;
        return false;
    }
    
    // Check dimensions
    if (refWidth != buffer->width || refHeight != buffer->height) {
        std::cerr << "CompareWithReference: Dimension mismatch. "
                  << "Expected: " << buffer->width << "x" << buffer->height << ", "
                  << "Got: " << refWidth << "x" << refHeight << std::endl;
        stbi_image_free(refData);
        return false;
    }
    
    // Convert buffer data to BGRA for comparison (same format as refData from LoadPNGToBuffer)
    std::vector<uint8_t> bufferBGRA(buffer->width * buffer->height * 4);
    uint8_t* src = static_cast<uint8_t*>(buffer->memory);
    const int stride = buffer->stride;
    
    for (int y = 0; y < buffer->height; ++y) {
        const uint8_t* row = src + y * stride;
        for (int x = 0; x < buffer->width; ++x) {
            int idx = (y * buffer->width + x) * 4;
            if (buffer->format == VG_LITE_BGRA8888 || buffer->format == VG_LITE_BGRX8888) {
                // Already BGRA, copy as-is
                bufferBGRA[idx + 0] = row[x * 4 + 0]; // B
                bufferBGRA[idx + 1] = row[x * 4 + 1]; // G
                bufferBGRA[idx + 2] = row[x * 4 + 2]; // R
                bufferBGRA[idx + 3] = row[x * 4 + 3]; // A
            } else {
                // RGBA to BGRA
                bufferBGRA[idx + 0] = row[x * 4 + 2]; // B <- R
                bufferBGRA[idx + 1] = row[x * 4 + 1]; // G
                bufferBGRA[idx + 2] = row[x * 4 + 0]; // R <- B
                bufferBGRA[idx + 3] = row[x * 4 + 3]; // A
            }
        }
    }
    
    // Compare buffers
    bool match = CompareBuffers(bufferBGRA.data(), refData,
                               buffer->width, buffer->height, tolerance);
    
    if (!match) {
        // Save diff image and actual image
        std::string outputPath = GetOutputPath();
        std::string baseName = reference_name;
        
        // Remove extension from base name
        size_t dotPos = baseName.rfind('.');
        if (dotPos != std::string::npos) {
            baseName = baseName.substr(0, dotPos);
        }
        
        std::string diffPath = outputPath + "/" + baseName + "_diff.png";
        std::string actualPath = outputPath + "/" + baseName + "_actual.png";
        
        fs::create_directories(fs::path(diffPath).parent_path());
        
        SaveDiffImage(static_cast<uint8_t*>(buffer->memory), refData,
                     buffer->width, buffer->height, diffPath);
        SaveBufferToPNG(buffer, actualPath);
        
        std::cerr << "CompareWithReference: Images differ. "
                  << "Diff saved to: " << diffPath << ", "
                  << "Actual saved to: " << actualPath << std::endl;
    }
    
    stbi_image_free(refData);
    return match;
}

} // namespace test
} // namespace vglite
