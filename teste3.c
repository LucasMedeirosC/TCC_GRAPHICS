#include <stdio.h>
#include <math.h>
#include <stdint.h>

/* -------- DEFINIÇÕES -------- */

#define PI 3.14159265358979323846f
#define PI2 (2.0f * PI)

#define FS 10000.0f
#define Ts (1.0f / FS)

#define TEST_TIME 1.0f

#define PLL_VARIABLES_NUMBER 2
#define WR_RADS 0
#define WS_RADS 1

static uint8_t f_nom = 60;
static float w_nom = 60.0f * PI2;

static float c1, c2;
static float C1, C2, C3;

static float A_notch[3] = {0.0f, 0.0f, 0.0f};
static float B_notch[3] = {0.0f, 0.0f, 0.0f};

static float Kp, Ki;
static float B_pi[2] = {0.0f, 0.0f};

static float x_notch_a[3] = {0.0f, 0.0f, 0.0f};
static float y_notch_a[3] = {0.0f, 0.0f, 0.0f};
static float y_pi_a[2] = {0.0f, 0.0f};
static float theta_pll_a[2] = {0.0f, 0.0f};
static float w_pll_a[2] = {0.0f, 0.0f};

void control_PLL_init(void)
{
    w_nom = (float)f_nom * PI2;

    c1 = 0.1f;
    c2 = 0.00001f;
    C1 = 4.0f * c2 * (2.0f * w_nom) * Ts;
    C2 = 4.0f * c1 * (2.0f * w_nom) * Ts;
    C3 = (2.0f * w_nom) * (2.0f * w_nom) * Ts * Ts;

    B_notch[0] = 4.0f + C1 + C3;
    B_notch[1] = -8.0f + 2.0f * C3;
    B_notch[2] = 4.0f - C1 + C3;

    A_notch[0] = 4.0f + C2 + C3;
    A_notch[1] = -8.0f + 2.0f * C3;
    A_notch[2] = 4.0f - C2 + C3;

    Kp = 166.6f / 170.0f;
    Ki = 27755.55f / 170.0f;

    B_pi[0] = (2.0f * Kp + Ki * Ts) * 0.5f;
    B_pi[1] = -(2.0f * Kp - Ki * Ts) * 0.5f;
}

float control_PLL_a(float voltage)
{
    x_notch_a[2] = x_notch_a[1];
    x_notch_a[1] = x_notch_a[0];
    x_notch_a[0] = voltage * cosf(theta_pll_a[0]);

    y_notch_a[2] = y_notch_a[1];
    y_notch_a[1] = y_notch_a[0];
    y_notch_a[0] = (-A_notch[1] * y_notch_a[1] - A_notch[2] * y_notch_a[2] + B_notch[0] * x_notch_a[0] + B_notch[1] * x_notch_a[1] + B_notch[2] * x_notch_a[2]) / A_notch[0];

    y_pi_a[1] = y_pi_a[0];
    y_pi_a[0] = y_pi_a[1] + B_pi[0] * y_notch_a[0] + B_pi[1] * y_notch_a[1];

    w_pll_a[1] = w_pll_a[0];
    w_pll_a[0] = w_nom + y_pi_a[0];

    theta_pll_a[1] = theta_pll_a[0];
    theta_pll_a[0] = theta_pll_a[1] + 0.5f * Ts * w_pll_a[0] + 0.5f * Ts * w_pll_a[1];

    if (theta_pll_a[0] > PI2)
        theta_pll_a[0] -= PI2;
    else if (theta_pll_a[0] < -PI2)
        theta_pll_a[0] += PI2;

    return theta_pll_a[0];
}

float *control_PLL_get_w(void)
{
    static float w_pll[PLL_VARIABLES_NUMBER] = {0.0f, 0.0f};
    w_pll[WR_RADS] = w_pll_a[0];
    w_pll[WS_RADS] = 0.0f;
    return w_pll;
}

/* -------- SINAL DE TESTE -------- */

float generate_grid(float t)
{
    float freq = 60.0f;
    float amplitude = 311.0f; // 220Vrms

    return amplitude * sinf(PI2 * freq * t);
}

/* -------- MAIN -------- */

int main()
{

    control_PLL_init();

    float t = 0;

    int samples = (int)(FS * TEST_TIME);
    int i = 0;

    while (i < samples)
    {

        /* gera senoide da rede */
        float v = generate_grid(t);

        /* roda PLL */
        float theta = control_PLL_a(v);

        float *w = control_PLL_get_w();

        float freq_est = w[WR_RADS] / PI2;

        printf("t=%f freq=%f Hz\n",
               theta,
               freq_est);

        t += Ts;
        i++;
    }

    return 0;
}