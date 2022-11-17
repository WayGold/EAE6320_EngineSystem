// Includes
//=========

#include "cMyGame.h"
#include <vector>
#include <utility>
#include <iostream>
#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>

// Static BOOLs
bool s_switchEffect = false;
bool s_notDrawn = false;
bool s_switchMovement = false;
bool s_wander = false;

// Inherited Implementation
//=========================

// Run
//----

void eae6320::cMyGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
}

void eae6320::cMyGame::UpdateSimulationBasedOnInput() {
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Tab)) {
		s_switchEffect = true;
	}
	else {
		s_switchEffect = false;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space)) {
		s_notDrawn = true;
	}
	else {
		s_notDrawn = false;
	}

	// Movement Behavior Switch
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Alt)) {
		s_switchMovement = true;
	}
	else {
		s_switchMovement = false;
	}

	// Wander Behavior Switch
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Control)) {
		s_wander = true;
	}
	else {
		s_wander = false;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up)) {
		m_camera.velocity = Math::sVector(0.0f, 0.0f, -2.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down)) {
		m_camera.velocity = Math::sVector(0.0f, 0.0f, 2.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left)) {
		m_camera.velocity = Math::sVector(-2.0f, 0.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right)) {
		m_camera.velocity = Math::sVector(2.0f, 0.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::F5)) {
		m_camera.velocity = Math::sVector(0.0f, 2.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::F6)) {
		m_camera.velocity = Math::sVector(0.0f, -2.0f, 0.0f);
	}
	else {
		m_camera.velocity = Math::sVector(0.0f, 0.0f, 0.0f);
	}

	// Control Sphere
	if (UserInput::IsKeyPressed('W')) {
		m_gameObject[1]->m_rigidBody.velocity = Math::sVector(0.0f, 0.0f, -10.0f);
	}
	else if (UserInput::IsKeyPressed('S')) {
		m_gameObject[1]->m_rigidBody.velocity = Math::sVector(0.0f, 0.0f, 10.0f);
	}
	else if (UserInput::IsKeyPressed('A')) {
		m_gameObject[1]->m_rigidBody.velocity = Math::sVector(-10.0f, 0.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed('D')) {
		m_gameObject[1]->m_rigidBody.velocity = Math::sVector(10.0f, 0.0f, 0.0f);
	}
	else {
		// Set Zero?
		m_gameObject[1]->m_rigidBody.velocity = Math::sVector(0.0f, 0.0f, 0.0f);
	}
}

void eae6320::cMyGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate) {
#pragma region AI_TEST

	/* Test AI Movement */ 
	DynamicSteeringOutput steering;

	if (s_wander) {
		// Wander Test
		dynamicWander->characterRB = &(m_gameObject[0]->m_rigidBody);
		steering = dynamicWander->getSteering();
	}
	else {
		// Apply Movement Steering Input
		if (s_switchMovement) {
			dynamicSeek->characterRB = &(m_gameObject[0]->m_rigidBody);
			dynamicSeek->targetRB = &(m_gameObject[1]->m_rigidBody);
			steering = dynamicSeek->getSteering();
		}
		else {
			dynamicArrive->characterRB = &(m_gameObject[0]->m_rigidBody);
			dynamicArrive->targetRB = &(m_gameObject[1]->m_rigidBody);
			steering = dynamicArrive->getSteering();
		}

		// Apply Turning Steering Input
		dynamicLWYG->characterRB = &(m_gameObject[0]->m_rigidBody);
		steering.rotAccel = dynamicLWYG->getSteering().rotAccel;
	}

	AIUpdate::updateRigidBody(steering, &(m_gameObject[0]->m_rigidBody),
		static_cast<float>(MAX_SPEED), static_cast<float>(MAX_ROTATION), i_elapsedSecondCount_sinceLastUpdate);

#pragma endregion

	// Update Camera and all rigidbodies
	m_camera.Update(i_elapsedSecondCount_sinceLastUpdate);
	for (int i = 0; i < gameObjectCount; i++) {
		m_gameObject[i]->m_rigidBody.Update(i_elapsedSecondCount_sinceLastUpdate);
	}
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	AI_Init();

	gameObjectCount = 0;
	effectCount = 0;

	Mesh::Mesh* boidMesh = nullptr;
	Mesh::Mesh* sphereMesh = nullptr;
	Mesh::Mesh* tubeMesh = nullptr;

	Effect::Effect* effect_1 = nullptr;
	Effect::Effect* effect_2 = nullptr;

	RenderObject::RenderObject* boidObject = nullptr;
	RenderObject::RenderObject* donutObject = nullptr;
	RenderObject::RenderObject* planeObject = nullptr;

	// Camera
	{
		m_camera.position = Math::sVector(0.0f, 10.0f, 50.0f);
		m_camera.velocity = Math::sVector(0.0f, 0.0f, 0.0f);
		m_camera.acceleration = Math::sVector(0.0f, 0.0f, 0.0f);
	}

	// Plane Mesh
	{
		auto result = Graphics::Mesh::Mesh::createMesh(boidMesh, "data/meshes/Boid.lua");
		if (result == Results::Success) {
			boidObject = new RenderObject::RenderObject(boidMesh, nullptr, m_gameObjectRigidBody[0]);
			gameObjectCount++;
			// Set Rigidbody Params
			boidObject->m_rigidBody.position = Math::sVector(-10.0f, 0.0f, 20.0f);
			auto test = boidObject->m_rigidBody.orientation.CalculateForwardDirection();
			boidObject->m_rigidBody.orientation = Math::cQuaternion(0, Math::sVector(0, 1, 0));
		}
		else {
			boidMesh->DecrementReferenceCount();
			Graphics::Mesh::Mesh::createMesh(boidMesh, "data/meshes/Mesh1.lua");
			boidObject = new RenderObject::RenderObject(boidMesh, nullptr, m_gameObjectRigidBody[0]);
			gameObjectCount++;
		}
	}

	// Sphere Mesh
	{
		auto result = Graphics::Mesh::Mesh::createMesh(sphereMesh, "data/meshes/donut.lua");
		if (result == Results::Success) {
			donutObject = new RenderObject::RenderObject(sphereMesh, nullptr, m_gameObjectRigidBody[1]);
			gameObjectCount++;
			// Set Rigidbody Params
			donutObject->m_rigidBody.position = Math::sVector(0.0f, 0.0f, 0.0f);
			//boidObject->m_rigidBody.orientation = Math::cQuaternion(0, Math::sVector(0, 1, 0));
		}
		else {
			sphereMesh->DecrementReferenceCount();
			Graphics::Mesh::Mesh::createMesh(sphereMesh, "data/meshes/Mesh1.lua");
			donutObject = new RenderObject::RenderObject(sphereMesh, nullptr, m_gameObjectRigidBody[1]);
			gameObjectCount++;
		}
	}

	// Tube Mesh
	{
		auto result = Graphics::Mesh::Mesh::createMesh(tubeMesh, "data/meshes/Plane.lua");
		if (result == Results::Success) {
			planeObject = new RenderObject::RenderObject(tubeMesh, nullptr, m_gameObjectRigidBody[2]);
			gameObjectCount++;
		}
		else {
			tubeMesh->DecrementReferenceCount();
			Graphics::Mesh::Mesh::createMesh(tubeMesh, "data/meshes/Mesh1.lua");
			planeObject = new RenderObject::RenderObject(tubeMesh, nullptr, m_gameObjectRigidBody[2]);
			gameObjectCount++;
		}
	}

	// Effects
	{
		Effect::Effect::createEffect(effect_1, "data/Shaders/Vertex/standard.shader",
			"data/Shaders/Fragment/anim_color.shader");
		effectCount++;

		Effect::Effect::createEffect(effect_2, "data/Shaders/Vertex/standard.shader",
			"data/Shaders/Fragment/anim_color1.shader");
		effectCount++;
	}

	/*pairArr = new std::pair<Mesh::Mesh*, Effect::Effect*>[gameObjectCount];*/

	/*pairArr[0] = std::make_pair(mesh_1, effect_1);
	pairArr[1] = std::make_pair(mesh_2, effect_2);*/

	meshArr[0] = boidMesh;
	meshArr[1] = sphereMesh;
	meshArr[2] = tubeMesh;

	effectArr[0] = effect_1;
	effectArr[1] = effect_2;

	m_gameObject[0] = boidObject;
	m_gameObject[1] = donutObject;
	m_gameObject[2] = planeObject;

	RigidBody_Init();

	auto sizeMesh = sizeof(*boidMesh);
	auto sizeEffect = sizeof(*effect_1);
	auto sizeObj = sizeof(*boidObject);
	auto sizeRig = sizeof(boidObject->m_rigidBody);

	Logging::OutputMessage("Size of Mesh - %d", sizeMesh);
	Logging::OutputMessage("Size of Effect - %d", sizeEffect);
	Logging::OutputMessage("Size of RenderObject - %d", sizeObj);
	Logging::OutputMessage("Size of RigidBody - %d", sizeRig);
	Logging::OutputMessage("My Game Initialized.");

	return Results::Success;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	for (int i = 0; i < gameObjectCount; i++) {
		meshArr[i]->DecrementReferenceCount();
		delete m_gameObject[i];
	}

	for (int i = 0; i < effectCount; i++) {
		effectArr[i]->DecrementReferenceCount();
	}

	auto result = AI_Cleanup();

	Logging::OutputMessage("My Game Cleaned Up.");
	return Results::Success;
}

// Rigid Bodies Init
eae6320::cResult eae6320::cMyGame::RigidBody_Init()
{
	for (int i = 0; i < gameObjectCount; i++) {
		m_gameObject[i]->m_rigidBody.velocity = Math::sVector(0.0f, 0.0f, 0.0f);
	}
	return Results::Success;
}

// AI SERVICE INIT / CLEANUP
eae6320::cResult eae6320::cMyGame::AI_Init()
{
	dynamicSeek = new AIMovement::DynamicSeek(nullptr, nullptr, MAX_ACCELERATION);
	dynamicArrive = new AIMovement::DynamicArrive(nullptr, nullptr, MAX_ACCELERATION, MAX_SPEED, TARGET_RADIUS, SLOW_RADIUS);
	dynamicLWYG = new AIMovement::DynamicLookWhereYouAreGoing(nullptr, MAX_ANG_ACCEL * 8, MAX_ROTATION, TARGET_RADIUS_ROT, SLOW_RADIUS_ROT);
	dynamicWander = new AIMovement::DynamicWander(nullptr, MAX_ANG_ACCEL * 8, MAX_ROTATION, MAX_ACCELERATION, TARGET_RADIUS_ROT, SLOW_RADIUS_ROT,
		WANDER_OFFSET, WANDER_RATE);
	return Results::Success;
}

eae6320::cResult eae6320::cMyGame::AI_Cleanup()
{
	delete dynamicSeek;
	delete dynamicArrive;
	delete dynamicLWYG;
	delete dynamicWander;
	return Results::Success;
}

// SUBMIT DATA TO GRAPHICS ENGINE
void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, 
	const float i_elapsedSecondCount_sinceLastSimulationUpdate) {

	std::vector<float> clearColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	Graphics::SubmitClearColor(clearColor);
	/*Graphics::SubmitMeshEffectData(pairArr, gameObjectCount);*/

	// Camera
	{
		// Calculate Camera Data
		auto cameraToProjected = Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective(
			float(M_PI / 4), 1.0f, 0.1f, 100.0f);
		auto cameraToWorld = Math::cMatrix_transformation::CreateWorldToCameraTransform(
			m_camera.PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
		// Submit Camera Data
		Graphics::SubmitCameraToProjectedData(cameraToProjected);
		Graphics::SubmitWorldToCameraData(cameraToWorld);
	}
	

	if (s_notDrawn) {
		// Bind Effect to GameObject
		m_gameObject[0]->BindEffect(effectArr[0]);
		Graphics::SubmitRenderObjectData(m_gameObject[0], 
			m_gameObject[0]->m_rigidBody.PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
	}
	else {
		if (s_switchEffect) {
			m_gameObject[0]->BindEffect(effectArr[0]);
			Graphics::SubmitRenderObjectData(m_gameObject[0], 
				m_gameObject[0]->m_rigidBody.PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
			m_gameObject[1]->BindEffect(effectArr[1]);
			Graphics::SubmitRenderObjectData(m_gameObject[1], 
				m_gameObject[1]->m_rigidBody.PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
			m_gameObject[2]->BindEffect(effectArr[1]);
			Graphics::SubmitRenderObjectData(m_gameObject[2],
				m_gameObject[2]->m_rigidBody.PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
		}
		else {
			m_gameObject[0]->BindEffect(effectArr[0]);
			Graphics::SubmitRenderObjectData(m_gameObject[0], 
				m_gameObject[0]->m_rigidBody.PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
			m_gameObject[1]->BindEffect(effectArr[0]);
			Graphics::SubmitRenderObjectData(m_gameObject[1], 
				m_gameObject[1]->m_rigidBody.PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
			m_gameObject[2]->BindEffect(effectArr[1]);
			Graphics::SubmitRenderObjectData(m_gameObject[2],
				m_gameObject[2]->m_rigidBody.PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
		}
	}	
}
