//
//
// Waveforms
//
//

#pragma once

static float Sine(float pha) {
	static float value;

	value = fastSin(TWO_PI * pha);

	return value;
}

/* Future use
static float Triangle(float pha) {
	static float value;

	value = ((pha < 0.5) * (4.0 * pha - 1.0)) + ((pha >= 0.5) * (1.0 - 4.0 * (pha - 0.5)));

	return value;
}

static float Saw(float pha) {
	static float value;

	value = fmod(fmod(pha, 1.0f) + 1.0f, 1.0f) * 2.0f - 1.0f;

	return value;
}

static float Ramp(float pha) {
	static float value;

	value = (1.0f - fmod(fmod(pha, 1.0f) + 1.0f, 1.0f) * 2.0f - 1.0f) + 1.0f;

	return value;
}

static float Square(float pha, float _pw) {
	static float value;

	value = (pha < _pw) ? 1.0f : -1.0f;

	return value;
}

static float HWRS(float pha) {
	static float value;

	value = (pha < 0.5f ? 2.0f * Sine(pha) - 2.0f / M_PI : - 2.0f / M_PI) - 0.4f;

	return value;
}

static float FWRS(float pha) {
	static float value;

	value = (2.0f * fastSin(M_PI * pha) - 4.0f / M_PI) + 0.3f;

	return value;
}

static float SawTri(float pha, float _pw) {
	static float value;

	if(pha < 0.5f)
		value = (-(2.f * pha < _pw) + fastSin(2.f * pha)) / 1.0f;
	else
	value = (-(-2.f + 2.f * pha) - (-2.f + 2.f * pha < _pw)) / 1.0f;

	return value;
}

static float SineSine(float pha) {
	static float value;

	value = (2.0f * Sine(pha) + 1.0f * fastSin(4.0f * M_PI * pha)) / 2.5f;

	return value;
}

static float SineSineSine(float pha) {
	static float value;

	value = (2.0f * Sine(pha) + 1.0f * fastSin(4.0f * M_PI * pha) + 1.5 * fastSin(8.0f* M_PI * pha)) / 3.0f;

	return value;
}

static float SineSineSineSine(float pha) {
	static float value;

	value = (2.0f * Sine(pha) + 1.0f * fastSin(4.0f * M_PI * pha) + 1.5 * fastSin(8.0f* M_PI * pha) + 2.0 * fastSin(10.0f* M_PI * pha)) / 4.5f;

	return value;
}

static float SawSine(float pha) {
	static float value;

	value = ((Saw(TWO_PI * pha) + 2.0f * Sine(pha) - 2.0f / M_PI) / 2.8f) + 0.2f;

	return value;
}

static float SawSine2(float pha) {
	static float value;

	value = (((2.f * pha) + 1.0f * (4.f * pha) + 1.5f * fastSin(8.f * M_PI * pha) - 4.0f / M_PI) / 3.5f) - 0.50f;

	return value;
}
*/
