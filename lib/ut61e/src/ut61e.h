/*
 * ut61e.h
 *
 *  Created on: 12.11.2009
 *      Author: steffen vogel
 *         git: https://github.com/stv0g/dmm_ut61e
 */

#ifndef UT61E_H_
#define UT61E_H_


class UT61E {
public:
	UT61E();
	virtual ~UT61E();
	bool check(char * data);
	void parse(char * data);
	const char* getMode();
	const char* getPower();
	const char* getRange();
	const char* getFMode();

	double value, max, min, average;
	long sample;
	char data[14];

	bool hold, rel, bat;

	enum { M_VOLTAGE, M_CURRENT, M_RESISTANCE, M_DIODE, M_FREQUENCY, M_CAPACITANCE, M_INDUCTANCE, M_CONDUCTANCE } mode, lastmode;
	enum { F_DUTY, F_FREQUENCY } fmode;
	enum { AC, DC } power;
	enum { AUTO, MANUAL } range;
	enum { OVERLOAD, NORMAL, UNDERLOAD } load;
	enum { MAX, MIN } peak;

private:
	static const char* modelbl[];
	static const char* fmodelbl[];
	static const char* powerlbl[];
	static const char* rangelbl[];
	static const char* loadlbl[];
	static const char* peaklbl[];
};

#endif /* UT61E_H_ */
