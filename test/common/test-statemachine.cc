#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include "StateMachine.hh"
using namespace mStateMachine;
using namespace std;

//static key_press_event = KeyEvent(KeyEvent::KeyPressed, 1);
//static mouse_event = KeyEvent(KeyEvent::KeyPressed, 1);
//static timer_event = KeyEvent(KeyEvent::KeyPressed, 1);

static string result;
const static string SPACE=" ";
#define enter(x) string(SPACE+string(x->name()) + string(".OnEnter "))
#define leave(x) string(SPACE+string(x->name()) + string(".OnLeave "))
#define in_trans(x,y) string(string(" trans(")+ string((x)->name()) + string("->") +  \
        string((y)->name()) + string(") "))
#define in_trans_x(x) string(string(" trans(")+ string((x)->name()) + string("->") +  \
        string("targetless") + string(") "))


class MyTransiton;

class MyState: public State{ 
public:
    MyState(StateMachine *stateMachine, const char *name=""):State(stateMachine, name){};
    MyState(State *parentState, const char *name=""):State(parentState, name){};
                                                     ;
    virtual void onEnter(const Event *event, const Transition *transition){
        result+=enter(this);
    };

    virtual void onLeave(const Event *event, const Transition *transition){
        result+=leave(this);
    };
};

class MyTransition : public Transition {
public:
    MyTransition(State *source, State *target)
        : Transition(source, target, "")
    {
        subscribe(this, onTrigger, NULL);
        std::ostringstream s("");
        s << source->name() << " -> ";
        if(NULL == target) {
            s << "target-less";
        } else {
            s << target->name();
        } 

        m_name = std::string(s.str().c_str());
    }
    ~MyTransition() {}
private:
    static void onTrigger(void *object, Transition *transition, const Event* event, void *param) {
        MyTransition *ins = (MyTransition *)object;
        if(ins->target()) {
            result+=in_trans(ins->source(), ins->target()); 
        } else {
            result+=in_trans_x(ins->source()); 
        }

        //printf("Transition[%s] triggered\n", ins->name());
    }
};

TEST (StateMachineTest, Transition_s1_s2) {
    result.clear();
    StateMachine machine;
    Transition *trans;
    Event *event;
    MyState *s1, *s2;
    s1 = new MyState(&machine, "s1");
    s2 = new MyState(&machine, "s2");
    machine.setInitialState(s1);
    trans = new MyTransition(s1, s2);

    machine.start();

    event = new KeyEvent(KeyEvent::KeyPressed, 1);
    machine.processEvent(event);
    string expected = enter(s1) + leave(s1) + in_trans(s1, s2) + enter(s2);
    
    cout<<"result:  "<<result<<endl;
    cout<<"expected:"<<expected<<endl;

    ASSERT_TRUE(expected==result); 
}


TEST (StateMachineTest, Transition_s1_s211_L3) {
    result.clear();
    StateMachine machine;
    Transition *trans;
    Event *event;
    MyState *s1, *s2, *s21, *s211;
    s1 = new MyState(&machine, "s1");
    s2 = new MyState(&machine, "s2");
    s21= new MyState(s2, "s21");
    s211 = new MyState(s21, "s211");
    machine.setInitialState(s1);
    s2->setInitialState(s21);
    s21->setInitialState(s211);
    trans = new MyTransition(s1, s211);

    machine.start();

    event = new KeyEvent(KeyEvent::KeyPressed, 1);
    machine.processEvent(event);
    string expected = enter(s1) + leave(s1) + in_trans(s1, s211) + enter(s2)+ enter(s21)+ enter(s211);
    
    cout<<"result:  "<<result<<endl;
    cout<<"expected:"<<expected<<endl;

    ASSERT_TRUE(expected==result); 
}

TEST (StateMachineTest, Transition_s1_s21_L3) {
    result.clear();
    StateMachine machine;
    Transition *trans1;
    Event *event;
    MyState *s1, *s2, *s21, *s211;
    s1 = new MyState(&machine, "s1");
    s2 = new MyState(&machine, "s2");
    s21= new MyState(s2, "s21");
    s211 = new MyState(s21, "s211");
    machine.setInitialState(s1);
    s2->setInitialState(s21);
    s21->setInitialState(s211);
    trans1 = new MyTransition(s1, s21);

    machine.start();

    event = new KeyEvent(KeyEvent::KeyPressed, 1);
    machine.processEvent(event);
    string expected = enter(s1) + leave(s1) + in_trans(s1, s21) + enter(s2)+ enter(s21)+ enter(s211);
    
    cout<<"result:  "<<result<<endl;
    cout<<"expected:"<<expected<<endl;

    ASSERT_TRUE(expected==result); 
}

TEST (StateMachineTest, Transition_s1_s2_L3) {
    result.clear();
    StateMachine machine;
    Transition *trans1;
    Event *event;
    MyState *s1, *s2, *s21, *s211;
    s1 = new MyState(&machine, "s1");
    s2 = new MyState(&machine, "s2");
    s21= new MyState(s2, "s21");
    s211 = new MyState(s21, "s211");
    machine.setInitialState(s1);
    s2->setInitialState(s21);
    s21->setInitialState(s211);
    trans1 = new MyTransition(s1, s2);

    machine.start();

    event = new KeyEvent(KeyEvent::KeyPressed, 1);
    machine.processEvent(event);
    string expected = enter(s1) + leave(s1) + in_trans(s1, s2) + enter(s2)+ enter(s21)+ enter(s211);
    
    cout<<"result:  "<<result<<endl;
    cout<<"expected:"<<expected<<endl;

    ASSERT_TRUE(expected==result); 
}


TEST (StateMachineTest, Transition_s11_s21_L2) {
    result.clear();
    StateMachine machine;
    Transition *trans1;
    Event *event;
    MyState *s1, *s2, *s11, *s12,*s21, *s22;
    s1 = new MyState(&machine, "s1");
    s2 = new MyState(&machine, "s2");
    s11= new MyState(s1, "s11");
    s12= new MyState(s1, "s12");
    s21= new MyState(s2, "s21");
    s22= new MyState(s2, "s22");
    machine.setInitialState(s1);
    s1->setInitialState(s11);
    s2->setInitialState(s21);
    trans1 = new MyTransition(s11, s21);

    machine.start();

    event = new KeyEvent(KeyEvent::KeyPressed, 1);
    machine.processEvent(event);
    string expected = enter(s1) + enter(s11) +leave(s11) +leave(s1) + in_trans(s11, s21) + enter(s2)+ enter(s21);
    
    cout<<"result:  "<<result<<endl;
    cout<<"expected:"<<expected<<endl;

    ASSERT_TRUE(expected==result); 
}

TEST (StateMachineTest, Transition_s1_s2_L2) {
    result.clear();
    StateMachine machine;
    Transition *trans1;
    Event *event;
    MyState *s1, *s2, *s11, *s12,*s21, *s22;
    s1 = new MyState(&machine, "s1");
    s2 = new MyState(&machine, "s2");
    s11= new MyState(s1, "s11");
    s12= new MyState(s1, "s12");
    s21= new MyState(s2, "s21");
    s22= new MyState(s2, "s22");
    machine.setInitialState(s1);
    s1->setInitialState(s11);
    s2->setInitialState(s22);
    trans1 = new MyTransition(s1, s2);

    machine.start();

    event = new KeyEvent(KeyEvent::KeyPressed, 1);
    machine.processEvent(event);
    string expected = enter(s1) + enter(s11) +leave(s11) +leave(s1) + in_trans(s1, s2) + enter(s2)+ enter(s22);
    
    cout<<"result:  "<<result<<endl;
    cout<<"expected:"<<expected<<endl;

    ASSERT_TRUE(expected==result); 
}

TEST (StateMachineTest, Transition_s11_s12_L2) {
    result.clear();
    StateMachine machine;
    Transition *trans1;
    Event *event;
    MyState *s1, *s2, *s11, *s12,*s21, *s22;
    s1 = new MyState(&machine, "s1");
    s2 = new MyState(&machine, "s2");
    s11= new MyState(s1, "s11");
    s12= new MyState(s1, "s12");
    s21= new MyState(s2, "s21");
    s22= new MyState(s2, "s22");
    machine.setInitialState(s1);
    s1->setInitialState(s11);
    s2->setInitialState(s22);
    trans1 = new MyTransition(s11, s12);

    machine.start();

    event = new KeyEvent(KeyEvent::KeyPressed, 1);
    machine.processEvent(event);
    
    string expected = enter(s1) + enter(s11) +leave(s11) + in_trans(s11, s12) + enter(s12);;
    cout<<"result:  "<<result<<endl;
    cout<<"expected:"<<expected<<endl;

    ASSERT_TRUE(expected==result); 
}


TEST (StateMachineTest, Transition_s1_s21_L2) {
    result.clear();
    StateMachine machine;
    Transition *trans1;
    Event *event;
    MyState *s1, *s2, *s11, *s12,*s21, *s22;
    s1 = new MyState(&machine, "s1");
    s2 = new MyState(&machine, "s2");
    s11= new MyState(s1, "s11");
    s12= new MyState(s1, "s12");
    s21= new MyState(s2, "s21");
    s22= new MyState(s2, "s22");
    machine.setInitialState(s1);
    s1->setInitialState(s11);
    s2->setInitialState(s22);
    trans1 = new MyTransition(s1, s12);

    machine.start();

    event = new KeyEvent(KeyEvent::KeyPressed, 1);
    machine.processEvent(event);
    string expected = enter(s1) + enter(s11) +leave(s11) +leave(s1) + in_trans(s1, s12) + enter(s1) + enter(s12);;
    
    cout<<"result:  "<<result<<endl;
    cout<<"expected:"<<expected<<endl;

    ASSERT_TRUE(expected==result); 
}

TEST (StateMachineTest, Transition_s1_s11_L2) {
    result.clear();
    StateMachine machine;
    Transition *trans1;
    Event *event;
    MyState *s1, *s2, *s11, *s12,*s21, *s22;
    s1 = new MyState(&machine, "s1");
    s2 = new MyState(&machine, "s2");
    s11= new MyState(s1, "s11");
    s12= new MyState(s1, "s12");
    s21= new MyState(s2, "s21");
    s22= new MyState(s2, "s22");
    machine.setInitialState(s1);
    s1->setInitialState(s11);
    s2->setInitialState(s22);
    trans1 = new MyTransition(s1, s11);

    machine.start();

    event = new KeyEvent(KeyEvent::KeyPressed, 1);
    machine.processEvent(event);
    string expected = enter(s1) + enter(s11) +leave(s11) +leave(s1) + in_trans(s1, s11) + enter(s1) + enter(s11);;
    
    cout<<"result:  "<<result<<endl;
    cout<<"expected:"<<expected<<endl;

    ASSERT_TRUE(expected==result); 
}


TEST (StateMachineTest, Transition_s1_s1_L1) {
    result.clear();
    StateMachine machine;
    Transition *trans1;
    Event *event;
    MyState *s1, *s2, *s11, *s12,*s21, *s22;
    s1 = new MyState(&machine, "s1");
    s2 = new MyState(&machine, "s2");
    s11= new MyState(s1, "s11");
    s12= new MyState(s1, "s12");
    s21= new MyState(s2, "s21");
    s22= new MyState(s2, "s22");
    machine.setInitialState(s1);
    s1->setInitialState(s11);
    s2->setInitialState(s22);
    trans1 = new MyTransition(s1, NULL);

    machine.start();

    event = new KeyEvent(KeyEvent::KeyPressed, 1);
    machine.processEvent(event);
    string expected = enter(s1) + enter(s11) + in_trans_x(s1);
    
    cout<<"result:  "<<result<<endl;
    cout<<"expected:"<<expected<<endl;

    ASSERT_TRUE(expected==result); 
}


#if 0
TEST (StateMachineTest, InterLevelTransition) {
    StateMachine machine;
    Transition *trans;
    Event *event;
    State *s1, *s2, *s21;
    s1 = new State(&machine, "1");
    s2 = new State(&machine, "2");
    s21 = new State(s2, "21");
    machine.setInitialState(s1);
    s2->setInitialState(s21);
    trans = new MyTransition(s1, s2);

    machine.start();

    event = new KeyEvent(KeyEvent::KeyPressed, 1);
    machine.processEvent(event);
}
TEST (StateMachineTest, Transition) {
    StateMachine machine;
    Transition *trans;
    Event *event;
    State *s1, *s2;
    State *s11, *s12;
    State *s111, *s112;
    State *s21, *s22;

    s1 = new State(&machine, "1");

    s11 = new State(s1, "11");
    s111 = new State(s11, "111");
    s112 = new State(s11, "112");

    s11->setInitialState(s111);

    s12 = new State(s1, "12");

    s1->setInitialState(s11);

    s2 = new State(&machine, "2");

    s21 = new State(s2, "21");
    s22 = new State(s2, "22");

    s2->setInitialState(s21);

    trans = new MyTransition(s11, s22);

    trans = new MyTransition(s2, s12);

    machine.setInitialState(s1);
    machine.start();

    event = new KeyEvent(KeyEvent::KeyPressed, 1);
    machine.processEvent(event);

    machine.processEvent(event);
}
#endif

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
