#include "MSM.hpp"
#include "MSMComponentLibrary.hpp"

struct VCA : Module {
	enum ParamIds {
		LEVEL_A_L,
		LEVEL_A_R,
		PAN_A,
		MODE_LIN_EXP_L_PARAM_A,
		MODE_LIN_EXP_R_PARAM_A,
		MODE_A,
		NUM_PARAMS
	};
	enum InputIds {
		IN_L_A,
		IN_R_A,
		IN_P_A,
		CV1_A,
		CV2_A,
		CV_PAN_INPUT_A,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTL_A,
		OUTR_A,
		OUTLP_A,
		OUTRP_A,
		SUM_POS_A,
		SUM_NEG_A,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	float mixL_A = 0.0f ? 0.0f : 0.0f;
	float mixR_A = 0.0f ? 0.0f : 0.0f;
	float mixLP_A = 0.0f ? 0.0f : 0.0f;
	float mixRP_A = 0.0f ? 0.0f : 0.0f;

	int Theme = 0;
	const float expBase = 50.0f;

	VCA() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(VCA::MODE_A, 0.0, 1.0, 1.0, "");
		configParam(VCA::MODE_LIN_EXP_L_PARAM_A, 0.0, 1.0, 1.0, "");
		configParam(VCA::MODE_LIN_EXP_R_PARAM_A, 0.0, 1.0, 1.0, "");
		configParam(VCA::LEVEL_A_L, 0.0, 1.0, 0.5, "");
		configParam(VCA::LEVEL_A_R, 0.0, 1.0, 0.5, "");
		configParam(VCA::PAN_A, -1.0, 1.0, 0.0, "");
	}

	void process(const ProcessArgs& args) override;

	//PAN LEVEL
	float PanL(float balance, float cv){ // -1...+1
			float p, gl;
			p=M_PI*(balance+1)/4;
			if(cv){
				gl=std::cos(p)*(1+((cv+1)/5));
			}else
				{gl=std::cos(p);
			}
			return gl;
	}

	float PanR(float balance , float cv){
			float p, gr;
			p=M_PI*(balance+1)/4;
			if(cv){gr=std::sin(p)*(1-((cv-1)/5));
			}else{
				gr=std::sin(p);
			}
			return gr;
	}

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

void VCA::process(const ProcessArgs& args) {

	float IN_A = inputs[IN_L_A].active ? inputs[IN_L_A].value : 0.0f;
	float IN_B = inputs[IN_R_A].active ? inputs[IN_R_A].value : 0.0f;
	float IN_PAN = IN_A + IN_B;

	if(params[MODE_A].value==1) { // normal mode
		//VCA L
		mixL_A = IN_A * params[LEVEL_A_L].value;
		if(inputs[CV1_A].active){
			if(params[MODE_LIN_EXP_L_PARAM_A].value==1){
				mixL_A *= clamp(inputs[CV1_A].value / 10.0f, 0.0f, 1.0f);
			}
			else{
				mixL_A *= rescale(powf(expBase, clamp(inputs[CV1_A].value / 10.0f, 0.0f, 1.0f)), 1.0f, expBase, 0.0f, 1.0f);
			}
		}

		//VCA R
		mixR_A = IN_B * params[LEVEL_A_R].value;
		if(inputs[CV2_A].active){
			if(params[MODE_LIN_EXP_R_PARAM_A].value==1){
				mixR_A *= clamp(inputs[CV2_A].value / 10.0f, 0.0f, 1.0f);
			}
			else{
				mixR_A *= rescale(powf(expBase, clamp(inputs[CV2_A].value / 10.0f, 0.0f, 1.0f)), 1.0f, expBase, 0.0f, 1.0f);
			}
		}

		// Outputs
		if(outputs[OUTL_A].active) {
			outputs[OUTL_A].value = 1.5f * saturate(mixL_A);
		}
		if(outputs[OUTR_A].active) {
			outputs[OUTR_A].value = 1.5f * saturate(mixR_A);
		}
		if(outputs[SUM_POS_A].active) {
			outputs[SUM_POS_A].value = saturate(mixL_A + mixR_A);
		}
		if(outputs[SUM_NEG_A].active) {
			outputs[SUM_NEG_A].value = 1.0f - saturate(mixL_A + mixR_A);
		}
	}


	if(params[MODE_A].value==0) { // pan mode
		// VCA L PAN
		mixRP_A = (IN_PAN * params[LEVEL_A_R].value) * PanR(params[PAN_A].value,(inputs[CV_PAN_INPUT_A].value));
		if(inputs[CV2_A].active) {
			if(params[MODE_LIN_EXP_R_PARAM_A].value==1) {
				mixRP_A *= clamp(inputs[CV2_A].value / 10.0f, 0.0f, 1.0f);
			}
			else {
				mixRP_A *= rescale(powf(expBase, clamp(inputs[CV2_A].value / 10.0f, 0.0f, 1.0f)), 1.0f, expBase, 0.0f, 1.0f);
			}
		}

		// VCA R PAN
		mixLP_A = (IN_PAN * params[LEVEL_A_L].value) * PanL(params[PAN_A].value,(inputs[CV_PAN_INPUT_A].value));
		if(inputs[CV1_A].active) {
			if(params[MODE_LIN_EXP_L_PARAM_A].value==1) {
				mixLP_A *= clamp(inputs[CV1_A].value / 10.0f, 0.0f, 1.0f);
			}
			else {
				mixLP_A *= rescale(powf(expBase, clamp(inputs[CV1_A].value / 10.0f, 0.0f, 1.0f)), 1.0f, expBase, 0.0f, 1.0f);
			}
		}

		// Outputs
		if(outputs[OUTL_A].active) {
		outputs[OUTL_A].value = saturate(mixLP_A);
		}
		if(outputs[OUTR_A].active) {
		outputs[OUTR_A].value = saturate(mixRP_A);
		}
		if(outputs[SUM_POS_A].active) {
			outputs[SUM_POS_A].value = saturate(mixLP_A + mixRP_A);
		}
		if(outputs[SUM_NEG_A].active) {
			outputs[SUM_NEG_A].value = 1.0f - saturate(mixLP_A + mixRP_A);
		}

	}
};

struct VCAClassicMenu : MenuItem {
	VCA *vca;
	void onAction(const event::Action &e) override {
		vca->Theme = 0;
	}
	void step() override {
		rightText = (vca->Theme == 0) ? "✔" : "";
		MenuItem::step();
	}
};

struct VCANightModeMenu : MenuItem {
	VCA *vca;
	void onAction(const event::Action &e) override {
		vca->Theme = 1;
	}
	void step() override {
		rightText = (vca->Theme == 1) ? "✔" : "";
		MenuItem::step();
	}
};

struct VCAWidget : ModuleWidget {
	// Panel Themes
	SvgPanel *panelClassic;
	SvgPanel *panelNightMode;

	void appendContextMenu(Menu* menu) override {
		VCA *vca = dynamic_cast<VCA*>(module);
		assert(vca);
		menu->addChild(construct<MenuEntry>());
		menu->addChild(construct<MenuLabel>(&MenuLabel::text, "Theme"));
		menu->addChild(construct<VCAClassicMenu>(&VCAClassicMenu::text, "Classic (default)", &VCAClassicMenu::vca, vca));
		menu->addChild(construct<VCANightModeMenu>(&VCANightModeMenu::text, "Night Mode", &VCANightModeMenu::vca, vca));
	}

	VCAWidget(VCA *module);
	void step() override;
};

VCAWidget::VCAWidget(VCA *module) {
	setModule(module);
	box.size = Vec(8 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	panelClassic = new SvgPanel();
	panelClassic->box.size = box.size;
	panelClassic->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Panels/VCA.svg")));
	addChild(panelClassic);

	panelNightMode = new SvgPanel();
	panelNightMode->box.size = box.size;
	panelNightMode->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Panels/VCA-Dark.svg")));
	addChild(panelNightMode);

	addChild(createWidget<MScrewA>(Vec(15, 0)));
	addChild(createWidget<MScrewD>(Vec(box.size.x-30, 0)));
	addChild(createWidget<MScrewC>(Vec(15, 365)));
	addChild(createWidget<MScrewB>(Vec(box.size.x-30, 365)));

	//addParam
	addParam(createParam<VioM2Switch>(Vec(53, 60), module, VCA::MODE_A));
	addParam(createParam<VioM2Switch>(Vec(10, 190), module, VCA::MODE_LIN_EXP_L_PARAM_A));
	addParam(createParam<VioM2Switch>(Vec(95, 190), module, VCA::MODE_LIN_EXP_R_PARAM_A));

	addParam(createParam<GreenLargeKnob>(Vec(10, 100), module, VCA::LEVEL_A_L));
	addParam(createParam<GreenLargeKnob>(Vec(64, 100), module, VCA::LEVEL_A_R));

	addParam(createParam<GreenLargeKnob>(Vec(36, 175), module, VCA::PAN_A));
	//addInput
	addInput(createInput<SilverSixPortA>(Vec(8, 240), module, VCA::IN_L_A));
	addInput(createInput<SilverSixPortD>(Vec(86, 240), module, VCA::IN_R_A));

	addInput(createInput<SilverSixPortC>(Vec(8, 280), module, VCA::CV1_A));
	addInput(createInput<SilverSixPortA>(Vec(86, 280), module, VCA::CV2_A));

	addInput(createInput<SilverSixPortD>(Vec(47, 240), module, VCA::CV_PAN_INPUT_A));

	//addOutput
	addOutput(createOutput<SilverSixPortB>(Vec(8, 320), module, VCA::OUTL_A));
	addOutput(createOutput<SilverSixPortC>(Vec(86, 320), module, VCA::OUTR_A));

	addOutput(createOutput<SilverSixPortD>(Vec(47, 280), module, VCA::SUM_POS_A));
	addOutput(createOutput<SilverSixPortA>(Vec(47, 320), module, VCA::SUM_NEG_A));

};

void VCAWidget::step() {
	if (module) {
		VCA *vca = dynamic_cast<VCA*>(module);
		assert(vca);
		panelClassic->visible = (vca->Theme == 0);
		panelNightMode->visible = (vca->Theme == 1);
	}

	ModuleWidget::step();
}

Model *modelVCA = createModel<VCA, VCAWidget>("PAN-VCA");
