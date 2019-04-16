#include "object.h"

/*
Credit to:
Emilian Cioca
Gil Robern
Jessica Le
*/

Object::Object(const std::string& bodyName, const std::string& textureName, std::string _name, bool _blend) {

	name = _name;

	//load texture
	if (!(texture.Load(textureName)))
	{
		std::cout << "Character Texture failed to load.\n";
		system("pause");
		exit(0);
	}

	//load model
	std::vector<std::string> file;
	file.push_back(bodyName);
	body.LoadFromFile(file);

	//Set Physics
	position = glm::vec3(0, 0, 0);
	velocity = glm::vec3(0, 0, 0);
	acceleration = glm::vec3(0, 0, 0);
	force = glm::vec3(0, 0, 0);
	mass = 1;

	//scaling
	scaleX = 1.0f;
	scaleY = 1.0f;
	scaleZ = 1.0f;
	//rotate
	rotX = 0.0f;
	rotY = 0.0f;
	rotZ = 0.0f;
	transform.Scale(glm::vec3(scaleX, scaleY, scaleZ));

	blending = _blend;
}

Object::~Object()
{
	texture.Unload();
	body.Unload();
}

void Object::update(int t, std::vector<bool> inputs) {

		//actual update
		transform = Transform::Identity();

		//physics update
		force = glm::vec3();
		acceleration = force / mass;
		velocity = velocity + (acceleration);

		//Update Position
		position = position + (velocity);

		transform.RotateY(rotY);

		transform.Scale(glm::vec3(scaleX, scaleY, scaleZ));
		transform.SetTranslation(glm::vec3(position.x, position.y, position.z));

	


}

//Returns the Players Position
glm::vec3 Object::getPosition()
{
	return position;
}

void Object::draw(ShaderProgram shader, float dt) {
	if (!hide) {
		if (blending) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else {
			glEnable(GL_CULL_FACE);
		}
		shader.SendUniformMat4("uModel", transform.data, true);
		shader.SendUniform("interp", 0);

		int modelLoc = glGetUniformLocation(shader.getProgram(), "uModel");
		glUniformMatrix4fv(modelLoc, 1, false, transform.data);

		texture.Bind();
		glBindVertexArray(body.VAO);

		glDrawArrays(GL_TRIANGLES, 0, body.GetNumVertices());
		glUniformMatrix4fv(modelLoc, 1, false, Transform().data);

		if (blending) {
			glDisable(GL_BLEND);
		}
		else {
			glDisable(GL_CULL_FACE);
		}


	}
}

//Sets player position
void Object::setPosition(glm::vec3 pos) {
	position = pos;

	transform = Transform::Identity();
	transform.Scale(glm::vec3(scaleX, scaleY, scaleZ));
	transform.RotateY(rotY);
	transform.SetTranslation(glm::vec3(position.x, position.y, position.z));
}
//Sets player scale
void Object::setScale(glm::vec3 scale) {
	scaleX = scale.x;
	scaleY = scale.y;
	scaleZ = scale.z;

	transform = Transform::Identity();
	transform.Scale(glm::vec3(scaleX, scaleY, scaleZ));
	transform.RotateY(rotY);
	transform.SetTranslation(glm::vec3(position.x, position.y, position.z));
}
//Sets player scale
void Object::setScale(float scale) {
	scaleX = scale;
	scaleY = scale;
	scaleZ = scale;

	transform = Transform::Identity();
	transform.Scale(glm::vec3(scale));
	transform.RotateY(rotY);
	transform.SetTranslation(glm::vec3(position.x, position.y, position.z));
}

//Sets player scale
void Object::RotateY(float rot) {
	rotY = rot;

	transform = Transform::Identity();
	transform.Scale(glm::vec3(scaleX, scaleY, scaleZ));
	transform.RotateY(rotY);
	transform.SetTranslation(glm::vec3(position.x, position.y, position.z));
}

