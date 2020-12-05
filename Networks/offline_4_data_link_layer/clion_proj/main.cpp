#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <assert.h>

using namespace std;

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: SLIGHTLY MODIFIED
 FROM VERSION 1.1 of J.F.Kurose

   This code should be used for PA2, unidirectional or bidirectional
   data transfer protocols (from A to B. Bidirectional transfer of data
   is for extra credit and is not required).  Network properties:
   - one way network delay averages five time_ units (longer if there
       are other packets in the channel for GBN), but can be larger
   - frames can be corrupted (either the header or the data portion)
       or lost, according to user-defined probabilities
   - frames will be delivered in the order in which they were sent
       (although some can be lost).
**********************************************************************/

#define BIDIRECTIONAL 1 /* change to 1 if you're doing extra credit */
/* and write a routine called B_output */

/* a "pkt" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 5 via the students transport level protocol entities.         */
struct pkt
{
    char data[4];
};

/* a frame is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined frame structure, which all   */
/* students must follow. */
struct frm
{
    int type;
    int seqnum;
    int acknum;
    string checksum;
    char payload[4];
};

/********* FUNCTION PROTOTYPES. DEFINED IN THE LATER PART******************/
void starttimer(int AorB, float increment);
void stoptimer(int AorB);
void tolayer1(int AorB, struct frm frame);
void tolayer3(int AorB, char datasent[4]);

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

int seq_num_A;
int ack_A;

int seq_num_B;
int ack_B;

bool received_A = false;
bool received_B = false;

int outstanding_ack_A = false;
int outstanding_ack_B = false;

string divisor = "10101";

struct frm global_frame_A;
struct frm global_frame_B;

void print_frm(frm frame){
    printf("type: %d, seq: %d, ack: %d\ncheckSum: %s\npkt: %s\n", frame.type, frame.seqnum, frame.acknum, frame.checksum.c_str(), frame.payload);
    printf("outstanding ackA : %d\n", outstanding_ack_A);
    printf("outstanding ackB : %d\n", outstanding_ack_B);
}

string get_bin(int n, int sz){ // seems ok
    string bin;
    bin.resize(sz, '0');
    int idx = 0;
    while(n){
        assert(idx < sz);
        if(n % 2){
            // bin.push_back('1');
            bin[idx++] = '1';
        }else{
            // bin.push_back('0');
            bin[idx++] = '0';
        }
        n /= 2;
    }
    reverse(bin.begin(), bin.end());
    return bin;
}

string divide(string divident, string divisor){
    assert((int)divident.size() == (int)divisor.size());
    string result;
    for(int i=0;i<divident.size();i++){
        if(divident[i] != divisor[i]){
            result.push_back('1');
        }else result.push_back('0');
    }
    return result;
}

pair<string, string> CRC(string divident, string divisor){
    int window_size = (int)divisor.size();
    string div_zero;
    for(int i=0;i<window_size;i++) div_zero.push_back('0');
    // append window_size - 1 zeroes
    for(int i=0;i<window_size-1;i++) divident.push_back('0');
    int divident_size = (int)divident.size();
    // find the first 1
    int idx = -1;
    for(int i=0;i<divident_size;i++) {
        if(divident[i] == '1') {
            idx = i;
            break;
        }
    }
    assert(idx != -1);
    string divi_str = divident.substr(idx, window_size);
    for(int i=idx;i<=divident_size - window_size;i++){
        // divi_str = divisor.substr(i, window_size);
        string div_by = divisor;
        if(divi_str[0] == '0') div_by = div_zero; 
        // cout << i << " " << div_by << endl;
        string res = divide(divi_str, div_by);
        res.erase(res.begin());
        if(i + window_size < divident_size)
            divi_str = res + divident[i + window_size];
        else{
            divi_str = res;
        }
    }
    string rem = divi_str;
    // cout << "divi " << divident << endl;
    // cout <<"rem " << rem << endl;
    int j = divident.size() - 1;
    for(int i=(int)rem.size() - 1;i>=0;i--){
        divident[j] = rem[i];
        j--;
    }
    // cout << "div_appn : " << divident << endl; 
    return {divident, rem};
}

string calculate_checksum_2(struct frm frame){
    // int i, sum = frame.acknum + frame.seqnum;
    // for(i=0;frame.payload[i];i++){
    //     sum += frame.payload[i];
    // }
    // return sum;
    string ack_bin = get_bin(frame.acknum, 32);
    string seq_bin = get_bin(frame.seqnum, 32);
    string type_bin = get_bin(frame.type, 32);
    string payload_bin;
    for(int i=0;frame.payload[i];i++){
        payload_bin += get_bin((int)frame.payload[i], 8);
    }
    // cout << "------------------\n";
    // cout << frame.acknum << " " << frame.seqnum << " " << frame.type << " " << frame.payload << endl;
    // cout << ack_bin << " " << seq_bin << " " << type_bin << " " << payload_bin << endl;
    return CRC(ack_bin + seq_bin + type_bin + payload_bin, divisor).first;
}

bool check_checksum(string checksum){
    string rem = CRC(checksum, divisor).second;
    for(auto ele : rem){
        if(ele != '0') return false;
    }
    return true;
}

int calculate_checksum(struct frm frame){
    int i, sum = frame.acknum + frame.seqnum;
    for(i=0;frame.payload[i];i++){
        sum += frame.payload[i];
    }
    return sum;
}



/* called from layer 3, passed the data to be sent to other side */
void A_output(struct pkt packet)
{
//    packet_global = packet;
    printf("A_output called\n");
    if(received_A) {
        struct frm frm_A;
        if(outstanding_ack_A){
            // do piggybacking
            frm_A.type = 2;
            frm_A.acknum = ack_A;
        }else{
            frm_A.type = 0;
            frm_A.acknum = -1; // don't care
        }
        frm_A.seqnum = seq_num_A;
        
        strcpy(frm_A.payload, packet.data);
        frm_A.checksum = calculate_checksum_2(frm_A);
        global_frame_A = frm_A;
        printf("A sending: ");
        print_frm(global_frame_A);
        tolayer1(0, frm_A);
        starttimer(0, 15.0);
        received_A = false;
        outstanding_ack_A = false;
    }else{
        printf("packet: %s dropped at A\n", packet.data);
    }
}

/* need be completed only for extra credit */
void B_output(struct pkt packet)
{
    printf("B_output called\n");
    if(received_B) {
        struct frm frm_B;
        if(outstanding_ack_B){
            // do piggybacking
            frm_B.type = 2;
            frm_B.acknum = ack_B;
        }else{
            frm_B.type = 0;
            frm_B.acknum = -1; // don't care
        }
        frm_B.seqnum = seq_num_B;
        
        strcpy(frm_B.payload, packet.data);
        frm_B.checksum = calculate_checksum_2(frm_B);
        global_frame_B = frm_B;
        printf("B sending: ");
        print_frm(global_frame_B);
        tolayer1(1, frm_B);
        starttimer(1, 15.0);
        received_B = false;
        outstanding_ack_B = false;
    }else{
        printf("packet: %s dropped at B\n", packet.data);
    }
}

/* called from layer 3, when a frame arrives for layer 4 */
void A_input(struct frm frame)
{
    printf("A_input called\n");
    printf("A received: ");
    print_frm(frame);

    // int check = calculate_checksum(frame);
    // bool check = false;
    bool check = check_checksum(frame.checksum);
    if(!check){
        printf("distorted data recieved at A\n");

        // send nack here
        struct frm frm_a_nack;
        frm_a_nack.type = 1;
        frm_a_nack.seqnum = -1;
        frm_a_nack.acknum = ack_A; // -_-
        strcpy(frm_a_nack.payload, "nnn");
        frm_a_nack.checksum = calculate_checksum_2(frm_a_nack);
        printf("A received\n");
        printf("data distorted : \n");
        print_frm(frame);
        tolayer1(0, frm_a_nack);

        return;
    }

    if(frame.type == 0){
        if(frame.seqnum == ack_A){
            // duplicate data found
            printf("Duplicate data found at A\n");
            ack_A = frame.seqnum;
            struct frm frm_A;
            frm_A.type = 1;
            frm_A.seqnum = -1;
            frm_A.acknum = ack_A;
            strcpy(frm_A.payload, "ign");
            frm_A.checksum = calculate_checksum_2(frm_A);
            printf("A sending: ");
            print_frm(frm_A);
            tolayer1(0, frm_A);

            outstanding_ack_A = true;
        }else{
            printf("A received (in else): ");
            outstanding_ack_A = 1;
            if(ack_A == -2){
                ack_A = frame.seqnum;
            }else
                ack_A = frame.seqnum;
            print_frm(frame);
            tolayer3(0, frame.payload);
        }
    }else if(frame.type == 1){
        if(frame.acknum == seq_num_A){
            received_A = true;
            outstanding_ack_A = 0;
            // seq_num_A = abs(1 - seq_num_A); // flip
            seq_num_A++;
            stoptimer(0);
        }else{
            printf("NACK received at A\n");
        }
    }else if(frame.type == 2){
        // piggybacked data found

        // first, handling the part of ack
        if(frame.acknum == seq_num_A){ // maybe not required to check this condition
            received_A = true;
            outstanding_ack_A = 0;
            // seq_num_A = abs(1 - seq_num_A); // flip
            seq_num_A++;
            stoptimer(0);

            // secondly, handing the data frame part
            if(frame.seqnum == ack_A){
                // duplicate data found
                printf("Duplicate data found at A\n");
                ack_A = frame.seqnum;
                struct frm frm_A;
                frm_A.type = 1;
                frm_A.seqnum = -1;
                frm_A.acknum = ack_A;
                strcpy(frm_A.payload, "ign");
                frm_A.checksum = calculate_checksum_2(frm_A);
                printf("A sending: ");
                print_frm(frm_A);
                tolayer1(0, frm_A);

                outstanding_ack_A = true;
            }else{
                printf("A received: ");
                outstanding_ack_A = 1;
                ack_A = frame.seqnum;
                print_frm(frame);
                tolayer3(0, frame.payload);
            }
        }
    }else{
        printf("undefined type\n");
    }
    

    // if(frame.acknum == seq_num_A){
    //     int check = calculate_checksum(frame);
    //     if(check != frame.checksum){
    //         printf("Corrupted data received at A (checksum mismatch).\n");
    //         return;
    //     }
    //     received = true;
    //     // seq_num_A = abs(1 - seq_num_A); // flip
    //     stoptimer(0);
    // }else{
    //     printf("NACK received at A\n");
    // }
}

/* called when A's time_r goes off */
void A_timerinterrupt(void)
{
    printf("(A_timerinterrupt) A sending: ");
    if(outstanding_ack_A){
        global_frame_A.type = 2;
        global_frame_A.acknum = ack_A;
    }else{
        global_frame_A.type = 0;
        global_frame_A.acknum = -1;
    }
    print_frm(global_frame_A);
    tolayer1(0, global_frame_A);
    starttimer(0, 15.0);
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init(void)
{
    printf("A_init called\n");
    received_A = true;
    seq_num_A = 0;
    ack_A = -2;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a frame arrives for layer 4 at B*/
void B_input(struct frm frame)
{
    // printf("B_input called\n");
    // int check = calculate_checksum(frame);
    // if(frame.checksum == check){
    //     if(frame.seqnum == ack_B){
    //         printf("Duplicate data found at B\n");
    //         ack_B = frame.seqnum;
    //         struct frm frm_B;
    //         frm_B.type = 1;
    //         frm_B.seqnum = -1;
    //         frm_B.acknum = ack_B;
    //         strcpy(frm_B.payload, "ig");
    //         frm_B.checksum = calculate_checksum(frm_B);
    //         printf("B sending: ");
    //         print_frm(frm_B);
    //         tolayer1(1, frm_B);

    //         outstanding_ack_B = 0;
    //     }else {
    //         printf("B received: ");
    //         outstanding_ack_B = 1;
    //         ack_B++;
    //         print_frm(frame);
    //         tolayer3(1, frame.payload);
    //     }
    //     // ack_B = frame.seqnum;
    //     // struct frm frm_B;
    //     // frm_B.seqnum = 0;
    //     // frm_B.acknum = ack_B;
    //     // strcpy(frm_B.payload, "ig");
    //     // frm_B.checksum = calculate_checksum(frm_B);
    //     // printf("B sending: ");
    //     // print_frm(frm_B);
    //     // tolayer1(1, frm_B);
    // }else{
    //     // send inverted received seq
    //     struct frm frm_b_nack;
    //     frm_b_nack.type = 1;
    //     frm_b_nack.seqnum = -1;
    //     frm_b_nack.acknum = ack_B; // -_-
    //     strcpy(frm_b_nack.payload, "n");
    //     frm_b_nack.checksum = calculate_checksum(frm_b_nack);
    //     printf("B received\n");
    //     printf("data distorted : \n");
    //     print_frm(frame);
    //     tolayer1(1, frm_b_nack);
    // }

    printf("B_input called\n");
    printf("B received: ");
    print_frm(frame);

    bool check = check_checksum(frame.checksum);
    if(!check){
        printf("distorted data recieved at B\n");

        // send nack here
        struct frm frm_b_nack;
        frm_b_nack.type = 1;
        frm_b_nack.seqnum = -1;
        frm_b_nack.acknum = ack_B; // -_-
        strcpy(frm_b_nack.payload, "nnn");
        frm_b_nack.checksum = calculate_checksum_2(frm_b_nack);
        printf("B received\n");
        printf("data distorted : \n");
        print_frm(frame);
        tolayer1(1, frm_b_nack);

        return;
    }

    if(frame.type == 0){
        if(frame.seqnum == ack_B){
            // duplicate data found
            printf("Duplicate data found at B\n");
            ack_B = frame.seqnum;
            struct frm frm_B;
            frm_B.type = 1;
            frm_B.seqnum = -1;
            frm_B.acknum = ack_B;
            strcpy(frm_B.payload, "ign");
            frm_B.checksum = calculate_checksum_2(frm_B);
            printf("B sending: ");
            print_frm(frm_B);
            tolayer1(1, frm_B);

            outstanding_ack_B = true;
        }else{
            printf("B received: (in else)");
            outstanding_ack_B = 1;
            if(ack_B == -2){
                ack_B = frame.seqnum;
            }else
                ack_B = frame.seqnum;
            print_frm(frame);
            tolayer3(1, frame.payload);
        }
    }else if(frame.type == 1){
        if(frame.acknum == seq_num_B){
            received_B = true;
            outstanding_ack_B = 0;
            // seq_num_A = abs(1 - seq_num_A); // flip
            seq_num_B++;
            stoptimer(1);
        }else{
            printf("NACK received at B\n");
        }
    }else if(frame.type == 2){
        // piggybacked data found

        // first, handling the part of ack
        if(frame.acknum == seq_num_B){ // maybe not required to check this condition
            received_B = true;
            outstanding_ack_B = 0;
            // seq_num_A = abs(1 - seq_num_A); // flip
            seq_num_B++;
            stoptimer(1);

            // secondly, handing the data frame part
            if(frame.seqnum == ack_B){
                // duplicate data found
                printf("Duplicate data found at B\n");
                ack_B = frame.seqnum;
                struct frm frm_B;
                frm_B.type = 1;
                frm_B.seqnum = -1;
                frm_B.acknum = ack_B;
                strcpy(frm_B.payload, "ign");
                frm_B.checksum = calculate_checksum_2(frm_B);
                printf("B sending: ");
                print_frm(frm_B);
                tolayer1(1, frm_B);

                outstanding_ack_B = true;
            }else{
                printf("B received: ");
                outstanding_ack_B = 1;
                ack_B = frame.seqnum;
                print_frm(frame);
                tolayer3(1, frame.payload);
            }
        }
    }else{
        printf("undefined type\n");
    }
}

/* called when B's time_r goes off */
void B_timerinterrupt(void)
{
    // printf("  B_timerinterrupt: B doesn't have a time_r. ignore.\n");
    printf("(B_timerinterrupt) B sending: ");
    if(outstanding_ack_B){
        global_frame_B.type = 2;
        global_frame_B.acknum = ack_A;
    }else{
        global_frame_B.type = 0;
        global_frame_B.acknum = -1;
    }
    print_frm(global_frame_B);
    tolayer1(1, global_frame_B);
    starttimer(1, 15.0);
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init(void)
{
    printf("B_init called\n");
    received_B = true;
    seq_num_B = 0;
    ack_B = -2;
}

/*****************************************************************
***************** NETWORK EMULATION CODE STARTS BELOW ***********
The code below emulates the layer 3 and below network environment:
    - emulates the tranmission and delivery (possibly with bit-level corruption
        and frame loss) of frames across the layer 3/20 interface
    - handles the starting/stopping of a time_r, and generates time_r
        interrupts (resulting in calling students time_r handler).
    - generates packet to be sent (passed from later 5 to 20)

THERE IS NOT REASON THAT ANY STUDENT SHOULD HAVE TO READ OR UNDERSTAND
THE CODE BELOW.  YOU SHOLD NOT TOUCH, OR REFERENCE (in your code) ANY
OF THE DATA STRUCTURES BELOW.  If you're interested in how I designed
the emulator, you're welcome to look at the code - but again, you should have
to, and you defeinitely should not have to modify
******************************************************************/

struct event
{
    float evtime;       /* event time_ */
    int evtype;         /* event type code */
    int eventity;       /* entity where event occurs */
    struct frm *frmptr; /* ptr to frame (if any) assoc w/ this event */
    struct event *prev;
    struct event *next;
};
struct event *evlist = NULL; /* the event list */

/* possible events: */
#define TIMER_INTERRUPT 0
#define FROM_layer3 1
#define FROM_layer1 2

#define OFF 0
#define ON 1
#define A 0
#define B 1

int TRACE = 1;     /* for my debugging */
int nsim = 0;      /* number of packets from 5 to 20 so far */
int nsimmax = 0;   /* number of pkts to generate, then stop */
float time_ = 0.000;
float lossprob;    /* probability that a frame is dropped  */
float corruptprob; /* probability that one bit is frame is flipped */
float lambda;      /* arrival rate of packets from layer 5 */
int ntolayer1;     /* number sent into layer 3 */
int nlost;         /* number lost in media */
int ncorrupt;      /* number corrupted by media*/

void init();
void generate_next_arrival(void);
void insertevent(struct event *p);

int main()
{
    #ifndef ONLINE_JUDGE
//        freopen("in", "r", stdin);
        // freopen("out", "w", stdout);
    #endif // ONLINE_JUDGE
    struct event *eventptr;
    struct pkt pkt2give;
    struct frm frm2give;

    int i, j;
    char c;

    init();
    A_init();
    B_init();

    while (1)
    {
        eventptr = evlist; /* get next event to simulate */
        if (eventptr == NULL)
            goto terminate;
        evlist = evlist->next; /* remove this event from event list */
        if (evlist != NULL)
            evlist->prev = NULL;
        if (TRACE >= 2)
        {
            printf("\nEVENT time_: %f,", eventptr->evtime);
            printf("  type: %d", eventptr->evtype);
            if (eventptr->evtype == 0)
                printf(", time_rinterrupt  ");
            else if (eventptr->evtype == 1)
                printf(", fromlayer3 ");
            else
                printf(", fromlayer1 ");
            printf(" entity: %d\n", eventptr->eventity);
        }
        time_ = eventptr->evtime; /* update time_ to next event time_ */
        if (eventptr->evtype == FROM_layer3)
        {
            if (nsim < nsimmax)
            {
                if (nsim + 1 < nsimmax)
                    generate_next_arrival(); /* set up future arrival */
                /* fill in pkt to give with string of same letter */
                j = nsim % 26;
                for (i = 0; i < 4; i++)
                    pkt2give.data[i] = 97 + j;
                pkt2give.data[3] = 0;
                if (TRACE > 2)
                {
                    printf("          MAINLOOP: data given to student: ");
                    for (i = 0; i < 4; i++)
                        printf("%c", pkt2give.data[i]);
                    printf("\n");
                }
                nsim++;
                if (eventptr->eventity == A)
                    A_output(pkt2give);
                else
                    B_output(pkt2give);
            }
        }
        else if (eventptr->evtype == FROM_layer1)
        {
            frm2give.seqnum = eventptr->frmptr->seqnum;
            frm2give.acknum = eventptr->frmptr->acknum;
            frm2give.checksum = eventptr->frmptr->checksum;
            frm2give.type = eventptr->frmptr->type;
            for (i = 0; i < 4; i++)
                frm2give.payload[i] = eventptr->frmptr->payload[i];

            if (eventptr->eventity == A) /* deliver frame by calling */
                A_input(frm2give); /* appropriate entity */
            else
                B_input(frm2give);
            free(eventptr->frmptr); /* free the memory for frame */
        }
        else if (eventptr->evtype == TIMER_INTERRUPT)
        {
            if (eventptr->eventity == A)
                A_timerinterrupt();
            else
                B_timerinterrupt();
        }
        else
        {
            printf("INTERNAL PANIC: unknown event type \n");
        }
        free(eventptr);
    }

    terminate:
    printf(
            " Simulator terminated at time_ %f\n after sending %d pkts from layer3\n",
            time_, nsim);
}

void init() /* initialize the simulator */
{
    int i;
    float sum, avg;
    float jimsrand();

    printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
    printf("Enter the number of packets to simulate: ");
    scanf("%d",&nsimmax);
    printf("Enter  frame loss probability [enter 0.0 for no loss]:");
    scanf("%f",&lossprob);
    printf("Enter frame corruption probability [0.0 for no corruption]:");
    scanf("%f",&corruptprob);
    printf("Enter average time_ between packets from sender's layer3 [ > 0.0]:");
    scanf("%f",&lambda);
    printf("Enter TRACE:");
    scanf("%d",&TRACE);

    srand(9999); /* init random number generator */
    sum = 0.0;   /* test random number generator for students */
    for (i = 0; i < 1000; i++)
        sum = sum + jimsrand(); /* jimsrand() should be uniform in [0,1] */
    avg = sum / 1000.0;
    if (avg < 0.25 || avg > 0.75)
    {
        printf("It is likely that random number generation on your machine\n");
        printf("is different from what this emulator expects.  Please take\n");
        printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
        exit(1);
    }

    ntolayer1 = 0;
    nlost = 0;
    ncorrupt = 0;

    time_ = 0.0;              /* initialize time_ to 0.0 */
    generate_next_arrival(); /* initialize event list */
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand(void)
{
    double mmm = RAND_MAX;
    float x;                 /* individual students may need to change mmm */
    x = rand() / mmm;        /* x should be uniform in [0,1] */
    return (x);
}

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/

void generate_next_arrival(void)
{
    double x, log(), ceil();
    struct event *evptr;
    float ttime;
    int tempint;

    if (TRACE > 2)
        printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");

    x = lambda * jimsrand() * 2; /* x is uniform on [0,2*lambda] */
    /* having mean of lambda        */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtime = time_ + x;
    evptr->evtype = FROM_layer3;
    if (BIDIRECTIONAL && (jimsrand() > 0.5))
        evptr->eventity = B;
    else
        evptr->eventity = A;
    insertevent(evptr);
}

void insertevent(struct event *p)
{
    struct event *q, *qold;

    if (TRACE > 2)
    {
        printf("            INSERTEVENT: time_ is %lf\n", time_);
        printf("            INSERTEVENT: future time_ will be %lf\n", p->evtime);
    }
    q = evlist;      /* q points to header of list in which p struct inserted */
    if (q == NULL)   /* list is empty */
    {
        evlist = p;
        p->next = NULL;
        p->prev = NULL;
    }
    else
    {
        for (qold = q; q != NULL && p->evtime > q->evtime; q = q->next)
            qold = q;
        if (q == NULL)   /* end of list */
        {
            qold->next = p;
            p->prev = qold;
            p->next = NULL;
        }
        else if (q == evlist)     /* front of list */
        {
            p->next = evlist;
            p->prev = NULL;
            p->next->prev = p;
            evlist = p;
        }
        else     /* middle of list */
        {
            p->next = q;
            p->prev = q->prev;
            q->prev->next = p;
            q->prev = p;
        }
    }
}

void printevlist(void)
{
    struct event *q;
    int i;
    printf("--------------\nEvent List Follows:\n");
    for (q = evlist; q != NULL; q = q->next)
    {
        printf("Event time_: %f, type: %d entity: %d\n", q->evtime, q->evtype,
               q->eventity);
    }
    printf("--------------\n");
}

/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started time_r */
void stoptimer(int AorB /* A or B is trying to stop time_r */)
{
    struct event *q, *qold;

    if (TRACE > 2)
        printf("          STOP TIMER: stopping time_r at %f\n", time_);
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB))
        {
            /* remove this event */
            if (q->next == NULL && q->prev == NULL)
                evlist = NULL;          /* remove first and only event on list */
            else if (q->next == NULL) /* end of list - there is one in front */
                q->prev->next = NULL;
            else if (q == evlist)   /* front of list - there must be event after */
            {
                q->next->prev = NULL;
                evlist = q->next;
            }
            else     /* middle of list */
            {
                q->next->prev = q->prev;
                q->prev->next = q->next;
            }
            free(q);
            return;
        }
    printf("Warning: unable to cancel your time_r. It wasn't running.\n");
}

void starttimer(int AorB /* A or B is trying to start time_r */, float increment)
{
    struct event *q;
    struct event *evptr;

    if (TRACE > 2)
        printf("          START TIMER: starting time_r at %f\n", time_);
    /* be nice: check to see if time_r is already started, if so, then  warn */
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB))
        {
            printf("Warning: attempt to start a time_r that is already started\n");
            return;
        }

    /* create future event for when time_r goes off */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtime = time_ + increment;
    evptr->evtype = TIMER_INTERRUPT;
    evptr->eventity = AorB;
    insertevent(evptr);
}

/************************** TOlayer1 ***************/
void tolayer1(int AorB, struct frm frame)
{
    struct frm *myfrmptr;
    struct event *evptr, *q;
    float lastime, x;
    int i;

    ntolayer1++;

    /* simulate losses: */
    if (jimsrand() < lossprob)
    {
        nlost++;
        if (TRACE > 0)
            printf("          TOlayer1: frame being lost\n");
        return;
    }

    /* make a copy of the frame student just gave me since he/she may decide */
    /* to do something with the frame after we return back to him/her */
    myfrmptr = (struct frm *)malloc(sizeof(struct frm));
    myfrmptr->seqnum = frame.seqnum;
    myfrmptr->acknum = frame.acknum;
    myfrmptr->checksum = frame.checksum;
    myfrmptr->type = frame.type;
    for (i = 0; i < 4; i++)
        myfrmptr->payload[i] = frame.payload[i];
    if (TRACE > 2)
    {
        printf("          TOlayer1: seq: %d, ack %d, check: %s ", myfrmptr->seqnum,
               myfrmptr->acknum, myfrmptr->checksum.c_str());
        for (i = 0; i < 4; i++)
            printf("%c", myfrmptr->payload[i]);
        printf("\n");
    }

    /* create future event for arrival of frame at the other side */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtype = FROM_layer1;      /* frame will pop out from layer1 */
    evptr->eventity = (AorB + 1) % 2; /* event occurs at other entity */
    evptr->frmptr = myfrmptr;         /* save ptr to my copy of frame */
    /* finally, compute the arrival time_ of frame at the other end.
       medium can not reorder, so make sure frame arrives between 1 and 10
       time_ units after the latest arrival time_ of frames
       currently in the medium on their way to the destination */
    lastime = time_;
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == FROM_layer1 && q->eventity == evptr->eventity))
            lastime = q->evtime;
    evptr->evtime = lastime + 1 + 9 * jimsrand();

    /* simulate corruption: */
    if (jimsrand() < corruptprob)
    {
        ncorrupt++;
        if ((x = jimsrand()) < .75)
            myfrmptr->payload[0] = 'Z'; /* corrupt payload */
        else if (x < .875)
            myfrmptr->seqnum = 999999;
        else
            myfrmptr->acknum = 999999;
        if (TRACE > 0)
            printf("          TOlayer1: frame being corrupted\n");
    }

    if (TRACE > 2)
        printf("          TOlayer1: scheduling arrival on other side\n");
    insertevent(evptr);
}

void tolayer3(int AorB, char datasent[4])
{
    int i;
    if (TRACE > 2)
    {
        printf("          TOlayer3: data received: ");
        for (i = 0; i < 4; i++)
            printf("%c", datasent[i]);
        printf("\n");
    }
}
