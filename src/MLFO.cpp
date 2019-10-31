#include "MSM.hpp"
#include "MSMComponentLibrary.hpp"
#include "Additional/LowFrequencyOscillator.hpp"

struct LFO : Module {

	enum ParamIds {
		SYNCED_PARAM,
		FREQ_PARAM,
		FM_PARAM,
		PW_1_PARAM,
		PWM_1_PARAM,
		OFFSET_1_PARAM,
		INVERT_1_PARAM,
		WAVE_A_PARAM,
		FM_2_PARAM,
		PW_2_PARAM,
		PWM_2_PARAM,
		OFFSET_2_PARAM,
		INVERT_2_PARAM,
		WAVE_B_PARAM,
		MIX_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		FM_1_INPUT,
		PW_1_INPUT,
		RESET_INPUT,
		WAVE_A_INPUT,
		FM_2_INPUT,
		PW_2_INPUT,
		WAVE_B_INPUT,
		CV_MIX_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SIN_OUTPUT,
		TRI_OUTPUT,
		SAW_OUTPUT,
		SQR_OUTPUT,
		SIN_2_OUTPUT,
		TRI_2_OUTPUT,
		SAW_2_OUTPUT,
		SQR_2_OUTPUT,
		LFO_A_OUTPUT,
		LFO_B_OUTPUT,
		OUT_MIX_OUTPUT,
		SH_OUTPUT,
		SH_2_OUTPUT,
		NUM_OUTPUTS
	};

	LowFrequencyOscillator oscillator;
	LowFrequencyOscillator oscillatorB;
	dsp::SchmittTrigger trigger1;
	dsp::SchmittTrigger trigger2;
	float sample1 = 0.0;
	float sample2 = 0.0;

	int Theme = 0;

	LFO() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(LFO::OFFSET_1_PARAM, 0.0, 1.0, 0.0, "");
		configParam(LFO::INVERT_1_PARAM, 0.0, 1.0, 1.0, "");
		configParam(LFO::FREQ_PARAM, -9.0, 7.0, -1.0, "");
		configParam(LFO::FM_PARAM, 0.0, 1.0, 0.0, "");
		configParam(LFO::PW_1_PARAM, 0.0, 1.0, 0.5, "");
		configParam(LFO::OFFSET_2_PARAM, 0.0, 1.0, 0.0, "");
		configParam(LFO::INVERT_2_PARAM, 0.0, 1.0, 1.0, "");
		configParam(LFO::FM_2_PARAM, 0.0, 1.0, 0.0, "");
		configParam(LFO::PW_2_PARAM, 0.0, 1.0, 0.5, "");
		configParam(LFO::WAVE_A_PARAM, 0.0, 3.0, 0.0, "");
		configParam(LFO::WAVE_B_PARAM, 0.0, 3.0, 0.0, "");
		configParam(LFO::MIX_PARAM, 0.0, 1.0, 0.5, "");
	}

	void process(const ProcessArgs& args) override;

	//Json for Panel Theme
	json_t *dataToJson() override	{
		json_t *rootJ = json_object();
		json_object_set_new(rootJ, "Theme", json_integer(Theme));
		return rootJ;
	}
	void dataFromJson(json_t *rootJ) override	{
		json_t *ThemeJ = json_object_get(rootJ, "Theme");
		if (ThemeJ)
			Theme = json_integer_value(ThemeJ);
	}

};

void LFO::process(const ProcessArgs& args)
{
	//Noise
	float noise1 = 2.0f * random::normal();
	float noise2 = 2.0f * random::normal();

	//LFO 1
	oscillator.setPitch(params[FREQ_PARAM].value + inputs[FM_1_INPUT].value * params[FM_PARAM].value);
	oscillator.setPulseWidth(params[PW_1_PARAM].value + inputs[PW_1_INPUT].value);
	oscillator.Settings(params[OFFSET_1_PARAM].value > 0.0f, params[INVERT_1_PARAM].value <= 0.0f);
	oscillator.step(1.0f / args.sampleRate);
	oscillator.setReset(inputs[RESET_INPUT].value);

	outputs[SIN_OUTPUT].value = 5.0f * oscillator.sin();
	outputs[TRI_OUTPUT].value = 5.0f * oscillator.tri();
	outputs[SAW_OUTPUT].value = 5.0f * oscillator.saw();
	outputs[SQR_OUTPUT].value = 5.0f * oscillator.sqr();

	//sample and hold
	 float Switch = params[OFFSET_1_PARAM].value;

	if (trigger1.process(outputs[SQR_OUTPUT].value)) {
		if(Switch > 0.0f) {
		sample1 = noise1 + 5.0f;
		}
		else {
			sample1 = noise1;
		}
	}

	float SHOut1 = sample1 / 5.0f;

	//Output
	outputs[SH_OUTPUT].value = 5.0f * SHOut1;

	float WAVE_A = params[WAVE_A_PARAM].value + inputs[WAVE_A_INPUT].value;
	WAVE_A = clamp(WAVE_A, 0.0f, 3.0f);
	float WAVE_1_OUT;
	if (WAVE_A < 1.0f)
		WAVE_1_OUT = crossfade(oscillator.sin(), oscillator.tri(), WAVE_A);
	else if (WAVE_A < 2.0f)
		WAVE_1_OUT = crossfade(oscillator.tri(), oscillator.saw(), WAVE_A - 1.0f);
	else
		WAVE_1_OUT = crossfade(oscillator.sqr(), SHOut1, WAVE_A - 2.0f);
	outputs[LFO_A_OUTPUT].value = 5.0f * WAVE_1_OUT;


	//LFO 2

	oscillatorB.setPitch(params[FREQ_PARAM].value + inputs[FM_2_INPUT].value * params[FM_2_PARAM].value);
	oscillatorB.setPulseWidth(params[PW_2_PARAM].value + inputs[PW_2_INPUT].value);
	oscillatorB.Settings(params[OFFSET_2_PARAM].value > 0.0f, params[INVERT_2_PARAM].value <= 0.0f);
	oscillatorB.step(1.0f / args.sampleRate);
	oscillatorB.setReset(inputs[RESET_INPUT].value);

	outputs[SIN_2_OUTPUT].value = 5.0f * oscillatorB.sin();
	outputs[TRI_2_OUTPUT].value = 5.0f * oscillatorB.tri();
	outputs[SAW_2_OUTPUT].value = 5.0f * oscillatorB.saw();
	outputs[SQR_2_OUTPUT].value = 5.0f * oscillatorB.sqr();

	//sample and hold
	if (trigger2.process(outputs[SQR_2_OUTPUT].value)) {
		if(Switch > 0.0f) {
		sample2 = noise1 + 5.0f;
		}
		else {
			sample2 = noise2;
		}
	}

	float SHOut2 = sample2 / 5.0f;

	//Output
	outputs[SH_2_OUTPUT].value = 5.0f * SHOut2;

	float WAVE_B = params[WAVE_B_PARAM].value + inputs[WAVE_B_INPUT].value;
	WAVE_B = clamp(WAVE_B, 0.0f, 3.0f);
	float WAVE_2_OUT;
	if (WAVE_B < 1.0f)
		WAVE_2_OUT = crossfade(oscillatorB.sin(), oscillatorB.tri(), WAVE_B);
	else if (WAVE_B < 2.0f)
		WAVE_2_OUT = crossfade(oscillatorB.tri(), oscillatorB.saw(), WAVE_B - 1.0f);
	else
		WAVE_2_OUT = crossfade(oscillatorB.sqr(), SHOut2, WAVE_B - 2.0f);
	outputs[LFO_B_OUTPUT].value = 5.0f * WAVE_2_OUT;


	//MIX LFO 1 & LFO 2
	float CrossfadeMix = clamp(params[MIX_PARAM].value + inputs[CV_MIX_INPUT].value, 0.0f, 1.0f);
	float MIX_IN_1 = outputs[LFO_A_OUTPUT].value;
	float MIX_IN_2 = outputs[LFO_B_OUTPUT].value;
	float OutMix;

	if(CrossfadeMix < 0.5f) {
		OutMix = crossfade(MIX_IN_1, MIX_IN_1, CrossfadeMix);
	}
	else(CrossfadeMix > 1.0f);
		OutMix = crossfade(MIX_IN_1, MIX_IN_2, CrossfadeMix);
	outputs[OUT_MIX_OUTPUT].value = OutMix;
};

struct LClassicMenu : MenuItem {
	LFO *lfo;
	void onAction(const event::Action &e) override {
		lfo->Theme = 0;
	}
	void step() override {
		rightText = (lfo->Theme == 0) ? "✔" : "";
		MenuItem::step();
	}
};

struct LNightModeMenu : MenuItem {
	LFO *lfo;
	void onAction(const event::Action &e) override {
		lfo->Theme = 1;
	}
	void step() override {
		rightText = (lfo->Theme == 1) ? "✔" : "";
		MenuItem::step();
	}
};

struct LFOWidget : ModuleWidget {
	// Panel Themes
	SvgPanel *panelClassic;
	SvgPanel *panelNightMode;

	void appendContextMenu(Menu *menu) override {
		LFO *lfo = dynamic_cast<LFO*>(module);
		assert(lfo);
		menu->addChild(construct<MenuEntry>());
		menu->addChild(construct<MenuLabel>(&MenuLabel::text, "Theme"));
		menu->addChild(construct<LClassicMenu>(&LClassicMenu::text, "Classic (default)", &LClassicMenu::lfo, lfo));
		menu->addChild(construct<LNightModeMenu>(&LNightModeMenu::text, "Night Mode", &LNightModeMenu::lfo, lfo));
	}

	LFOWidget(LFO *module);
	void step() override;
};

LFOWidget::LFOWidget(LFO *module) {
		setModule(module);
	box.size = Vec(16 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	panelClassic = new SvgPanel();
	panelClassic->box.size = box.size;
	panelClassic->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Panels/MLFO.svg")));
	addChild(panelClassic);

	panelNightMode = new SvgPanel();
	panelNightMode->box.size = box.size;
	panelNightMode->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Panels/MLFO-Dark.svg")));
	addChild(panelNightMode);

	addChild(createWidget<MScrewB>(Vec(15, 0)));
	addChild(createWidget<MScrewC>(Vec(box.size.x-30, 0)));
	addChild(createWidget<MScrewA>(Vec(15, 365)));
	addChild(createWidget<MScrewD>(Vec(box.size.x-30, 365)));

	//LFO 1
	addParam(createParam<VioM2Switch>(Vec(70, 150), module, LFO::OFFSET_1_PARAM));
	addParam(createParam<VioM2Switch>(Vec(70, 215), module, LFO::INVERT_1_PARAM));
	addParam(createParam<RedLargeKnob>(Vec(100, 40), module, LFO::FREQ_PARAM));
	addParam(createParam<BlueSmallKnob>(Vec(11, 142), module, LFO::FM_PARAM));
	addParam(createParam<BlueSmallKnob>(Vec(11, 207), module, LFO::PW_1_PARAM));

	addInput(createInput<SilverSixPortA>(Vec(40, 256), module, LFO::FM_1_INPUT));
	addInput(createInput<SilverSixPort>(Vec(80, 256), module, LFO::RESET_INPUT));
	addInput(createInput<SilverSixPortC>(Vec(11, 298), module, LFO::WAVE_A_INPUT));

	addOutput(createOutput<SilverSixPortD>(Vec(54, 298), module, LFO::LFO_A_OUTPUT));
	addOutput(createOutput<SilverSixPortE>(Vec(35, 338), module, LFO::SIN_OUTPUT));
	addOutput(createOutput<SilverSixPortA>(Vec(69, 338), module, LFO::TRI_OUTPUT));
	addOutput(createOutput<SilverSixPort>(Vec(110, 338), module, LFO::SH_OUTPUT));


	//LFO 2
	addParam(createParam<VioM2Switch>(Vec(162, 150), module, LFO::OFFSET_2_PARAM));
	addParam(createParam<VioM2Switch>(Vec(162, 215), module, LFO::INVERT_2_PARAM));
	addParam(createParam<BlueSmallKnob>(Vec(199, 142), module, LFO::FM_2_PARAM));
	addParam(createParam<BlueSmallKnob>(Vec(199, 207), module, LFO::PW_2_PARAM));

	addInput(createInput<SilverSixPort>(Vec(180, 256), module, LFO::FM_2_INPUT));
	addInput(createInput<SilverSixPortA>(Vec(206, 298), module, LFO::WAVE_B_INPUT));

	addOutput(createOutput<SilverSixPortC>(Vec(162, 298), module, LFO::LFO_B_OUTPUT));
	addOutput(createOutput<SilverSixPortD>(Vec(151, 338), module, LFO::SAW_2_OUTPUT));
	addOutput(createOutput<SilverSixPortA>(Vec(185, 338), module, LFO::SQR_2_OUTPUT));

	//Wave Selectors
	addParam(createParam<GreenSmallKnob>(Vec(40, 50), module, LFO::WAVE_A_PARAM));
	addParam(createParam<GreenSmallKnob>(Vec(175, 50), module, LFO::WAVE_B_PARAM));

	//LFO MIX
	addParam(createParam<GreenSmallKnob>(Vec(108, 105), module, LFO::MIX_PARAM));
	addInput(createInput<SilverSixPort>(Vec(140, 256), module, LFO::CV_MIX_INPUT));
	addOutput(createOutput<SilverSixPortA>(Vec(110, 298), module, LFO::OUT_MIX_OUTPUT));
};

void LFOWidget::step() {
	if (module) {
		LFO *lfo = dynamic_cast<LFO*>(module);
		assert(lfo);
		panelClassic->visible = (lfo->Theme == 0);
		panelNightMode->visible = (lfo->Theme == 1);
	}

	ModuleWidget::step();
}

Model *modelLFO = createModel<LFO, LFOWidget>("MLFO");
