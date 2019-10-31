/*

	Delay module for VCV Rack written by Michael Struggl.
	Based on Andrew Belt's Delay module for VCV Rack.

*/

#include "MSM.hpp"
#include "MSMComponentLibrary.hpp"

class OP {
public:
    OP()
	{

	}

	inline void reset ()
    {
        a0 = 1.0;
        b1 = z1 = 0.0;
    }

	inline void setFc(float Fc) {
		b1 = exp(-2.0 * M_PI * Fc);
		a0 = 1.0 - b1;
	}

	inline float process(float in) {
		return z1 = in * a0 + z1 * b1;
	}


protected:
    float a0 = 1.0f;
	float b1 = 0.0f;
	float z1 = 0.0f;
};



struct Delay : Module {
	enum ParamIds {
		SYNCA_PARAM,
		SYNCB_PARAM,
		TIME_A_PARAM,
		TIME_B_PARAM,
		FEEDBACK_A_PARAM,
		FEEDBACK_B_PARAM,
		LP_A_PARAM,
		HP_A_PARAM,
		LP_B_PARAM,
		HP_B_PARAM,
		MIX_A_PARAM,
		MIX_B_PARAM,
		LEVEL_A,
		LEVEL_B,
		CROSSFEED_A,
		CROSSFEED_B,
		CLEAR_A,
		CLEAR_B,
		NUM_PARAMS
	};
	enum InputIds {
		CLOCK_INPUT,
		TIME_A_INPUT,
		TIME_B_INPUT,
		FEEDBACK_A_INPUT,
		FEEDBACK_B_INPUT,
		LP_A_INPUT,
		HP_A_INPUT,
		LP_B_INPUT,
		HP_B_INPUT,
		MIX_A_INPUT,
		MIX_B_INPUT,
		IN_A_INPUT,
		IN_B_INPUT,
		LEVEL_A_CV,
		LEVEL_B_CV,
		CLEAR_A_INPUT,
		CLEAR_B_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_A_OUTPUT,
		OUT_B_OUTPUT,
		WET_A_OUTPUT,
		WET_B_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightIds {
		CLEAR_A_LIGHT,
		CLEAR_B_LIGHT,
		NUM_LIGHTS
	};

	OP smofilterA;
	OP smofilterB;

	dsp::DoubleRingBuffer<float, HISTORY_SIZE_A> historyBufferA;
	dsp::DoubleRingBuffer<float, HISTORY_SIZE_B> historyBufferB;

	dsp::DoubleRingBuffer<float, 16> outBufferA;
	dsp::DoubleRingBuffer<float, 16> outBufferB;

	dsp::SampleRateConverter<1> srcA;
	dsp::SampleRateConverter<1> srcB;

	float delayA = 0.0f;
	float delayB = 0.0f;

	float wetfiltA = 0.0f;
	float wetfiltB = 0.0f;

	float lastWetA = 0.0f;
	float lastWetB = 0.0f;

	float indexA = 0.0f;
	float indexB = 0.0f;

	dsp::RCFilter lowpassFilterA;
	dsp::RCFilter highpassFilterA;

	dsp::RCFilter lowpassFilterB;
	dsp::RCFilter highpassFilterB;

	int Theme = 0;
	int DISPLAYA = 0;
	int DISPLAYB = 0;
	/////////////////////////////////////

	int lcd_tempoA = 0.0f;
	int lcd_tempoB = 0.0f;
	char lcd_TRatioA = '*';
	char lcd_TRatioB = '*';

	/////////////////////////////////////
	float SYNCa = 0.0f;
	float SYNCb = 0.0f;

	dsp::SchmittTrigger clockTriggerA, clockTriggerB;
	dsp::SchmittTrigger clear_a, clear_b;

	float clearA_light = 0.0f;
	float clearB_light = 0.0f;

	float divisions[DIVISIONS] = { 0.02083f, 0.03125f, 0.04688f, 0.04166f, 0.0625f, 0.09375f, 0.08333f, 0.125f, 0.1875f, 0.16666f, 0.25f, 0.375f, 0.33333f, 0.5f, 0.75f, 0.66666f, 1.0f, 1.5f, 1.33332f, 2.0f, 3.0f, 4.0f, 8.0f, 16.0f, 32.0f};
	const char* divisionNames[DIVISIONS] = { "1/128 T", "1/128", "1/128 D", "1/64 T", "1/64", "1/64 D", "1/32 T", "1/32", "1/32 D", "1/16 T", "1/16", "1/16 D", "1/8 T", "1/8", "1/8 D", "1/4 T", "1/4","1/4 D", "1/2 T","1/2","1/2 D", "1 BAR", "2 BAR", "4 BAR", "8 BAR"};
	int divisionA = 0.0;
	int divisionB = 0.0;
	float timeA = 0.0;
	float timeB = 0.0;
	float durationA = 0.0f;
	float durationB = 0.0f;
	bool ClockInputA = false;
	bool ClockInputB = false;

	/////////////////////////////////////

	Delay() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    configParam(Delay::SYNCA_PARAM, 0.0f, 1.0f, 0.0f, "");
    configParam(Delay::CLEAR_A, 0.0f, 1.0f, 0.0f, "");
    configParam(Delay::LEVEL_A, 0.0f, 1.0f, 0.5f, "");
    configParam(Delay::CROSSFEED_A, 0.0f, 1.0f, 0.0f, "");
    configParam(Delay::TIME_A_PARAM, 0.0f, 1.0f, 0.6748f, "");
    configParam(Delay::FEEDBACK_A_PARAM, -1.0f, 1.0f, 0.0f, "");
    configParam(Delay::LP_A_PARAM, 0.0f, 1.0f, 1.0f, "");
    configParam(Delay::HP_A_PARAM, 0.0f, 1.0f, 0.0f, "");
    configParam(Delay::MIX_A_PARAM, 0.0f, 1.0f, 0.5f, "");
    configParam(Delay::SYNCB_PARAM, 0.0f, 1.0f, 0.0f, "");
    configParam(Delay::CLEAR_B, 0.0f, 1.0f, 0.0f, "");
    configParam(Delay::LEVEL_B, 0.0f, 1.0f, 0.5f, "");
    configParam(Delay::CROSSFEED_B, 0.0f, 1.0f, 0.0f, "");
    configParam(Delay::TIME_B_PARAM, 0.0f, 1.0f, 0.6748f, "");
    configParam(Delay::FEEDBACK_B_PARAM, -1.0f, 1.0f, 0.0f, "");
    configParam(Delay::LP_B_PARAM, 0.0f, 1.0f, 1.0f, "");
    configParam(Delay::HP_B_PARAM, 0.0f, 1.0f, 0.0f, "");
    configParam(Delay::MIX_B_PARAM, 0.0f, 1.0f, 0.5f, "");
		historyBufferA.clear();
		historyBufferB.clear();
		smofilterA.reset();
		smofilterB.reset();

	}

	void process(const ProcessArgs& args) override;

	void ModeTypeA(int SYNCa) {
		switch(SYNCa) {
			case 0:
				// Free Mode
				delayA = 1e-3 * powf(10.0f / 1e-3, clamp(params[TIME_A_PARAM].value + clamp(inputs[TIME_A_INPUT].value / 10.0f, 0.0f, 1.0f), 0.0f, 1.0f));
				lcd_tempoA = std::round(delayA*1000);
			break;
			case 1:
				// Synced
				timeA += 1.0f / APP->engine->getSampleRate();
				if(inputs[CLOCK_INPUT].active) {
					if(clockTriggerA.process(inputs[CLOCK_INPUT].value)) {
						if(ClockInputA) {
							durationA = timeA;
						}
						timeA = 0;
						ClockInputA = true;
					}
				}

				float divisionfreqA = clamp(params[TIME_A_PARAM].value + inputs[TIME_A_INPUT].value / 10.0f, 0.0f, 1.0f); // * DIVISIONS;
					divisionfreqA *= 25.0f;
						divisionfreqA = clamp(divisionfreqA,0.0f,24.0f);
							divisionA = int(divisionfreqA);

				if(durationA != 0) {
					// Compute delay time in seconds
					delayA = (durationA * divisions[divisionA]);
				}
				lcd_TRatioA = divisionA;
			break;
		}
	}

	void ModeTypeB(int SYNCb) {
		switch(SYNCb) {
			case 0:
				// Free Mode
				delayB = 1e-3 * powf(10.0f / 1e-3, clamp(params[TIME_B_PARAM].value + clamp(inputs[TIME_B_INPUT].value / 10.0f, 0.0f, 1.0f), 0.0f, 1.0f));
				lcd_tempoB = std::round(delayB*1000);
			break;
			case 1:
				// Synced
				timeB += 1.0f / APP->engine->getSampleRate();
				if(inputs[CLOCK_INPUT].active) {
					if(clockTriggerB.process(inputs[CLOCK_INPUT].value)) {
						if(ClockInputB) {
							durationB = timeB;
						}
						timeB = 0;
						ClockInputB = true;
					}
				}

				float divisionfreqB = clamp(params[TIME_B_PARAM].value + inputs[TIME_B_INPUT].value / 10.0f, 0.0f, 1.0f);
					divisionfreqB *= 25.0f;
						divisionfreqB = clamp(divisionfreqB,0.0f,24.0f);
							divisionB = int(divisionfreqB);

				if(durationB != 0) {
					// Compute delay time in seconds
					delayB = (durationB * divisions[divisionB]);
				}
				lcd_TRatioB = divisionB;
			break;
		}
	}

	//Json for Panel Theme
	json_t *dataToJson() override	{
		json_t *rootJ = json_object();
		json_object_set_new(rootJ, "Theme", json_integer(Theme));
		json_object_set_new(rootJ, "DISPLAYA", json_integer(DISPLAYA));
		json_object_set_new(rootJ, "DISPLAYB", json_integer(DISPLAYB));
		return rootJ;
	}
	void dataFromJson(json_t *rootJ) override	{
		json_t *ThemeJ = json_object_get(rootJ, "Theme");
		if (ThemeJ)
			Theme = json_integer_value(ThemeJ);
		json_t *DisplayAJ = json_object_get(rootJ, "DISPLAYA");
		if (DisplayAJ)
			DISPLAYA = json_integer_value(DisplayAJ);
		json_t *DisplayBJ = json_object_get(rootJ, "DISPLAYB");
		if (DisplayBJ)
			DISPLAYB = json_integer_value(DisplayBJ);
	}

};


void Delay::process(const ProcessArgs& args) {

	float inA = inputs[IN_A_INPUT].value * params[CROSSFEED_B].value;
	float inB = inputs[IN_B_INPUT].value * params[CROSSFEED_A].value;

	// DELAY A

	///////////////////////////////////////////////////

	// Get input to delay block

	float IA = ((inputs[IN_A_INPUT].active ? inputs[IN_A_INPUT].value : 0.0f) * (params[LEVEL_A].value)) + inB;
	float feedbackA = clamp(params[FEEDBACK_A_PARAM].value + inputs[FEEDBACK_A_INPUT].value / 10.0f, -1.0f, 1.0f);
	float dryA = IA + lastWetA * feedbackA;

	SYNCa = params[SYNCA_PARAM].value;

	if((int)SYNCa == 1.0f) {
		DISPLAYA = 1.0f;
		ModeTypeA(1);
	}
	else {
		ModeTypeA(0);
		DISPLAYA = 0.0f;
	}

	// Number of delay samples
	smofilterA.setFc(0.003f);
	indexA = (float)(smofilterA.process(delayA) * args.sampleRate);

	// Push dry sample into history buffer
	if (!historyBufferA.full()) {
		historyBufferA.push(dryA);
	}

	// How many samples do we need consume to catch up?
	float consumeA = indexA - historyBufferA.size();

	if (outBufferA.empty()) {
		float ratioA = 1.0;
        if (consumeA <= -16)
            ratioA = 0.5;
        else if (consumeA >= 16)
            ratioA = 2.0;
		/*
		float ratioA = 1.f;
		if (fabsf(consumeA) >= 16.f) {
			ratioA = powf(10.f, clamp(consumeA / 10000.f, -1.f, 1.f));
		}
		*/
		float inSRA = args.sampleRate;
        float outSRA = ratioA * inSRA;
        int inFramesA = std::min(historyBufferA.size(), (size_t) 16);
        int outFramesA = outBufferA.capacity();
        srcA.setRates(inSRA, outSRA);
        srcA.process((const dsp::Frame<1>*)historyBufferA.startData(), &inFramesA, (dsp::Frame<1>*)outBufferA.endData(), &outFramesA);
        historyBufferA.startIncr(inFramesA);
        outBufferA.endIncr(outFramesA);

    }

	if(clear_a.process(clamp(params[CLEAR_A].value + inputs[CLEAR_A_INPUT].value / 10.0f, 0.0f, 1.0f))) {
		historyBufferA.clear();
	}


	float wetA = 0.0f;
	if (!outBufferA.empty()) {
		wetA = outBufferA.shift();
		wetfiltA = wetA;
	}

	// Apply LP/HP filter to Wet Output
	float LPA = clamp(params[LP_A_PARAM].value + inputs[LP_A_INPUT].value / 10.0f, 0.0f, 1.0f);
	float lowpassFreqA = 1000.0f * powf(10.0f, clamp(2.0f*LPA, 0.0f, 1.0f));
	lowpassFilterA.setCutoff(lowpassFreqA / args.sampleRate);
	lowpassFilterA.process(wetfiltA);
	wetfiltA = lowpassFilterA.lowpass();
	float HPA = clamp(params[HP_A_PARAM].value + inputs[HP_A_INPUT].value / 10.0f, 0.0f, 1.0f);
	float highpassFreqA = 1000.0f * powf(10.0f, clamp(2.0f*HPA, 0.0f, 1.0f));
	highpassFilterA.setCutoff(highpassFreqA / args.sampleRate);
	highpassFilterA.process(wetfiltA);
	wetfiltA = highpassFilterA.highpass();

	lastWetA = wetA;

	// Wet Output
	outputs[WET_A_OUTPUT].value = saturate(wetfiltA);

	// Dry-Wet Output
	float mixA = clamp(params[MIX_A_PARAM].value + inputs[MIX_A_INPUT].value / 10.0f, 0.0f, 1.0f);
	float outA = crossfade(IA, wetfiltA, mixA);
	outputs[OUT_A_OUTPUT].value = saturate(outA);

	///////////////////////////////////////////////////



	// DELAY B

	///////////////////////////////////////////////////

	// Get input to delay block
	float IB = ((inputs[IN_B_INPUT].active ? inputs[IN_B_INPUT].value : 0.0f) * (params[LEVEL_B].value)) + inA;
	float feedbackB = clamp(params[FEEDBACK_B_PARAM].value + inputs[FEEDBACK_B_INPUT].value / 10.0f, -1.0f, 1.0f);
	float dryB = IB + lastWetB * feedbackB;

	SYNCb = params[SYNCB_PARAM].value;

	if((int)SYNCb == 1.0f) {
		DISPLAYB = 1.0f;
		ModeTypeB(1);
	}
	else {
		ModeTypeB(0);
		DISPLAYB = 0.0f;
	}

	// Number of delay samples
	smofilterB.setFc(0.003f);
	indexB = (float)(smofilterB.process(delayB) * args.sampleRate);

	// Push dry sample into history buffer
	if (!historyBufferB.full()) {
		historyBufferB.push(dryB);
	}

	// How many samples do we need consume to catch up?
	float consumeB = indexB - historyBufferB.size();

	if (outBufferB.empty()) {
		float ratioB = 1.0;
        if (consumeB <= -16)
            ratioB = 0.5;
        else if (consumeB >= 16)
            ratioB = 2.0;
		/*
		float ratioB = 1.f;
		if (fabsf(consumeB) >= 16.f) {
			ratioB = powf(10.f, clamp(consumeB / 10000.f, -1.f, 1.f));
		}
		*/
		float inSRB = args.sampleRate;
        float outSRB = ratioB * inSRB;

        int inFramesB = std::min(historyBufferB.size(), (size_t) 16);
        int outFramesB = outBufferB.capacity();
        srcB.setRates(inSRB, outSRB);
        srcB.process((const dsp::Frame<1>*)historyBufferB.startData(), &inFramesB, (dsp::Frame<1>*)outBufferB.endData(), &outFramesB);
        historyBufferB.startIncr(inFramesB);
        outBufferB.endIncr(outFramesB);

	}

	if(clear_b.process(clamp(params[CLEAR_B].value + inputs[CLEAR_B_INPUT].value / 10.0f, 0.0f, 1.0f))) {
		historyBufferB.clear();
	}



	float wetB = 0.0f;
	if (!outBufferB.empty()) {
		wetB = outBufferB.shift();
		wetfiltB = wetB;
	}

	// Apply LP/HP filter to Wet Output
	float LPB = clamp(params[LP_B_PARAM].value + inputs[LP_B_INPUT].value / 10.0f, 0.0f, 1.0f);
	float lowpassFreqB = 1000.0f * powf(10.0f, clamp(2.0f*LPB, 0.0f, 1.0f));
	lowpassFilterB.setCutoff(lowpassFreqB / args.sampleRate);
	lowpassFilterB.process(wetfiltB);
	wetfiltB = lowpassFilterB.lowpass();
	float HPB = clamp(params[HP_B_PARAM].value + inputs[HP_B_INPUT].value / 10.0f, 0.0f, 1.0f);
	float highpassFreqB = 1000.0f * powf(10.0f, clamp(2.0f*HPB, 0.0f, 1.0f));
	highpassFilterB.setCutoff(highpassFreqB / args.sampleRate);
	highpassFilterB.process(wetfiltB);
	wetfiltB = highpassFilterB.highpass();

	lastWetB = wetB;

	// Wet Output
	outputs[WET_B_OUTPUT].value = saturate(wetfiltB);

	// Dry-Wet Output
	float mixB = clamp(params[MIX_B_PARAM].value + inputs[MIX_B_INPUT].value / 10.0f, 0.0f, 1.0f);
	float outB = crossfade(IB, wetfiltB, mixB);
	outputs[OUT_B_OUTPUT].value = saturate(outB);

};

struct DisplayWidget : TransparentWidget {

	int *value = nullptr;
	std::shared_ptr<Font> font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Fonts/Crysta.ttf"));

	DisplayWidget()
	{

	};

	void draw(const DrawArgs& args) override {
		// Background
		NVGcolor backgroundColor = nvgRGB(0x6, 0x9, 0x37);
		NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
		nvgFillColor(args.vg, backgroundColor);
		nvgFill(args.vg);
		nvgStrokeWidth(args.vg, 1.5);
		nvgStrokeColor(args.vg, borderColor);
		nvgStroke(args.vg);
		// text
		nvgFontSize(args.vg, 16);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextLetterSpacing(args.vg, 2.5);

		std::stringstream to_display;
    if (value) {
		  to_display << std::left << std::setw(5) << *value;
    } else {
      to_display << std::left << std::setw(5) << 500;
    }

		Vec textPos = Vec(3.0f, 17.0f);

		NVGcolor textColor = nvgRGB(12, 216, 255);
		nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
		nvgText(args.vg, textPos.x, textPos.y, "", NULL);

		textColor = nvgRGB(0xda, 0xe9, 0x29);
		nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
		nvgText(args.vg, textPos.x, textPos.y, "", NULL);

		textColor = nvgRGB(12, 216, 255);
		nvgFillColor(args.vg, textColor);
		nvgText(args.vg, textPos.x, textPos.y, to_display.str().c_str(), NULL);
	}
};

struct TRatioADisplay : TransparentWidget {

	Delay *module{};
	std::shared_ptr<Font> font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Fonts/Crysta.ttf"));;

	TRatioADisplay()
	{

	}

	void drawDivision(NVGcontext *vg, Vec pos, int division) {
		// Background
    if (module) {
  		NVGcolor backgroundColor = nvgRGB(0x6, 0x9, 0x37);
  		NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
  		nvgBeginPath(vg);
  		nvgRoundedRect(vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
  		nvgFillColor(vg, backgroundColor);
  		nvgFill(vg);
  		nvgStrokeWidth(vg, 1.5);
  		nvgStrokeColor(vg, borderColor);
  		nvgStroke(vg);

  		nvgFontSize(vg, 16);
  		nvgFontFaceId(vg, font->handle);
  		nvgTextLetterSpacing(vg, -2);

  		Vec textPos = Vec(3.0f, 17.0f);

  		nvgFillColor(vg, nvgRGB(12, 216, 255));
  		char text[128];
  		snprintf(text, sizeof(text), "%s", module->divisionNames[division]);
  		nvgText(vg, textPos.x, textPos.y, text, NULL);
  }
	}

	void draw(const DrawArgs& args) override {
    if (module) {
		   drawDivision(args.vg, Vec(0, box.size.y - 153), module->divisionA);
    }
	}
};

struct TRatioBDisplay : TransparentWidget {

	Delay *module{};
	std::shared_ptr<Font> font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Fonts/Crysta.ttf"));

	TRatioBDisplay()
	{

	}


	void drawDiv(NVGcontext *vg, Vec pos, int division) {
    if (module) {
  		// Background
  		NVGcolor backgroundColor = nvgRGB(0x6, 0x9, 0x37);
  		NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
  		nvgBeginPath(vg);
  		nvgRoundedRect(vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
  		nvgFillColor(vg, backgroundColor);
  		nvgFill(vg);
  		nvgStrokeWidth(vg, 1.5);
  		nvgStrokeColor(vg, borderColor);
  		nvgStroke(vg);

  		nvgFontSize(vg, 16);
  		nvgFontFaceId(vg, font->handle);
  		nvgTextLetterSpacing(vg, -2);

  		Vec textPos = Vec(3.0f, 17.0f);

  		nvgFillColor(vg, nvgRGB(12, 216, 255));
  		char text[128];
  		snprintf(text, sizeof(text), "%s", module->divisionNames[division]);
  		nvgText(vg, textPos.x, textPos.y, text, NULL);
    }
	}

	void draw(const DrawArgs& args) override {
    if (module) {
		   drawDiv(args.vg, Vec(0, box.size.y - 153), module->divisionB);
    }
	}
};

struct BPClassicMenu : MenuItem {
	Delay *delay;
	void onAction(const event::Action &e) override {
		delay->Theme = 0;
	}
	void step() override {
		rightText = (delay->Theme == 0) ? "✔" : "";
		MenuItem::step();
	}
};

struct BPNightModeMenu : MenuItem {
	Delay *delay;
	void onAction(const event::Action &e) override {
		delay->Theme = 1;
	}
	void step() override {
		rightText = (delay->Theme == 1) ? "✔" : "";
		MenuItem::step();
	}
};

struct DelayWidget : ModuleWidget {
	//Displays
	DisplayWidget *displayA;
	DisplayWidget *displayB;
	TRatioADisplay *TRdisplayA;
	TRatioBDisplay *TRdisplayB;

	// Panel Themes
	SvgPanel *panelClassic;
	SvgPanel *panelNightMode;

  void appendContextMenu(Menu *menu) override {
  	Delay *delay = dynamic_cast<Delay*>(module);
  	assert(delay);
  	menu->addChild(construct<MenuEntry>());
  	menu->addChild(construct<MenuLabel>(&MenuLabel::text, "Theme"));
  	menu->addChild(construct<BPClassicMenu>(&BPClassicMenu::text, "Classic (default)", &BPClassicMenu::delay, delay));
  	menu->addChild(construct<BPNightModeMenu>(&BPNightModeMenu::text, "Night Mode", &BPNightModeMenu::delay, delay));
  }

	DelayWidget(Delay *module);
	void step() override;
};

DelayWidget::DelayWidget(Delay *module) {
	setModule(module);
	box.size = Vec(25 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	panelClassic = new SvgPanel();
	panelClassic->box.size = box.size;
	panelClassic->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Panels/Delay.svg")));
	addChild(panelClassic);

	panelNightMode = new SvgPanel();
	panelNightMode->box.size = box.size;
	panelNightMode->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Panels/Delay-Dark.svg")));
	addChild(panelNightMode);

	{
		displayA = new DisplayWidget();
		displayA->box.pos = Vec(9,60);
		displayA->box.size = Vec(64, 20);
    if (module) {
		  displayA->value = &module->lcd_tempoA;
    }
		addChild(displayA);

		displayB = new DisplayWidget();
		displayB->box.pos = Vec(box.size.x-73,60);
		displayB->box.size = Vec(64, 20);
    if (module) {
		  displayB->value = &module->lcd_tempoB;
    }
		addChild(displayB);
	}

	if (module){
		TRdisplayA = new TRatioADisplay();
		TRdisplayA->module = module;
		TRdisplayA->box.pos = Vec(9, 60);
		TRdisplayA->box.size = Vec(64, 20);
		addChild(TRdisplayA);

		TRdisplayB = new TRatioBDisplay();
		TRdisplayB->module = module;
		TRdisplayB->box.pos = Vec(box.size.x-73, 60);
		TRdisplayB->box.size = Vec(64, 20);
		addChild(TRdisplayB);
	}


	addChild(createWidget<MScrewA>(Vec(15, 0)));
	addChild(createWidget<MScrewC>(Vec(box.size.x-30, 0)));
	addChild(createWidget<MScrewD>(Vec(15, 365)));
	addChild(createWidget<MScrewB>(Vec(box.size.x-30, 365)));

	addParam(createParam<VioMSwitch>(Vec(33, 99), module, Delay::SYNCA_PARAM));
	addParam(createParam<MSwitchGrey2>(Vec(39, 19), module, Delay::CLEAR_A));
	addInput(createInput<SilverSixPortB>(Vec(12, 20.5), module, Delay::CLEAR_A_INPUT));

	addParam(createParam<RedSmallKnob>(Vec(145, 254), module, Delay::LEVEL_A));
	addParam(createParam<RedSmallKnob>(Vec(145, 105), module, Delay::CROSSFEED_A));
	addParam(createParam<BlueLargeKnob>(Vec(77, 32), module, Delay::TIME_A_PARAM));
	addParam(createParam<GreenLargeKnob>(Vec(77, 106), module, Delay::FEEDBACK_A_PARAM));
	addParam(createParam<BlueSmallKnob>(Vec(60.5, 189.5), module, Delay::LP_A_PARAM));
	addParam(createParam<BlueSmallKnob>(Vec(109, 189.5), module, Delay::HP_A_PARAM));
	addParam(createParam<GreenLargeKnob>(Vec(77, 261), module, Delay::MIX_A_PARAM));

	addParam(createParam<VioMSwitch>(Vec(329, 99), module, Delay::SYNCB_PARAM));
	addParam(createParam<MSwitchGrey2>(Vec(box.size.x-67, 19), module, Delay::CLEAR_B));
	addInput(createInput<SilverSixPortC>(Vec(box.size.x-37, 20.5), module, Delay::CLEAR_B_INPUT));

	addParam(createParam<RedSmallKnob>(Vec(199, 254), module, Delay::LEVEL_B));
	addParam(createParam<RedSmallKnob>(Vec(199, 105), module, Delay::CROSSFEED_B));
	addParam(createParam<BlueLargeKnob>(Vec(250, 32), module, Delay::TIME_B_PARAM));
	addParam(createParam<GreenLargeKnob>(Vec(250, 106), module, Delay::FEEDBACK_B_PARAM));
	addParam(createParam<BlueSmallKnob>(Vec(235, 189.5), module, Delay::LP_B_PARAM));
	addParam(createParam<BlueSmallKnob>(Vec(281, 189.5), module, Delay::HP_B_PARAM));
	addParam(createParam<GreenLargeKnob>(Vec(250, 261), module, Delay::MIX_B_PARAM));

	addInput(createInput<SilverSixPortB>(Vec(174, 150), module, Delay::CLOCK_INPUT));

	addInput(createInput<SilverSixPortB>(Vec(148, 43), module, Delay::TIME_A_INPUT));
	addInput(createInput<SilverSixPort>(Vec(26.5, 140), module, Delay::FEEDBACK_A_INPUT));
	addInput(createInput<SilverSixPortD>(Vec(22.5, 192.5), module, Delay::LP_A_INPUT));
	addInput(createInput<SilverSixPortE>(Vec(147.5, 192.5), module, Delay::HP_A_INPUT));
	addInput(createInput<SilverSixPort>(Vec(22.5, 272.5), module, Delay::MIX_A_INPUT));
	addInput(createInput<SilverSixPortA>(Vec(30, 324.5), module, Delay::IN_A_INPUT));
	addOutput(createOutput<SilverSixPortD>(Vec(88, 324.5), module, Delay::OUT_A_OUTPUT));
	addOutput(createOutput<SilverSixPort>(Vec(148, 324.5), module, Delay::WET_A_OUTPUT));

	addInput(createInput<SilverSixPortA>(Vec(202, 43), module, Delay::TIME_B_INPUT));
	addInput(createInput<SilverSixPortC>(Vec(324.5, 140), module, Delay::FEEDBACK_B_INPUT));
	addInput(createInput<SilverSixPort>(Vec(200, 192.5), module, Delay::LP_B_INPUT));
	addInput(createInput<SilverSixPortD>(Vec(327.5, 192.5), module, Delay::HP_B_INPUT));
	addInput(createInput<SilverSixPortA>(Vec(327.5, 272.5), module, Delay::MIX_B_INPUT));
	addInput(createInput<SilverSixPort>(Vec(322, 324.5), module, Delay::IN_B_INPUT));
	addOutput(createOutput<SilverSixPortE>(Vec(264, 324.5), module, Delay::OUT_B_OUTPUT));
	addOutput(createOutput<SilverSixPortB>(Vec(204, 324.5), module, Delay::WET_B_OUTPUT));

};

void DelayWidget::step() {
  if (module) {
  	Delay *delay = dynamic_cast<Delay*>(module);
  	assert(delay);
  	// Panel Theme
  	panelClassic->visible = (delay->Theme == 0);
  	panelNightMode->visible = (delay->Theme == 1);
  	// Display
  	displayA->visible = (delay->DISPLAYA == 0);
  	TRdisplayA->visible = (delay->DISPLAYA == 1);
  	displayB->visible = (delay->DISPLAYB == 0);
  	TRdisplayB->visible = (delay->DISPLAYB == 1);
  }

	ModuleWidget::step();
}

Model *modelDelay = createModel<Delay, DelayWidget>("Delay");
