#include "Application.h"
#include <cmath>
#include "Logic.h"

using namespace s3dge;

void Application::Initialize()
{
	CreateGameWindow("S3DGE Application", 1280, 720, false, false);

	horizontalAngle = 0;
	verticalAngle = 0;

	_shaderScene = new ShaderProgram("Resources/Shaders/basic.vert", "Resources/Shaders/light.frag");
	_shaderHUD = new ShaderProgram("Resources/Shaders/basic.vert", "Resources/Shaders/static.frag");
	
	_camera = new FPSCamera();

	_shaderScene->SetProjection(_camera->GetProjection());
	_shaderHUD->SetProjection(Matrix4::GetOrthographic(0.0f, 16.0f, 0.0f, 9.0f, -1.0f, 1.0f));
	
	TextureManager::Add("Box", "Resources/Textures/box.jpg");
	TextureManager::Add("Brick", "Resources/Textures/brick.jpg");
	TextureManager::Add("Concrete", "Resources/Textures/concrete.jpg");
	FontManager::Add("test_font", "Resources/Fonts/Assistant-Regular.ttf", 24);
	SoundManager::Add("back-in-black", "Resources/Audio/back-in-black.ogg");
	
	Sprite* rect = SpriteFactory::CreateSprite(Point2D(0, 0), -3, Size2D(1, 1), TextureManager::Get("Box"));
	Mesh* arrow = CreateArrowMesh();
	Mesh* room = CreateRoomMesh();
	Mesh* cube = CreateTexturedCube(TextureManager::Get("Concrete"));
	Label* label = LabelFactory::CreateLabel("startup...", FontManager::Get("test_font"), Point2D(0.3f, 8.4f), 0, Size2D(2, 2));
	Label* label2 = LabelFactory::CreateLabel("p:", FontManager::Get("test_font"), Point2D(0.3f, 7.4f), 0, Size2D(2, 2));
	
	GraphicsManager::AddSprite("rect", rect);
	GraphicsManager::AddMesh("arrow", arrow);
	GraphicsManager::AddMesh("room", room);
	GraphicsManager::AddMesh("cube", cube);
	GraphicsManager::AddLabel("fps", label);
	GraphicsManager::AddLabel("position", label2);

	_sceneLayer = new Layer(_shaderScene);
	_hudLayer = new Layer(_shaderHUD);

	_sceneLayer->Add(GraphicsManager::GetMesh("arrow"));
	_sceneLayer->Add(GraphicsManager::GetSprite("rect"));
	_sceneLayer->Add(GraphicsManager::GetMesh("room"));
	_sceneLayer->Add(GraphicsManager::GetMesh("cube"));
	_hudLayer->Add(GraphicsManager::GetLabel("fps"));
	_hudLayer->Add(GraphicsManager::GetLabel("position"));

	//SoundManager::Get("back-in-black")->Play();
}

void Application::Update()
{
	Point3D cameraPosition = _camera->GetPosition();

	GraphicsManager::GetLabel("fps")->SetText(std::to_string(GetFPS()) + " fps");
	GraphicsManager::GetLabel("position")->SetText(std::to_string(cameraPosition.x) + " " + std::to_string(cameraPosition.y) + " " + std::to_string(cameraPosition.z));

	UpdateCamera(InputManager::GetMouseDisplacement());

	_shaderScene->SetProjection(_camera->GetProjection());
	_shaderScene->SetView(_camera->GetView());

	_shaderScene->SetUniform3f("light.position", Vector3(0,0,0));
	
}

void Application::UpdateCamera(const Vector2& displacement)
{
	float speed = 0.1f;
	float mouseSpeed = 3.0f;

	Point3D cameraPosition = _camera->GetPosition();
	Vector3 position(cameraPosition);

	horizontalAngle += mouseSpeed * displacement.x;
	verticalAngle -= mouseSpeed * displacement.y;

	if (verticalAngle > 1.57f)
		verticalAngle = 1.57f;
	if (verticalAngle < -1.57f)
		verticalAngle = -1.57f;

	Vector3 direction(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle), -cos(verticalAngle) * cos(horizontalAngle));
	Vector3 right(-(float)sin(horizontalAngle - 3.14 / 2.0f), 0, (float)cos(horizontalAngle - 3.14 / 2.0f));
	Vector3 up((Vector3::GetCrossProduct(right, direction)));

	if (InputManager::KeyDown(S3_KEY_W))
		position += speed * direction;
	if (InputManager::KeyDown(S3_KEY_S))
		position -= speed * direction;
	if (InputManager::KeyDown(S3_KEY_A))
		position -= speed * right;
	if (InputManager::KeyDown(S3_KEY_D))
		position += speed * right;
	if (InputManager::KeyDown(S3_KEY_Q))
		position -= speed * up;
	if (InputManager::KeyDown(S3_KEY_E))
		position += speed * up;
	if (InputManager::KeyDown(S3_KEY_SPACE))
	{
		horizontalAngle = 0;
		verticalAngle = 0;
	}
	if (InputManager::KeyDown(S3_KEY_MWDOWN))
		_camera->SetFOV(_camera->GetFOV() - 1);
	if (InputManager::KeyDown(S3_KEY_MWUP))
		_camera->SetFOV(_camera->GetFOV() + 1);
	if (InputManager::KeyDown(S3_KEY_MMB))
		_camera->SetFOV(_camera->GetFOV() + 1);

	_camera->SetPosition(Point3D(position.x, position.y, position.z));
	_camera->SetViewDirection(direction);
	_camera->SetUp(up);

	_shaderScene->SetProjection(_camera->GetProjection());

	_shaderScene->SetUniform3f("viewPos", Vector3(cameraPosition.x, cameraPosition.y, cameraPosition.z));
}

void Application::Render()
{
	_shaderScene->Enable();
	_shaderScene->SetUniform3f("light.ambient", Vector3(0.2f, 0.2f, 0.2f));
	_shaderScene->SetUniform3f("light.diffuse", Vector3(0.5f, 0.5f, 0.5f));
	_shaderScene->SetUniform3f("light.specular", Vector3(1.0f, 1.0f, 1.0f));
	_shaderScene->SetUniform3f("material.ambient", Vector3(1.0f, 0.5f, 0.31f));
	_shaderScene->SetUniform3f("material.diffuse", Vector3(1.0f, 0.5f, 0.31f));
	_shaderScene->SetUniform3f("material.specular", Vector3(0.5f, 0.5f, 0.5f));
	_shaderScene->SetUniform1f("material.shininess", 32.0f);
	GraphicsManager::GetMesh("room")->Draw();
	GraphicsManager::GetMesh("arrow")->Draw();
	GraphicsManager::GetMesh("cube")->Draw();
	//_sceneLayer->Render();

	_hudLayer->Render();
}

void Application::Dispose()
{
	//SoundManager::Get("back-in-black")->Stop();
	SafeDelete(_hudLayer);
	SafeDelete(_sceneLayer);
	SafeDelete(_shaderHUD);
	SafeDelete(_camera);
}