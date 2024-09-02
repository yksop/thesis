#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include "fft-small/fft.h"
// #include "libfvad/include/fvad.h"
#include "libmfcc/libmfcc.h"

#include "SC/params.h"
#include "SC/weights.h"
// #include "SC/input.h"

#define CONT_RUN
#define TILE_SIZE 784

// suggested 8kHZ and a frame size of about 80 to 240
// I can declare audio buffer as a short and then copy it into a 16-bytes floating point

#define SAMPLE_RATE 8000
#define FRAME_SIZE_VAD 5
#define HOP_SIZE 200
#define N_FEATS 13
#define MFCC_TIME_SIZE SAMPLE_RATE / HOP_SIZE // 40
#define MFCC_SIZE N_FEATS * MFCC_TIME_SIZE
#define N_MELS 400
#define TOT_FRAMES 2

// Moved large arrays to FRAM
//#pragma DATA_SECTION(frame_vad, ".fram_data")
double complex frame_vad[FRAME_SIZE_VAD];

//#pragma DATA_SECTION(freq_frame, ".fram_data")
double complex freq_frame[FRAME_SIZE_VAD];

//#pragma DATA_SECTION(spectrum, ".fram_data")
double spectrum[FRAME_SIZE_VAD];

//#pragma DATA_SECTION(mfcc_result, ".fram_data")
double mfcc_result[N_FEATS];

int counter_vad = 0;
int counter_result = 0; // counter for final fft result with all 31 frames

unsigned int vx;
unsigned int averageVX;
int result;
int coeff;
// volatile unsigned int overflow_counter = 0;

void configureADC(void);
void configureTimer(void);

void configureADC(void)
{
    // Configure GPIO for ADC (P3.2 as ADC input, A10)
    P3SEL0 |= BIT2;  // Configure P3.2 for ADC
    P3SEL1 |= BIT2;

    // Configure ADC12_B
    ADC12CTL0 &= ~ADC12ENC;           // Disable ADC12_B before configuration
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;           // Sampling time, ADC12 on
    ADC12CTL1 = ADC12SHP;             // Use sampling timer
    ADC12MCTL0 = ADC12INCH_10;        // ADC input channel A10 (P3.2)
    ADC12IER0 = ADC12IE0;            // Enable ADC conversion complete interrupt

    // Enable ADC12_B
    ADC12CTL0 |= ADC12ENC;
}

void configureTimer(void)
{
    // Configure Timer_A to trigger ADC at 8kHz (125 �s period)
    TA0CCR0 = 125 - 1;               // Set Timer A period (assuming 1MHz clock)
    TA0CCTL0 = CCIE;                  // Enable Timer A interrupt
    TA0CTL = TASSEL_2 | MC_1 | TACLR; // SMCLK, up mode, clear TAR
}

// Timer_A interrupt service routine
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
    ADC12CTL0 |= ADC12SC;  // Start ADC conversion
}

#pragma vector=ADC12_B_VECTOR
__interrupt void ADC12_ISR(void)
{
    switch (__even_in_range(ADC12IV, ADC12IV_ADC12IFG0))
    {
    case ADC12IV_NONE:
        break;            // No interrupt
    case ADC12IV_ADC12IFG0:              // ADC12MEM0 interrupt flag
        vx = ADC12MEM0;
        frame_vad[counter_vad] = vx;
        averageVX += vx;

        if (counter_vad == FRAME_SIZE_VAD - 1)
        {
            averageVX = averageVX / FRAME_SIZE_VAD;
            if (averageVX > 1000)
            {
                printf("Active Voice\n");

                fft(frame_vad, freq_frame, FRAME_SIZE_VAD);

                spectrum[counter_result] = freq_frame[0];

                printf("Counter Result: %d\n", counter_result);

                if (counter_result == TOT_FRAMES - 1)
                {
                    printf("FFT Result:\n");
                    int i;

                    counter_result = 0;
                    for (coeff = 0; coeff < 1; coeff++)
                    {
                        printf("hi\n");
                        mfcc_result[coeff] = GetCoefficient(spectrum,
                        SAMPLE_RATE,
                                                            48, 256, coeff);

                        // printf("%d %d\n", coeff, tmp);
                    }

                    result = tree_inference(&mfcc_result); // array mffc_results

                    printf("Result: %d\n", result);

                    exit(0);
                }
                else
                {
                    counter_result++;
                }
            }
            else
            {
                printf("Non-active Voice\n");
            }
            counter_vad = 0;
        }
        else
        {
            counter_vad++;
        }

        break;
    default:
        break;
    }
}

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

void main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    WDTCTL = WDTPW | WDTHOLD;  // Stop Watchdog Timer

    configureADC();
    configureTimer();

    __bis_SR_register(LPM0_bits + GIE);
}
