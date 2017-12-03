
// test-ActivityProperty.cc
// Test case for struct ActivityProperty

#include <gtest/gtest.h>
#include "Activity.hh"

using namespace std;

const static string TEST_STR = "TEST_STR";

TEST (ActivityPropertyTest, ActivityProperty_Test) {

    ActivityProperty ap("NULL");

    EXPECT_TRUE(ap.isEffectInTurnOn());
    EXPECT_TRUE(ap.isEffectOutTurnOn());
    
    ap.effectInTurnOff();
    ap.effectOutTurnOff();
    EXPECT_FALSE(ap.isEffectInTurnOn());
    EXPECT_FALSE(ap.isEffectOutTurnOn());

    ap.effectInName(TEST_STR);
    ap.effectOutName(TEST_STR);
    EXPECT_EQ(ap.effectInName().compare(TEST_STR), 0);
    EXPECT_EQ(ap.effectOutName().compare(TEST_STR), 0);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
