/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\10_Formal_Verification.md
 * Line: 873
 * Language: c
 * Block ID: 128711d7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef enum { IDLE, CONNECTING, CONNECTED, ERROR } State;

/*@ type invariant valid_state(State s) =
      s == IDLE || s == CONNECTING || s == CONNECTED || s == ERROR;
*/

/*@ assigns state;
    ensures state == CONNECTING;
*/
void connect(void);

/*@ requires state == CONNECTING;
    assigns state;
    behavior success:
      assumes success_condition;
      ensures state == CONNECTED;
    behavior fail:
      assumes !success_condition;
      ensures state == ERROR;
    complete behaviors;
    disjoint behaviors;
*/
void handle_connect_response(int success);

/*@ requires state == CONNECTED;
    assigns state;
    ensures state == IDLE;
*/
void disconnect(void);
