#include <msp430.h>
#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <libalpaca/alpaca.h>
#include <libmsp/mem.h>
#include <libmsp/watchdog.h>
#include <libPF/PF_sim.h>
#include "vad.h"
#include "libmfcc/libmfcc.h"

#include "SC/params.h"
#include "SC/weights.h"

#define SAMPLE_RATE 8000
#define FRAME_SIZE_VAD 160
#define HOP_SIZE 200
#define N_FEATS 13
#define TOT_FRAMES SAMPLE_RATE / HOP_SIZE // 40

GLOBAL_SB2(bool, previous_frame_was_speech);

GLOBAL_SB2(int, counter_valid_frames);
GLOBAL_SB2(int, counter_vad);
GLOBAL_SB2(int, spectrum_index);
GLOBAL_SB2(int, detected_speech);
GLOBAL_SB2(int, i);

GLOBAL_SB2(unsigned int, coeff);
GLOBAL_SB2(unsigned int, vx);

GLOBAL_SB2(double complex, frame_vad[FRAME_SIZE_VAD]);
GLOBAL_SB2(double complex, freq_frame[FRAME_SIZE_VAD]);
GLOBAL_SB2(double, spectrum[FRAME_SIZE_VAD * TOT_FRAMES]);
GLOBAL_SB2(double, mfcc_result[N_FEATS]);

#define MEM_SIZE 0x4
__nv uint8_t *data_src[MEM_SIZE];
__nv uint8_t *data_dest[MEM_SIZE];
__nv unsigned int data_size[MEM_SIZE];
__nv uint8_t exacution_counter = 0;
void clear_isDirty()
{
}

void init();
void task_init();
void getSample();
void allocateVAD();
void processVAD();
void computeFFT();
void nextPath();
void computeMFCC();
void kwsAlg();
void task_end();

TASK(1, init)
TASK(2, task_init)
TASK(3, getSample)
TASK(4, allocateVAD)
TASK(5, processVAD)
TASK(6, computeFFT)
TASK(7, nextPath)
TASK(8, computeMFCC)
TASK(9, kwsAlg)
TASK(10, task_end)

unsigned overflow = 0;

int powof2(int a, int b)
{
    return 1 << b;
}

double neuron(double weights[], double bias, double inputs[], int input_size)
{
    /**
     * Compute the output of a neuron using ReLU as activation.
     * :weights: the weights of the neuron.
     * :bias: the bias of the neuron.
     * :inputs: the inputs to the neuron.
     */

    int volatile i;
    i = 0;
    double accumulator = 0;
    for (i; i < input_size; ++i)
    {
        accumulator += weights[i] * inputs[i];
    }
    accumulator += bias;
    return accumulator;
}

int leaf(int leaf_id, double *input)
{
    /*
     * Compute the output of a leaf
     * :leaf_id: the id (index) of the leaf
     * :input: the input to the leaf
     */
    double hidden[LEAF_WIDTH];
    double output[OUTPUT_SIZE];

    // msp_status msp_mac_q15(const msp_mac_q15_params *params, const _q15 *srcA, const _q15 *srcB, _iq31 *result)
    int argmax = 0;

    // uint32_t cycleCount = 0;
    int i;
    for (i = 0; i < LEAF_WIDTH; ++i)
    {
        hidden[i] = hidden[i] < 0 ? 0 : hidden[i];
    }
    // // printf("Total Cycles: %u\n", cycleCount);
    for (i = 0; i < OUTPUT_SIZE; ++i)
    {
        output[i] = neuron(LEAF_OUTPUT_WEIGHTS[leaf_id][i],
                           LEAF_OUTPUT_BIASES[leaf_id][i], hidden, LEAF_WIDTH);
    }
    for (i = 0; i < OUTPUT_SIZE; ++i)
    {
        if (output[i] > output[argmax])
        {
            argmax = i;
        }
    }
    // // printf("argmax idx: %d, max val: %d", argmax, output[argmax]);

    return argmax;
}

int tree_inference(double *input)
{
    double tdest = 0.0;
    int cur_node = 0;
    int platform = 0;
    int next_platform = 0;
    int choice = 0;
    int depth;
    int i;

    for (depth = 0; depth < DEPTH; depth++)
    {
        tdest = neuron(NODE_WEIGHTS[cur_node], NODE_BIASES[cur_node], input,
        INPUT_SIZE);
        platform = powof2(2, depth) - 1;
        next_platform = powof2(2, (depth + 1)) - 1;
        choice = tdest >= 0 ? 1 : 0;
        cur_node = (cur_node - platform) * 2 + choice + next_platform;
    }
    cur_node = (cur_node - next_platform);
    return leaf(cur_node, input); // 0 is no, 1 means unknown, 2 is noise, 3 is yes
}

void configureADC(void)
{
    // Configure GPIO for ADC (P3.2 as ADC input, A10)
    P3SEL0 |= BIT2;  // Configure P3.2 for ADC
    P3SEL1 |= BIT2;

    // Configure ADC12_B
    ADC12CTL0 &= ~ADC12ENC;          // Disable ADC12_B before configuration
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;           // Sampling time, ADC12 on
    ADC12CTL1 = ADC12SHP;             // Use sampling timer
    ADC12MCTL0 = ADC12INCH_10;        // ADC input channel A10 (P3.2)
    ADC12IER0 = ADC12IE0;        // Enable ADC conversion complete interrupt

    // Enable ADC12_B
    ADC12CTL0 |= ADC12ENC;
}

void configureTimer(void)
{
    // Configure Timer_A to trigger ADC at 8kHz (125 �s period)
    TA0CCR0 = 125 - 1;           // Set Timer A period (assuming 1MHz clock)
    TA0CCTL0 = CCIE;                  // Enable Timer A interrupt
    TA0CTL = TASSEL_2 | MC_1 | TACLR; // SMCLK, up mode, clear TAR
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
    ADC12CTL0 |= ADC12SC;  // Start ADC conversion
}

static void init_hw()
{
    msp_watchdog_disable();
    PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode
    // P3DIR ^= 0xFF;
    //  P3OUT ^=0x04;
    //  P3OUT ^=0x04;
    //msp_gpio_unlock();
    //msp_clock_setup();
}

void init()
{
    init_hw();
    PF_sim_start();
    //INIT_CONSOLE();

    configureADC();
    configureTimer();

    __bis_SR_register(LPM0_bits + GIE);
    __enable_interrupt();

#pragma vector=ADC12_B_VECTOR
    __interrupt void ADC12_ISR(void)
    {
        switch (__even_in_range(ADC12IV, ADC12IV_ADC12IFG0))
        {
        case ADC12IV_NONE:
            break;            // No interrupt
        case ADC12IV_ADC12IFG0:              // ADC12MEM0 interrupt flag

            TRANSITION_TO(getSample);

            break;
        default:
            break;
        }
    }
}

void task_init()
{
    GV(counter_valid_frames) = 0;
    GV(counter_vad) = 0;
    GV(result) = 0;
    GV(coeff) = 0;
    GV(previous_frame_was_speech) = FALSE;

    TRANSITION_TO(getSample);
}

void getSample()
{
    GV(vx) = ADC12MEM0;

    GV(frame_vad[GV(counter_vad]))= GV(vx);

    TRANSITION_TO(allocateVAD);
}

void allocateVAD()
{
    if (GV(counter_vad) == FRAME_SIZE_VAD - 1)
    {
        if (GV(vad) == NULL)
        {
            GV(vad) = fvad_new();
            if (GV(vad) == NULL)
            {
                // printf("Memory allocation failed for VAD\n");
                return;
            }
        }
        TRANSITION_TO(processVAD);
    }
    else
    {
        GV(counter_vad)++;
    }
    TRANSITION_TO(getSample);
}

void processVAD()
{
    GV(result) = fvad_process(GV(vad), GV(frame_vad), FRAME_SIZE_VAD);
    TRANSITION_TO(computeFFT);
}

void computeFFT()
{
    if (GV(result) == 1)
    {
        // printf("Active Voice\n");

        fft((int*) GV(frame_vad), GV(freq_frame), FRAME_SIZE_VAD);

        GV(spectrum[GV(counter_result)])= GV(freq_frame[0]);

        // printf("Counter Result: %d\n", counter_result);

        TRANSITION_TO(nextPath);
    }
    TRANSITION_TO(getSample);
}

void nextPath()
{
    if (GV(counter_result) == TOT_FRAMES - 1)
    {
        GV(counter_result) = 0;
        TRANSITION_TO(computeMFCC);
    }
    else
    {
        GV(counter_result++);
    }
    TRANSITION_TO(getSample);
}

void computeMFCC()
{
    for (GV(coeff) = 0; GV(coeff) < N_FEATS; GV(coeff++))
    {
        GV(mfcc_result[GV(coeff)])= GetCoefficient(GV(spectrum),
                SAMPLE_RATE, 48,
                256, GV(coeff));

        // printf("%i %f\n", coeff, mfcc_result[coeff]);
    }

    TRANSITION_TO(kwsAlg);
}

void kwsAlg()
{
    GV(result) = tree_inference(&GV(mfcc_result));

    fvad_free(GV(vad));
    GV(counter_vad) = 0;

    TRANSITION_TO(task_end);
}

void task_end()
{
    P3OUT ^= 0x01;
    P3OUT ^= 0x01;
    exacution_counter++;
    TRANSITION_TO(init);
}

ENTRY_TASK(task_init)
INIT_FUNC(init)
