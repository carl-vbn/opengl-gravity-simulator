#include "ui_manager.h"
#include "../universe/scene_loader.h"

namespace ui {
	BodyPropertyComponents bodyPropertyComponents;
	SceneSettingsComponents sceneSettingsComponents;

	// DEPRECATED FUNCTION TODO: REMOVE (just left here in case i need to copy something from it)
	Universe* generateUniverse(unsigned int id) {
		Universe* universe = new Universe();
		switch (id)
		{
		case 0:
		{
			universe->AddBody(new MassBody(glm::vec3(0, 0, 0), 50000, 1, Color(0.5f, 0.5f, 1.0f)));

			MassBody* moon = new MassBody(glm::vec3(1.5f, 0, 0), 500, 0.1f, Color(0.5f, 1.0f, 0.5f));
			moon->velocity = glm::vec3(0, 0, 2);
			universe->AddBody(moon);

			break;
		}
		case 1:
		{
			universe->AddBody(new MassBody(glm::vec3(0, 0, 0), 5000000, 2, Color(1.0f, 0.4f, 0.0f)));

			MassBody* moon1 = new MassBody(glm::vec3(20, 0, 0), 100, 0.5f, Color(1.0f, 1.0f, 0.0f));
			moon1->velocity = glm::vec3(0, 0, 5);

			MassBody* moon2 = new MassBody(glm::vec3(-20, 0, 0), 100, 0.5f, Color(1.0f, 1.0f, 0.0f));
			moon2->velocity = glm::vec3(0, 0, -5);

			MassBody* moon3 = new MassBody(glm::vec3(5, 0, 0), 160, 0.8f, Color(1.0f, 0.0f, 0.0f));
			moon3->velocity = glm::vec3(0, 0, -10);

			MassBody* moon4 = new MassBody(glm::vec3(-5, 0, 0), 160, 0.8f, Color(1.0f, 0.0f, 0.0f));
			moon4->velocity = glm::vec3(0, 0, 10);

			universe->AddBody(moon1);
			universe->AddBody(moon2);
			universe->AddBody(moon3);
			universe->AddBody(moon4);

			break;
		}
		case 2:
		{
			MassBody* body1 = new MassBody(glm::vec3(-5, 0, 0), 160, 0.8f, Color(1.0f, 0.0f, 0.0f));
			body1->velocity = glm::vec3(0, 0, 10);

			MassBody* body2 = new MassBody(glm::vec3(5, 0, 0), 160, 0.8f, Color(0.0f, 1.0f, 0.0f));
			body2->velocity = glm::vec3(0, 0, -10);

			MassBody* body3 = new MassBody(glm::vec3(0, 0, 0), 160, 0.8f, Color(0.0f, 0.0f, 1.0f));
			body3->velocity = glm::vec3(0, 2, 0);

			universe->AddBody(body3);
			universe->AddBody(body1);
			universe->AddBody(body2);
		}
		default:
			break;
		}

		return universe;
	}

	void setupUIPanels() {
		bodyPropertyComponents.btnApplyProperties = new ButtonComponent("Apply", []() {
			Universe* universe = renderer::loadedUniverse;
			if (universe != nullptr) {
				MassBody* focusedBody = universe->GetBodies()->at(renderer::camera.focusedBodyIndex);
				focusedBody->mass = std::stof(bodyPropertyComponents.massInput->getText());
				focusedBody->radius = std::stof(bodyPropertyComponents.sizeInput->getText());

				std::stringstream hexStream;
				unsigned int hexRGB;
				hexStream << std::hex << bodyPropertyComponents.colorInput->getText();
				hexStream >> hexRGB;
				focusedBody->color = Color(hexRGB);
			}
		});

		bodyPropertyComponents.massInput = new TextFieldComponent("Mass", "", TFF_DECIMAL_NUMBER);
		bodyPropertyComponents.sizeInput = new TextFieldComponent("Size", "", TFF_DECIMAL_NUMBER);
		bodyPropertyComponents.colorInput = new TextFieldComponent("Color", "", TFF_HEX_NUMBER);
		bodyPropertyComponents.affectedByGravityCB = new CheckBoxComponent("Affected by gravity");
		bodyPropertyComponents.affectsOthersCB = new CheckBoxComponent("Affects others");

		bodyPropertyComponents.affectedByGravityCB->setToggleCallback([](bool checked) {
			Universe* universe = renderer::loadedUniverse;
			if (universe != nullptr) {
				universe->GetBodies()->at(renderer::camera.focusedBodyIndex)->affectedByGravity = checked;
			}
		});

		bodyPropertyComponents.affectsOthersCB->setToggleCallback([](bool checked) {
			Universe* universe = renderer::loadedUniverse;
			if (universe != nullptr) {
				universe->GetBodies()->at(renderer::camera.focusedBodyIndex)->affectsOthers = checked;
			}
		});

		sceneSettingsComponents.timeScaleInput = new TextFieldComponent("Timescale", std::to_string(renderer::loadedUniverse->timeScale), TFF_DECIMAL_NUMBER);
		sceneSettingsComponents.gravityConstantInput = new TextFieldComponent("G-Constant", std::to_string(renderer::loadedUniverse->gConstant), TFF_DECIMAL_NUMBER);
		sceneSettingsComponents.applySettingsBtn = new ButtonComponent("Apply", []() {
			Universe* universe = renderer::loadedUniverse;
			if (universe != nullptr) {
				universe->timeScale = std::stof(sceneSettingsComponents.timeScaleInput->getText());
				universe->gConstant = std::stof(sceneSettingsComponents.gravityConstantInput->getText());
			}
		});

		sceneSettingsComponents.sceneNameInput = new TextFieldComponent("Scene name", "");

		sceneSettingsComponents.saveButton = new ButtonComponent("Save", []() {
			saveScene(renderer::loadedUniverse, std::string("Scenes\\"+sceneSettingsComponents.sceneNameInput->getText()+".scene").c_str());
		});
		sceneSettingsComponents.loadButton = new ButtonComponent("Load", []() {
			std::string sceneName = sceneSettingsComponents.sceneNameInput->getText();
			if (sceneName.length() > 0) {
				if (renderer::loadedUniverse != nullptr) delete renderer::loadedUniverse;
				renderer::setUniverse(loadScene(std::string("Scenes\\" + sceneName + ".scene").c_str()));
			}
		});

		Container bodyPropertiesContainer = Container("Properties");
		bodyPropertiesContainer.AddComponent(bodyPropertyComponents.massInput);
		bodyPropertiesContainer.AddComponent(bodyPropertyComponents.sizeInput);
		bodyPropertiesContainer.AddComponent(bodyPropertyComponents.colorInput);
		bodyPropertiesContainer.AddComponent(bodyPropertyComponents.btnApplyProperties);

		Container gravitySettingsContainer = Container("Gravity settings");
		gravitySettingsContainer.AddComponent(bodyPropertyComponents.affectedByGravityCB);
		gravitySettingsContainer.AddComponent(bodyPropertyComponents.affectsOthersCB);

		Container universeSettingsContainer = Container("Universe settings");
		universeSettingsContainer.AddComponent(sceneSettingsComponents.timeScaleInput);
		universeSettingsContainer.AddComponent(sceneSettingsComponents.gravityConstantInput);
		universeSettingsContainer.AddComponent(sceneSettingsComponents.applySettingsBtn);

		Container sceneSettingsContainer = Container("Scene");
		sceneSettingsContainer.AddComponent(sceneSettingsComponents.sceneNameInput);
		sceneSettingsContainer.AddComponent(sceneSettingsComponents.saveButton);
		sceneSettingsContainer.AddComponent(sceneSettingsComponents.loadButton);

		Panel objectPanel = Panel("Object", Rectangle(0.8f, 0.5f, 0.99f, 0.98f, Rectangle(-1.0f, -1.0f, 1.0f, 1.0f)));
		objectPanel.AddContainer(bodyPropertiesContainer);
		objectPanel.AddContainer(gravitySettingsContainer);

		Panel universePanel = Panel("Scene settings", Rectangle(0.02f, 0.02f, 0.2f, 0.5f, Rectangle(-1.0f, -1.0f, 1.0f, 1.0f)));
		universePanel.AddContainer(universeSettingsContainer);
		universePanel.AddContainer(sceneSettingsContainer);

		renderer::uiPanels.push_back(objectPanel);
		renderer::uiPanels.push_back(universePanel);

		showBodyProperties(renderer::loadedUniverse->GetBodies()->at(renderer::camera.focusedBodyIndex));
	}

	// Delete all components from memory
	void disposeUI() {
		for (unsigned int panelIndex = 0; panelIndex < renderer::uiPanels.size(); panelIndex++) {
			for (unsigned int containerIndex = 0; containerIndex < renderer::uiPanels.at(panelIndex).GetContainers()->size(); containerIndex++) {
				std::vector<Component*>* components = renderer::uiPanels.at(panelIndex).GetContainers()->at(containerIndex).GetComponents();
				for (unsigned int i = 0; i < components->size(); i++) {
					delete components->at(i);
				}
			}
		}
	}

	void showBodyProperties(MassBody* body) {
		std::string massStr = std::to_string(body->mass);
		std::string sizeStr = std::to_string(body->radius);

		// Remove unecessary zeros
		massStr.erase(massStr.find_last_not_of('0') + (massStr.at(massStr.find_last_not_of('0')) == '.' ? 0 : 1), std::string::npos);
		sizeStr.erase(sizeStr.find_last_not_of('0') + (sizeStr.at(sizeStr.find_last_not_of('0')) == '.' ? 0 : 1), std::string::npos);

		bodyPropertyComponents.massInput->setText(massStr);
		bodyPropertyComponents.sizeInput->setText(sizeStr);
		bodyPropertyComponents.affectedByGravityCB->setChecked(body->affectedByGravity);
		bodyPropertyComponents.affectsOthersCB->setChecked(body->affectsOthers);

		std::stringstream colorHexStr;
		colorHexStr << std::hex << body->color.toHex();
		bodyPropertyComponents.colorInput->setText(colorHexStr.str());
			
	}
}