#include <math.h>
#include "PLL.h"

/** This variable represents the nominal frequency used in the system. */
static uint8_t f_nom = 60;

/** This variable represents the nominal angular frequency used in the system. */
static float w_nom = 60.f*PI2;

/** Constants defining the filtering behavior. */
static float c1 , c2;

/** Constants for notch filter coefficients calculation. */
static float C1, C2, C3;

/** Coefficients for the notch filter. */
static float A_notch[3] = {};

/** Coefficients for the notch filter. */
static float B_notch[3] = {};

/** Proportional and Integral gains for the PI loop filter. */
static float Kp , Ki;

/** Coefficients for the PI loop filter. */
static float B_pi[2] = {};

/** Notch filter state variables for PLL function 'pll_a()'. */
static float x_notch_a[3] = {};  		/**< Stores the current and previous notch input value. */
static float y_notch_a[3] = {};  		/**< Stores the current and previous notch output value. */
static float y_pi_a[2] = {};     		/**< Stores the current and previous output of the PI loop filter. */
static float theta_pll_a[2] = {};  		/**< Stores the current and previous angle value. */
static float w_pll_a[2] = {};     		/**< Stores the current and previous state of the integrator. */

/** Notch filter state variables for PLL function 'pll_b()'. */
static float x_notch_b[3] = {};  		/**< Stores the current and previous notch input value. */
static float y_notch_b[3] = {};  		/**< Stores the current and previous notch output value. */
static float y_pi_b[2] = {};     		/**< Stores the current and previous output of the PI loop filter. */
static float theta_pll_b[2] = {};  		/**< Stores the current and previous angle value. */
static float w_pll_b[2] = {};     		/**< Stores the current and previous state of the integrator. */

/**
 * @brief Initialize the Phase-Locked Loop (PLL).
 *
 * This function initializes the Phase-Locked Loop (PLL) control parameters.
 * It calculates coefficients for notch and proportional-integral (PI) controllers.
 *
 * @see control_get_fn()
 */
void control_PLL_init()
{
    // Obtain nominal frequency and calculate angular frequency

    w_nom = (float)f_nom * PI2;

    // Calculate notch filter coefficients
    c1 = 0.1;
    c2 = 0.00001;
    C1 = 4 * c2 * (2 * w_nom) * Ts;
    C2 = 4 * c1 * (2 * w_nom) * Ts;
    C3 = (2 * w_nom) * (2 * w_nom) * Ts * Ts;

    B_notch[0] =  4.f + C1 + C3;
    B_notch[1] = -8.f +  2 * C3;
    B_notch[2] =  4.f - C1 + C3;
    A_notch[0] =  4.f + C2 + C3;
    A_notch[1] = -8.f +  2 * C3;
    A_notch[2] =  4.f - C2 + C3;

    // Design PI controller
    Kp = 166.6f / 170.f;
    Ki = 27755.55f / 170.f;
    B_pi[0] = (2.f * Kp + Ki * Ts) * 0.5f;
    B_pi[1] = -(2.f * Kp - Ki * Ts) * 0.5f;
}

/**
 * @brief Computes the position angle corresponding to a given voltage using
 *        Phase-Locked Loop (PLL) A algorithm.
 *
 * This function calculates the position angle associated with a given voltage
 * using the Phase-Locked Loop (PLL) algorithm.
 *
 * @param voltage The first input voltage for which the position angle is to be
 *                determined.
 * @return The position angle corresponding to the first input voltage.
 */
float control_PLL_a(float voltage)
{
	/** Phase Detect. */
	x_notch_a[2] = x_notch_a[1]; 							/**< Previous notch value stored for phase detect. */
	x_notch_a[1] = x_notch_a[0]; 							/**< Last notch value stored for phase detect. */
	x_notch_a[0] = voltage*(cosf(theta_pll_a[0]));	/**< Current notch value calculated based on the input voltage and PLL angle */

	/** Notch Filter. */
	y_notch_a[2] = y_notch_a[1];							/**< Previous notch output stored for notch filter */
	y_notch_a[1] = y_notch_a[0];							/**< Last notch output stored for notch filter. */
	y_notch_a[0] = (- A_notch[1]*y_notch_a[1] - A_notch[2]*y_notch_a[2] + B_notch[0]*x_notch_a[0] + B_notch[1]*x_notch_a[1] + B_notch[2]*x_notch_a[2])/A_notch[0];	/**< Current notch output calculated using filter coefficients. */

	/** PI Loop Filter. */
	y_pi_a[1] = y_pi_a[0];									/**< Previous output of PI loop filter stored. */
	y_pi_a[0] = y_pi_a[1] + B_pi[0]*y_notch_a[0] + B_pi[1]*y_notch_a[1];	/**< Current output of PI loop filter calculated based on notch filter output. */

	/** Integrator. */
	w_pll_a[1] = w_pll_a[0];								/**< Previous integrator state stored. */
	w_pll_a[0] = w_nom + y_pi_a[0];							/**< Current integrator state updated based on PI loop filter output. */

	/** Angle detection. */
	theta_pll_a[1] = theta_pll_a[0];						/**< Previous PLL angle stored. */
	theta_pll_a[0] = theta_pll_a[1] + 0.5f*Ts*w_pll_a[0] + 0.5f*Ts*w_pll_a[1];	/**< Current PLL angle calculated based on previous angle and current integrator state. */

	/** Adjust angle within range [-PI, PI]. */
	if(theta_pll_a[0] > PI2)
		theta_pll_a[0] -= PI2;								/**< Adjust angle if it exceeds PI2. */
	else if (theta_pll_a[0] < -PI2)
		theta_pll_a[0] += PI2;								/**< Adjust angle if it goes below -PI2. */

	return theta_pll_a[0];									/**< Return the calculated angle position. */
}

/**
 * @brief Computes the position angle corresponding to a given voltage using
 *        Phase-Locked Loop (PLL) B algorithm.
 *
 * This function calculates the position angle associated with a given voltage
 * using the Phase-Locked Loop (PLL) algorithm.
 *
 * @param voltage The second input voltage for which the position angle is to be
 *                determined.
 * @return The position angle corresponding to the second input voltage.
  */
float control_PLL_b(float voltage)
{
	/** Phase Detect. */
	x_notch_b[2] = x_notch_b[1]; 							/**< Previous notch value stored for phase detect. */
	x_notch_b[1] = x_notch_b[0]; 							/**< Last notch value stored for phase detect. */
	x_notch_b[0] = voltage*(cosf(theta_pll_b[0]));	/**< Current notch value calculated based on the input voltage and PLL angle */

	/** Notch Filter. */
	y_notch_b[2] = y_notch_b[1];							/**< Previous notch output stored for notch filter */
	y_notch_b[1] = y_notch_b[0];							/**< Last notch output stored for notch filter. */
	y_notch_b[0] = (- A_notch[1]*y_notch_b[1] - A_notch[2]*y_notch_b[2] + B_notch[0]*x_notch_b[0] + B_notch[1]*x_notch_b[1] + B_notch[2]*x_notch_b[2])/A_notch[0];	/**< Current notch output calculated using filter coefficients. */

	/** PI Loop Filter. */
	y_pi_b[1] = y_pi_b[0];									/**< Previous output of PI loop filter stored. */
	y_pi_b[0] = y_pi_b[1] + B_pi[0]*y_notch_b[0] + B_pi[1]*y_notch_b[1];	/**< Current output of PI loop filter calculated based on notch filter output. */

	/** Integrator. */
	w_pll_b[1] = w_pll_b[0];								/**< Previous integrator state stored. */
	w_pll_b[0] = w_nom + y_pi_b[0];							/**< Current integrator state updated based on PI loop filter output. */

	/** Angle detection. */
	theta_pll_b[1] = theta_pll_b[0];						/**< Previous PLL angle stored. */
	theta_pll_b[0] = theta_pll_b[1] + 0.5f*Ts*w_pll_b[0] + 0.5f*Ts*w_pll_b[1];	/**< Current PLL angle calculated based on previous angle and current integrator state. */

	/** Adjust angle within range [-PI, PI]. */
	if(theta_pll_b[0] > PI2)
		theta_pll_b[0] -= PI2;								/**< Adjust angle if it exceeds PI2. */
	else if (theta_pll_b[0] < -PI2)
		theta_pll_b[0] += PI2;								/**< Adjust angle if it goes below -PI2. */

	return theta_pll_b[0];									/**< Return the calculated angle position. */
}

/**
 * @brief Returns a pointer to an array containing the current values of the PLL frequencies.
 *
 * @return Pointer to an array containing the current values of the PLL frequencies.
 */
float * control_PLL_get_w(void)
{
	/**< Array to hold the current values of the PLL frequencies. */
    static float w_pll[PLL_VARIABLES_NUMBER] = {};

    /**< Assign the value of the PLL A frequency to the array. */
    w_pll[WR_RADS] = w_pll_a[0];

    /**< Assign the value of the PLL B frequency to the array. */
    w_pll[WS_RADS] = w_pll_b[0];

    /**< Return a pointer to the array containing the PLL frequencies. */
    return w_pll;
}

/**
 * @brief Determines the phase sequence based on two voltage positions.
 *
 * This function calculates the phase sequence based on the positions of two
 * voltages. It compares the positions of the voltages represented by their
 * angles and determines the sequence in which the phases are arranged.
 *
 * @param theta1 The position angle of the first voltage.
 * @param theta2 The position angle of the second voltage.
 * @return An integer representing the phase sequence:
 *         -1 if the phase sequence is negative
 *          1 if the phase sequence is positive
 */
uint8_t control_PLL_phase_seq(float theta1, float theta2)
{
	/** Calculate the angular difference between the two voltage positions. */
	float error = theta1 - theta2;
    int seq;

	/** Determine the phase sequence based on the error. */
    if(error > PI)
        seq = -1; 			/**< Negative sequence. */
    else if (error > 0)
        seq = 1;  			/**< Positive sequence. */
    else if (error > -PI)
        seq = -1; 			/**< Negative sequence. */
    else
        seq = 1;  			/**< Positive sequence. */

    return seq;				/**< Return the identified phase sequence. */
}
