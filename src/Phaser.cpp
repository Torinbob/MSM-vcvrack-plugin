#include "MSM.hpp"
#include "MSMComponentLibrary.hpp"

template <int OVERSAMPLE, int QUALITY>
class Phaser {
public:
    Phaser()  //initialise to some usefull defaults...
        : _fb(0.5f),
		_lfoPhase(0.0f),
        _depth(1.0f),
		_stage(0.0f),
        _zm1(0.0f),
		_pw(0.5f),
		dlfo(0.0f),
		y(0.0f),
		_lfodepth(1.0f),
		in(0.0f)
    {
		Range(10.0f, 8000.0f);
		Rate(0.5f);
    }

    void Range(float fMin, float fMax)	{ // Hz
        _dmin = fMin / (SR / 2.0f);
        _dmax = fMax / (SR / 2.0f);
    }

    void Rate(float rate)	{ // calculate phaser rate/speed
		_lfoInc = 2.0f * M_PI * (rate / SR);
    }

    void Feedback(float fb)	{ // 0 -> <1.
        _fb = fb;
    }

    void Depth(float depth)	{  // 0 -> 1.
        _depth = depth;
    }

	void LFODepth(float lfodepth) {
		_lfodepth = lfodepth;
	}

	void Stage(int stage) {
		_stage = stage;
	}

	void Pulsewidth(float pw) {
		_pw = pw;
	}

    float Update(float inSamp, int type)	{
		in = inSamp;
		for (int i = 0; i < OVERSAMPLE; i++) {
			//calculate and update phaser sweep lfo...
			float sine = ((sin(_lfoPhase) + 1.0f)/2.0f);
			float triangle = 2.0f * fabs(_lfoPhase / ( 2.0f * M_PI) - 0.5f);
			float saw = _lfoPhase / (2.0f * M_PI);
			float square = std::isless((_lfoPhase / (2.0f * M_PI)),_pw);

			switch(type) {
				case 3:
					dlfo  = _dmin + square * (_dmax - _dmin);
				break;
				case 2:
					dlfo  = _dmin + saw * (_dmax - _dmin);
				break;
				case 1:
					dlfo  = _dmin + triangle * (_dmax - _dmin);
				break;
				case 0:
					dlfo  = _dmin + sine * (_dmax - _dmin);
				break;
			}

			dlfo *= _lfodepth;

			_lfoPhase += _lfoInc;

			if(_lfoPhase >= M_PI * 2.0f)
			   _lfoPhase -= M_PI * 2.0f;

			switch(_stage) {
				case 18:
					//update filter coeffs
					for(int i = 0; i < 20; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(
								_allpass[3].Update(
								_allpass[4].Update(
								_allpass[5].Update(
								_allpass[6].Update(
								_allpass[7].Update(
								_allpass[8].Update(
								_allpass[9].Update(
								_allpass[10].Update(
								_allpass[11].Update(
								_allpass[12].Update(
								_allpass[13].Update(
								_allpass[14].Update(
								_allpass[15].Update(
								_allpass[16].Update(
								_allpass[17].Update(
								_allpass[18].Update(
								_allpass[19].Update(in + _zm1 * _fb ))))))))))))))))))));
				break;
				case 17:
					//update filter coeffs
					for(int i = 0; i < 19; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(
								_allpass[3].Update(
								_allpass[4].Update(
								_allpass[5].Update(
								_allpass[6].Update(
								_allpass[7].Update(
								_allpass[8].Update(
								_allpass[9].Update(
								_allpass[10].Update(
								_allpass[11].Update(
								_allpass[12].Update(
								_allpass[13].Update(
								_allpass[14].Update(
								_allpass[15].Update(
								_allpass[16].Update(
								_allpass[17].Update(
								_allpass[18].Update(in + _zm1 * _fb )))))))))))))))))));
				break;
				case 16:
					//update filter coeffs
					for(int i = 0; i < 18; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(
								_allpass[3].Update(
								_allpass[4].Update(
								_allpass[5].Update(
								_allpass[6].Update(
								_allpass[7].Update(
								_allpass[8].Update(
								_allpass[9].Update(
								_allpass[10].Update(
								_allpass[11].Update(
								_allpass[12].Update(
								_allpass[13].Update(
								_allpass[14].Update(
								_allpass[15].Update(
								_allpass[16].Update(
								_allpass[17].Update(in + _zm1 * _fb ))))))))))))))))));
				break;
				case 15:
					//update filter coeffs
					for(int i = 0; i < 17; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(
								_allpass[3].Update(
								_allpass[4].Update(
								_allpass[5].Update(
								_allpass[6].Update(
								_allpass[7].Update(
								_allpass[8].Update(
								_allpass[9].Update(
								_allpass[10].Update(
								_allpass[11].Update(
								_allpass[12].Update(
								_allpass[13].Update(
								_allpass[14].Update(
								_allpass[15].Update(
								_allpass[16].Update(in + _zm1 * _fb )))))))))))))))));
				break;
				case 14:
					//update filter coeffs
					for(int i = 0; i < 16; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(
								_allpass[3].Update(
								_allpass[4].Update(
								_allpass[5].Update(
								_allpass[6].Update(
								_allpass[7].Update(
								_allpass[8].Update(
								_allpass[9].Update(
								_allpass[10].Update(
								_allpass[11].Update(
								_allpass[12].Update(
								_allpass[13].Update(
								_allpass[14].Update(
								_allpass[15].Update(in + _zm1 * _fb ))))))))))))))));
				break;
				case 13:
					//update filter coeffs
					for(int i = 0; i < 15; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(
								_allpass[3].Update(
								_allpass[4].Update(
								_allpass[5].Update(
								_allpass[6].Update(
								_allpass[7].Update(
								_allpass[8].Update(
								_allpass[9].Update(
								_allpass[10].Update(
								_allpass[11].Update(
								_allpass[12].Update(
								_allpass[13].Update(
								_allpass[14].Update(in + _zm1 * _fb )))))))))))))));
				break;
				case 12:
					//update filter coeffs
					for(int i = 0; i < 14; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(
								_allpass[3].Update(
								_allpass[4].Update(
								_allpass[5].Update(
								_allpass[6].Update(
								_allpass[7].Update(
								_allpass[8].Update(
								_allpass[9].Update(
								_allpass[10].Update(
								_allpass[11].Update(
								_allpass[12].Update(
								_allpass[13].Update(in + _zm1 * _fb ))))))))))))));
				break;
				case 11:
					//update filter coeffs
					for(int i = 0; i < 13; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(
								_allpass[3].Update(
								_allpass[4].Update(
								_allpass[5].Update(
								_allpass[6].Update(
								_allpass[7].Update(
								_allpass[8].Update(
								_allpass[9].Update(
								_allpass[10].Update(
								_allpass[11].Update(
								_allpass[12].Update(in + _zm1 * _fb )))))))))))));
				break;
				case 10:
					//update filter coeffs
					for(int i = 0; i < 12; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(
								_allpass[3].Update(
								_allpass[4].Update(
								_allpass[5].Update(
								_allpass[6].Update(
								_allpass[7].Update(
								_allpass[8].Update(
								_allpass[9].Update(
								_allpass[10].Update(
								_allpass[11].Update(in + _zm1 * _fb ))))))))))));
				break;
				case 9:
					//update filter coeffs
					for(int i = 0; i < 11; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(
								_allpass[3].Update(
								_allpass[4].Update(
								_allpass[5].Update(
								_allpass[6].Update(
								_allpass[7].Update(
								_allpass[8].Update(
								_allpass[9].Update(
								_allpass[10].Update(in + _zm1 * _fb )))))))))));
				break;
				case 8:
					//update filter coeffs
					for(int i = 0; i < 10; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(
								_allpass[3].Update(
								_allpass[4].Update(
								_allpass[5].Update(
								_allpass[6].Update(
								_allpass[7].Update(
								_allpass[8].Update(
								_allpass[9].Update(in + _zm1 * _fb ))))))))));
				break;
				case 7:
					//update filter coeffs
					for(int i = 0; i < 9; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(
								_allpass[3].Update(
								_allpass[4].Update(
								_allpass[5].Update(
								_allpass[6].Update(
								_allpass[7].Update(
								_allpass[8].Update(in + _zm1 * _fb )))))))));
				break;
				case 6:
					//update filter coeffs
					for(int i = 0; i < 8; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(
								_allpass[3].Update(
								_allpass[4].Update(
								_allpass[5].Update(
								_allpass[6].Update(
								_allpass[7].Update(in + _zm1 * _fb ))))))));
				break;
				case 5:
					//update filter coeffs
					for(int i = 0; i < 7; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(
								_allpass[3].Update(
								_allpass[4].Update(
								_allpass[5].Update(
								_allpass[6].Update(in + _zm1 * _fb )))))));
				break;
				case 4:
					//update filter coeffs
					for(int i = 0; i < 6; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(
								_allpass[3].Update(
								_allpass[4].Update(
								_allpass[5].Update(in + _zm1 * _fb ))))));
				break;
				case 3:
					//update filter coeffs
					for(int i = 0; i < 5; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(
								_allpass[3].Update(
								_allpass[4].Update(in + _zm1 * _fb )))));
				break;
				case 2:
					//update filter coeffs
					for(int i = 0; i < 4; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(
								_allpass[3].Update(in + _zm1 * _fb ))));
				break;
				case 1:
					//update filter coeffs
					for(int i = 0; i < 3; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(
								_allpass[2].Update(in + _zm1 * _fb )));
				break;
				case 0:
					//update filter coeffs
					for(int i = 0; i < 2; i++)
					_allpass[i].Delay(dlfo);
					//calculate output
					y =			_allpass[0].Update(
								_allpass[1].Update(in + _zm1 * _fb ));
				break;
			}
			_zm1 = y;
			return PhaserBuffer[i] = in + y * _depth;
		}

		return PhaserBuffer[7];
    };

	inline float Output() {
		return PhaserDecimator.process(PhaserBuffer);
	}
	private:
		class AllpassDelay {
		public:
			AllpassDelay():
				_a1(0.0f),
				_zm1(0.0f)
				{

				}

			void Delay(float delay)	{ //sample delay time
				_a1 = (1.0f - delay) / (1.0f + delay);
			}

			float Update(float inSamp){
				float y = inSamp * -_a1 + _zm1;
				_zm1 = y * _a1 + inSamp;

				return y;
			}

		private:
			float _a1, _zm1;
		};

    AllpassDelay _allpass[20];

	float PhaserBuffer[OVERSAMPLE] = {};
	dsp::Decimator<OVERSAMPLE, QUALITY> PhaserDecimator;

    float _dmin, _dmax; //range
    float _fb; //feedback
    float _lfoPhase;
    float _lfoInc;
    float _depth;
	int _stage;
    float _zm1;
	float _pw;
	float dlfo;
	float y;
	float _lfodepth;
	float in;
};

struct PhaserModule : Module {

	enum ParamIds {
		RANGE_LOW,
		RANGE_HIGH,
		STAGE_PARAM,
		RATE_PARAM,
		FEEDBACK_PARAM,
		DEPTH_PARAM,
		TYPE,
		LFODEPTH,
		PW_PARAM,
		NUM_PARAMS
	};

	enum InputIds {
		LFODEPTH_INPUT,
		LOW_INPUT,
		HIGH_INPUT,
		INPUT,
		NUM_INPUTS
	};

	enum OutputIds {
		OUTPUT,
		NUM_OUTPUTS
	};

	int Theme = 0;

	float rate = 0.0f;
	float feedback = 0.0f;
	float depth = 0.0f;
	float input = 0.0f;
	float out = 0.0f;
	int stage = 0;
	int type = 0;
	float phase = 0.0f;
	float low = 0.0f;
	float high = 0.0f;
	float lfodepth = 0.0f;
	float PW = 0.0f;

	Phaser<8, 8> pha;

	PhaserModule() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
    configParam(PhaserModule::TYPE, 0.0, 3.0, 0.0, "");
    configParam(PhaserModule::RANGE_LOW, 0.0, 1.0, 0.2, "");
    configParam(PhaserModule::RANGE_HIGH, 0.0, 1.0, 0.4, "");
    configParam(PhaserModule::RATE_PARAM, 0.0, 1.0, 0.05, "");
    configParam(PhaserModule::FEEDBACK_PARAM, 0.0, 0.95, 0.475, "");
    configParam(PhaserModule::STAGE_PARAM, 0.0, 18.0, 1.0, "");
    configParam(PhaserModule::LFODEPTH, 0.0, 1.0, 0.1, "");
    configParam(PhaserModule::PW_PARAM, 0.0, 1.0, 0.5, "");
    configParam(PhaserModule::DEPTH_PARAM, 0.0, 1.0, 0.5, "");
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

void PhaserModule::process(const ProcessArgs& args) {

	rate = params[RATE_PARAM].value;
	rate *= 7.0f;

	lfodepth = clamp(params[LFODEPTH].value + inputs[LFODEPTH_INPUT].value / 10.0f, 0.0f, 1.0f);

	type = params[TYPE].value;

	feedback = params[FEEDBACK_PARAM].value;
	depth = params[DEPTH_PARAM].value;

	stage = params[STAGE_PARAM].value;

	input = inputs[INPUT].value / 5.0f;

	PW = params[PW_PARAM].value;

	low = clamp(params[RANGE_LOW].value + inputs[LOW_INPUT].value / 10.0f, 0.0f, 1.0f);
	low *= 20000.0f;
	high = clamp(params[RANGE_HIGH].value + inputs[HIGH_INPUT].value / 10.0f, 0.0f, 1.0f);
	high *= 20000.0f;

		pha.Rate(rate);
		pha.Range(low, high);
		pha.LFODepth(lfodepth);
		pha.Pulsewidth(PW);

		pha.Feedback(feedback);
		pha.Depth(depth);

		pha.Stage(stage);

	out = saturate(2.5f * (pha.Update(input, type)));

	outputs[OUTPUT].value = out;
};

struct PhaserClassicMenu : MenuItem {
	PhaserModule *phasermodule;
	void onAction(const event::Action &e) override {
		phasermodule->Theme = 0;
	}
	void step() override {
		rightText = (phasermodule->Theme == 0) ? "✔" : "";
		MenuItem::step();
	}
};

struct PhaserNightModeMenu : MenuItem {
	PhaserModule *phasermodule;
	void onAction(const event::Action &e) override {
		phasermodule->Theme = 1;
	}
	void step() override {
		rightText = (phasermodule->Theme == 1) ? "✔" : "";
		MenuItem::step();
	}
};

struct PhaserModuleWidget : ModuleWidget {
	// Panel Themes
	SvgPanel *panelClassic;
	SvgPanel *panelNightMode;

  void appendContextMenu(Menu* menu) override {
  	PhaserModule *phasermodule = dynamic_cast<PhaserModule*>(module);
  	assert(phasermodule);
  	menu->addChild(construct<MenuEntry>());
  	menu->addChild(construct<MenuLabel>(&MenuLabel::text, "Theme"));
  	menu->addChild(construct<PhaserClassicMenu>(&PhaserClassicMenu::text, "Classic (default)", &PhaserClassicMenu::phasermodule, phasermodule));
  	menu->addChild(construct<PhaserNightModeMenu>(&PhaserNightModeMenu::text, "Night Mode", &PhaserNightModeMenu::phasermodule, phasermodule));
  }

	PhaserModuleWidget(PhaserModule *module);
	void step() override;
};

PhaserModuleWidget::PhaserModuleWidget(PhaserModule *module) {
	setModule(module);
	box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	panelClassic = new SvgPanel();
	panelClassic->box.size = box.size;
	panelClassic->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Panels/Phaser.svg")));
	addChild(panelClassic);

	panelNightMode = new SvgPanel();
	panelNightMode->box.size = box.size;
	panelNightMode->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Panels/Phaser-Dark.svg")));
	addChild(panelNightMode);

	addChild(createWidget<MScrewA>(Vec(0, 0)));
	addChild(createWidget<MScrewC>(Vec(box.size.x-15, 0)));
	addChild(createWidget<MScrewD>(Vec(0, 365)));
	addChild(createWidget<MScrewA>(Vec(box.size.x-15, 365)));


	addParam(createParam<FMSM>(Vec(14, 258), module, PhaserModule::TYPE));

	addParam(createParam<GreenSmallKnob>(Vec(9, 48), module, PhaserModule::RANGE_LOW));
	addParam(createParam<GreenSmallKnob>(Vec(50, 48), module, PhaserModule::RANGE_HIGH));

	addParam(createParam<GreenSmallKnob>(Vec(9, 98), module, PhaserModule::RATE_PARAM));
	addParam(createParam<GreenSmallKnob>(Vec(50, 98), module, PhaserModule::FEEDBACK_PARAM));

	addParam(createParam<BlueSmallKnob>(Vec(9, 148), module, PhaserModule::STAGE_PARAM));
	addParam(createParam<GreenSmallKnob>(Vec(50, 148), module, PhaserModule::LFODEPTH));

	addParam(createParam<BlueSmallKnob>(Vec(50, 208), module, PhaserModule::PW_PARAM));
	addParam(createParam<BlueSmallKnob>(Vec(3, 208), module, PhaserModule::DEPTH_PARAM));

	addInput(createInput<SilverSixPort>(Vec(5, 290), module, PhaserModule::LOW_INPUT));
	addInput(createInput<SilverSixPortD>(Vec(33, 290), module, PhaserModule::HIGH_INPUT));
	addInput(createInput<SilverSixPortE>(Vec(60, 290), module, PhaserModule::LFODEPTH_INPUT));

	addInput(createInput<SilverSixPortA>(Vec(5, 328), module, PhaserModule::INPUT));
	addOutput(createOutput<SilverSixPortB>(Vec(60, 328), module, PhaserModule::OUTPUT));
};

void PhaserModuleWidget::step() {
  if (module) {
  	PhaserModule *phasermodule = dynamic_cast<PhaserModule*>(module);
  	assert(phasermodule);
  	panelClassic->visible = (phasermodule->Theme == 0);
  	panelNightMode->visible = (phasermodule->Theme == 1);
  }

	ModuleWidget::step();
}

Model *modelPhaserModule = createModel<PhaserModule, PhaserModuleWidget>("Phaser");
