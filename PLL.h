
/**
 * @brief Initialize the Phase-Locked Loop (PLL).
 *
 * This function initializes the Phase-Locked Loop (PLL) control parameters.
 * It calculates coefficients for notch and proportional-integral (PI) controllers.
 *
 * @see control_get_fn()
 */
void control_PLL_init();

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
float control_PLL_a(float voltage);

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
float control_PLL_b(float voltage);

/**
 * @brief Returns a pointer to an array containing the current values of the PLL frequencies.
 *
 * @return Pointer to an array containing the current values of the PLL frequencies.
 */
float * control_PLL_get_w(void);

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
uint8_t control_PLL_phase_seq(float theta1, float theta2);