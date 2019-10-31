#include "MSM.hpp"
#include "MSMComponentLibrary.hpp"

/*
class One {
public:
    One()
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
*/
struct CrazyMult : Module {

	enum ParamIds {
		//AVG_PARAM,
		AorB_1_PARAM,
		AorB_2_PARAM,
		AorB_3_PARAM,
		AorB_4_PARAM,
		AorB_5_PARAM,
		AorB_6_PARAM,
		AorB_7_PARAM,
		AorB_8_PARAM,
		NUM_PARAMS
	};

	enum InputIds {
		IN_1_INPUT,
		IN_2_INPUT,
		IN_3_INPUT,
		IN_4_INPUT,
		IN_5_INPUT,
		IN_6_INPUT,
		IN_7_INPUT,
		IN_8_INPUT,

		CV_1_INPUT,
		CV_2_INPUT,
		CV_3_INPUT,
		CV_4_INPUT,
		CV_5_INPUT,
		CV_6_INPUT,
		CV_7_INPUT,
		CV_8_INPUT,
		NUM_INPUTS
	};

	enum OutputIds {
		OUTA1_OUTPUT,
		OUTA2_OUTPUT,
		OUTA3_OUTPUT,
		OUTA4_OUTPUT,
		OUTA5_OUTPUT,
		OUTA6_OUTPUT,
		OUTA7_OUTPUT,
		OUTA8_OUTPUT,

		OUTB1_OUTPUT,
		OUTB2_OUTPUT,
		OUTB3_OUTPUT,
		OUTB4_OUTPUT,
		OUTB5_OUTPUT,
		OUTB6_OUTPUT,
		OUTB7_OUTPUT,
		OUTB8_OUTPUT,
		NUM_OUTPUTS
	};

	int Theme = 0;
	int Type = 0;

	//One op;

	CrazyMult() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(CrazyMult::AorB_1_PARAM, -1.0, 1.0, 0.0, "");
		configParam(CrazyMult::AorB_2_PARAM, -1.0, 1.0, 0.0, "");
		configParam(CrazyMult::AorB_3_PARAM, -1.0, 1.0, 0.0, "");
		configParam(CrazyMult::AorB_4_PARAM, -1.0, 1.0, 0.0, "");
		configParam(CrazyMult::AorB_5_PARAM, -1.0, 1.0, 0.0, "");
		configParam(CrazyMult::AorB_6_PARAM, -1.0, 1.0, 0.0, "");
		configParam(CrazyMult::AorB_7_PARAM, -1.0, 1.0, 0.0, "");
		configParam(CrazyMult::AorB_8_PARAM, -1.0, 1.0, 0.0, "");
	}

	void process(const ProcessArgs& args) override;

	//Json for Panel Theme
	json_t *dataToJson() override	{
		json_t *rootJ = json_object();
		json_object_set_new(rootJ, "Theme", json_integer(Theme));
		json_object_set_new(rootJ, "Type", json_integer(Type));
		return rootJ;
	}
	void dataFromJson(json_t *rootJ) override	{
		json_t *ThemeJ = json_object_get(rootJ, "Theme");
		if (ThemeJ)
			Theme = json_integer_value(ThemeJ);
		json_t *TypeJ = json_object_get(rootJ, "Type");
		if (TypeJ)
			Type = json_integer_value(TypeJ);
	}

	void onReset() override {
		Type = 0;
	}
};

void CrazyMult::process(const ProcessArgs& args) {
	//double Out_A[8] = {};
	//double Out_B[8] = {};

	double Out_A = 0.0f; //? 0.0f : 0.0f;
	double Out_B = 0.0f; //? 0.0f : 0.0f;

	int countA[8] = {};
	int countB[8] = {};

	char ParamsA = 0.0f;

	switch(Type) {
		case 0:
			for(int i = 0; i < 8; i++) {
				ParamsA = clamp((params[AorB_1_PARAM+i].value + dsp::quadraticBipolar(inputs[CV_1_INPUT+i].value) / 10.0f), -1.0f, 1.0f);

				if(ParamsA == -1.0f) {
						Out_A += inputs[IN_1_INPUT+i].getNormalVoltage(0.0f);
							countA[i]++;
				}
			}
			for(int i = 0; i < 8; i++) {
				if (1 && countA[i] > 0)
					if(params[AorB_1_PARAM+i].value == -1.0f) Out_A *= (0.8 * countA[i]);

				outputs[OUTA1_OUTPUT].value = saturate(Out_A);
				outputs[OUTA2_OUTPUT].value = saturate(Out_A);
				outputs[OUTA3_OUTPUT].value = saturate(Out_A);
				outputs[OUTA4_OUTPUT].value = saturate(Out_A);
				outputs[OUTA5_OUTPUT].value = saturate(Out_A);
				outputs[OUTA6_OUTPUT].value = saturate(Out_A);
				outputs[OUTA7_OUTPUT].value = saturate(Out_A);
				outputs[OUTA8_OUTPUT].value = saturate(Out_A);
			}

			for(int i = 0; i < 8; i++) {

				ParamsA = clamp((params[AorB_1_PARAM+i].value + dsp::quadraticBipolar(inputs[CV_1_INPUT+i].value) / 10.0f), -1.0f, 1.0f);

				if(ParamsA == 1.0f) {
						Out_B += inputs[IN_1_INPUT+i].getNormalVoltage(0.0f);
							countB[i]++;
				}

			}
			for(int i = 0; i < 8; i++) {
				if (1 && countB[i] > 0)
					if(params[AorB_1_PARAM+i].value == 1.0f) Out_B *= (0.8 * countB[i]);

				outputs[OUTB1_OUTPUT].value = saturate(Out_B);
				outputs[OUTB2_OUTPUT].value = saturate(Out_B);
				outputs[OUTB3_OUTPUT].value = saturate(Out_B);
				outputs[OUTB4_OUTPUT].value = saturate(Out_B);
				outputs[OUTB5_OUTPUT].value = saturate(Out_B);
				outputs[OUTB6_OUTPUT].value = saturate(Out_B);
				outputs[OUTB7_OUTPUT].value = saturate(Out_B);
				outputs[OUTB8_OUTPUT].value = saturate(Out_B);
			}
		break;
		case 1:
			for(int i = 0; i < 8; i++) {
				ParamsA = clamp((params[AorB_1_PARAM+i].value + dsp::quadraticBipolar(inputs[CV_1_INPUT+i].value) / 10.0f), -1.0f, 1.0f);

				if(ParamsA == -1.0f) {
						Out_A -= inputs[IN_1_INPUT+i].getNormalVoltage(0.0f);
							countA[i]++;
				}
			}
			for(int i = 0; i < 8; i++) {
				if (1 && countA[i] > 0)
					if(params[AorB_1_PARAM+i].value == -1.0f) Out_A *= (0.8 * countA[i]);

				outputs[OUTA1_OUTPUT].value = saturate(Out_A);
				outputs[OUTA2_OUTPUT].value = saturate(Out_A);
				outputs[OUTA3_OUTPUT].value = saturate(Out_A);
				outputs[OUTA4_OUTPUT].value = saturate(Out_A);
				outputs[OUTA5_OUTPUT].value = saturate(Out_A);
				outputs[OUTA6_OUTPUT].value = saturate(Out_A);
				outputs[OUTA7_OUTPUT].value = saturate(Out_A);
				outputs[OUTA8_OUTPUT].value = saturate(Out_A);
			}

			for(int i = 0; i < 8; i++) {

				ParamsA = clamp((params[AorB_1_PARAM+i].value + dsp::quadraticBipolar(inputs[CV_1_INPUT+i].value) / 10.0f), -1.0f, 1.0f);

				if(ParamsA == 1.0f) {
						Out_B -= inputs[IN_1_INPUT+i].getNormalVoltage(0.0f);
							countB[i]++;
				}

			}
			for(int i = 0; i < 8; i++) {
				if (1 && countB[i] > 0)
					if(params[AorB_1_PARAM+i].value == 1.0f) Out_B *= (0.8 * countB[i]);

				outputs[OUTB1_OUTPUT].value = saturate(Out_B);
				outputs[OUTB2_OUTPUT].value = saturate(Out_B);
				outputs[OUTB3_OUTPUT].value = saturate(Out_B);
				outputs[OUTB4_OUTPUT].value = saturate(Out_B);
				outputs[OUTB5_OUTPUT].value = saturate(Out_B);
				outputs[OUTB6_OUTPUT].value = saturate(Out_B);
				outputs[OUTB7_OUTPUT].value = saturate(Out_B);
				outputs[OUTB8_OUTPUT].value = saturate(Out_B);
			}
		break;
	}
};

struct ClassicMenu : MenuItem {
	CrazyMult *crazymult;
	void onAction(const event::Action &e) override {
		crazymult->Theme = 0;
	}
	void step() override {
		rightText = (crazymult->Theme == 0) ? "✔" : "";
		MenuItem::step();
	}
};

struct NightModeMenu : MenuItem {
	CrazyMult *crazymult;
	void onAction(const event::Action &e) override {
		crazymult->Theme = 1;
	}
	void step() override {
		rightText = (crazymult->Theme == 1) ? "✔" : "";
		MenuItem::step();
	}
};

struct TypeMenuPlus : MenuItem {
	CrazyMult *crazymult;
	void onAction(const event::Action &e) override {
		crazymult->Type = 0;
	}
	void step() override {
		rightText = (crazymult->Type == 0) ? "✔" : "";
		MenuItem::step();
	}
};

struct TypeMenuMinus : MenuItem {
	CrazyMult *crazymult;
	void onAction(const event::Action &e) override {
		crazymult->Type = 1;
	}
	void step() override {
		rightText = (crazymult->Type == 1) ? "✔" : "";
		MenuItem::step();
	}
};

struct CrazyMultWidget : ModuleWidget {
		// Panel Themes
	SvgPanel *panelClassic;
	SvgPanel *panelNightMode;

	void appendContextMenu(Menu *menu) override {
		CrazyMult *crazymult = dynamic_cast<CrazyMult*>(module);
		assert(crazymult);
		menu->addChild(construct<MenuEntry>());
		menu->addChild(construct<MenuLabel>(&MenuLabel::text, "Theme"));
		menu->addChild(construct<ClassicMenu>(&ClassicMenu::text, "Classic (default)", &ClassicMenu::crazymult, crazymult));
		menu->addChild(construct<NightModeMenu>(&NightModeMenu::text, "Night Mode", &NightModeMenu::crazymult, crazymult));
		menu->addChild(construct<MenuLabel>(&MenuLabel::text, "Type"));
		menu->addChild(construct<TypeMenuPlus>(&TypeMenuPlus::text, "Positive", &TypeMenuPlus::crazymult, crazymult));
		menu->addChild(construct<TypeMenuMinus>(&TypeMenuMinus::text, "Negative", &TypeMenuMinus::crazymult, crazymult));
	}

	CrazyMultWidget(CrazyMult *module);
	void step() override;
};

CrazyMultWidget::CrazyMultWidget(CrazyMult *module) {
	setModule(module);
	box.size = Vec(16 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	panelClassic = new SvgPanel();
	panelClassic->box.size = box.size;
	panelClassic->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Panels/CrazyMult.svg")));
	addChild(panelClassic);

	panelNightMode = new SvgPanel();
	panelNightMode->box.size = box.size;
	panelNightMode->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Panels/CrazyMult-Dark.svg")));
	addChild(panelNightMode);

	int space = 25;
	int horspace = 20;
	//Params
	int vertspace = 29;

	addChild(createWidget<MScrewA>(Vec(15, 0)));
	addChild(createWidget<MScrewA>(Vec(15, 365)));
	addChild(createWidget<MScrewD>(Vec(210, 0)));
	addChild(createWidget<MScrewC>(Vec(210, 365)));

	//PARAM

	//addParam(createParam<MToggleGrey>(Vec(55 , 5), module, CrazyMult::AVG_PARAM));

	addParam(createParam<MThree2>(Vec(105 , 20 + vertspace), module, CrazyMult::AorB_1_PARAM));
	addParam(createParam<MThree2>(Vec(105, 60 + vertspace), module, CrazyMult::AorB_2_PARAM));
	addParam(createParam<MThree2>(Vec(105, 100 + vertspace), module, CrazyMult::AorB_3_PARAM));
	addParam(createParam<MThree2>(Vec(105, 140 + vertspace), module, CrazyMult::AorB_4_PARAM));
	addParam(createParam<MThree2>(Vec(105, 180 + vertspace), module, CrazyMult::AorB_5_PARAM));
	addParam(createParam<MThree2>(Vec(105, 220 + vertspace), module, CrazyMult::AorB_6_PARAM));
	addParam(createParam<MThree2>(Vec(105, 260 + vertspace), module, CrazyMult::AorB_7_PARAM));
	addParam(createParam<MThree2>(Vec(105, 300 + vertspace), module, CrazyMult::AorB_8_PARAM));
	//INPUTS
	addInput(createInput<SilverSixPortC>(Vec(10, 20 + space), module, CrazyMult::IN_1_INPUT));
	addInput(createInput<SilverSixPortD>(Vec(10, 60 + space), module, CrazyMult::IN_2_INPUT));
	addInput(createInput<SilverSixPortC>(Vec(10, 100 + space), module, CrazyMult::IN_3_INPUT));
	addInput(createInput<SilverSixPortB>(Vec(10, 140 + space), module, CrazyMult::IN_4_INPUT));
	addInput(createInput<SilverSixPortA>(Vec(10, 180 + space), module, CrazyMult::IN_5_INPUT));
	addInput(createInput<SilverSixPort>(Vec(10, 220 + space), module, CrazyMult::IN_6_INPUT));
	addInput(createInput<SilverSixPortC>(Vec(10, 260 + space), module, CrazyMult::IN_7_INPUT));
	addInput(createInput<SilverSixPortD>(Vec(10, 300 + space), module, CrazyMult::IN_8_INPUT));

	//CV INPUTS
	addInput(createInput<SilverSixPortE>(Vec(40 + horspace, 20 + space), module, CrazyMult::CV_1_INPUT));
	addInput(createInput<SilverSixPortC>(Vec(40 + horspace, 60 + space), module, CrazyMult::CV_2_INPUT));
	addInput(createInput<SilverSixPortD>(Vec(40 + horspace, 100 + space), module, CrazyMult::CV_3_INPUT));
	addInput(createInput<SilverSixPort>(Vec(40 + horspace, 140 + space), module, CrazyMult::CV_4_INPUT));
	addInput(createInput<SilverSixPortA>(Vec(40 + horspace, 180 + space), module, CrazyMult::CV_5_INPUT));
	addInput(createInput<SilverSixPortC>(Vec(40 + horspace, 220 + space), module, CrazyMult::CV_6_INPUT));
	addInput(createInput<SilverSixPortB>(Vec(40 + horspace, 260 + space), module, CrazyMult::CV_7_INPUT));
	addInput(createInput<SilverSixPort>(Vec(40 + horspace, 300 + space), module, CrazyMult::CV_8_INPUT));

	//OUTPUTS
	addOutput(createOutput<SilverSixPortA>(Vec(162, 20 + space), module, CrazyMult::OUTA1_OUTPUT));
	addOutput(createOutput<SilverSixPortC>(Vec(162, 60 + space), module, CrazyMult::OUTA2_OUTPUT));
	addOutput(createOutput<SilverSixPortA>(Vec(162, 100 + space), module, CrazyMult::OUTA3_OUTPUT));
	addOutput(createOutput<SilverSixPortB>(Vec(162, 140 + space), module, CrazyMult::OUTA4_OUTPUT));
	addOutput(createOutput<SilverSixPortD>(Vec(162, 180 + space), module, CrazyMult::OUTA5_OUTPUT));
	addOutput(createOutput<SilverSixPortA>(Vec(162, 220 + space), module, CrazyMult::OUTA6_OUTPUT));
	addOutput(createOutput<SilverSixPortC>(Vec(162, 260 + space), module, CrazyMult::OUTA7_OUTPUT));
	addOutput(createOutput<SilverSixPortD>(Vec(162, 300 + space), module, CrazyMult::OUTA8_OUTPUT));

	addOutput(createOutput<SilverSixPortA>(Vec(206, 20 + space), module, CrazyMult::OUTB1_OUTPUT));
	addOutput(createOutput<SilverSixPortC>(Vec(206, 60 + space), module, CrazyMult::OUTB2_OUTPUT));
	addOutput(createOutput<SilverSixPort>(Vec(206, 100 + space), module, CrazyMult::OUTB3_OUTPUT));
	addOutput(createOutput<SilverSixPortC>(Vec(206, 140 + space), module, CrazyMult::OUTB4_OUTPUT));
	addOutput(createOutput<SilverSixPortE>(Vec(206, 180 + space), module, CrazyMult::OUTB5_OUTPUT));
	addOutput(createOutput<SilverSixPortB>(Vec(206, 220 + space), module, CrazyMult::OUTB6_OUTPUT));
	addOutput(createOutput<SilverSixPortB>(Vec(206, 260 + space), module, CrazyMult::OUTB7_OUTPUT));
	addOutput(createOutput<SilverSixPortC>(Vec(206, 300 + space), module, CrazyMult::OUTB8_OUTPUT));

};

void CrazyMultWidget::step() {
	if (module) {
		CrazyMult *crazymult = dynamic_cast<CrazyMult*>(module);
		assert(crazymult);
		panelClassic->visible = (crazymult->Theme == 0);
		panelNightMode->visible = (crazymult->Theme == 1);
	}

	ModuleWidget::step();
}

Model *modelCrazyMult = createModel<CrazyMult, CrazyMultWidget>("CrazyMult");
