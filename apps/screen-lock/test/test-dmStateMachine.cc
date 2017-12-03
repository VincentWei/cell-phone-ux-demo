
#include <gtest/gtest.h>
#include "dmStateMachine.hh"

using namespace std;

enum StateId {
    S1 = 0,
    S2,
    S3,

    STATE_NUM,
};

enum EventId {
    E1 = 0,
    E2,
    E3,

    EVT_NUM,
};

BEGIN_STATE_DECLARE(stateTable)
    DECLARE_STATE(S1)
    DECLARE_STATE(S2)
    DECLARE_STATE(S3)
END_STATE_DELCARE

BEGIN_TRANSITION_DECLARE(transitionTable)
    DECLARE_TRANSITION(S1, E1, S2)
    DECLARE_TRANSITION(S1, E2, S3)
    DECLARE_TRANSITION(S2, E2, S3)
    DECLARE_TRANSITION(S3, E3, S1)
END_TRANSITION_DELCARE

TEST (dmStateMachineTest, dmStateMachineTest_init_with_table) {

    dmStateMachine sm;

    sm.addStateTable(stateTable);
    sm.addTransitionTable(transitionTable);

    sm.setInitState(S1);

    EXPECT_TRUE(sm.currentStateId() == S1);

    sm.moveOn(E3);
    EXPECT_TRUE(!sm.isStateChanged());
    EXPECT_TRUE(sm.currentStateId() == S1);

    sm.moveOn(E1);
    EXPECT_TRUE(sm.isStateChanged());
    EXPECT_TRUE(sm.currentStateId() == S2);

    sm.moveOn(E2);
    EXPECT_TRUE(sm.isStateChanged());
    EXPECT_TRUE(sm.currentStateId() == S3);

    sm.moveOn(E3);
    EXPECT_TRUE(sm.isStateChanged());
    EXPECT_TRUE(sm.currentStateId() == S1);

    sm.moveOn(STATE_NUM);
    EXPECT_TRUE(!sm.isStateChanged());
    EXPECT_TRUE(sm.currentStateId() == S1);

    sm.moveOn(E2);
    EXPECT_TRUE(sm.isStateChanged());
    EXPECT_TRUE(sm.currentStateId() == S3);

    sm.reset();
    EXPECT_TRUE(sm.isStateChanged());
    EXPECT_TRUE(sm.currentStateId() == S1);

    sm.moveOn(E2);
    EXPECT_TRUE(sm.isStateChanged());
    EXPECT_TRUE(sm.currentStateId() == S3);
}

static const char* state_list[] = {
    "S1", "S2", "S3",
};

TEST (dmStateMachineTest, dmStateMachineTest_normal) {

    dmStateMachine sm;
    dmState* s[STATE_NUM];


    for (int i = 0; i < STATE_NUM; ++i) {
        s[i] = sm.addState(state_list[i], i);
    }

    sm.setInitState(S1);

    dmTransition* t1 = sm.addTransition(S1, E1, S2, "E1");
    dmTransition* t2 = sm.addTransition(S1, E2, S3, "E2");
    dmTransition* t3 = sm.addTransition(S2, E2, S3, "E2");
    dmTransition* t4 = sm.addTransition(S3, E3, S1, "E3");

    EXPECT_TRUE(sm.currentStateId() == S1);

    sm.moveOn(E3);
    EXPECT_TRUE(!sm.isStateChanged());
    EXPECT_TRUE(sm.currentStateId() == S1);

    sm.moveOn(E1);
    EXPECT_TRUE(sm.isStateChanged());
    EXPECT_TRUE(sm.currentStateId() == S2);

    sm.moveOn(E2);
    EXPECT_TRUE(sm.isStateChanged());
    EXPECT_TRUE(sm.currentStateId() == S3);

    sm.moveOn(E3);
    EXPECT_TRUE(sm.isStateChanged());
    EXPECT_TRUE(sm.currentStateId() == S1);

    sm.moveOn(STATE_NUM);
    EXPECT_TRUE(!sm.isStateChanged());
    EXPECT_TRUE(sm.currentStateId() == S1);

    sm.moveOn(E2);
    EXPECT_TRUE(sm.isStateChanged());
    EXPECT_TRUE(sm.currentStateId() == S3);

    sm.reset();
    EXPECT_TRUE(sm.isStateChanged());
    EXPECT_TRUE(sm.currentStateId() == S1);

    sm.moveOn(E2);
    EXPECT_TRUE(sm.isStateChanged());
    EXPECT_TRUE(sm.currentStateId() == S3);

    char desc[128];
    printf("Please check transition desc: %s\n", t1->getTransitionDesc(desc, 128));
    printf("Please check transition desc: %s\n", t2->getTransitionDesc(desc, 128));
    printf("Please check transition desc: %s\n", t3->getTransitionDesc(desc, 128));
    printf("Please check transition desc: %s\n", t4->getTransitionDesc(desc, 128));
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
