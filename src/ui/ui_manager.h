#pragma once

#include "../rendering/renderer.h"

#include "component.h"
#include "container.h"
#include "panel.h"
#include "label.h"
#include "button.h"
#include "checkbox.h"
#include "text_field.h"

#include "../universe/mass_body.h"

namespace ui {
	struct BodyPropertyComponents {
		TextFieldComponent* massInput;
		TextFieldComponent* sizeInput;
		TextFieldComponent* colorInput;
		CheckBoxComponent* affectedByGravityCB;
		CheckBoxComponent* affectsOthersCB;
		ButtonComponent* btnApplyProperties;
		ButtonComponent* btnSetEmissive;
	};

	struct SceneSettingsComponents {
		TextFieldComponent* gravityConstantInput;
		TextFieldComponent* timeScaleInput;
		ButtonComponent* applySettingsBtn;
		TextFieldComponent* sceneNameInput;
		ButtonComponent* saveButton;
		ButtonComponent* loadButton;
	};

	extern Panel* objectPanel;

	extern BodyPropertyComponents bodyPropertyComponents;
	extern SceneSettingsComponents sceneSettingsComponents;
	extern MassBody* selectedBody; // Can be different from the focused body (currently just during the spawn process)

	void setupUIPanels();
	void showBodyProperties(MassBody* body, std::string label="Selected body");

	Universe* generateUniverse(unsigned int id); // Depreacated and should be removed

	// Delete all components from memory
	void disposeUI();
}