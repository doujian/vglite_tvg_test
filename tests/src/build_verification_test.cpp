// Build test to verify GoogleTest infrastructure works
#include <gtest/gtest.h>

// Simple test to verify basic functionality
TEST(BuildVerification, GoogleTestWorks) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

// Test that the parent sources are accessible
TEST(BuildVerification, SourcesIncluded) {
    // This test verifies the build infrastructure is working
    GTEST_SKIP() << "Add actual tests after verifying build";
}
