#include "rendering/baseModels/cube.h"
#include "rendering/renderer.h"
#include "rendering/baseModels/sphere.h"
#include "rendering/color.h"

#include "ui/component.h"
#include "ui/container.h"
#include "ui/panel.h"
#include "ui/label.h"
#include "ui/button.h"
#include "ui/checkbox.h"
#include "ui/text_field.h"

void setupUIPanels() {
	ui::LabelComponent* testComp1 = new ui::LabelComponent("Test label");
	ui::ButtonComponent* testComp2 = new ui::ButtonComponent("Button", []() {
		std::cout << "Button clicked" << std::endl;
	});

	ui::TextFieldComponent* testComp3 = new ui::TextFieldComponent("Text field", "test", TFF_ALL_CHARS);

	ui::Container testContainer = ui::Container("Test container");
	testContainer.AddComponent(testComp1);
	testContainer.AddComponent(testComp2);
	testContainer.AddComponent(testComp3);

	ui::Panel testPanel = ui::Panel("Test panel", ui::Rectangle(0.75f, 0.65f, 0.99f, 0.98f, ui::Rectangle(-1.0f, -1.0f, 1.0f, 1.0f)));
	testPanel.AddContainer(testContainer);

	renderer::uiPanels.push_back(testPanel);
}

// Delete all components from memory
void disposeUI() {
	for (unsigned int panelIndex = 0; panelIndex < renderer::uiPanels.size(); panelIndex++) {
		for (unsigned int containerIndex = 0; containerIndex < renderer::uiPanels.at(panelIndex).GetContainers()->size(); containerIndex++) {
			std::vector<ui::Component*>* components = renderer::uiPanels.at(panelIndex).GetContainers()->at(containerIndex).GetComponents();
			for (unsigned int i = 0; i < components->size(); i++) {
				delete components->at(i);
			}
		}
	}
}

Universe* generateUniverse(unsigned int id) {
	Universe* universe = new Universe();
	switch (id)
	{
	case 0:
	{
		universe->AddBody(MassBody(glm::vec3(0, 0, 0), 5000, 1, Color(0.5f, 0.5f, 1.0f)));

		MassBody moon(glm::vec3(1.5f, 0, 0), 50, 0.1f, Color(0.5f, 1.0f, 0.5f));
		moon.velocity = glm::vec3(0, 0, 2);
		universe->AddBody(moon);

		break;
	}
	case 1:
	{
		universe->AddBody(MassBody(glm::vec3(0, 0, 0), 10000, 2, Color(1.0f, 0.4f, 0.0f)));

		MassBody moon1(glm::vec3(20, 0, 0), 100, 0.5f, Color(1.0f, 1.0f, 0.0f));
		moon1.velocity = glm::vec3(0, 0, 100);

		MassBody moon2(glm::vec3(-20, 0, 0), 100, 0.5f, Color(1.0f, 1.0f, 0.0f));
		moon2.velocity = glm::vec3(0, 0, -100);

		MassBody moon3(glm::vec3(5, 0, 0), 160, 0.8f, Color(1.0f, 0.0f, 0.0f));
		moon3.velocity = glm::vec3(0, 0, -10);

		MassBody moon4(glm::vec3(-5, 0, 0), 160, 0.8f, Color(1.0f, 0.0f, 0.0f));
		moon4.velocity = glm::vec3(0, 0, 10);

		universe->AddBody(moon1);
		universe->AddBody(moon2);
		universe->AddBody(moon3);
		universe->AddBody(moon4);

		break;
	}
	case 2:
	{
		MassBody body1(glm::vec3(-5, 0, 0), 160, 0.8f, Color(1.0f, 0.0f, 0.0f));
		body1.velocity = glm::vec3(0, 0, 10);

		MassBody body2(glm::vec3(5, 0, 0), 160, 0.8f, Color(0.0f, 1.0f, 0.0f));
		body2.velocity = glm::vec3(0, 0, -10);

		MassBody body3(glm::vec3(0, 0, 0), 160, 0.8f, Color(0.0f, 0.0f, 1.0f));
		body3.velocity = glm::vec3(0, 2, 0);

		universe->AddBody(body3);
		universe->AddBody(body1);
		universe->AddBody(body2);
	}
	default:
		break;
	}

	return universe;
}

int main(void)
{
	if (!renderer::init()) {
		std::cout << "Failed to initialize renderer!" << std::endl;
		return -1;
	}

	GLFWwindow* window = renderer::getWindow();

	Universe* universe = generateUniverse(0);

	renderer::loadUniverse(universe);

	setupUIPanels();

	while (!glfwWindowShouldClose(window)) {
		renderer::renderAll();
	}

	delete universe;

	renderer::terminate();
	ui::fontRendering::dispose();

	disposeUI();
	
	return 0;
}